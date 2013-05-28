//----------------------------------------------------------------------------------------------//
//                                                                                              //
//      FILE		:	ifsttar-gpio-drv.c	 													//
//      AUTHOR		:	Jonathan AILLET <jonathan.aillet@gmail.com>								//
//      Descritpion	:	Header file of the gpio driver.											//
//						It defines the used structures, the IOCTLs, ...							//
//																								//
//----------------------------------------------------------------------------------------------//

#if !defined( IFSTTAR_GPIO_DRV_H )
#define IFSTTAR_GPIO_DRV_H

// ---- Include Files ------------------------------------------------------------------------- //

#include <linux/ioctl.h>

// Drivers errors header file
#include "../drivers_errors.h"

// Synchronization driver header file
#include "../ifsttar-synchronization-drv/ifsttar-synchronization-drv.h"

// ---- Definitions --------------------------------------------------------------------------- //

// Max multi-muxed number
#define MAX_MUX_NB						3

// Directions defines
#define INPUT							921		// Input direction
#define OUTPUT							922		// Output direction

// Modes defines
#define IO_MODE							931		// Input/Output mode
#define IT_MODE							932		// Interrupt mode

// Max debounce value
#define MAX_DEBOUNCE_VALUE				500

// This part is a copy of linux/interrupt.h
#define IRQF_TRIGGER_RISING				0x00000001											// Rising edge iD
#define IRQF_TRIGGER_FALLING			0x00000002											// Falling edge iD
#define IRQF_TRIGGER_EDGE				(IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)		// Both edge iD

// Initialize value
#define INIT_VALUE						255

// ---- Constants and Types ------------------------------------------------------------------- //

// Structure GpioAttributes
typedef struct _GpioAttributes {
	char sLabel[8];
	unsigned int bValue;						// Last known value of the gpio (0 or 1)
	unsigned int bMode; 						// Gpio mode : Input/Output mode (I/O mode) or Interrupt mode (IT mode)
	unsigned int bDirection;					// Gpio direction : Input or Output
	unsigned int bDone;							// Indicate if the gpio event has been detected
	unsigned int bIsOpened;						// Indicate if the gpio is currently used
	unsigned int uiTriggerMode;					// Indicate in which event the interruption will be triggered
	unsigned int uiMsDebounceDelay;				// Debounce delay : minimum time between two gpio events
	unsigned int uiPreviousMode[MAX_MUX_NB];	// Previous gpio register mode array
	unsigned int uiDefaultMode[MAX_MUX_NB];		// Default gpio register mode array
	unsigned int uiNbMuxed;						// Number of register which mux this gpio (number of output pin)
	unsigned int uiRegisterAdress[MAX_MUX_NB];	// 
	struct device * pDeviceEntry;
	GeoDatationData oLastItGDData;				// Structure event qui date la dernière IT sur le gpio
} GpioAttributes;

// ---- IOCTL Commands ------------------------------------------------------------------------ //

// Magic number used to generate the ioctl command iD
#define GPIO_MAGIC  'G'

// Command used to generate the ioctl command iD
#define GPIO_CMD_GET_INPUT_VALUE		0x51
#define GPIO_CMD_GET_LAST_IT_GD_DATA	0x52
#define GPIO_CMD_SET_DIRECTION			0x53
#define GPIO_CMD_SET_OUTPUT_VALUE		0x54
#define GPIO_CMD_SET_INTERRUPT			0x55
#define GPIO_CMD_SET_DEBOUNCE_DELAY		0x56
#define GPIO_CMD_FREE_INTERRUPT			0x57

// Generation of the ioctl command iD using 'linux/ioctl.h'
// Command used to get the get the gpio input value
#define GPIO_IOCTL_GET_INPUT_VALUE		_IO(GPIO_MAGIC,	GPIO_CMD_GET_INPUT_VALUE)								// arg is a integer
// Command used to get the get the last collected GeoDatationData structure matching with the last triggered gpio event
#define GPIO_IOCTL_GET_LAST_IT_GD_DATA	_IOWR(GPIO_MAGIC, GPIO_CMD_GET_LAST_IT_GD_DATA, GeoDatationData)		// arg is a GeoDatationData pointer
// Command used to get the set the gpio direction
#define GPIO_IOCTL_SET_DIRECTION		_IO(GPIO_MAGIC,	GPIO_CMD_SET_DIRECTION)									// arg is a integer
// Command used to get the set the gpio output value
#define GPIO_IOCTL_SET_OUTPUT_VALUE		_IO(GPIO_MAGIC,	GPIO_CMD_SET_OUTPUT_VALUE)								// arg is a integer
// Command used to get the set the gpio in interrupt mode
#define GPIO_IOCTL_SET_INTERRUPT		_IO(GPIO_MAGIC,	GPIO_CMD_SET_INTERRUPT)									// arg is a integer
// Command used to get the set the gpio debounce delay
#define GPIO_IOCTL_SET_DEBOUNCE_DELAY	_IO(GPIO_MAGIC,	GPIO_CMD_SET_DEBOUNCE_DELAY)							// arg is a integer
// Command used to get the set back the gpio in I/O mode
#define GPIO_IOCTL_FREE_INTERRUPT		_IO(GPIO_MAGIC,	GPIO_CMD_FREE_INTERRUPT)								// arg is a integer


// ---- Variable Externs ---------------------------------------------------------------------- //

// ---- Function Prototypes ------------------------------------------------------------------- //


#endif  // IFSTTAR_GPIO_DRV_H
