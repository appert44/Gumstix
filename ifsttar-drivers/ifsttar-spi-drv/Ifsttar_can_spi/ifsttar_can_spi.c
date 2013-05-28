/*
ifsttar_can_spi.c
Copyright Scott Ellis, 2010
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/spi/spi.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/moduleparam.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <asm/io.h>
#include <asm/io.h>
#include"ifsttar_can_spi.h"

#define NANOSECS_PER_SEC 1000000000
#define SPI_BUFF_SIZE 4000
#define USER_BUFF_SIZE 128

#define SPI_BUS 1
#define SPI_BUS_CS1 1
#define SPI_BUS_SPEED 100000//1000000


#define DEFAULT_WRITE_FREQUENCY 100
static int write_frequency = DEFAULT_WRITE_FREQUENCY;
module_param(write_frequency, int, S_IRUGO);
MODULE_PARM_DESC(write_frequency, "ifsttar_can_spi write frequency in Hz");

static int bits_per_words 	= 16;
static int bus_speed		= SPI_BUS_SPEED;

static int running=0;

static DECLARE_WAIT_QUEUE_HEAD (Read_wait);

u8 *rxbuff;

const char this_driver_name[] = "ifsttar_can_spi";

struct ifsttar_can_spi_control
 {
	struct spi_message msg;
	struct spi_transfer transfer;
	u32 busy;
	u32 spi_callbacks;
	u32 busy_counter;
	u8 *tx_buff;
	u8 *rx_buff;
};

static struct ifsttar_can_spi_control ifsttar_can_spi_ctl;

struct ifsttar_can_spi_dev
{
	spinlock_t spi_lock;
	struct semaphore fop_sem;
	dev_t devt;
	struct cdev cdev;
	struct class *class;
	struct spi_device *spi_device;
	char *user_buff;
};

static struct ifsttar_can_spi_dev ifsttar_can_spi_dev;

struct spi_device *spi_device;
struct spi_master *spi_master;
struct device *pdev;


static void ifsttar_can_spi_completion_handler(void *arg)
{
	
	ifsttar_can_spi_ctl.spi_callbacks++;
	ifsttar_can_spi_ctl.busy = 0;

	rxbuff= ifsttar_can_spi_ctl.transfer.rx_buf ;

	
    // Réveille le read bloquant
     wake_up_interruptible (&Read_wait);

	running = 0;

}

static unsigned int spi_poll(struct file *filp, struct poll_table_struct *wait)
{
	poll_wait (filp, &Read_wait, wait);
	
	return 0;
}

 int ifsttar_can_spi_queue_spi_write(void)
{
	int status;
	unsigned long flags;

	spi_message_init(&ifsttar_can_spi_ctl.msg);

	/* this gets called when the spi_message completes */
	ifsttar_can_spi_ctl.msg.complete = ifsttar_can_spi_completion_handler;
	ifsttar_can_spi_ctl.msg.context = NULL;


	ifsttar_can_spi_ctl.transfer.tx_buf = NULL;//ifsttar_can_spi_ctl.tx_buff;
	ifsttar_can_spi_ctl.transfer.rx_buf = ifsttar_can_spi_ctl.rx_buff;
	ifsttar_can_spi_ctl.transfer.len = ifsttar_can_spi_ctl.transfer.len;

	spi_message_add_tail(&ifsttar_can_spi_ctl.transfer, &ifsttar_can_spi_ctl.msg);

	spin_lock_irqsave(&ifsttar_can_spi_dev.spi_lock, flags);

	if (ifsttar_can_spi_dev.spi_device)
	status = spi_async(ifsttar_can_spi_dev.spi_device, &ifsttar_can_spi_ctl.msg);
	else
	status = -ENODEV;

	spin_unlock_irqrestore(&ifsttar_can_spi_dev.spi_lock, flags);

	if (status == 0)
	ifsttar_can_spi_ctl.busy = 1;

	running = 1;;
	return status;
}



static ssize_t ifsttar_can_spi_read(struct file *filp, char __user *buff, size_t count,
loff_t *offp)
{
	size_t len;
	ssize_t status = 0;

	if (!buff)
	return -EFAULT;


	if (down_interruptible(&ifsttar_can_spi_dev.fop_sem))
	return -ERESTARTSYS;

	while (1) {
			/* Check for data available */
			if (running == 0)
				break;
			interruptible_sleep_on (&Read_wait);

			if (signal_pending(current))
				return -EINTR;
			else break;
		}

	if (copy_to_user(buff, rxbuff, count))
	{
		printk(KERN_ALERT "ifsttar_can_spi_read(): copy_to_user() failed\n");
		status = -EFAULT;
	} 
	else
	{
		*offp += count;
		status = count;
	}

	up(&ifsttar_can_spi_dev.fop_sem);

	running = 1 ;
	return status;
}

/*
* We accept two commands 'start' or 'stop' and ignore anything else.
*/
static ssize_t ifsttar_can_spi_write(struct file *filp, const char __user *buff,
size_t count, loff_t *f_pos)
{
	size_t len;
	ssize_t status = 0;

	if (down_interruptible(&ifsttar_can_spi_dev.fop_sem))
	return -ERESTARTSYS;

	memset(ifsttar_can_spi_dev.user_buff, 0, 16);
	len = count > 8 ? 8 : count;

	if (copy_from_user(ifsttar_can_spi_dev.user_buff, buff, len)) {
	status = -EFAULT;
	goto ifsttar_can_spi_write_done;
	}

	/* we'll act as if we looked at all the data */
	status = count;

	/* but we only care about the first 5 characters */
	if (!strnicmp(ifsttar_can_spi_dev.user_buff, "start", 5)) 
	{
	

	if (ifsttar_can_spi_ctl.busy) {
	printk(KERN_ALERT "waiting on a spi transaction\n");
	goto ifsttar_can_spi_write_done;
	}

	ifsttar_can_spi_ctl.spi_callbacks = 0;
	ifsttar_can_spi_ctl.busy_counter = 0;
	}
	else if (!strnicmp(ifsttar_can_spi_dev.user_buff, "stop", 4)) {
	
	}

	ifsttar_can_spi_write_done:

	up(&ifsttar_can_spi_dev.fop_sem);

	return status;
}

static int ifsttar_can_spi_open(struct inode *inode, struct file *filp)
{
	int status = 0;

	if (down_interruptible(&ifsttar_can_spi_dev.fop_sem))
	return -ERESTARTSYS;

	if (!ifsttar_can_spi_dev.user_buff) {
	ifsttar_can_spi_dev.user_buff = kmalloc(USER_BUFF_SIZE, GFP_KERNEL);
	if (!ifsttar_can_spi_dev.user_buff)
	status = -ENOMEM;
	}

	init_waitqueue_head (&Read_wait);

	up(&ifsttar_can_spi_dev.fop_sem);

	return status;
}

static void ifsttar_can_spi_close(struct inode *inode, struct file *filp)
{
	kfree(ifsttar_can_spi_dev.user_buff);
}

static int ifsttar_can_spi_probe(struct spi_device *spi_device)
{
	unsigned long flags;

	spin_lock_irqsave(&ifsttar_can_spi_dev.spi_lock, flags);
	ifsttar_can_spi_dev.spi_device = spi_device;
	spin_unlock_irqrestore(&ifsttar_can_spi_dev.spi_lock, flags);

return 0;
}

static int ifsttar_can_spi_remove(struct spi_device *spi_device)
{
unsigned long flags;



spin_lock_irqsave(&ifsttar_can_spi_dev.spi_lock, flags);
ifsttar_can_spi_dev.spi_device = NULL;
spin_unlock_irqrestore(&ifsttar_can_spi_dev.spi_lock, flags);

return 0;
}

static int __init add_ifsttar_can_spi_device_to_bus(void)
{
	char buff[64];
	int status = 0;

	spi_master = spi_busnum_to_master(SPI_BUS);
	if (!spi_master) {
	printk(KERN_ALERT "spi_busnum_to_master(%d) returned NULL\n",
	SPI_BUS);
	printk(KERN_ALERT "Missing modprobe omap2_mcspi?\n");
	return -1;
	}

	spi_device = spi_alloc_device(spi_master);
	if (!spi_device) {
	put_device(&spi_master->dev);
	printk(KERN_ALERT "spi_alloc_device() failed\n");
	return -1;
	}

	spi_device->chip_select = SPI_BUS_CS1;

	/* Check whether this SPI bus.cs is already claimed */
	snprintf(buff, sizeof(buff), "%s.%u",
	dev_name(&spi_device->master->dev),
	spi_device->chip_select);

	pdev = bus_find_device_by_name(spi_device->dev.bus, NULL, buff);

	if (pdev) 
	{
		/* We are not going to use this spi_device, so free it */
		spi_dev_put(spi_device);

		/*
		* There is already a device configured for this bus.cs
		* It is okay if it us, otherwise complain and fail.
		*/
		if (pdev->driver && pdev->driver->name &&
		strcmp(this_driver_name, pdev->driver->name)) 
		{
			printk(KERN_ALERT
			"Driver [%s] already registered for %s\n",
			pdev->driver->name, buff);
			status = -1;
		}
	} 
	else
	{
		spi_device->max_speed_hz = bus_speed;
		spi_device->mode = SPI_MODE_0;
		spi_device->bits_per_word =bits_per_words;
		spi_device->irq = -1;
		spi_device->controller_state = NULL;
		spi_device->controller_data = NULL;
		strlcpy(spi_device->modalias, this_driver_name, SPI_NAME_SIZE);

		status = spi_add_device(spi_device);
		if (status < 0) {
		spi_dev_put(spi_device);
		printk(KERN_ALERT "spi_add_device() failed: %d\n",
		status);
	}
	}

	put_device(&spi_master->dev);

	return status;
}

static struct spi_driver ifsttar_can_spi_driver = {
.driver = {
	.name = this_driver_name,
	.owner = THIS_MODULE,
	},
	.probe = ifsttar_can_spi_probe,
	.remove = __devexit_p(ifsttar_can_spi_remove),
};

static int __init ifsttar_can_spi_init_spi(void)
{
	int error;

	ifsttar_can_spi_ctl.tx_buff = kmalloc(SPI_BUFF_SIZE, GFP_KERNEL | GFP_DMA);
	if (!ifsttar_can_spi_ctl.tx_buff) {
	error = -ENOMEM;
	goto ifsttar_can_spi_init_error;
	}

	ifsttar_can_spi_ctl.rx_buff = kmalloc(SPI_BUFF_SIZE, GFP_KERNEL | GFP_DMA);
	if (!ifsttar_can_spi_ctl.rx_buff) {
		error = -ENOMEM;
		goto ifsttar_can_spi_init_error;
	}


	error = spi_register_driver(&ifsttar_can_spi_driver);
	if (error < 0) {
	printk(KERN_ALERT "spi_register_driver() failed %d\n", error);
	goto ifsttar_can_spi_init_error;
	}

	error = add_ifsttar_can_spi_device_to_bus();
	if (error < 0) {
	printk(KERN_ALERT "add_ifsttar_can_spi_to_bus() failed\n");
	spi_unregister_driver(&ifsttar_can_spi_driver);
	goto ifsttar_can_spi_init_error;
	}

	return 0;

	ifsttar_can_spi_init_error:

	if (ifsttar_can_spi_ctl.tx_buff) {
	kfree(ifsttar_can_spi_ctl.tx_buff);
	ifsttar_can_spi_ctl.tx_buff = 0;
	}

	return error;
}




long Spi_ioctl (struct file * file, unsigned int cmd, unsigned long arg)
{
	int ret =0 ; 
	 switch (cmd) 
	{
	/* read requests */
		case CMD_SPI_START:
	
			ret = ifsttar_can_spi_queue_spi_write();
        
		break;

		case CMD_SPI_STOP:

			ifsttar_can_spi_ctl.transfer.len = 0;
	
		break;

		case CMD_SPI_FREQUENCY:
			
			spi_device->max_speed_hz = spi_device->bits_per_word * arg ;
			printk(" %d \n", spi_device->max_speed_hz);
			spi_setup(spi_device);

			break;

	    case CMD_SPI_BITS_PER_WORD:
				if( (arg!=8) && (arg!=16) && (arg!=32))
				return -1;

				spi_device->bits_per_word = arg;
				spi_setup(spi_device);
			
				break;
	case CMD_SPI_NB_ECH:
				if((arg<160) || (arg>10000))
				return -1;

				ifsttar_can_spi_ctl.transfer.len = arg;
				break;

	case CMD_TEST:
			//ret = ifsttar_can_spi_queue_spi_write();
			break;

	    default:
            return -1;
	}

    return ret;
}


static const struct file_operations ifsttar_can_spi_fops = 
{
	.owner =  THIS_MODULE,
	.read =   ifsttar_can_spi_read,
	.write =  ifsttar_can_spi_write,
	.open  =  ifsttar_can_spi_open,
    .release=   ifsttar_can_spi_close,
	.poll= spi_poll,
	.unlocked_ioctl=Spi_ioctl,
};

static int __init ifsttar_can_spi_init_cdev(void)
{
	int error;

	ifsttar_can_spi_dev.devt = MKDEV(0, 0);

	error = alloc_chrdev_region(&ifsttar_can_spi_dev.devt, 0, 1, this_driver_name);
	if (error < 0) {
	printk(KERN_ALERT "alloc_chrdev_region() failed: %d \n",
	error);
	return -1;
	}

	cdev_init(&ifsttar_can_spi_dev.cdev, &ifsttar_can_spi_fops);
	ifsttar_can_spi_dev.cdev.owner = THIS_MODULE;

	error = cdev_add(&ifsttar_can_spi_dev.cdev, ifsttar_can_spi_dev.devt, 1);
	if (error) {
	printk(KERN_ALERT "cdev_add() failed: %d\n", error);
	unregister_chrdev_region(ifsttar_can_spi_dev.devt, 1);
	return -1;
	}

	return 0;
}

static int __init ifsttar_can_spi_init_class(void)
{
	ifsttar_can_spi_dev.class = class_create(THIS_MODULE, this_driver_name);

	if (!ifsttar_can_spi_dev.class) {
	printk(KERN_ALERT "class_create() failed\n");
	return -1;
	}

	if (!device_create(ifsttar_can_spi_dev.class, NULL, ifsttar_can_spi_dev.devt, NULL,
	this_driver_name)) {
	printk(KERN_ALERT "device_create(..., %s) failed\n",
	this_driver_name);
	class_destroy(ifsttar_can_spi_dev.class);
	return -1;
	}

	return 0;
}

static int __init ifsttar_can_spi_init(void)
{
	memset(&ifsttar_can_spi_dev, 0, sizeof(ifsttar_can_spi_dev));
	memset(&ifsttar_can_spi_ctl, 0, sizeof(ifsttar_can_spi_ctl));

	spin_lock_init(&ifsttar_can_spi_dev.spi_lock);
	sema_init(&ifsttar_can_spi_dev.fop_sem, 1);

	if (ifsttar_can_spi_init_cdev() < 0)
	goto fail_1;

	if (ifsttar_can_spi_init_class() < 0)
	goto fail_2;

	if (ifsttar_can_spi_init_spi()<0)
	goto fail_3;

	return 0;

	fail_3:
	device_destroy(ifsttar_can_spi_dev.class, ifsttar_can_spi_dev.devt);
	class_destroy(ifsttar_can_spi_dev.class);

	fail_2:
	cdev_del(&ifsttar_can_spi_dev.cdev);
	unregister_chrdev_region(ifsttar_can_spi_dev.devt, 1);

	fail_1:
	return -1;
}
module_init(ifsttar_can_spi_init);

static void __exit ifsttar_can_spi_exit(void)
{
	printk(" exit \n");
	spi_unregister_device(ifsttar_can_spi_dev.spi_device);
	spi_unregister_driver(&ifsttar_can_spi_driver);

	device_destroy(ifsttar_can_spi_dev.class, ifsttar_can_spi_dev.devt);
	class_destroy(ifsttar_can_spi_dev.class);

	cdev_del(&ifsttar_can_spi_dev.cdev);
	unregister_chrdev_region(ifsttar_can_spi_dev.devt, 1);

	if (ifsttar_can_spi_ctl.tx_buff)
	kfree(ifsttar_can_spi_ctl.tx_buff);

	if (ifsttar_can_spi_dev.user_buff)
	kfree(ifsttar_can_spi_dev.user_buff);
}
module_exit(ifsttar_can_spi_exit);

MODULE_AUTHOR("L.Lemarchand");
MODULE_DESCRIPTION("ifsttar_can_spi module ");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.3");


