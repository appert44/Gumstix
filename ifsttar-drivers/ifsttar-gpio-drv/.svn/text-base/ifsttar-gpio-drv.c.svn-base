//----------------------------------------------------------------------------------------------//
//                                                                                              //
//      FILE		:	ifsttar-gpio-drv.c	 													//
//      AUTHOR		:	Jonathan AILLET <jonathan.aillet@gmail.com>								//
//      Descritpion	:	Driver which allows to use gumstix gpio on OpenEmbedde using an entry	//
//						on '/dev/', it can configure and drive the gpio.						//
//																								//
//----------------------------------------------------------------------------------------------//

//----- Includes Files -------------------------------------------------------------------------//

//#include <asm/io.h>
//#include <asm/ioctls.h>
//#include <asm/uaccess.h>

#include <linux/cdev.h>
#include <linux/delay.h>
//#include <linux/device.h>
//#include <linux/errno.h>
//#include <linux/fs.h>
#include <linux/gpio.h>
//#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/sched.h>
//#include <linux/slab.h>
//#include <linux/string.h>
//#include <linux/version.h>

//----- Includes Applications ------------------------------------------------------------------//

#include "ifsttar-gpio-drv.h"

//----- Divers informations --------------------------------------------------------------------//

MODULE_AUTHOR ("Jonathan Aillet <jonathan.aillet@gmail.com>");
MODULE_DESCRIPTION ("IFSTTAR GPIO Driver");
MODULE_LICENSE ("GPL");
MODULE_VERSION ("0.1");

//------------ Definitions ---------------------------------------------------------------------//

#define DRIVER_DEV_REGISTER_NB 	192 	// Minor number on which we will check availability, book if it's possible (using a 'register_chrdev_region'), and with which we will create the '/dev' entry (using several 'device_create')

// Names used during the driver initialization
#define DRV_NAME					"ifsttar-gpio-drv > "			// Name used during a print form the driver (using printk). Typically, this name is the same that the one shows when a 'lsmod' is called
#define DRV_PROC_NAME				"ifsttar-gpio-drv"			// Name used to declare the link between a driver function with the driver '/proc' entry (after using a 'create_proc_read_entry') but it's also the name used to shows the driver entry in '/proc/devices'
#define DRV_SYS_NAME				"ifsttar-gpio-drv"			// Name used during the creation of the link between the driver and its entry in '/sys/class/*' (/sys/class/"ifsttar-gpio-drv")
#define DRV_DEV_NAME				"gpio"						// Name used for the '/dev/*' link(s) declaration. In the case where there is several minor linked in '/dev', this name can be follow by a suffix, this links can be used to send systems calls to the driver (open, close, read, write, ioctl, ...) because these links are seen by the system as files

//----- Constants and Types --------------------------------------------------------------------//

static char     sBanner[] __initdata = KERN_INFO "GPIO Driver: v0.50 :P\n";

//----- Global Variables -----------------------------------------------------------------------//

// Global variables which contains major and the minor of the driver
unsigned int m_uiGpioMajor = 0;
unsigned int m_uiGpioMinor = 0;

// Global variable which contains the first node of the allocated region
static  dev_t			m_oGpioDev;

// Global pointer which will be used to create and delete an entry in "/sys/class/"MyDriverSysName"
static  struct class *	m_pGpioClass = NULL;

// Global varaiable which contains the link to the file_operations (used to link the driver's file_operations and the created node into 'm_oGpioDev')
static  struct cdev		m_oGpioCDev;

// Global table wich contains some informations about the gpio pin
static GpioAttributes m_pGpioAttributesTab[DRIVER_DEV_REGISTER_NB];

// Global mutex used to perform the blocking read
static DECLARE_WAIT_QUEUE_HEAD (Read_wait);

// Debug
unsigned int			m_bDebugPrints = 0;

//----- Driver parameter(s) ------------------------------------------------------------------- //

module_param (m_bDebugPrints, int, 0444);

//----- Functions  ---------------------------------------------------------------------------- //

GeoDatationData EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER (void);

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	CheckMinor             												//
//																								//
// Description			:	Function which return the minor number of the entry using a			//
//							inode structure pointer				  								//
//																								//
// Return				:	The minor number of the entry										//
//																								//
//----------------------------------------------------------------------------------------------//
static int CheckMinor (struct inode * inode)
{
	// Vairbale initialization
	static int minor;
	
	// Get the minor
	minor = MINOR (inode->i_rdev);
	
	// Return it
	return minor;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	SaveRegisterAddr         											//
//																								//
// Description			:	Function which save in the gpio structure the register mux adress	//
//							which match with the gpio number									//
//																								//
// Return				:	0																	//
//																								//
//----------------------------------------------------------------------------------------------//
static int SaveRegisterAddr (void)
{
	// Variable initialization
	static unsigned int iCpt;
	
	// For gpio_0 to gpio_1
	m_pGpioAttributesTab[0].uiRegisterAdress[0] = 0x480021E0;
	m_pGpioAttributesTab[1].uiRegisterAdress[0] = 0x48002A06;
	
	// For gpio_2 to gpio_10
	for (iCpt=0; iCpt <= 10-2; iCpt++)
		m_pGpioAttributesTab[2+iCpt].uiRegisterAdress[0] = 0x48002A0A  +(2*iCpt);
	
	// For gpio_11
	m_pGpioAttributesTab[11].uiRegisterAdress[0] = 0x48002A24;
	
	// For gpio_12 to gpio_29
	for (iCpt=0; iCpt <= 29-12; iCpt++)
		m_pGpioAttributesTab[12+iCpt].uiRegisterAdress[0] = 0x480025D8 + (2*iCpt);
	
	// For gpio_30 to gpio_32
	m_pGpioAttributesTab[30].uiRegisterAdress[0] = 0x48002A08;
	m_pGpioAttributesTab[31].uiRegisterAdress[0] = 0x48002A26;
	m_pGpioAttributesTab[32].uiRegisterAdress[0] = 0x4800223A;
	
	// This gpio is not muxed (gpio_33)
	m_pGpioAttributesTab[33].uiNbMuxed = 0;
	
	// For gpio_34 to gpio_43
	for (iCpt=0; iCpt <= 43-34; iCpt++)
		m_pGpioAttributesTab[34+iCpt].uiRegisterAdress[0] = 0x4800207A + (2*iCpt);
	
	// For gpio_44 to gpio_51
	for (iCpt=0; iCpt <= 51-44; iCpt++)
		m_pGpioAttributesTab[44+iCpt].uiRegisterAdress[0] = 0x4800209E + (2*iCpt);
	
	// For gpio_52 to gpio_59
	for (iCpt=0; iCpt <= 59-52; iCpt++)
		m_pGpioAttributesTab[52+iCpt].uiRegisterAdress[0] = 0x480020B0 + (2*iCpt);
	
	// For gpio_60 to gpio_62
	for (iCpt=0; iCpt <= 62-60; iCpt++)
		m_pGpioAttributesTab[60+iCpt].uiRegisterAdress[0] = 0x480020C6 + (2*iCpt);
	
	// For gpio_63 to gpio_111
	for (iCpt=0; iCpt <= 111-63; iCpt++)
		m_pGpioAttributesTab[63+iCpt].uiRegisterAdress[0] = 0x480020CE + (2*iCpt);
	
	// For gpio_112 to gpio_166
	for (iCpt=0; iCpt <=166-112; iCpt++)
		m_pGpioAttributesTab[112+iCpt].uiRegisterAdress[0] = 0x48002134 + (2*iCpt);
	
	// For gpio_167 to gpio_169
	m_pGpioAttributesTab[167].uiRegisterAdress[0] = 0x48002130;
	m_pGpioAttributesTab[168].uiRegisterAdress[0] = 0x480021BE;
	m_pGpioAttributesTab[169].uiRegisterAdress[0] = 0x480021B0;
	
	// For gpio_170 to gpio_182
	for (iCpt=0; iCpt <=182-170; iCpt++)
		m_pGpioAttributesTab[170+iCpt].uiRegisterAdress[0] = 0x480021C6 + (2*iCpt);
	
	// For gpio_183 to gpio_185
	for (iCpt=0; iCpt <=185-183; iCpt++)
		m_pGpioAttributesTab[183+iCpt].uiRegisterAdress[0] = 0x480021C0 + (2*iCpt);
	
	// For gpio_186 to gpio_187
	m_pGpioAttributesTab[186].uiRegisterAdress[0] = 0x480021E2;
	m_pGpioAttributesTab[187].uiRegisterAdress[0] = 0x48002238;
	
	// For gpio_188 to gpio_191
	for (iCpt=0; iCpt <=191-188; iCpt++)
		m_pGpioAttributesTab[188+iCpt].uiRegisterAdress[0] = 0x480021B2 + (2*iCpt);
	
	// For multi-muxed gpio
	m_pGpioAttributesTab[120].uiRegisterAdress[1] = 0x480021A2;		// For gpio_120
	m_pGpioAttributesTab[120].uiNbMuxed ++;							// Increments the gpio register number
	m_pGpioAttributesTab[121].uiRegisterAdress[1] = 0x480021A4;		// For gpio_121
	m_pGpioAttributesTab[121].uiNbMuxed ++;							// Increments the gpio register number
	m_pGpioAttributesTab[122].uiRegisterAdress[1] = 0x480021A6;		// For gpio_122
	m_pGpioAttributesTab[122].uiNbMuxed ++;							// Increments the gpio register number
	m_pGpioAttributesTab[124].uiRegisterAdress[1] = 0x480021A8;		// For gpio_124
	m_pGpioAttributesTab[124].uiNbMuxed ++;							// Increments the gpio register number
	m_pGpioAttributesTab[125].uiRegisterAdress[1] = 0x480021AA;		// For gpio_125
	m_pGpioAttributesTab[125].uiNbMuxed ++;							// Increments the gpio register number
	m_pGpioAttributesTab[126].uiRegisterAdress[1] = 0x48002132;		// For gpio_126
	m_pGpioAttributesTab[126].uiNbMuxed ++;							// Increments the gpio register number
	m_pGpioAttributesTab[130].uiRegisterAdress[1] = 0x480021AC;		// For gpio_130
	m_pGpioAttributesTab[130].uiNbMuxed ++;							// Increments the gpio register number
	m_pGpioAttributesTab[131].uiRegisterAdress[1] = 0x480021AE;		// For gpio_131
	m_pGpioAttributesTab[131].uiNbMuxed ++;							// Increments the gpio register number

	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	SaveDefaultMode         											//
//																								//
// Description			:	Function which check every gpio register value to save them all		//
//							into an array														//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int SaveDefaultMode (void)
{
	// Variables initialization
	static int iCpt1, iCpt2;
	static void * addr;
	
	// For each gpio
	for (iCpt1 = 0; iCpt1 < DRIVER_DEV_REGISTER_NB; iCpt1++) {
		// For each gpio register
		for (iCpt2 = 0; iCpt2 < m_pGpioAttributesTab[iCpt1].uiNbMuxed ; iCpt2++) {
			if (m_bDebugPrints)
				printk(KERN_DEBUG "%s%s : Gpio %d, Register number %d, adress 0x%x\n", DRV_NAME, __func__, iCpt1, iCpt2, m_pGpioAttributesTab[iCpt1].uiRegisterAdress[iCpt2]);
			
			// Register access request
			addr = ioremap(m_pGpioAttributesTab[iCpt1].uiRegisterAdress[iCpt2], 2);
			// If an error occured during the request
			if (!addr) {
				printk (KERN_ERR "%s%s : IOREMAP error on adresse 0x%x for GPIO %d", DRV_NAME, __func__, m_pGpioAttributesTab[iCpt1].uiRegisterAdress[iCpt2], iCpt1);
				return -ERR_GPIO_SAVEDEFAULTMODE_MUX_REQUEST;
			}
			else {
				// Save the current mode
				m_pGpioAttributesTab[iCpt1].uiDefaultMode[iCpt2] = ioread16(addr);
				// Unmap the register access
				iounmap(addr);
			}
		}
	}
	
	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	ForceGpioMode	             										//
//																								//
// Description			:	Function which set a gpio registr in gpio mode.						//
//							To make this function work, you need to specify the gpio and the	//
//							gpio register you want to set.										//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int ForceGpioMode (unsigned int uiGpioNumber, int iRegisterToMux)
{
	// Variables initialization
	static int iCpt;
	static void * addr;
	
	// If the register to mux is not available
	if (iRegisterToMux >= m_pGpioAttributesTab[uiGpioNumber].uiNbMuxed)
		return -ERR_GPIO_FORCEGPIOMODE_MUX_UNAVAILABLE;
	
	// For each gpio register
	for (iCpt = 0; iCpt < m_pGpioAttributesTab[uiGpioNumber].uiNbMuxed; iCpt++) 
	{
		// Register access request
		addr = ioremap(m_pGpioAttributesTab[uiGpioNumber].uiRegisterAdress[iCpt], 2);
		// If an error occured during the request
		if (!addr) {
			printk (KERN_WARNING "%s%s : IOREMAP error on adresse 0x%x for GPIO %d", DRV_NAME, __func__, m_pGpioAttributesTab[uiGpioNumber].uiRegisterAdress[iCpt], uiGpioNumber);
			return -ERR_GPIO_FORCEGPIOMODE_MUX_REQUEST;
		}
		else {
			// Save current mode
			m_pGpioAttributesTab[uiGpioNumber].uiPreviousMode[iCpt] = ioread16(addr);
			
			// If it's the register we want to set in Gpio mode
			if (iCpt == iRegisterToMux)
				iowrite16(0x010C, addr);		// Set in GPIO Mode
			else
				iowrite16(0x010F, addr);		// Set in Safe Mode
			
			// Unmap the register access
			iounmap(addr);
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	RestorePreviousGpioMode	             								//
//																								//
// Description			:	Function which restore the gpio previous mode (which settting all	//
//							the gpio registers in their previous mode).							//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int RestorePreviousGpioMode (unsigned int uiGpioNumber)
{
	// Variables initialization
	static int iCpt;
	static u16 uiToRestoreMode;
	static void * addr;
	
	// For each gpio register
	for (iCpt = 0; iCpt < m_pGpioAttributesTab[uiGpioNumber].uiNbMuxed; iCpt++) {
		// Register access request
		addr = ioremap(m_pGpioAttributesTab[uiGpioNumber].uiRegisterAdress[iCpt], 2);
		// If an error occured during the request
		if (!addr) {
			printk (KERN_WARNING "%s%s : IOREMAP error on adresse 0x%x for GPIO %d", DRV_NAME, __func__, m_pGpioAttributesTab[uiGpioNumber].uiRegisterAdress[iCpt], uiGpioNumber);
			return -ERR_GPIO_RESTOREPREVIOUSGPIOMODE_MUX_REQUEST;
		} 
		else {
			// Save the mode to restore
			uiToRestoreMode = m_pGpioAttributesTab[uiGpioNumber].uiPreviousMode[iCpt];
			
			// Save the current mode
			m_pGpioAttributesTab[uiGpioNumber].uiPreviousMode[iCpt] = ioread16(addr);
			
			// If the mode to restore and the current mode are not the same
			if (m_pGpioAttributesTab[uiGpioNumber].uiPreviousMode[iCpt] != uiToRestoreMode)
					iowrite16(uiToRestoreMode, addr);			// Restore previous mode
			
			// Unmap the register access
			iounmap(addr);
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	RestoreAllChangedMux		             							//
//																								//
// Description			:	Function which restore all the gpio registers of all gpio			//
//							in their previous mode.												//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int RestoreAllChangedMux (void)
{
	// Variables initialization
	static int iCpt1, iCpt2;
	static u16 uiToRestoreMode;
	static void * addr;
	
	// For each gpio
	for (iCpt1 = 0; iCpt1 < DRIVER_DEV_REGISTER_NB; iCpt1++) {
		// For each gpio
		for (iCpt2 = 0; iCpt2 < m_pGpioAttributesTab[iCpt1].uiNbMuxed; iCpt2++) {
			// Register access request
			addr = ioremap(m_pGpioAttributesTab[iCpt1].uiRegisterAdress[iCpt2], 2);
			// If an error occured during the request
			if (!addr) {
				printk (KERN_WARNING "%s%s : IOREMAP error on adresse 0x%x for GPIO %d", DRV_NAME, __func__, m_pGpioAttributesTab[iCpt1].uiRegisterAdress[iCpt1], iCpt1);
				return -ERR_GPIO_RESTOREALLCHANGEDMUX_MUX_REQUEST;
			}
			else {
				// Save the mode to restore
				uiToRestoreMode = m_pGpioAttributesTab[iCpt1].uiDefaultMode[iCpt2];
				
				// Save the current mode
				m_pGpioAttributesTab[iCpt1].uiPreviousMode[iCpt2] = ioread16(addr);
				
				// If the mode to restore and the current mode are not the same
				if (m_pGpioAttributesTab[iCpt1].uiPreviousMode[iCpt2] != uiToRestoreMode)
					iowrite16(uiToRestoreMode, addr);			// Restore previous mode
				
				// Unmap the register access
				iounmap(addr);
			}
		}
	}
	
	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	GpioIrqHandler														//
//																								//
// Description			:	This function is called when the event specified during the			//
//							interrupt request happened.											//
//							It saved the current GeoDatationData structure in the gpio			//
//							structure, read the current gpio input value, wait during the		//
//							debounde delay, and unlock the blocking read indicating which gpio	//
//							did it.																//
//																								//
// Return				:	Irq handled															//
//																								//
//----------------------------------------------------------------------------------------------//
static irqreturn_t GpioIrqHandler (int irq, void * dev_id)
{
	// Variables initialization
	static int minor;
	static GeoDatationData oGDData;
	
	// Get the minor of this entry
	minor = (int) dev_id;

	// Get the current GeoDatationData structure using the synchronization driver exported function
	oGDData = EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER ();
	
	// Save the previously retrieve structure
	m_pGpioAttributesTab[minor].oLastItGDData = oGDData;
	
	// Read and save the gpio input value
	m_pGpioAttributesTab[minor].bValue = gpio_get_value(minor);

	// Wait during the gpio debounce delay
	mdelay(m_pGpioAttributesTab[minor].uiMsDebounceDelay);
	
	// Indicate that an event happened on this gpio
	m_pGpioAttributesTab[minor].bDone  = 1;
	// Unlock the blocking read
	wake_up_interruptible (&Read_wait);

	return IRQ_HANDLED;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Gpio_open															//
//																								//
// Description			:	Function called when an open is perfommed on a						//
//							'/dev/gpio...' entry.								  				//
//							Here, the function check if the gpio is available, booked it, set	//
//							the gpio pin in gpio mode (using the gpio register(s)) and read		//
//							the open mode to configure the gpio in Input or in Ouput.			//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int Gpio_open (struct inode * inode, struct file * filp)
{
	// Variables initialization
	static int iRet, oOpenMode;
	static char cMode;
	static int minor;
	
	iRet = 0;
	cMode = '\0';
	
	// Get the minor of this entry
	minor = CheckMinor (inode);
	// Check if the minor is good
	if (minor < 0)
		return -ERR_GPIO_OPEN_WRONG_MINOR;
	
	// Check for multiple open
	if (m_pGpioAttributesTab[minor].bIsOpened) {
		printk (KERN_INFO "%s%s : Gpio number %d is already open !\n", DRV_NAME, __func__, minor);
		return -ERR_GPIO_OPEN_GPIO_BUSY_IN_DRIVER;
	}
	
	// Force the gpio mux mode in the first register
	ForceGpioMode (minor, 0);
	
	// Book gpio and link it to the minor
	if (gpio_request (minor, m_pGpioAttributesTab[minor].sLabel)) {
		m_pGpioAttributesTab[minor].bIsOpened = 0;
		return -ERR_GPIO_OPEN_GPIO_BUSY_OUT_DRIVER;
	}
	
	// Get the gpio open mode
	oOpenMode = filp->f_mode;
	// If it's a read mode (read only, read/write, ...)
	if (oOpenMode & 1) {
		// Set the gpio in input mode
		m_pGpioAttributesTab[minor].bDirection = INPUT;		// In the gpio structure
		iRet = gpio_direction_input(minor);					// Physically
		// Get the input value and copy it in the structure
		m_pGpioAttributesTab[minor].bValue = gpio_get_value(minor);
		
		// Check if the direction is well setted
		if (iRet < 0) {
			printk (KERN_WARNING "%s%s : Error %d during the setting of input direction for the gpio %d\n", DRV_NAME, __func__, iRet, minor);
			return -ERR_GPIO_OPEN_GPIO_SET_DIRECTION_INPUT;
		}
	}
	else {
		// Set the gpio default output value
		m_pGpioAttributesTab[minor].bValue = 0;
		// Set the gpio in input mode
		m_pGpioAttributesTab[minor].bDirection = OUTPUT;							// In the gpio structure
		iRet = gpio_direction_output(minor, m_pGpioAttributesTab[minor].bValue);	// Physically
		
		// Check if the direction is well setted
		if (iRet < 0) {
			printk (KERN_WARNING "%s%s : Error %d during the setting of output direction for the gpio %d\n", DRV_NAME, __func__, iRet, minor);
			return -ERR_GPIO_OPEN_GPIO_SET_DIRECTION_OUTPUT;
		}
	}

	// Set the gpio structure variables
	m_pGpioAttributesTab[minor].bMode = IO_MODE;
	m_pGpioAttributesTab[minor].bDone = 0;
	m_pGpioAttributesTab[minor].bIsOpened = 1;
	
	if (m_bDebugPrints) {
		if (m_pGpioAttributesTab[minor].bDirection == INPUT)
			printk(KERN_DEBUG "%s%s : GPIO %d is openned in INPUT mode\n", DRV_NAME, __func__, minor);
		else
			printk(KERN_DEBUG "%s%s : GPIO %d is openned in OUTPUT mode\n", DRV_NAME, __func__, minor);
	}

	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Gpio_close															//
//																								//
// Description			:	Function called when a close is perfommed on a						//
//							'/dev/gpio...' entry.								  				//
//							Here, the function release the gpio, and restore the previous gpio	//
//							pin mode (using the gpio register(s)).								//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int Gpio_close (struct inode * inode, struct file * filp)
{
	// Variable initialization
	static int minor;
	
	// Get the minor of this entry
	minor = CheckMinor(inode);
	// Check if the minor is good
	if (minor < 0)
		return -ERR_GPIO_CLOSE_WRONG_MINOR;
	
	// If the gpio is in Input/Output mode
	if (m_pGpioAttributesTab[minor].bMode == IO_MODE)
		// Release the gpio
		gpio_free (minor);
	// Else if the gpio is in Interrupt mode
	else if (m_pGpioAttributesTab[minor].bMode == IT_MODE)
		// Release the gpio interrupt
		free_irq (gpio_to_irq (minor), (void *) minor);

	// Set the gpio openning indicator boolean to 0
	m_pGpioAttributesTab[minor].bIsOpened = 0;
	
	// Restore the gpio previous mode
	RestorePreviousGpioMode(minor);

	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Gpio_read															//
//																								//
// Description			:	Function called when a read is perfommed on a						//
//							'/dev/gpio...' entry.								  				//
//							Here, when a gpio is in IO mode, read the input value,				//
//							otherwise (if the gpio is IT mode), the read become a blocking read	//
//							which is unlock when the event specified during gpio interrupt		//
//							request happened.													//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static ssize_t Gpio_read (struct file * filp, char * buf, size_t size, loff_t * offp)
{
	// Variables initialization
	static int minor;
	static const char * bit = NULL;
	
	// Get the minor of this entry
	minor = CheckMinor (filp->f_dentry->d_inode);
	
	// Check if the minor is good
	if (minor < 0)
		return -ERR_GPIO_READ_WRONG_MINOR;
	// Check if the message size is good
	else if (size < 2)
		return -ERR_GPIO_READ_SPECIFIED_SIZE;
	// Check if the gpio is in Input mode
	else if (m_pGpioAttributesTab[minor].bMode == IO_MODE && m_pGpioAttributesTab[minor].bDirection != INPUT) {
		printk(KERN_WARNING "%s%s : Impossible to get the input value in this direction (input direction is needed in IO mode)\n", DRV_NAME, __func__);
		return -ERR_GPIO_READ_WRONG_GPIO_DIRECTION;
	}
	
	// If the gpio is in IO mode
	if (m_pGpioAttributesTab[minor].bMode == IO_MODE) {
		// Read and save the gpio input value
		m_pGpioAttributesTab[minor].bValue = gpio_get_value(minor);
	}
	// Else if the gpio is in IT mode
	else if (m_pGpioAttributesTab[minor].bMode == IT_MODE) {
		// Wait for the next interruption which will unlock the mutex waiting, and then, the read will unlock
		while (1) {
			// Check for data available
			if (m_pGpioAttributesTab[minor].bDone == 1)
				break;
			
			// Wait for the interrpution
			interruptible_sleep_on (&Read_wait);
			if (signal_pending(current))
				return -EINTR;
		}
		
		// Say that data is not available anymore
		m_pGpioAttributesTab[minor].bDone = 0;
	}
	else {
		printk(KERN_WARNING "%s%s : Impossible to read the input value in this mode (IO mode or IT mode are needed)\n", DRV_NAME, __func__);
		return -ERR_GPIO_READ_WRONG_GPIO_MODE;
	}
	
	// Copy the gpio input value into the buffer which will be send to the user
	bit = m_pGpioAttributesTab[minor].bValue ? "1" : "0"; 
	
	// Return the read bytes number
	return (copy_to_user(buf, bit, 2)) ? -ERR_GPIO_READ_WRONG_READED_BYTES : 2;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Gpio_write															//
//																								//
// Description			:	Function called when a write is perfommed on a						//
//							'/dev/gpio...' entry.								  				//
//							Here, when a gpio is in IO mode and in Output direction, the write	//
//							check if the buffer is good, and write the value passed in this		//
//							buffer on the gpio.													//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static ssize_t Gpio_write (struct file * filp, const char * buf, size_t size, loff_t * offp)
{
	// Variable initialization
	static int minor;
	
	// Get the minor of this entry
	minor = CheckMinor(filp->f_dentry->d_inode);
	
	// Check if the minor is good
	if (minor < 0)
		return -ERR_GPIO_WRITE_WRONG_MINOR;
	// Check if the message size is good
	else if (size < 2)
		return -ERR_GPIO_WRITE_SPECIFIED_SIZE;
	// Check if the buffer pointer is not null
	else if (!buf)
		return -ERR_GPIO_WRITE_SPECIFIED_BUFFER;
	// Check if the gpio is in Input/Output mode
	else if (m_pGpioAttributesTab[minor].bMode != IO_MODE) {
		printk(KERN_WARNING "%s%s : Impossible to set an output value in this mode (IO mode is needed)\n", DRV_NAME, __func__);
		return -ERR_GPIO_WRITE_WRONG_GPIO_MODE;
	}
	// Check if the gpio is in Input direction
	else if (m_pGpioAttributesTab[minor].bDirection != OUTPUT) {
		printk(KERN_WARNING "%s%s : Impossible to set an output value in this direction (output direction is needed)\n", DRV_NAME, __func__);
		return -ERR_GPIO_WRITE_WRONG_GPIO_DIRECTION;
	}
	
	// Get and save (in a gpio structure variable) the new gpio output value
	m_pGpioAttributesTab[minor].bValue = ((buf[0] == 0) || (buf[0] == '0')) ? 0 : 1;
	
	// Set the new gpio output value
	gpio_set_value(minor, m_pGpioAttributesTab[minor].bValue);
	
	if (m_bDebugPrints)
		printk(KERN_DEBUG "%s%s : Output value set to %d in the GPIO %d\n", DRV_NAME, __func__, m_pGpioAttributesTab[minor].bValue, minor);
	
	return size;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Gpio_poll															//
//																								//
// Description			:	Function called when a poll is perfommed on a						//
//							'/dev/gpio...' entry.								  				//
//							Here, the poll can be unlocked when the read has been unlocked, or 	//
//							when the time specified in its called is reached.					//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static unsigned int Gpio_poll (struct file * filp, struct poll_table_struct * wait)
{
	// Variables initialization
	static int minor;
	static unsigned int mask;
	
	minor = CheckMinor(filp->f_dentry->d_inode);
	mask = 0;
	
 	// If the gpio is in interrupt mode
	if (m_pGpioAttributesTab[minor].bMode == IT_MODE) {
		// Wait for the read to be unlock or for the timeout
		poll_wait (filp, &Read_wait, wait);
		// If a read has been unlocked
		if (m_pGpioAttributesTab[minor].bDone)
			// Specify it in the mask
			mask = POLLIN | POLLRDNORM;
	}

	// Send back the mask
	return mask;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Gpio_ioctl		   		           									//
//																								//
// Description			:	Function called when an ioctl is perfommed on a						//
//							'/dev/gpio...' entry.								  				//
//							See the IOCTL commands section on the header.						//
//																								//
// Return				:   A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
long Gpio_ioctl (struct file * file, unsigned int cmd, unsigned long arg)
{
	// Variables initialization
	static int iRet;
	static int minor;
	
	iRet = 0;
	minor = CheckMinor(file->f_dentry->d_inode);
	
	// Depending on the IOCLT send command
    switch (cmd)
    {
        // If it's the set direction command
        case GPIO_IOCTL_SET_DIRECTION:
			// Check if the gpio is in IO mode
			if (m_pGpioAttributesTab[minor].bMode != IO_MODE) {
				printk(KERN_WARNING "%s%s : Impossible to set a direction in this mode (IO mode is needed)\n", DRV_NAME, __func__);
				return -ERR_GPIO_IOCTL_WRONG_GPIO_MODE;
			}
			// If the user want to set the input direction
			else if ((unsigned int) arg == INPUT) {
				// Set the gpio in input mode
				m_pGpioAttributesTab[minor].bDirection = INPUT;		// In the gpio structure
				iRet = gpio_direction_input(minor);					// Physically
				// Get the input value and copy it in the structure
				m_pGpioAttributesTab[minor].bValue = gpio_get_value(minor);
				
				if (m_bDebugPrints)
					printk(KERN_DEBUG "%s%s : Input mode set in the GPIO %d\n", DRV_NAME, __func__, minor);
			}
			// If the user want to set the output direction
			else if ((unsigned int) arg == OUTPUT) {
				// Set the gpio default output value
				m_pGpioAttributesTab[minor].bValue = 0;
				// Set the gpio in input mode
				m_pGpioAttributesTab[minor].bDirection = OUTPUT;							// In the gpio structure
				iRet = gpio_direction_output(minor, m_pGpioAttributesTab[minor].bValue);	// Physically
				
				if (m_bDebugPrints)
					printk(KERN_DEBUG "%s%s : Output mode set in the GPIO %d\n", DRV_NAME, __func__, minor);
			}
			// If the user has specified an unknown direction
			else {
				printk(KERN_WARNING "%s%s : Wrong indicated direction, reconfiguration impossible\n", DRV_NAME, __func__);
				return -ERR_GPIO_IOCTL_SPECIFIED_DIRECTION;
			}
						
			break;
		
		// If it's the set output value command
		case GPIO_IOCTL_SET_OUTPUT_VALUE:
			// Check if the gpio is in IO mode
			if (m_pGpioAttributesTab[minor].bMode != IO_MODE) {
				printk(KERN_WARNING "%s%s : Impossible to set an output value in this mode (IO mode is needed)\n", DRV_NAME, __func__);
				return -ERR_GPIO_IOCTL_WRONG_GPIO_MODE;
			}
			// Check if the gpio is in Output direction
			else if (m_pGpioAttributesTab[minor].bDirection != OUTPUT) {
				printk(KERN_WARNING "%s%s : Impossible to set an output value in this direction (output direction is needed)\n", DRV_NAME, __func__);
				return -ERR_GPIO_IOCTL_WRONG_GPIO_DIRECTION;
			}
			// Check if the user want to set a correct ouput value
			else if ((unsigned int) arg > 1) {
				printk(KERN_WARNING "%s%s : Impossible to set an output value, the argument is not valid (it must be 0 or 1)\n", DRV_NAME, __func__);
				return -ERR_GPIO_IOCTL_SPECIFIED_OUTPUT_VALUE;
			}
			
			// Set the gpio output value
			m_pGpioAttributesTab[minor].bValue = (unsigned int) arg;		// In the gpio structure
			gpio_set_value(minor, m_pGpioAttributesTab[minor].bValue);		// Physically
			
			if (m_bDebugPrints)
				printk(KERN_DEBUG "%s%s : Output value set to %d in the GPIO %d\n", DRV_NAME, __func__, (unsigned int) arg, minor);
			
			break;
		
		// If it's the get input value command
		case GPIO_IOCTL_GET_INPUT_VALUE:
			// Check if the gpio is in IO mode
			if (m_pGpioAttributesTab[minor].bMode != IO_MODE) {
				printk(KERN_WARNING "%s%s : Impossible to get the input value in this mode (IO mode is needed)\n", DRV_NAME, __func__);
				return -ERR_GPIO_IOCTL_WRONG_GPIO_MODE;
			}
			// Check if the gpio is in Input direction
			else if (m_pGpioAttributesTab[minor].bDirection != INPUT) {
				printk(KERN_WARNING "%s%s : Impossible to get the input value in this direction (input direction is needed)\n", DRV_NAME, __func__);
				return -ERR_GPIO_IOCTL_WRONG_GPIO_DIRECTION;
			}
			
			// Get the input value and copy it in the structure
			m_pGpioAttributesTab[minor].bValue = gpio_get_value(minor);
			// Send back the input value to the user using the 'arg' parameter
			*((unsigned int *) arg) = m_pGpioAttributesTab[minor].bValue;
			
			break;
		
		// If it's the set interrupt command
		case GPIO_IOCTL_SET_INTERRUPT:
			// Check if the user specified a good event
			if ((unsigned int) arg != IRQF_TRIGGER_RISING && (unsigned int) arg != IRQF_TRIGGER_FALLING) {
				printk(KERN_WARNING "%s%s : The indicated interrupt trigger is not valid : %d\n", DRV_NAME, __func__, (unsigned int) arg);
				return -ERR_GPIO_IOCTL_SPECIFIED_EVENT;
			}
			// If the gpio is in IT mode
			else if (m_pGpioAttributesTab[minor].bMode == IT_MODE) {
				// Release the gpio interrupt
				free_irq (gpio_to_irq (minor), (void *) minor);
			}
			else {
				// Release the gpio
				gpio_free (minor);
				// Set the gpio mode
				m_pGpioAttributesTab[minor].bMode = IT_MODE;		// In the gpio structure
			}
			
			// Save the user specified event
			m_pGpioAttributesTab[minor].uiTriggerMode = (unsigned int) arg;
			
			// Gpio interrupt request
			iRet = request_irq(gpio_to_irq (minor), GpioIrqHandler, m_pGpioAttributesTab[minor].uiTriggerMode, m_pGpioAttributesTab[minor].sLabel, (void *) minor);
	        if(iRet != 0) {
	        	printk(KERN_WARNING "%s%s : Error %d during the GPIO interruption request on minor %d\n", DRV_NAME, __func__, iRet, minor); 	
	        	return -ERR_GPIO_IOCTL_INTERRUPTION_REQUEST;
	        }
	        
	        // Says that the read is currently locked
	        m_pGpioAttributesTab[minor].bDone = 0;
			
			break;
		
		// If it's the set debounce delay command
		case GPIO_IOCTL_SET_DEBOUNCE_DELAY:
			// Check if the user specified a good debounce value
			if ((unsigned int) arg > MAX_DEBOUNCE_VALUE) {
				printk(KERN_WARNING "%s%s : The indicated debounce delay is not valid, the maximum value is %d\n", DRV_NAME, __func__, MAX_DEBOUNCE_VALUE); 	
	        	return -ERR_GPIO_IOCTL_SPECIFIED_DEBOUNCE_VALUE;
			}
			
			// Set the gpio debounce delay
			m_pGpioAttributesTab[minor].uiMsDebounceDelay = (unsigned int) arg;	
			
			break;
		
		// If it's the free interrupt command
		case GPIO_IOCTL_FREE_INTERRUPT:
			// Check if the gpio is in IT mode
			if (m_pGpioAttributesTab[minor].bMode != IT_MODE) {
				printk(KERN_WARNING "%s%s : Impossible to free the interrupt in this mode (IT mode is needed)\n", DRV_NAME, __func__);
				return -ERR_GPIO_IOCTL_WRONG_GPIO_MODE;
			}
			
			// Release the gpio interrupt
			free_irq (gpio_to_irq (minor), (void *) minor);
			
			// Rebook gpio and relink it to the minor and check if an error occured
			if (gpio_request (minor, m_pGpioAttributesTab[minor].sLabel)) {
				printk(KERN_WARNING "%s%s : Error during the GPIO_IOCTL_FREE_INTERRUPT on gpio_request for minor %d\n", DRV_NAME, __func__, minor);
				// Set the gpio in an abstract mode
				m_pGpioAttributesTab[minor].bMode = 0;
			}
			else
				// Set the gpio mode
				m_pGpioAttributesTab[minor].bMode = IO_MODE;

			// Indicate that an event happened on this gpio
			m_pGpioAttributesTab[minor].bDone  = 1;
			// Unlock the blocking read
			wake_up_interruptible (&Read_wait);
			
			break;
		
		// If it's the get last interruption GeoDataion data command
		case GPIO_IOCTL_GET_LAST_IT_GD_DATA :			
			// Copy the last GeoDatationData structure of this gpio to the user
			iRet = copy_to_user((GeoDatationData *) arg, &m_pGpioAttributesTab[minor].oLastItGDData, sizeof(GeoDatationData));
			// Check if an error occured
			if (iRet != 0) {
				printk(KERN_WARNING "%s%s : Error during the copy of the GeoDatation daye to the user : %d\n", DRV_NAME, __func__, iRet);
				return -ERR_GPIO_IOCTL_COPY_GDDATA_TO_USER;
			}
			
			break;
		
		// If it's an unknown IOCTL command
        default :
        	printk (KERN_WARNING "%s%s : Unrecognized ioctl : '0x%x'\n", DRV_NAME, __func__, cmd);
            return -ERR_GPIO_IOCTL_SPECIFIED_IOCTL_COMMAND;
    }

    return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// 	Creation of a structure which allows to link the system calls to the 						//
//	previously coded functions.																	//
// 	Structure used during the driver initialization												//
// 																								//
//----------------------------------------------------------------------------------------------//
static struct file_operations gpio_fops = {
	owner:      		THIS_MODULE,
	open:       		Gpio_open,
	release:    		Gpio_close,
	read:				Gpio_read,
	write:	    		Gpio_write,
	poll:				Gpio_poll,
	unlocked_ioctl:		Gpio_ioctl,
};

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Gpio_initialize														//
//																								//
// Description			:	Function called when the driver is insert (insmod).					//
// 							Allows to provide driver informations and to link the functions.	//
//																								//
// Return				:	A negative number if an error occured and prevents the 'insmod',	//
//							0 otherwise.														//
//																								//
//----------------------------------------------------------------------------------------------//
static int __init Gpio_initialize (void)
{
    // Variables initialization
    int iRet = 0, iCpt = 0, iCpt2 = 0, iNbToDestroy = 0;
    char sLabel[8];
	
	// Print the driver banner
    printk(KERN_NOTICE "%s", sBanner);
    
    // Allocate a driver 'DRV_DEV_NAME' with a major number obtain dynamically. We can found this entry in "/proc/devices"
    iRet = alloc_chrdev_region (&m_oGpioDev, 0, DRIVER_DEV_REGISTER_NB, DRV_DEV_NAME);
    // And check if the allocation happened well
    if (iRet < 0) {
		printk(KERN_ERR "%s%s : alloc_chrdev_region failed, error : %d\n", DRV_NAME, __func__, iRet);
		return -ERR_GPIO_INIT_ALLOC_CHRDEV_REGION;
	}
	
	// Save the dynamically obtained major and minor
	m_uiGpioMajor = MAJOR(m_oGpioDev);
	m_uiGpioMinor = MINOR(m_oGpioDev);
	
	// Initialize the global variable Initialise 'm_oGpioCDev' and associate it with the file_operation 'gpio_fops'
    cdev_init (&m_oGpioCDev, &gpio_fops);
    m_oGpioCDev.owner = THIS_MODULE;				// Indicate the module owner

	// Add the driver in the system and link it with the file_operations linked in 'm_oGpioCDev' to the node using 'm_oGpioDev'
    iRet = cdev_add(&m_oGpioCDev, m_oGpioDev, DRIVER_DEV_REGISTER_NB);
    // And check if the creation of the link happened well
    if (iRet != 0) {
        printk (KERN_ERR "%s%s : cdev_add failed, err : %d\n", DRV_NAME, __func__, iRet);
        iRet = -ERR_GPIO_INIT_CDEV_ADD;
        goto out_unregister;
    }
    
    // Now that we've added the device, create a class, so that udev will make the /dev entries (also create an entry in "/sys/class/")
    m_pGpioClass = class_create (THIS_MODULE, DRV_SYS_NAME);
    // And check if the operation happened well
	if (IS_ERR (m_pGpioClass)) {
        printk (KERN_ERR "%s%s : Unable to create class\n", DRV_NAME, __func__);
        iRet = -ERR_GPIO_INIT_CLASS_CREATE;
        goto out_cdev_del;
    }
    
    // Entries creation in "/dev"
	for (iCpt=0; iCpt<DRIVER_DEV_REGISTER_NB; iCpt++) {
		m_pGpioAttributesTab[iCpt].pDeviceEntry = device_create (m_pGpioClass, NULL, MKDEV (m_uiGpioMajor, m_uiGpioMinor+iCpt), (void *) iCpt, "%s%03d", DRV_DEV_NAME, iCpt);
		// Check if the operation happened well
		if (IS_ERR(m_pGpioAttributesTab[iCpt].pDeviceEntry)) {
			printk(KERN_ERR "%s%s : Unable to create device : %s%d\n", DRV_NAME, __func__, DRV_DEV_NAME, iCpt);
			iRet = -ERR_GPIO_INIT_DEVICE_CREATE;
			goto out_dev_del;
		}
	
		// Save the mux label of this GPIO
		memset(sLabel, 0, sizeof(sLabel));		// Reset the gpio label
		sprintf(sLabel, "gpio%d", iCpt);		// Create the label for the current gpio
		memcpy(m_pGpioAttributesTab[iCpt].sLabel, sLabel, sizeof(sLabel));		// Save the label in the gpio structure
	
		// Initialize the variables of the table's structures
		m_pGpioAttributesTab[iCpt].bValue = INIT_VALUE;
		m_pGpioAttributesTab[iCpt].bMode = IO_MODE;
		m_pGpioAttributesTab[iCpt].bDirection = INPUT;
		m_pGpioAttributesTab[iCpt].bDone = 0;
		m_pGpioAttributesTab[iCpt].bIsOpened = 0;
		m_pGpioAttributesTab[iCpt].uiTriggerMode = INIT_VALUE;
		m_pGpioAttributesTab[iCpt].uiMsDebounceDelay = 0;
		m_pGpioAttributesTab[iCpt].uiNbMuxed = 1;
		
		// For each gpio mux
		for (iCpt2 = 0; iCpt2 < MAX_MUX_NB; iCpt2++) {
			// Previous mode set to safe mode
			m_pGpioAttributesTab[iCpt].uiPreviousMode[iCpt2] = 0x010F;
			// Default mode set to safe mode
			m_pGpioAttributesTab[iCpt].uiDefaultMode[iCpt2] = 0x010F;
			// Register adress is unknown
			m_pGpioAttributesTab[iCpt].uiRegisterAdress[iCpt2] = 0x00000000;
		}
	}
	
	// Save register adress in the gpio structure (adress come frome OMAP reference manual)
	iRet = SaveRegisterAddr();
	// And check if the operation happened well
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Error %d during the save of the gpio register adress\n", DRV_NAME, __func__, iRet); 
		goto out_all_del;
	}
	
	// Save default mux mode in the gpio structure
	iRet = SaveDefaultMode();
	// And check if the operation happened well
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Error %d during the save of the gpio register default mux mode\n", DRV_NAME, __func__, iRet);
		goto out_all_del;
	}
	
	printk (KERN_NOTICE "%s%s : Driver loaded. Major = %d, Minor = %d.\n", DRV_NAME, __func__, m_uiGpioMajor, m_uiGpioMinor);

    goto done;
    
    // Exit shortcuts
    out_all_del:
    	// Says that we want to destroy all gpio entries
    	iCpt = DRIVER_DEV_REGISTER_NB;

	out_dev_del:
		// Says that we want to destroy the previously created gpio entries
		iNbToDestroy = iCpt;
		for (iCpt = 0; iNbToDestroy >= 0; iCpt++)
			device_destroy (m_pGpioClass, MKDEV (m_uiGpioMajor, m_uiGpioMinor+iCpt));

	out_cdev_del:
    	// Destroy the driver from the system and destroy the link between the file_operations linked in 'm_oGpioCDev' and the node using 'm_oGpioDev'
    	cdev_del (&m_oGpioCDev);

	out_unregister:
    	// De-allocate the driver 'DRV_DEV_NAME'
    	unregister_chrdev_region (m_oGpioDev, DRIVER_DEV_REGISTER_NB);

	done:
    	return iRet;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Gpio_cleanup														//
//																								//
// Description			:	Module destroy function	(rmmod)										//
//																								//
// Return				:   Nothing																//
//																								//
//----------------------------------------------------------------------------------------------//
static void __exit Gpio_cleanup (void)
{
	// Variable initialization
	int iCpt = 0;
    
    // For all gpio
	for (iCpt=0; iCpt<DRIVER_DEV_REGISTER_NB; iCpt++) {
		// Delete its node and its entrie in "/dev"
		device_destroy (m_pGpioClass, MKDEV (m_uiGpioMajor, m_uiGpioMinor+iCpt));
	}
    
    // Delete the entries in "/sys/class"
    class_unregister(m_pGpioClass);
    class_destroy (m_pGpioClass);

	// Destroy the 'm_oGpioCDev' global variable
    cdev_del (&m_oGpioCDev);
	
	// Deallocates the driver 'DRV_DEV_NAME' using its first node 'm_oGpioDev' and the number of minor allocated
	unregister_chrdev_region (m_oGpioDev, DRIVER_DEV_REGISTER_NB);
	
	// Restore gpio register default mode for all change mux mode
	RestoreAllChangedMux ();
    
    printk (KERN_NOTICE "%s%s : Driver unloaded\n", DRV_NAME, __func__);
}

//----------------------------------------------------------------------------------------------//
//																								//
//	Mapping of the 'module_init' and 'module_exit' functions									//
//																								//
//----------------------------------------------------------------------------------------------//

// Link 'insmod' to the 'Gpio_initialize' function
module_init (Gpio_initialize);
// Link 'rmmod' to the 'Gpio_cleanup' function
module_exit (Gpio_cleanup);
