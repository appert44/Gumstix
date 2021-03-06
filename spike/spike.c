/*
  spike.c
 
  Copyright Scott Ellis, 2010
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
#include <asm/uaccess.h>
#include "spike.h"

#define SPI_BUFF_SIZE	4
#define USER_BUFF_SIZE	128

#define SPI_BUS 1
#define SPI_BUS_CS1 1
#define SPI_BUS_SPEED 1000


const char this_driver_name[] = "spike";

struct spike_control {
	struct spi_message msg;
	struct spi_transfer transfer;
	u8 *tx_buff; 
	u8 *rx_buff;
};

static struct spike_control spike_ctl;

struct spike_dev {
	struct semaphore spi_sem;
	struct semaphore fop_sem;
	dev_t devt;
	struct cdev cdev;
	struct class *class;
	struct spi_device *spi_device;
	char *user_buff;
	u8 test_data;	
};

static struct spike_dev spike_dev;


static void spike_prepare_spi_message(void)
{

	//printk(KERN_ALERT "spike_prepare_spi_message 1\n");

	spi_message_init(&spike_ctl.msg);

	/* put some changing values in tx_buff for testing */	
	spike_ctl.tx_buff[0] = 0 ;
	spike_ctl.tx_buff[1] = 0 ;

	memset(spike_ctl.rx_buff, 0, SPI_BUFF_SIZE);

	spike_ctl.transfer.tx_buf = spike_ctl.tx_buff;
	spike_ctl.transfer.rx_buf = spike_ctl.rx_buff;
	spike_ctl.transfer.len = 2;

	spi_message_add_tail(&spike_ctl.transfer, &spike_ctl.msg);
}

static int spike_do_one_message(void)
{
	int status;

	//printk(KERN_ALERT "spike_do_one_message 1\n");
	
	if (down_interruptible(&spike_dev.spi_sem))
		return -ERESTARTSYS;

	if (!spike_dev.spi_device) {
		up(&spike_dev.spi_sem);
		return -ENODEV;
	}

	spike_prepare_spi_message();

	status = spi_sync(spike_dev.spi_device, &spike_ctl.msg);
	
	up(&spike_dev.spi_sem);

	return status;	
}

static ssize_t spike_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	size_t len;
	ssize_t status = 0;

	//printk(KERN_ALERT "spike_read %d\n",spike_dev.test_data++);

	if (!buff) 
		return -EFAULT;
	printk (KERN_ALERT "read1");

	if (*offp > 0) 
		return 0;
	printk (KERN_ALERT "read2");

	if (down_interruptible(&spike_dev.fop_sem)) 
		return -ERESTARTSYS;
	printk (KERN_ALERT "read3");

	status = spike_do_one_message();

	if (status) {
		sprintf(spike_dev.user_buff, 
			"spike_do_one_message failed : %d\n",
			status);
	}
	else {
		/*sprintf(spike_dev.user_buff,
		"%d%d",
			spike_ctl.rx_buff[0], spike_ctl.rx_buff[1]
			);*/
		spike_dev.user_buff=spike_ctl.rx_buff;
		printk("Status: %d\nRX: 0x%2x 0x%2x\n",
							spike_ctl.msg.status,
							spike_ctl.rx_buff[0], spike_ctl.rx_buff[1]
							);
	}
		
	len = strlen(spike_dev.user_buff);
 
	if (len < count) 
		count = len;

	if (copy_to_user(buff, spike_dev.user_buff, count))  {
		printk(KERN_ALERT "spike_read(): copy_to_user() failed\n");
		status = -EFAULT;
	} else {
		*offp += count;
		status = count;
	}

	up(&spike_dev.fop_sem);
	spike_dev.user_buff[0]=0;
	spike_dev.user_buff[1]=0;
	spike_ctl.rx_buff[0]=0 ;
	spike_ctl.rx_buff[1]=0;

	return status;	
}

static void spike_prepare_lm74_config_message(int mode)
{
	printk(KERN_ALERT "spike_prepare_lm74_config_message mode = %d\n", mode);

	spi_message_init(&spike_ctl.msg);
	/* put some changing values in tx_buff for testing */
	spike_ctl.tx_buff[0] = 0 ;
	spike_ctl.tx_buff[1] = 0 ;
	spike_ctl.tx_buff[2] = 0 ;
	spike_ctl.tx_buff[3] = mode ;

	memset(spike_ctl.rx_buff, 0, SPI_BUFF_SIZE);

	spike_ctl.transfer.tx_buf = spike_ctl.tx_buff;
	spike_ctl.transfer.rx_buf = spike_ctl.rx_buff;
	spike_ctl.transfer.len = 4;

	spi_message_add_tail(&spike_ctl.transfer, &spike_ctl.msg);
}

static int spike_do_one_lm74_config_message(int mode)
{
	int status = -ENOTTY;

	printk(KERN_ALERT "spike_do_one_lm74_config_message\n");

	if ((mode != 0xff) && (mode != 0x00))
	{
		return -ENOTTY;
	}

	if (down_interruptible(&spike_dev.spi_sem))
		return -ERESTARTSYS;

	if (!spike_dev.spi_device) {
		up(&spike_dev.spi_sem);
		return -ENODEV;
	}

	spike_prepare_lm74_config_message(mode);

	status = spi_sync(spike_dev.spi_device, &spike_ctl.msg);

	up(&spike_dev.spi_sem);

	return status;
}

static int spike_ioctl(struct file *file,
				 unsigned int cmd,	/* number and param for ioctl */
				 unsigned long arg)
{
	int retval = 0;
	int status = 0;

	printk(KERN_ALERT "spike_ioctl cmd=%u arg=%lu\n",cmd,arg);

//	/*
//	* extract the type and number bitfields, and don't decode
//	* wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok( )
//	*/
//	if (_IOC_TYPE(cmd) != LM74_IOC_MAGIC) return -ENOTTY;
//	if (_IOC_NR(cmd) > LM74_IOC_MAXNR) return -ENOTTY;

	printk(KERN_ALERT "test cmd\n");

	switch (cmd) {
		case LM74_IOCTL_SET_MODE: /* Tell: arg is the value */
			printk("spike_ioctl : set mode\n");
			status = spike_do_one_lm74_config_message(arg);
			if (status) {
				printk("spike_do_one_lm74_config_message failed : %d\n",
					status);
				sprintf(spike_dev.user_buff,
					"spike_do_one_lm74_config_message failed : %d\n",
					status);
			}
			else {
				printk(	"Status: %d\nRX: 0x%2x 0x%2x 0x%2x 0x%2x \n",
					spike_ctl.msg.status,
					spike_ctl.rx_buff[0], spike_ctl.rx_buff[1],
					spike_ctl.rx_buff[2], spike_ctl.rx_buff[3]
					);
				sprintf(spike_dev.user_buff,
					"Status: %d\nRX: %d %d %d %d\n",
					spike_ctl.msg.status,
					spike_ctl.rx_buff[0], spike_ctl.rx_buff[1],
					spike_ctl.rx_buff[2], spike_ctl.rx_buff[3]
					);
			}
		break;
		default: /* redundant, as cmd was checked against MAXNR */
			printk("KERN_ALERT Call LM74 ioctl no cmd match\n");
			return -ENOTTY;
	}
	return retval;
}



static int spike_open(struct inode *inode, struct file *filp)
{	
	int status = 0;

	if (down_interruptible(&spike_dev.fop_sem)) 
		return -ERESTARTSYS;

	if (!spike_dev.user_buff) {
		spike_dev.user_buff = kmalloc(USER_BUFF_SIZE, GFP_KERNEL);
		if (!spike_dev.user_buff) 
			status = -ENOMEM;
	}	

	up(&spike_dev.fop_sem);

	return status;
}

static int spike_probe(struct spi_device *spi_device)
{
	if (down_interruptible(&spike_dev.spi_sem))
		return -EBUSY;

	spike_dev.spi_device = spi_device;

	up(&spike_dev.spi_sem);

	return 0;
}

static int spike_remove(struct spi_device *spi_device)
{
	if (down_interruptible(&spike_dev.spi_sem))
		return -EBUSY;
	
	spike_dev.spi_device = NULL;

	up(&spike_dev.spi_sem);

	return 0;
}

static int __init add_spike_device_to_bus(void)
{
	struct spi_master *spi_master;
	struct spi_device *spi_device;
	struct device *pdev;
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
 	if (pdev) {
		/* We are not going to use this spi_device, so free it */ 
		spi_dev_put(spi_device);
		
		/* 
		 * There is already a device configured for this bus.cs  
		 * It is okay if it us, otherwise complain and fail.
		 */
		if (pdev->driver && pdev->driver->name && 
				strcmp(this_driver_name, pdev->driver->name)) {
			printk(KERN_ALERT 
				"Driver [%s] already registered for %s\n",
				pdev->driver->name, buff);
			status = -1;
		} 
	} else {
		spi_device->max_speed_hz = SPI_BUS_SPEED;
		spi_device->mode = SPI_MODE_1;
		spi_device->bits_per_word = 8;
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

static struct spi_driver spike_driver = {
	.driver = {
		.name =	this_driver_name,
		.owner = THIS_MODULE,
	},
	.probe = spike_probe,
	.remove = __devexit_p(spike_remove),	
};

static int __init spike_init_spi(void)
{
	int error;

	spike_ctl.tx_buff = kmalloc(SPI_BUFF_SIZE, GFP_KERNEL | GFP_DMA);
	if (!spike_ctl.tx_buff) {
		error = -ENOMEM;
		goto spike_init_error;
	}

	spike_ctl.rx_buff = kmalloc(SPI_BUFF_SIZE, GFP_KERNEL | GFP_DMA);
	if (!spike_ctl.rx_buff) {
		error = -ENOMEM;
		goto spike_init_error;
	}

	error = spi_register_driver(&spike_driver);
	if (error < 0) {
		printk(KERN_ALERT "spi_register_driver() failed %d\n", error);
		goto spike_init_error;
	}

	error = add_spike_device_to_bus();
	if (error < 0) {
		printk(KERN_ALERT "add_spike_to_bus() failed\n");
		spi_unregister_driver(&spike_driver);
		goto spike_init_error;	
	}

	return 0;

spike_init_error:

	if (spike_ctl.tx_buff) {
		kfree(spike_ctl.tx_buff);
		spike_ctl.tx_buff = 0;
	}

	if (spike_ctl.rx_buff) {
		kfree(spike_ctl.rx_buff);
		spike_ctl.rx_buff = 0;
	}
	
	return error;
}

static const struct file_operations spike_fops = {
	.owner =	THIS_MODULE,
	.read = 	spike_read,
	.open =		spike_open,	
	.unlocked_ioctl = 	spike_ioctl,
};

static int __init spike_init_cdev(void)
{
	int error;

	spike_dev.devt = MKDEV(0, 0);

	error = alloc_chrdev_region(&spike_dev.devt, 0, 1, this_driver_name);
	if (error < 0) {
		printk(KERN_ALERT "alloc_chrdev_region() failed: %d \n", 
			error);
		return -1;
	}

	cdev_init(&spike_dev.cdev, &spike_fops);
	spike_dev.cdev.owner = THIS_MODULE;
	
	error = cdev_add(&spike_dev.cdev, spike_dev.devt, 1);
	if (error) {
		printk(KERN_ALERT "cdev_add() failed: %d\n", error);
		unregister_chrdev_region(spike_dev.devt, 1);
		return -1;
	}	

	return 0;
}

static int __init spike_init_class(void)
{
	spike_dev.class = class_create(THIS_MODULE, this_driver_name);

	if (!spike_dev.class) {
		printk(KERN_ALERT "class_create() failed\n");
		return -1;
	}

	if (!device_create(spike_dev.class, NULL, spike_dev.devt, NULL, 	
			this_driver_name)) {
		printk(KERN_ALERT "device_create(..., %s) failed\n",
			this_driver_name);
		class_destroy(spike_dev.class);
		return -1;
	}

	return 0;
}

static int __init spike_init(void)
{
	memset(&spike_dev, 0, sizeof(spike_dev));
	memset(&spike_ctl, 0, sizeof(spike_ctl));

	sema_init(&spike_dev.spi_sem, 1);
	sema_init(&spike_dev.fop_sem, 1);
	
	if (spike_init_cdev() < 0) 
		goto fail_1;
	
	if (spike_init_class() < 0)  
		goto fail_2;

	if (spike_init_spi() < 0) 
		goto fail_3;

	return 0;

fail_3:
	device_destroy(spike_dev.class, spike_dev.devt);
	class_destroy(spike_dev.class);

fail_2:
	cdev_del(&spike_dev.cdev);
	unregister_chrdev_region(spike_dev.devt, 1);

fail_1:
	return -1;
}
module_init(spike_init);

static void __exit spike_exit(void)
{
	spi_unregister_device(spike_dev.spi_device);
	spi_unregister_driver(&spike_driver);

	device_destroy(spike_dev.class, spike_dev.devt);
	class_destroy(spike_dev.class);

	cdev_del(&spike_dev.cdev);
	unregister_chrdev_region(spike_dev.devt, 1);

	if (spike_ctl.tx_buff)
		kfree(spike_ctl.tx_buff);

	if (spike_ctl.rx_buff)
		kfree(spike_ctl.rx_buff);

	if (spike_dev.user_buff)
		kfree(spike_dev.user_buff);
}
module_exit(spike_exit);

MODULE_AUTHOR("Scott Ellis");
MODULE_DESCRIPTION("spike module - an example SPI driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.2");

