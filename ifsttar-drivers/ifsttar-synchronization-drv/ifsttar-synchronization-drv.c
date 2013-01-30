//----------------------------------------------------------------------------------------------//
//                                                                                              //
//      FILE		:	iffstar-synchronization-drv.c											//
//      AUTHOR		:	Jonathan AILLET <jonathan.aillet@gmail.com>								//
//      Description	:	Driver which allows to synchronize severals boards using a Gps module.	//
//						It also allows to date a event accurately								//
//																								//
//----------------------------------------------------------------------------------------------//

//----- Includes Files -------------------------------------------------------------------------//
// #include <asm/io.h>
// #include <asm/ioctls.h>
// #include <asm/irq.h>
// #include <asm/uaccess.h>

#include <linux/cdev.h>
// #include <linux/delay.h>
// #include <linux/device.h>
// #include <linux/errno.h>
// #include <linux/fs.h>
#include <linux/gpio.h>
// #include <linux/init.h>
#include <linux/interrupt.h>
// #include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
// #include <linux/signal.h>
#include <linux/sched.h>
// #include <linux/slab.h>
// #include <linux/string.h>
// #include <linux/time.h>
// #include <linux/types.h>
// #include <linux/version.h>

#include <plat/dmtimer.h>

//----- Includes Applications ------------------------------------------------------------------//
#include "ifsttar-synchronization-drv.h"

//----- Divers informations --------------------------------------------------------------------//

MODULE_AUTHOR ("Jonathan Aillet <jonathan.aillet@gmail.com>");
MODULE_DESCRIPTION ("IFSTTAR Synchronisation Driver");
MODULE_LICENSE ("GPL");
MODULE_VERSION ("0.1");

// ----------- Definitions ---------------------------------------------------------------------//
#define DRIVER_DEV_REGISTER_NB 	1 	// Minor number on which we will check availability, book if it's possible (using a 'register_chrdev_region'), and with which we will create the '/dev' entry (using several 'device_create')

// Names used during the driver initialization
#define DRV_NAME					"ifsttar-synchronization-drv > "			// Name used during a print form the driver (using printk). Typically, this name is the same that the one shows when a 'lsmod' is called
#define DRV_PROC_PREEMPT_ENTRY		"ifsttar-synchronization-drv-preemption-time"	// Name used to declare the link between a driver function with the driver '/proc' entry (after using a 'create_proc_read_entry').
#define DRV_SYS_NAME				"ifsttar-synchronization-drv"				// Name used during the creation of the link between the driver and its entry in '/sys/class/*' (/sys/class/"ifsttar-synchronization-drv")
#define DRV_DEV_NAME				"synchronization"			// Name used for the '/dev/*' link(s) declaration, it's also the name used to shows the driver entry in '/proc/devices'. In the case where there is several minor linked in '/dev', this name can be follow by a suffix, this links can be used to send systems calls to the driver (open, close, read, write, ioctl, ...) because these links are seen by the system as files.

//----- Constants and Types --------------------------------------------------------------------//

// Baner prints at the begin of the driver initialization
static char     sBanner[] __initdata = KERN_INFO "Synchonization Driver : v0.50 :P\n";

//----- Global Variables -----------------------------------------------------------------------//

// Global variables which contains major and the minor of the driver
unsigned int 			m_uiSynchronizationMajor = 0;
unsigned int 			m_uiSynchronizationMinor = 0;

// Global variable which contains the first node of the allocated region
dev_t					m_oSynchronizationDev;

// Global pointer which will be used to create and delete an entry in "/sys/class/"MyDriverSysName"
struct class *			m_pSynchronizationClass = NULL;

// Global variable which contains the link to the file_operations (used to link the driver's file_operations and the created node into 'm_oGpioDev')
struct cdev				m_oSynchronizationCDev;

// Global pointer which contain the structure sent back by the "device_create(...);"
struct device * 		m_pDeviceEntry = NULL;

// Global pointer which contain the structure sent back by the "create_proc_entry(...);"
struct proc_dir_entry *	m_pPreemptProcEntry = NULL;

// Global variables which contains the GPS data (temporary and valid data)
GpsData 				m_oGpsOfficialData;
GpsData 				m_oGpsTemporaryData;

// Global boolean which indicate if data has been supplied since the last Pps
unsigned int			m_bDataSupplied = 0;

// Global variable which indicate the current run mode
unsigned int			m_uiDriverRunningMode = MODE1;

// Global variable which indicate the current state of the driver
SYCHRONIZATION_DRIVER_STATES m_DriverCurrentState = STATE_Initialization;

// Global variables which contain the Pps Timer informations
unsigned int			m_uiPpsTimerNumber = 8;
unsigned int			m_uiPpsTimerRegisterAddress[TIMER_MUX_PINS_NB] = {0x4800217A, 0x480020BC, 0x480021DE};
unsigned int			m_uiPpsTimerPreviousMode[TIMER_MUX_PINS_NB] = {0x010F, 0x010F, 0x010F};
unsigned int			m_uiPpsTimerInputClock = 0;
unsigned int 			m_uiLastTimerValue = 0;
unsigned int 			m_uioldLastTimerValue = 0;
int						m_iPpsTimerErrorValueTable[QUARTZ_DRIFT_NUMBER];
int						m_uiLastUsedTimerErrorValueTableCase = 0;
int						m_iAverageTimerErrorValue = 0;
unsigned int			m_uiTimerDriftCounter = 0;
struct omap_dm_timer * 	m_pPpsTimerStruct = NULL;

// Global variables which contain the Relay Timer informations
unsigned int			m_uiRelayTimerNumber = 11;
unsigned int			m_uiRelayTimerInputClock = 0;
unsigned int			m_uiRelayTimerLeftTime = 0;
unsigned int			m_uiTimerReloadValue = 0;
struct omap_dm_timer * 	m_pRelayTimerStruct = NULL;


// Global variables which contains the Gps Software Shutdown Gpio informations
unsigned int			m_uiGpsShutdownGpioNumber = 67;
unsigned int			m_uiGpsShutdownGpioRegisterAddress = 0x480020D6;
unsigned int			m_uiGpsShutdownGpioPreviousMode = 0x010F;
unsigned int			m_uiGpsTimeToOff = 0;
unsigned int			m_bGpsStatus = 0;

// Global variables which contains the preemption time informations
int						m_iPreemptTimeValueTable[PREEMPTION_TIME_VALUE_NUMBER];
int						m_uiLastUsedPreemptTimeValueTableCase = 0;

// Global variable which indicate if the read has been detected
unsigned int			m_bReadDone = 0;
unsigned int			m_uiReadKind = 0;

// Global mutex used to perform the blocking read
static DECLARE_WAIT_QUEUE_HEAD (Read_wait);

// Thread used to check the seconds incrementation
struct task_struct * 	SecondsIncrementsVerificationThread;

// Debug
unsigned int			m_bDebugPrints = 0;

// Thread
unsigned int			m_bThreadLaunching = 1;			// Boolean used to know if we should launched the seconds verification thread (defined by 'insmod')
// N.B. : define the boolean default value to launch (or not) the thread if no parameter is set

// Prints strings
const char * sDriverMode[] = {"Mode 1", "Mode 2", "Mode 3"};
const char * sDriverState[] = {"Initialization", "Wait_Gps_Fixed", "Gps_Fixed", "Switch_to_Gps_Off", "STATE_Gps_Off"};
const char * sGpsStatus[] = {"Off", "On"};

//----- Driver parameter(s) ------------------------------------------------------------------- //

module_param (m_bDebugPrints, int, 0444);
module_param (m_bThreadLaunching, int, 0444);

//----- Functions  ---------------------------------------------------------------------------- //

/**
 * omap_dm_timer_read_reg - read timer registers in posted and non-posted mode
 * @timer:      timer pointer over which read operation to perform
 * @reg:        lowest byte holds the register offset
 *
 * The posted mode bit is encoded in reg. Note that in posted mode write
 * pending bit must be checked. Otherwise a read of a non completed write
 * will produce an error.
 */
static inline u32 omap_dm_timer_read_reg(struct omap_dm_timer *timer, u32 reg)
{
	WARN_ON((reg & 0xff) < _OMAP_TIMER_WAKEUP_EN_OFFSET);
	return __omap_dm_timer_read(timer, reg, timer->posted);
}

/**
 * omap_dm_timer_write_reg - write timer registers in posted and non-posted mode
 * @timer:      timer pointer over which write operation is to perform
 * @reg:        lowest byte holds the register offset
 * @value:      data to write into the register
 *
 * The posted mode bit is encoded in reg. Note that in posted mode the write
 * pending bit must be checked. Otherwise a write on a register which has a
 * pending write will be lost.
 */
static void omap_dm_timer_write_reg(struct omap_dm_timer *timer, u32 reg,
						u32 value)
{
	WARN_ON((reg & 0xff) < _OMAP_TIMER_WAKEUP_EN_OFFSET);
	__omap_dm_timer_write(timer, reg, value, timer->posted);
}

/**
 * omap_dm_timer_set_capture - configure the timer to be used in capture mode
 * @timer:      timer pointer over which timer to configure
 * @bEventCaptureMode:		Integer which indicate the capture running mode		(see OMAP Hardware Reference Manual)
 * 							- 0 : write in TCAR1 only
 *							- others : write in TCAR1 and TCAR 2
 * @uiEventToCapture:		Unsigned integer which indicate the event(s) to capture
 *
 * The posted mode bit is encoded in reg. Note that in posted mode write
 * pending bit must be checked. Otherwise a read of a non completed write
 * will produce an error.
 */
int omap_dm_timer_set_capture(struct omap_dm_timer * timer, int bEventCaptureMode, unsigned int uiEventToCapture)
{
	u32 l;

	if (unlikely(!timer))
		return -EINVAL;

	omap_dm_timer_enable(timer);

	l = omap_dm_timer_read_reg(timer, OMAP_TIMER_CTRL_REG);
	
	l &= ~(OMAP_TIMER_CTRL_CAPTMODE | OMAP_TIMER_CTRL_TCM_BOTHEDGES | (0x02 << 10));
	
	l |= OMAP_TIMER_CTRL_GPOCFG;
	
	if (bEventCaptureMode != 0)
		l |= OMAP_TIMER_CTRL_CAPTMODE;						// Capture in TCAR1 & TCAR2
		
	if (uiEventToCapture == IRQF_TRIGGER_RISING)			// Configure the event for a rising edge
		l |= OMAP_TIMER_CTRL_TCM_LOWTOHIGH;
	else if (uiEventToCapture == IRQF_TRIGGER_FALLING)		// Configure the event for a falling edge
		l |= OMAP_TIMER_CTRL_TCM_HIGHTOLOW;
	else													// Configure the event for both edges
		l |= OMAP_TIMER_CTRL_TCM_BOTHEDGES;
		
	omap_dm_timer_write_reg(timer, OMAP_TIMER_CTRL_REG, l);

	// Save the context
	timer->context.tclr = l;
	omap_dm_timer_disable(timer);
	
	return 0;
}

/**
 * omap_dm_timer_read_capture - read the content of the capture register
 * @timer:      timer pointer over which timer capture registers to read
 * @bEventCaptureMode:		Integer which indicate the capture running mode		(see OMAP Hardware Reference Manual)
 * 							- 0 : write in TCAR1 only
 *							- others : write in TCAR1 and TCAR 2
 * @uiEventToCapture:		Integer which indicate the event(s) to capture
 *
 * The posted mode bit is encoded in reg. Note that in posted mode write
 * pending bit must be checked. Otherwise a read of a non completed write
 * will produce an error.
 */
unsigned int omap_dm_timer_read_capture(struct omap_dm_timer * timer, int bCaptureRegister, int bReaded)
{
	u32 l;
	
	if (unlikely(!timer))
		return -EINVAL;
	
	if (!bCaptureRegister)
		l = omap_dm_timer_read_reg(timer, OMAP_TIMER_CAPTURE_REG);
	else
		l = omap_dm_timer_read_reg(timer, OMAP_TIMER_CAPTURE2_REG);
	
	if (bReaded) {
		omap_dm_timer_write_status(m_pPpsTimerStruct, OMAP_TIMER_INT_CAPTURE);
		omap_dm_timer_read_status(m_pPpsTimerStruct);
	}
	
	return l;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	SynchronizationPreemptionTimeOutput									//
//																								//
// Description			:	Function which filled a buffer which will be the page send back by	//
//							a function exported in "/proc", this buffer will contain the		//
//							preemption time informations. This function which also send back	//
//							the page lenght.													//
//																								//
// Return				:	The page length.													//
//																								//
//----------------------------------------------------------------------------------------------//
static int SynchronizationPreemptionTimeOutput (char * buf)
{
	// Variables initialization
	char * p;
	short i;
	
	p = buf;
	
	// Filed the first line of the page, this line present what is exported
	p += sprintf(p, "\nPreemption Time array : (%d values, last value on the top)\n\n", PREEMPTION_TIME_VALUE_NUMBER);
	
	// Filled the others line with the preemption time values (begining with the lastest value)
	i = m_uiLastUsedPreemptTimeValueTableCase-1;
	
	while (i >= 0) {
		p += sprintf(p, "\tValue %3d : %4d micro-seconds\n", i+1, m_iPreemptTimeValueTable[i]);
		i--;
	}
	
	i = PREEMPTION_TIME_VALUE_NUMBER-1;
	
	while (i >= m_uiLastUsedPreemptTimeValueTableCase) {
		p += sprintf(p, "\tValue %3d : %4d micro-seconds\n", i+1, m_iPreemptTimeValueTable[i]);
		i--;
	}
	
	p += sprintf(p, "\n\n");
	
	// Return the page lenght  
	return p - buf;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	SynchronizationPreemptionTimeRead									//
//																								//
// Description			:	Function exported in "/proc", the user can call it to get the		//
//							preemption time array.												//
//							Manage the returns of a read on the "/proc" entry an calls a 		//
//							function which will return the page and the page lenght.			//
//																								//
// Return				:	The return buffer length.											//
//																								//
//----------------------------------------------------------------------------------------------//
static int SynchronizationPreemptionTimeRead (char * page, char ** start, off_t off,int count, int * eof, void * data)
{
	// Variable initialization and get the page lenght
	int len = SynchronizationPreemptionTimeOutput (page);
	
	// Check if the user want to read until the end of the page
	if (len <= off + count)
		// Indicate to the user that the page end has been reached
		*eof = 1;
	
	// Send back the page starting at the offset
	*start = page + off;
	
	// Define the lenght using the offset
	len -= off;
	
	// If the user don't want to read until the end of the page
	if (len > count)
		// Set the lenght to the wanted read size
		len = count;
	
	// If the lenght is negative, that's because the user commited an error
	if (len < 0)
		// So, says that the lenght is null (equal to 0)
		len = 0;
		
	return len;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	SecondIncrements													//
//																								//
// Description			:	Increments the driver official time	of iIncrementsSecondNumber		//
//							seconds.															//
//							As the time in this driver is expresses in time in seconds since	//
//							the Epoch : 1st of january of 1970 (Linux time), the				//
//							year/month/day/hour/minute changes do not have to be managed		//
//																								//
// Return				:	Nothing																//
//																								//
//----------------------------------------------------------------------------------------------//
static void SecondIncrements (unsigned int iIncrementsSecondNumber)
{
	// Increments the driver official time of 'iIncrementsSecondNumber' seconds
	m_oGpsOfficialData.oTimeTTimeInSeconds = m_oGpsOfficialData.oTimeTTimeInSeconds + iIncrementsSecondNumber;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	ComputeAndAddQuartzError											//
//																								//
// Description			:	Compute the quartz error if its valid (if data has been supplied	//
//							by a CGPS object, and if the last two supplied data are valid),		//
//							and add it to the quartz error array.								//
//							Manage the timer drift indicator.									//
//																								//
// Return				:	Nothing																//
//																								//
//----------------------------------------------------------------------------------------------//
static void ComputeAndAddQuartzError (unsigned int uiCaptureTimerValue)
{
	// If the quartz error is valid
	if (m_bDataSupplied == 1 && (m_oGpsTemporaryData.iQuality == 1 || m_oGpsTemporaryData.iQuality == 2) && (m_oGpsOfficialData.iQuality == 1 || m_oGpsOfficialData.iQuality == 2)) {
		// If the array is full
		if (m_uiLastUsedTimerErrorValueTableCase >= QUARTZ_DRIFT_NUMBER)
			// Manage the array to be used as circular buffer
			m_uiLastUsedTimerErrorValueTableCase = 0;
		
		// Manage the case where the timer has overflow
		if (uiCaptureTimerValue < m_uiLastTimerValue)
			// Compute and add the quartz error in the quartz error array
			m_iPpsTimerErrorValueTable[m_uiLastUsedTimerErrorValueTableCase++] = 0xFFFFFFFF - m_uiLastTimerValue + uiCaptureTimerValue - m_uiPpsTimerInputClock;	// 0xFFFFFFFF = 4294967295
		else
			// Compute and add the quartz error in the quartz error array
			m_iPpsTimerErrorValueTable[m_uiLastUsedTimerErrorValueTableCase++] = uiCaptureTimerValue - m_uiLastTimerValue - m_uiPpsTimerInputClock;
		
		// Manage Timer drift indicator
		m_uiTimerDriftCounter++;
	}
	else if (m_uiTimerDriftCounter > 0)
		// Manage Timer drift indicator
		m_uiTimerDriftCounter--;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	ComputeMicroSecond													//
//																								//
// Description			:	Compute the microsecond using the 'bSelectedTimer' paramater.		//
//																								//
// Return				:	The computed microsecond if everything happened well,				//
//							a negative number otherwise.										//
//																								//
//----------------------------------------------------------------------------------------------//
static unsigned int ComputeMicroSecond (unsigned int bSelectedTimer)
{
	// Variables initialization
	static unsigned int uiTimerValue, uiCurrentMicroSeconds;
	
	// Depending on the selected timer
	switch (bSelectedTimer) {
		// If its the capture timer
		case CAPTURE_TIMER_ID :
			// Compute the microsecond using the timer send value
			uiTimerValue = omap_dm_timer_read_counter(m_pPpsTimerStruct) - m_uiLastTimerValue;
			uiCurrentMicroSeconds = uiTimerValue/(m_uiPpsTimerInputClock/1000000);				// Frequency in MHz
			break;
		
		// If its the relay timer
		case RELAY_TIMER_ID :
			// Compute the microsecond using the timer send value
			uiTimerValue = omap_dm_timer_read_counter(m_pRelayTimerStruct);
			uiCurrentMicroSeconds = (uiTimerValue - m_uiTimerReloadValue)/(m_uiRelayTimerInputClock/1000000);
			break;
		
		default :
			printk (KERN_WARNING "%s%s : Wrong timer, computation not possible\n", DRV_NAME, __func__);
			return -ERR_SYNCHRONIZATION_COMPUTEMICROSECOND_SPECIFIED_TIMER;
	}
	
	if (m_bDebugPrints)
		printk(KERN_DEBUG "%s%s : Export used, driver mode %d, timer delta : %d, computed microsecond : %d\n", DRV_NAME, __func__, m_uiDriverRunningMode - 110, uiTimerValue, uiCurrentMicroSeconds);
	
	// Return the computed microsecond
	return uiCurrentMicroSeconds;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	GetAverageQuartzError												//
//																								//
// Description			:	This function compute the average quartz error of the capture		//
//							timer using the m_iPpsTimerErrorValueTable array					//
//																								//
// Return				:	The average average quartz error of the capture	timer				//
//																								//
//----------------------------------------------------------------------------------------------//
static int GetAverageQuartzError (void)
{
	// Variables initialization
	static unsigned int iCpt, iValue;
	
	iValue = 0;
	
	// Add of all values of the array in a unisgned integer
	for (iCpt = 0; iCpt < QUARTZ_DRIFT_NUMBER; iCpt++)
		iValue += m_iPpsTimerErrorValueTable[iCpt];
	
	// Divide this value by the array size ti get the average preemption time of the GpioIrqHandler function
	iValue /= QUARTZ_DRIFT_NUMBER;
	
	// Return this computed average quartz error
	return iValue;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	SetGpsStatus														//
//																								//
// Description			:	Set the gps status and the status indicator to 	the value specified	//
//							in the 'bGpsDriverStatus' parameter									//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int SetGpsStatus (unsigned int bGpsDriverStatus)
{
	// Depending on the status we want to set to the Gps module
	switch (bGpsDriverStatus) {
		// If we want to switch it Off
		case 0 :
			// Set the Gps shutdown Gpio number
			//gpio_set_value(m_uiGpsShutdownGpioNumber, 0);
			// Set the Gps status indicator boolean
			m_bGpsStatus = 0;
			break;
		
		// If we want to switch it On
		case 1 :
			// Set the Gps shutdown Gpio number
			gpio_set_value(m_uiGpsShutdownGpioNumber, 1);
			// Set the Gps status indicator boolean
			m_bGpsStatus = 1;
			break;
		
		default :
			return -ERR_SYNCHRONIZATION_SETGPSSTATUS_SPECIFIED_STATUS;
	}
	
	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	SecondsIncrementsVerificationFunction								//
//																								//
// Description			:	Thread which check every 0,8 seonds if the seconds incrementation	//
//							happened well.														//
//																								//
// Return				:	0																	//
//																								//
//----------------------------------------------------------------------------------------------//
static int SecondsIncrementsVerificationFunction (void * arg)
{
	// Variables initialization
	unsigned int uiPreviousSecond = 0, uiCurrentSecond = 0;
	
	printk(KERN_INFO "%s%s : The thread which check the seconds incrementation is launched\n", DRV_NAME, __func__);
	
	// While the driver haven't received data form a CGPS object or while we haven't said to the thread to stop
	while(!m_bDataSupplied && !kthread_should_stop())
		// Sleep
		msleep(50);
	
	// If the driver haven't received data form a CGPS object
	if (!kthread_should_stop())
		printk(KERN_INFO "%s%s : data has been supplied, the thread can do its work\n", DRV_NAME, __func__);
	
	// While we haven't said to the thread to stop
	while (!kthread_should_stop()) {
		// Sleep
		msleep(800);
		
		// Get the current second
		uiCurrentSecond = m_oGpsOfficialData.oTimeTTimeInSeconds;
		
		// If the second have been decremented or if there have been incremented by more than one
		if ((uiCurrentSecond < uiPreviousSecond) && (uiCurrentSecond - uiPreviousSecond < 1))
			// Print that an error occured
			printk(KERN_WARNING "%s%s : Synchronization error :\n\tPrevious second : %d\n\tCurrent second : %d\n", DRV_NAME, __func__, uiPreviousSecond, uiCurrentSecond);
		
		// Save the current second to the previous second
		uiPreviousSecond = uiCurrentSecond;
	}
	
	printk(KERN_INFO "%s%s : The thread which check the seconds incrementation has been stopped\n", DRV_NAME, __func__);
	
	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	RelayTimerIrqHandler												//
//																								//
// Description			:	This function is called when a relay timer overflow is detected.	//
//							It manages the driver state machine state, and the Gps restart when	//
//							the relay timer is launch (typically when the Gps is set to Off).	//
//							It also unlock the blocking read when an overflow happened			//
//							(an overflow simulate that a Pps signal has been detected).			//
//																								//
// Return				:	Irq handled															//
//																								//
//----------------------------------------------------------------------------------------------//
static irqreturn_t RelayTimerIrqHandler (int irq, void * dev_id)
{
	static unsigned int uiPreemptTimerValue, uiPreemptMicroSeconds;
	unsigned int PPS_timer_val, PPS_Capture_val;
	unsigned int RelayTimer_val, RelayTimerPreemptVal, PPSTimer_val_when_Relay_Trig, ReloadValue;
	unsigned int uidiffppsMicroSeconds, tmp, uidiffpps;
	static int i;
	
	// Get the timer raw preemption data
	uiPreemptTimerValue = omap_dm_timer_read_counter(m_pRelayTimerStruct);

	// Get pps timer
	PPS_timer_val = omap_dm_timer_read_counter(m_pPpsTimerStruct);
	
	// Get the timer capture value
	PPS_Capture_val = omap_dm_timer_read_capture(m_pPpsTimerStruct, 0, 0);
	
	// Clear the interrupt flag
	omap_dm_timer_write_status(m_pRelayTimerStruct, OMAP_TIMER_INT_OVERFLOW);
	omap_dm_timer_read_status(m_pRelayTimerStruct);
	
	if (m_bDebugPrints)
		printk (KERN_DEBUG "%s%s : Pps simulated, state -> %s\n", DRV_NAME, __func__, sDriverState[m_DriverCurrentState]);
	
	// If the Gps is currently Off
	if (m_DriverCurrentState == STATE_Gps_Off) {
		// Increments the official time
		SecondIncrements (1);
		// Decrements the left time before restarting the Gps module
		m_uiRelayTimerLeftTime--;
		
		// If the Gps module has to keep being Off
		if (m_uiRelayTimerLeftTime > 0)
			// Remain in the same state
			m_DriverCurrentState = STATE_Gps_Off;
		else
			// Turn on the Gps module
			SetGpsStatus (1);
		
		// Indicate to the read that a simulated Pps has been received
		m_uiReadKind = 2;
		// Unlock the blocking read
		m_bReadDone  = 1;
		wake_up_interruptible (&Read_wait);
	}
	else
		printk(KERN_WARNING "%s%s : Error, the timer relay is still running besides the Gps is not Off\n", DRV_NAME, __func__);

	// Compute the diff time between physical pps en simulated pps (only if last pps IT is done)
	RelayTimer_val = uiPreemptTimerValue;
	ReloadValue = 0xFFFFFFFF - m_uiTimerReloadValue;
	RelayTimerPreemptVal = (RelayTimer_val - m_uiTimerReloadValue);
	PPSTimer_val_when_Relay_Trig = (PPS_timer_val - RelayTimerPreemptVal);
	if(PPSTimer_val_when_Relay_Trig < PPS_Capture_val && (PPS_Capture_val - PPSTimer_val_when_Relay_Trig) < (ReloadValue/2)) {
		// Relay timer is early
		//printk(KERN_INFO "%s%s : Relay timer is early\n", DRV_NAME, __func__);
		uidiffppsMicroSeconds = (PPS_Capture_val - PPSTimer_val_when_Relay_Trig)/(m_uiRelayTimerInputClock/1000000);
		//printk(KERN_INFO "%s%s : %dus - Diff Time Between last real pps and simulated pps\n", DRV_NAME, __func__, uidiffppsMicroSeconds);
	}
	if(PPSTimer_val_when_Relay_Trig > PPS_Capture_val && (PPSTimer_val_when_Relay_Trig - PPS_Capture_val) < (ReloadValue/2)) {
		// Relay Timer is late
		//printk(KERN_INFO "%s%s : Relay timer is late\n", DRV_NAME, __func__);
		uidiffppsMicroSeconds = (PPSTimer_val_when_Relay_Trig - PPS_Capture_val)/(m_uiRelayTimerInputClock/1000000);
		//printk(KERN_INFO "%s%s : %dus - Diff Time Between last real pps and simulated pps\n", DRV_NAME, __func__, uidiffppsMicroSeconds);

	}

	if(i == 10) {
		uidiffpps = (PPSTimer_val_when_Relay_Trig - PPS_Capture_val) + (m_uiRelayTimerInputClock/1000000)*3; // diif time + 3us
		tmp = omap_dm_timer_read_counter(m_pRelayTimerStruct);
		omap_dm_timer_write_counter(m_pRelayTimerStruct, tmp + uidiffpps);
	}

	// Indicate that no data has been received since the last (simulated) Pps
	m_bDataSupplied = 0;
	//omap_dm_timer_write_counter(m_pPpsTimerStruct, 4000000);
	
	// Compute the preemption time
	uiPreemptMicroSeconds = (uiPreemptTimerValue - m_uiTimerReloadValue)/(m_uiRelayTimerInputClock/1000000);
	
	// Handle the preemption time circular buffer
	if (m_uiLastUsedPreemptTimeValueTableCase >= PREEMPTION_TIME_VALUE_NUMBER)
		// Return to the start of the circular buffer
		m_uiLastUsedPreemptTimeValueTableCase = 0;
	
	// Save the preemption time
	m_iPreemptTimeValueTable[m_uiLastUsedPreemptTimeValueTableCase++] = uiPreemptMicroSeconds;
	
	i++;

	return IRQ_HANDLED;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	PpsTimerIrqHandler													//
//																								//
// Description			:	This function is called when the intended event has been detected	//
//							on the capture output pin.											//
//							It manages the driver state machine state, the Gps shutdown when	//
//							conditions requires it, the congiuration and the launch of the		//
// 							relay timer, and the compute of the average quartz error.			//
//							It also unlock the blocking read when an the event happened.		//
//																								//
// Return				:	Irq handled															//
//																								//
//----------------------------------------------------------------------------------------------//
static irqreturn_t PpsTimerIrqHandler (int irq, void * dev_id)
{
	// Variables initialization
	static unsigned int uiCaptureTimerValue, uiPreemptTimerValue, uiPreemptMicroSeconds;
	unsigned int RelayTimer_val, uiPpsTimerValue;

	// Get the PPS timer raw preemption data
	uiPreemptTimerValue = omap_dm_timer_read_counter(m_pPpsTimerStruct);

	// Get the Relay timer raw preemption data
	//RelayTimer_val = omap_dm_timer_read_counter(m_pRelayTimerStruct);
	
	// Get the timer capture value
	uiCaptureTimerValue = omap_dm_timer_read_capture(m_pPpsTimerStruct, 0, 1);
	
	if (m_bDebugPrints)
		printk(KERN_DEBUG "%s%s : Pps received, state -> %s\n", DRV_NAME, __func__, sDriverState[m_DriverCurrentState]);
	
	// Depending on the state machine state
	switch (m_DriverCurrentState) {
		// If it's the 'Initailization' state
		case STATE_Initialization :
			// Turn on the Gps module (again)
			SetGpsStatus (1);
			
			// Initialize the Gps Off time
			m_uiRelayTimerLeftTime = m_uiGpsTimeToOff;
			
			// Reset the used variables
			m_uiTimerDriftCounter = 0;
			m_uiLastUsedTimerErrorValueTableCase = 0;
			
			// Reset the Gps Data structures
			memset(&m_oGpsOfficialData, 0, sizeof(GpsData));
			memset(&m_oGpsTemporaryData, 0, sizeof(GpsData));
			
			// Reset the array(s) averaging
			memset(m_iPpsTimerErrorValueTable, 0, QUARTZ_DRIFT_NUMBER*sizeof(int));
			//memset(m_iGpioPreemptTimeValueTable, 0, QUARTZ_DRIFT_NUMBER*sizeof(int));
			
			// Initialize the Gps official data
			m_oGpsOfficialData.oTimeTTimeInSeconds = 1340982288;
			m_oGpsOfficialData.dLatitude = 4709.37471;
			m_oGpsOfficialData.dLongitude = 00138.29279;
			m_oGpsOfficialData.cLatitudeRef = 'N';
			m_oGpsOfficialData.cLongitudeRef = 'W';
			m_oGpsOfficialData.iQuality = 0;
			m_oGpsOfficialData.uiNbSat = 0;
			
			// Change the state to 'Wait for Gps Fixed'
			m_DriverCurrentState = STATE_Wait_for_Gps_Fixed;
			printk(KERN_INFO "%s%s : State machine (re-)launched. The state has changed for : STATE_Wait_for_Gps_Fixed\n", DRV_NAME, __func__);
			break;
		
		// If it's the 'Wait for Gps fixed' state
		case STATE_Wait_for_Gps_Fixed :
			// If data have been supplied by a CGPS class
			if (m_bDataSupplied == 1)
				// Upload in official the received temporary GPS data
				m_oGpsOfficialData = m_oGpsTemporaryData;
			
			// Compute the quartz error and add it to the array
			ComputeAndAddQuartzError (uiCaptureTimerValue);
			
			// If the timer drift indicator says taht we received enough of valid data
			if (m_uiTimerDriftCounter >= QUARTZ_DRIFT_NUMBER) {
				// Change the state to 'Gps Fixed'
				m_DriverCurrentState = STATE_Gps_Fixed;
				if (m_bDebugPrints)
					printk(KERN_DEBUG "%s%s : The state has changed for : STATE_Gps_Fixed\n", DRV_NAME, __func__);
			}
			else
				// Remain in the same state
				m_DriverCurrentState = STATE_Wait_for_Gps_Fixed;
			break;
		
		// If it's the 'Gps fixed' state
		case STATE_Gps_Fixed :
			// If data have been supplied by a CGPS class
			if (m_bDataSupplied == 1)
				// Upload in official temporary GPS data
				m_oGpsOfficialData = m_oGpsTemporaryData;
			
			// Compute the quartz error and add it to the array
			ComputeAndAddQuartzError (uiCaptureTimerValue);
			
			// Compute the average quartz error
			m_iAverageTimerErrorValue = ((GetAverageQuartzError () * 1000000) / m_uiPpsTimerInputClock);

			// If we haven't received enough of valid data
			if (m_uiTimerDriftCounter < QUARTZ_DRIFT_NUMBER)
				// Change the state to 'Wait for Gps Fixed'
				m_DriverCurrentState = STATE_Wait_for_Gps_Fixed;
			else {
				// If the driver running mode is in MODE3 and if the two timers have the same clock
				if (m_uiDriverRunningMode == MODE3 && m_uiPpsTimerInputClock == m_uiRelayTimerInputClock) {
					// Timer relay configuration (using the load/reload value)
					//m_uiTimerReloadValue = 0xFFFFFFFF - (m_uiRelayTimerInputClock + (m_iAverageTimerErrorValue * (m_uiRelayTimerInputClock / 1000000)));
					omap_dm_timer_set_pwm(m_pRelayTimerStruct, 1, 1, OMAP_TIMER_TRIGGER_OVERFLOW);
					uiPpsTimerValue = omap_dm_timer_read_counter(m_pPpsTimerStruct);
					m_uiTimerReloadValue = 0xFFFFFFFF - ((uiCaptureTimerValue - m_uiLastTimerValue)-1);
					//printk(KERN_INFO "%s%s : Delta Timer PPS : %d fclk\n", DRV_NAME, __func__, uiCaptureTimerValue - m_uiLastTimerValue);
					//omap_dm_timer_set_load(m_pRelayTimerStruct, 1, m_uiTimerReloadValue);
					omap_dm_timer_set_load_start(m_pRelayTimerStruct, 1, m_uiTimerReloadValue);
					//omap_dm_timer_start(m_pRelayTimerStruct);
					m_uiRelayTimerLeftTime = m_uiGpsTimeToOff;
					
					// Change the state to 'Switch Gps to Off'
					m_DriverCurrentState = STATE_Switch_Gps_to_Off;

					printk(KERN_INFO "%s%s : Delta Timer PPS : %d fclk\n", DRV_NAME, __func__, uiCaptureTimerValue - m_uiLastTimerValue);
					
					if (m_bDebugPrints) {
						printk(KERN_DEBUG "%s%s : The state has changed for : STATE_Switch_Gps_to_Off\n", DRV_NAME, __func__);
						printk(KERN_DEBUG "%s%s : The timer relay has been configured with a reload value of %u -> 0x%08x, or %d impulsions to overflow\n", DRV_NAME, __func__, m_uiTimerReloadValue, m_uiTimerReloadValue, 0xFFFFFFFF - m_uiTimerReloadValue);
					}
				}
				else
					// Remain in the same state
					m_DriverCurrentState = STATE_Gps_Fixed;
			}
			break;
		
		// If it's the 'Switch Gps to Off' state
		case STATE_Switch_Gps_to_Off :
			// Relay timer start
			//m_uiTimerReloadValue = 0xFFFFFFFF - (uiCaptureTimerValue - m_uiLastTimerValue);
			//omap_dm_timer_set_load(m_pRelayTimerStruct, 1, m_uiTimerReloadValue);
			//omap_dm_timer_set_load(m_pRelayTimerStruct, 1, 0xFFFFFFFF - (uiCaptureTimerValue - m_uiLastTimerValue)/4);
			//omap_dm_timer_set_load(m_pRelayTimerStruct, 1, m_uiTimerReloadValue);
			//omap_dm_timer_write_counter(m_pPpsTimerStruct, 4000000);
			
			//omap_dm_timer_start(m_pRelayTimerStruct);
			//m_uiTimerReloadValue = 0xFFFFFFFF - (uiCaptureTimerValue - m_uiLastTimerValue);
			//omap_dm_timer_set_load(m_pRelayTimerStruct, 1, m_uiTimerReloadValue);
			//omap_dm_timer_set_load(m_pRelayTimerStruct, 1, m_uiTimerReloadValue);
			//omap_dm_timer_write_counter(m_pPpsTimerStruct, 0xFFFFFFFF - (uiCaptureTimerValue - m_uiLastTimerValue)/4);
			
			// Shutdown the Gps module
			SetGpsStatus (0);
			
			// Increments the official time
			SecondIncrements (1);
			
			// Indicate that no data has been received since the last (real) Pps
			m_bDataSupplied = 0;
			
			// Indicate to the read that a real Pps has been received
			m_uiReadKind = 1;
			// Unlock the blocking read
			m_bReadDone  = 1;
			wake_up_interruptible (&Read_wait);
			
			// Change the state to 'Gps Off'
			m_DriverCurrentState = STATE_Gps_Off;
			
			if (m_bDebugPrints)
				printk(KERN_DEBUG "%s%s : The state has changed for : STATE_Gps_Off\n", DRV_NAME, __func__);
			break;
		
		// If it's the 'Gps Off' state
		case STATE_Gps_Off :
			// Compute the diff time between physical pps en simulated pps (only if last pps IT is done)
			/*ReloadValue = 0xFFFFFFFF - m_uiTimerReloadValue;
			PPS_timer_val = uiPreemptTimerValue;
			PPS_Capture_val = uiCaptureTimerValue;
			if((PPS_timer_val - PPS_Capture_val) < (ReloadValue/2)) {
				// Relay timer is early
				printk(KERN_INFO "%s%s : Relay timer is early\n", DRV_NAME, __func__);
				RelayTimerPreemptVal = (RelayTimer_val - m_uiTimerReloadValue);
				PPSTimer_val_when_Relay_Trig = (PPS_timer_val - RelayTimerPreemptVal);
			} else {
				// Relay Timer is late
				printk(KERN_INFO "%s%s : Relay timer is late\n", DRV_NAME, __func__);
				if(PPS_Capture_val == m_uiLastTimerValue) {
					//pps it is done
					RelayTimerPreemptVal = (RelayTimer_val - m_uiTimerReloadValue);
					PPSTimer_val_when_Relay_Trig = (PPS_timer_val - RelayTimerPreemptVal);
					if(PPS_Capture_val < PPSTimer_val_when_Relay_Trig) {
						uidiffppsMicroSeconds = (PPSTimer_val_when_Relay_Trig - PPS_Capture_val)/(m_uiRelayTimerInputClock/1000000);
					} else {
						uidiffppsMicroSeconds = (PPS_Capture_val - PPSTimer_val_when_Relay_Trig)/(m_uiRelayTimerInputClock/1000000);
					}
					printk(KERN_INFO "%s%s : %dms - Diff Time Between last real pps and simulated pps\n", DRV_NAME, __func__, uidiffppsMicroSeconds);
				}
			}*/
			// If two valid data were supplied in a row
			/*if (m_bDataSupplied == 1 && (m_oGpsTemporaryData.iQuality == 1 || m_oGpsTemporaryData.iQuality == 2)) {
				// Relay timer stop
				omap_dm_timer_stop(m_pRelayTimerStruct);
								
				// Reset the timer drift indicator
				m_uiTimerDriftCounter = 0;
				
				// Reset the array(s) averaging
				memset(m_iPpsTimerErrorValueTable, 0, QUARTZ_DRIFT_NUMBER*sizeof(int));
				//memset(m_iGpioPreemptTimeValueTable, 0, QUARTZ_DRIFT_NUMBER*sizeof(int));
							
				// Change the state to 'Wait for Gps Fixed'
				m_DriverCurrentState = STATE_Wait_for_Gps_Fixed;
				
				if (m_bDebugPrints)
					printk(KERN_DEBUG "%s%s : The state has changed for : STATE_Wait_for_Gps_Fixed\n", DRV_NAME, __func__);
			}
			else*/
				// Remain in the same state
				m_DriverCurrentState = STATE_Gps_Off;
			break;
		
		// If it's not a known state
		default :
			printk(KERN_WARNING "%s%s : Error, state of the state machine is wrong\n", DRV_NAME, __func__);
			
			// Change the state to 'Initialization'
			m_DriverCurrentState = STATE_Initialization;
			break;
	}
	
	// Copy the capture timer value to the last timer value
	m_uiLastTimerValue = uiCaptureTimerValue;
	m_uioldLastTimerValue = m_uiLastTimerValue;		
	
	// If the state is not 'Gps_Off'
	if (1) {//if (m_DriverCurrentState != STATE_Gps_Off) {
		// Indicate that no data has been received since the last (real) Pps
		m_bDataSupplied = 0;
		
		// Indicate to the read that a real Pps has been received
		m_uiReadKind = 1;
		// Unlock the blocking read
		m_bReadDone  = 1;
		wake_up_interruptible (&Read_wait);
	}
	
	// Compute the preemption time
	uiPreemptMicroSeconds = (uiPreemptTimerValue - m_uiLastTimerValue)/(m_uiPpsTimerInputClock/1000000);
	
	// Handle the preemption time circular buffer
	if (m_uiLastUsedPreemptTimeValueTableCase >= PREEMPTION_TIME_VALUE_NUMBER)
		// Return to the start of the circular buffer
		m_uiLastUsedPreemptTimeValueTableCase = 0;
	
	// Save the preemption time
	m_iPreemptTimeValueTable[m_uiLastUsedPreemptTimeValueTableCase++] = uiPreemptMicroSeconds;
		
	return IRQ_HANDLED;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER  							//
//																								//
// Description			:	Exported funtion usable by others drivers.							//
// 							This function alloxed to export a GeoDatationData strucure which is	//
//							updated when the function is called.								//
//																								//
// Return				:	The GeoDatationData strucure updated when the function is called.	//
//																								//
//----------------------------------------------------------------------------------------------//
static GeoDatationData EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER (void)
{
	// Variables initialization
	static int uiCurrentMicroSeconds;
	static GeoDatationData oActualGDData;
	
	// Depending on the driver running mode
	switch (m_uiDriverRunningMode) {
		// If it's the MODE1
		case MODE1 :
			// Get the microsecond using 'ComputeMicroSecond' function by specifing that we must get them from the capture timer
			uiCurrentMicroSeconds = ComputeMicroSecond (CAPTURE_TIMER_ID);
			break;
		
		// If it's the MODE2
		case MODE2 :
			// Get the microsecond using 'ComputeMicroSecond' function by specifing that we must get them from the capture timer
			uiCurrentMicroSeconds = ComputeMicroSecond (CAPTURE_TIMER_ID);
			break;
		
		// If it's the MODE3
		case MODE3 :
			// If the state machine state is 'Gps Off'
			if (m_DriverCurrentState == STATE_Gps_Off)
				// Get the microsecond using 'ComputeMicroSecond' function by specifing that we must get them from the relay timer
				uiCurrentMicroSeconds = ComputeMicroSecond (RELAY_TIMER_ID);
			else
				// Get the microsecond using 'ComputeMicroSecond' function by specifing that we must get them from the capture timer
				uiCurrentMicroSeconds = ComputeMicroSecond (CAPTURE_TIMER_ID);
			break;
		
		// If it's an unknown mode
		default :
			printk(KERN_WARNING "%s%s : Wrong driver mode, exportation values won't be correct\n", DRV_NAME, __func__);
			
			// Set the microsecond to 0
			uiCurrentMicroSeconds = 0;
			break;
	}
	
	// Reset the GeoDatationData which will be send to the user
	memset (&oActualGDData, 0, sizeof(GeoDatationData));
	
	// Copy the current Gps official data in it
	memcpy (&oActualGDData.oData, &m_oGpsOfficialData, sizeof(GpsData));
	
	// Add the microsecond
	oActualGDData.uiMicroSecond = uiCurrentMicroSeconds;
	// Add the average timer error value
	oActualGDData.iQuartzError = m_iAverageTimerErrorValue;
	// Add the driver running mode
	oActualGDData.iDriverMode = m_uiDriverRunningMode;
	
	return oActualGDData;
}
EXPORT_SYMBOL(EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER);

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Synchronization_open												//
//																								//
// Description			:	Function called when an open is perfommed on the					//
//							'/dev/Synchronization' entry.								  		//
//                       																		//
// Return				:   0            														//
//																								//
//----------------------------------------------------------------------------------------------//
static int Synchronization_open(struct inode *inode, struct file *filp)
{
	if (m_bDebugPrints)
		printk(KERN_DEBUG "%s%s : A new openned has been seen in the synchronization driver\n", DRV_NAME, __func__);

	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Synchronization_close												//
//																								//
// Description			:	Function called when a close is perfommed on the					//
//							'/dev/synchronization' entry.								  		//
//                       																		//
// Return				:   0            														//
//																								//
//----------------------------------------------------------------------------------------------//
static int Synchronization_close(struct inode *inode, struct file *filp)
{
	if (m_bDebugPrints)
		printk(KERN_DEBUG "%s%s : A new closed has been seen in the synchronization driver\n", DRV_NAME, __func__);

	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Synchronization_read												//
//																								//
// Description			:	Function called when a read is performed on the						//
//							'/dev/synchronization' entry.										//
//							Here, there are a blocking read which is unlock when a Pps has been //
//							detected and send back the reason of why the read has been 			//
//							unlocked : real or simulated Pps.									//
//																								//
// Return				:	The bytes number readed fi everything went fine, a negative 		//
//							number otherwise.													//
//																								//
//----------------------------------------------------------------------------------------------//
static ssize_t Synchronization_read(struct file *filp, char *buf, size_t size, loff_t * offp)
{
	// Variables initialization
	static char sPpsDetected[14] = "Pps detected\0";
	static char sPpsSimulated[15] = "Pps simulated\0";
	static int iCopiedBytes, iRet;
	
	// If the read hasn't been unlocked
	if (m_bReadDone == 0) {
		// Wait for a read unlock
		interruptible_sleep_on (&Read_wait);
		if (signal_pending(current))
			return -EINTR;
	}
	
	// If the read has been unlocked
	if (m_bReadDone == 1)
		// Say it's no longer unlocked
		m_bReadDone = 0;
	
	if (m_bDebugPrints)
		printk(KERN_DEBUG "%s%s : Waiting read operation of %d bytes in the synchronization driver\n", DRV_NAME, __func__,  (int) size);
	
	// If a real Pps has been detected
	if (m_uiReadKind == 1) {
		// Copy to the user the corresponding string
		iRet = copy_to_user(buf, sPpsDetected, strlen(sPpsDetected)+1);
		// And the corresponding number of read bytes
		iCopiedBytes = strlen(sPpsDetected)+1;
	}
	// If a simulated Pps has been detected
	else if (m_uiReadKind == 2) {
		// Copy to the user the corresponding string
		iRet = copy_to_user(buf, sPpsSimulated, strlen(sPpsSimulated)+1);
		// And the corresponding number of read bytes
		iCopiedBytes = strlen(sPpsSimulated)+1;
	}
	else
		return 0;
	
	// Reset the read performed kind
	m_uiReadKind = 0;
	
	return iCopiedBytes;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Synchronization_write												//
//																								//
// Description			:	Function called when a write is perfommed on the					//
//							'/dev/synchronization' entry.								  		//
//                       																		//
// Return				:   0            														//
//																								//
//----------------------------------------------------------------------------------------------//
static ssize_t Synchronization_write(struct file *filp, const char *buf, size_t size, loff_t * offp)
{
	if (m_bDebugPrints)
		printk(KERN_DEBUG "%s%s : A write has been seen in the synchronization driver, but it's impossible to write this string : %s\n", DRV_NAME, __func__, buf);
	
	return 0;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Synchronization_poll												//
//																								//
// Description			:	Function called when a poll is perfommed on the						//
//							'/dev/synchronization' entry.								  		//
//							Here, the poll can be unlocked when the read has been unlocked, or 	//
//							when the time specified in its called is reached.					//
//																								//
// Return				:	The mask which indicate with the poll has been unlocked.			//
//																								//
//----------------------------------------------------------------------------------------------//
static unsigned int Synchronization_poll (struct file * filp, struct poll_table_struct * wait)
{
	// Variable initialization
	static unsigned int mask;
	
	// Reset the mask
	mask = 0;
	
	// Wait for the read to be unlock or for the timeout
	poll_wait (filp, &Read_wait, wait);
	// If a read has been unlocked
	if (m_bReadDone == 1)
		// Specify it in the mask
		mask = POLLIN | POLLRDNORM;
	
	// Send back the mask
	return mask;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Synchronization_ioctl              									//
//																								//
// Description			:	Function called when an ioctl is perfommed on the					//
//							'/dev/synchronization' entry.								  		//
//							See the IOCTL commands section on the header.						//
//																								//
// Return				:   A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
long Synchronization_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	// Variables initialization
	static int iRet = 0;
	static GeoDatationData oActualGDData;
	
	// Depending on the IOCLT send command
	switch(cmd)
	{
		// If it's the Get official GeoDatationData command
		case SYNC_IOCTL_GET_OFFICIAL_GD_DATA :
			// Get the current GeoDatationData
			oActualGDData = EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER ();
			
			// Copy this data to the user
			iRet = copy_to_user((GeoDatationData *) arg, &oActualGDData, sizeof(GeoDatationData));
			// If the an error occured during the copy
			if (iRet !=0) {
				printk(KERN_WARNING "%s%s : Error %d during the copy of the GD Data to the user\n", DRV_NAME, __func__, iRet);
				// Send back an error code
				return -ERR_SYNCHRONIZATION_IOCTL_COPY_OFFICIAL_GDDATA_TO_USER;
			}
			
			break;
		
		// If it's the Get driver mode command
		case SYNC_IOCTL_GET_DRIVER_MODE :
			// Copy the driver running mode to the user
			*((unsigned int *)arg) = m_uiDriverRunningMode;
			
			break;
		
		// If it's the Get driver state command
		case SYNC_IOCTL_GET_DRIVER_STATE :
			// Copy the driver running state to the user
			*((unsigned int *)arg) = m_DriverCurrentState;
			
			break;
		
		// If it's the Get Gps staus command
		case SYNC_IOCTL_GET_GPS_STATUS :
			// Copy the Gps module status to the user
			*((unsigned int *)arg) = m_bGpsStatus;
			
			break;
		
		// If it's the time since the Gps is Off command
		case SYNC_IOCTL_GET_TIME_SINCE_GPS_OFF :
			// If the Gps is Off
			if (!m_bGpsStatus)
				// Copy the time since the Gps is Off to the user
				*((unsigned int *)arg) = m_uiGpsTimeToOff - m_uiRelayTimerLeftTime;
			else
				// Copy 0 to the user
				*((unsigned int *)arg) = 0;
				
			break;
		
		// If it's the Get Gps Off Time command
		case SYNC_IOCTL_GET_GPS_OFF_TIME :
			// Copy the Gps off time to the user
			*((unsigned int *)arg) = m_uiGpsTimeToOff / 60;
			
			break;
		
		// If it's the Get temperature command
		case SYNC_IOCTL_GET_TEMPERATURE :
			// TODO : compute temperature
			// Copy the temperature to the user
			*((unsigned int *)arg) = 32;
			
			break;
		
		// If it's the capture timer clock command
		case SYNC_IOCTL_GET_CAPTURE_TIMER_CLOCK :
			// Copy the capture timer clock to the user
			*((unsigned int *)arg) = m_uiPpsTimerInputClock;
			
			break;
		
		// If it's the relay timer clock command
		case SYNC_IOCTL_GET_RELAY_TIMER_CLOCK :
			// Copy the relay timer clock to the user
			*((unsigned int *)arg) = m_uiRelayTimerInputClock;
			
			break;
		
		// If it's the Set temporary GeoDatationData command
		case SYNC_IOCTL_SET_TEMPORARY_GPS_DATA :
			// Copy the new temporary data from the user
			iRet = copy_from_user(&m_oGpsTemporaryData, (GpsData *) arg, sizeof(GpsData));
			// If the an error occured during the copy
			if (iRet !=0) {
				printk(KERN_WARNING "%s%s : Error %d during the copy of the Gps data to the driver\n", DRV_NAME, __func__, iRet);
				// Send back an error code
				return -ERR_SYNCHRONIZATION_IOCTL_COPY_TEMPORARY_GPSDATA_FROM_USER;
			}
			
			// Says that GeoDatationData has been provided to the driver
			m_bDataSupplied = 1;
			
			if (m_bDebugPrints)
				printk(KERN_DEBUG "%s%s : Gps data supplied\n", DRV_NAME, __func__);
			break;
		
		// If it's the Set driver mode command
		case SYNC_IOCTL_SET_DRIVER_MODE :
			// If the passed mode is correct
			if ((unsigned int) arg < MODE1 && (unsigned int) arg > MODE3)
				return -ERR_SYNCHRONIZATION_IOCTL_SPECIFIED_MODE;
			
			// Save the new driver running mode
			m_uiDriverRunningMode = (unsigned int) arg;
			break;
		
		// If it's the Set Gps Off time command
		case SYNC_IOCTL_SET_GPS_OFF_TIME :
			// If the passed Gps Off Time is correct
			if ((unsigned int) arg > GPS_MAXIMUM_OFF_TIME)
				return -ERR_SYNCHRONIZATION_IOCTL_SPECIFIED_GPS_OFF_TIME;
			
			// Save the new Gps Off Time
			m_uiGpsTimeToOff = (unsigned int) arg * 60;
			break;
		
		// If it's an unknown IOCTL command
		default :
			printk(KERN_WARNING "%s%s : IOCTL unrecognized : %d\n", DRV_NAME, __func__, cmd);
			// Send back an error code
			return -ERR_SYNCHRONIZATION_IOCTL_SPECIFIED_IOCTL_COMMAND;
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
static struct file_operations synchronization_fops =
{
	.owner = THIS_MODULE,
	.open = Synchronization_open,
	.release = Synchronization_close,
	.read = Synchronization_read,
	.write = Synchronization_write,
	.poll = Synchronization_poll,
	.unlocked_ioctl = Synchronization_ioctl,
};

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	PpsTimerConfiguration												//
//																								//
// Description			:	Function which mux the external pin of the capture timer,			//
//							and which configure the capture timer to be used.					//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int PpsTimerConfiguration (void)
{
	// Variables intialization
	int iRet = 0, iCpt;
	void * addr;
	struct clk * fclk;
	
	// Set all pins of the Pps Timer mux registers in the right mode
	for (iCpt = 0; iCpt < TIMER_MUX_PINS_NB; iCpt++) {
		// Register access request
		addr = ioremap(m_uiPpsTimerRegisterAddress[iCpt], 2);
		// If an error occured during the request
		if (!addr){
			printk(KERN_ERR "%s%s : Error during the Pps capture pin mux\n", DRV_NAME, __func__);
			return -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_MUX_REQUEST;
		} else {
			// Read the current register value and save it as previous mode
			m_uiPpsTimerPreviousMode[iCpt] = ioread16(addr);
			
			// If we currently mux the intended output
			if (iCpt == 0)
				iowrite16(0xC102, addr);			// Set in Timer Mode
			else
				iowrite16(0x010F, addr);			// Set in Safe Mode
			
			// Unmap the register access
			iounmap(addr);
		}		
	}
	
	// Request the intended capture timer
	m_pPpsTimerStruct = omap_dm_timer_request_specific(m_uiPpsTimerNumber);
	// If an error occured during the request
	if (!m_pPpsTimerStruct) {
		iRet = -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_TIMER_REQUEST;
		printk(KERN_ERR "%s%s : Unable to request Pps timer : %d\n", DRV_NAME, __func__, m_uiPpsTimerNumber);
		goto out_pps_timer_unmux;
	}
	
	// Set the timer source clock and check if it happened well
	if(omap_dm_timer_set_source(m_pPpsTimerStruct, OMAP_TIMER_SRC_SYS_CLK)) {
		iRet = -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_TIMER_SET_SRC_CLK;
		printk(KERN_ERR "%s%s : Unable to set source clock of Pps timer : %d\n", DRV_NAME, __func__, m_uiPpsTimerNumber);
		goto out_pps_timer_free;
	}
	
	// Get the clk struct of the Pps capture timer
	fclk = omap_dm_timer_get_fclk(m_pPpsTimerStruct);
	if (!fclk) {
		printk(KERN_ERR "%s%s : Unable to get the source clock of Pps timer : %d\n", DRV_NAME, __func__, m_uiRelayTimerNumber);
		iRet = -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_TIMER_GET_SRC_CLK;
		goto out_pps_timer_free;
	}
	// And use this struct to get the timer clock frequency
	m_uiPpsTimerInputClock = clk_get_rate(fclk);
	
	// Set the Pps capture timer in capture mode and check if it happened well
	iRet = omap_dm_timer_set_capture(m_pPpsTimerStruct, 0, IRQF_TRIGGER_RISING);
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Unable to set the capture mode in Pps timer : %d\n", DRV_NAME, __func__, m_uiRelayTimerNumber);
		iRet = -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_TIMER_SET_CAPTURE;
		goto out_pps_timer_free;
	}
	
	// Set the Pps capture timer load value and check if it happened well
	iRet = omap_dm_timer_set_load(m_pPpsTimerStruct, 1, 0);
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Unable to set the load value, and the auto-reload mode of the Pps timer : %d\n", DRV_NAME, __func__, m_uiRelayTimerNumber);
		iRet = -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_TIMER_SET_LOAD;
		goto out_pps_timer_free;
	}
	
	// Disable all interruptions for the Pps capture timer and check if it happened well
	iRet = omap_dm_timer_set_int_enable(m_pPpsTimerStruct, 0);
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Error %d during Pps timer interruption settings\n", DRV_NAME, __func__, iRet);
		iRet = -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_TIMER_CLEAR_INT;
		goto out_pps_timer_unconfigure;
	}
	
	// Request interruption on Pps Timer event and check if it happened well
	iRet = request_irq(omap_dm_timer_get_irq(m_pPpsTimerStruct), PpsTimerIrqHandler, IRQF_DISABLED | IRQF_TIMER, "Pps_TIMER", (void *) m_uiPpsTimerNumber);
	if(iRet != 0) {
		printk(KERN_ERR "%s%s : Error %d during the Timer interruption request on Pps Timer\n", DRV_NAME, __func__, iRet); 	
		iRet = -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_REQUEST_IRQ;
		goto out_pps_timer_unconfigure;
	}
	
	// Set the intended interruption(s) for the Pps capture timer and check if it happened well
	omap_dm_timer_set_int_enable(m_pPpsTimerStruct, OMAP_TIMER_INT_CAPTURE);
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Error %d during Pps timer interruption settings\n", DRV_NAME, __func__, iRet);
		iRet = -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_TIMER_SET_INT;
		goto out_pps_timer_unconfigure;
	}
	
	goto done;
	
	// Exit shortcuts
	out_pps_timer_unconfigure :
		// Disable all interruptions for the Pps capture timer
		omap_dm_timer_set_int_enable(m_pPpsTimerStruct, 0);
    
    out_pps_timer_free :
    	// Release the intended capture timer
    	omap_dm_timer_free(m_pPpsTimerStruct);
    
    out_pps_timer_unmux :
    	// Set all pins of the Pps Timer mux registers in the previous mode
    	for (iCpt = 0; iCpt < TIMER_MUX_PINS_NB; iCpt++) {
			// Register access request
			addr = ioremap(m_uiPpsTimerRegisterAddress[iCpt], 2);
			
			// If no error occured during the request
			if (addr) {
				// If the mode isn't the same that the previous mode
				if (ioread16(addr) != m_uiPpsTimerPreviousMode[iCpt])
					// Restore previous mode
					iowrite16(m_uiPpsTimerPreviousMode[iCpt], addr);
			
				// Unmap the register access
				iounmap(addr);
			}
		}
	
	done :
    	return iRet;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	RelayTimerConfiguration												//
//																								//
// Description			:	Function which configure the relay timer to be used.				//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int RelayTimerConfiguration (void)
{
	// Variables intialization
	int iRet = 0;
	struct clk * fclk;
	
	// gpio146 out
	void * addr = ioremap(0x48002178, 2);
	if (!addr){
		printk(KERN_ERR "%s%s : Error during the Relay Timer pin mux\n", DRV_NAME, __func__);
		return -ERR_SYNCHRONIZATION_PPSTIMERCONFIGURATION_MUX_REQUEST;
	} else {
		iowrite16(0x0002, addr);

		// Unmap the register access
		iounmap(addr);
	}

	// Request the intended timer
	m_pRelayTimerStruct = omap_dm_timer_request_specific(m_uiRelayTimerNumber);
	// If an error occured during the request
	if (!m_pRelayTimerStruct) {
		printk(KERN_ERR "%s%s : Unable to request relay timer : %d\n", DRV_NAME, __func__, m_uiRelayTimerNumber);
		return -ERR_SYNCHRONIZATION_RELAYTIMERCONFIGURATION_TIMER_REQUEST;
	}
	
	// Set the timer source clock and check if it happened well
	if (omap_dm_timer_set_source(m_pRelayTimerStruct, OMAP_TIMER_SRC_SYS_CLK)) {
		printk(KERN_ERR "%s%s : Unable to set source clock of relay timer : %d\n", DRV_NAME, __func__, m_uiRelayTimerNumber);
		iRet = -ERR_SYNCHRONIZATION_RELAYTIMERCONFIGURATION_TIMER_SET_SRC_CLK;
		goto out_relay_timer_free;
	}
	
	// Get the clk struct of the relay timer
	fclk = omap_dm_timer_get_fclk(m_pRelayTimerStruct);		// Make sure we know the source clock frequency
	if (!fclk) {
		printk(KERN_ERR "%s%s : Unable to get the source clock of relay timer : %d\n", DRV_NAME, __func__, m_uiRelayTimerNumber);
		iRet = -ERR_SYNCHRONIZATION_RELAYTIMERCONFIGURATION_TIMER_GET_SRC_CLK;
		goto out_relay_timer_free;
	}
	// And use this struct to get the timer clock frequency
	m_uiRelayTimerInputClock = clk_get_rate(fclk);
	
	// Disable all interruptions for the Pps capture timer and check if it happened well
	iRet = omap_dm_timer_set_int_enable(m_pRelayTimerStruct, 0);
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Error %d during relay timer interruption settings\n", DRV_NAME, __func__, iRet);
		iRet = -ERR_SYNCHRONIZATION_RELAYTIMERCONFIGURATION_TIMER_CLEAR_INT;
		goto out_relay_timer_unconfigure;
	}
	
	// Request interruption on Relay Timer overflow and check if it happened well
	iRet = request_irq(omap_dm_timer_get_irq(m_pRelayTimerStruct), RelayTimerIrqHandler, IRQF_DISABLED | IRQF_TIMER, "Relay_TIMER", (void *) m_uiRelayTimerNumber);
	if (iRet != 0) {
		printk(KERN_ERR "%s%s : Error %d during the Timer interruption request on Relay Timer\n", DRV_NAME, __func__, iRet);
		iRet = -ERR_SYNCHRONIZATION_RELAYTIMERCONFIGURATION_REQUEST_IRQ;
		goto out_relay_timer_unconfigure;
	}
	
	// Set the intended interruption(s) for the Relay timer and check if it happened well
	iRet = omap_dm_timer_set_int_enable(m_pRelayTimerStruct, OMAP_TIMER_INT_OVERFLOW);
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Error %d during relay timer interruption settings\n", DRV_NAME, __func__, iRet);
		iRet = -ERR_SYNCHRONIZATION_RELAYTIMERCONFIGURATION_TIMER_SET_INT;
		goto out_relay_timer_unconfigure;
	}
	
	goto done;
	
	// Exit shortcuts
	out_relay_timer_unconfigure :
    	// Disable all interruptions for the relay timer
    	omap_dm_timer_set_int_enable(m_pRelayTimerStruct, 0);
    
    out_relay_timer_free :
    	// Release the intended timer
    	omap_dm_timer_free(m_pRelayTimerStruct);
	
	done :
    	return iRet;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	GpsSoftwareShutdownGpioConfiguration								//
//																								//
// Description			:	Function which configure Gps Software Shutdown Gpio to be used.		//
//																								//
// Return				:	A negative number if an error occured, 0 otherwise.					//
//																								//
//----------------------------------------------------------------------------------------------//
static int GpsSoftwareShutdownGpioConfiguration (void)
{
	// Variables intialization
	int iRet = 0;
	void * addr;
	
	// Set the Gps Software shutdown Gpio register in the right mode
	addr = ioremap(m_uiGpsShutdownGpioRegisterAddress, 2);			// Register access request
	// If an error occured during the request
	if (!addr) {
		printk(KERN_ERR "%s%s : Error during the software shutdown Gpio mux\n", DRV_NAME, __func__);
		return -ERR_SYNCHRONIZATION_GPSSHUTDOWNGPIOCONFIGURATION_MUX_REQUEST;
	}
	else {
		// Read the current register value and save it as previous mode
		m_uiGpsShutdownGpioPreviousMode = ioread16(addr);
		
		// Set in Gpio Mode
		iowrite16(0x010C, addr);
		
		// Unmap the register access
		iounmap(addr);
	}
	
	// Gps Software shutdown Gpio request
	iRet = gpio_request (m_uiGpsShutdownGpioNumber, (const char *) "Gps Software Shutdown Gpio");
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : Error %d during the request of the software shutdown Gpio\n", DRV_NAME, __func__, iRet);
		iRet = -ERR_SYNCHRONIZATION_GPSSHUTDOWNGPIOCONFIGURATION_GPIO_REQUEST;
		goto out_gpio_unmux;
	}
	
	// Gps Software shutdown Gpio configuration (set utput direction and default output value to 1)
	iRet = gpio_direction_output(m_uiGpsShutdownGpioNumber, 1);
	if(iRet < 0) {
		printk(KERN_ERR "%s%s : Error %d during the configuration of the software shutdown Gpio\n", DRV_NAME, __func__, iRet); 	
		iRet = -ERR_SYNCHRONIZATION_GPSSHUTDOWNGPIOCONFIGURATION_GPIO_SET_DIRECTION;
		goto out_gpio_free;
	}
	
	goto done;
	
	// Exit shortcuts
	out_gpio_free :
		// Free the Gps Software shutdown Gpio
		gpio_free (m_uiGpsShutdownGpioNumber);
	
	out_gpio_unmux :
		// Set the Gps Software shutdown Gpio register in the previous mode
		addr = ioremap(m_uiGpsShutdownGpioRegisterAddress, 2);		// Register access request
		// If no error occured during the request
		if (addr) {
			// If the mode isn't the same that the previous mode
			if (ioread16(addr) != m_uiGpsShutdownGpioPreviousMode)
				// Restore previous mode
				iowrite16(m_uiGpsShutdownGpioPreviousMode, addr);
				
			// Unmap the register access
			iounmap(addr);
		}
		
	done :
		return iRet;
}

//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Synchronization_initialize											//
//																								//
// Description			:	Function called when the driver is insert (insmod).					//
// 							Allows to provide driver informations and to link the functions.	//
//																								//
// Return				:	A negative number if an error occured and prevents the 'insmod',	//
//							0 otherwise.														//
//																								//
//----------------------------------------------------------------------------------------------//
int __init Synchronization_initialize(void)
{
	// Variable initialization
	int iRet = 0;
	
	// Print the driver banner
	printk(KERN_NOTICE "%s", sBanner);
	
	// Allocate a driver 'DRV_DEV_NAME' with a major number obtain dynamically. We can found this entry in "/proc/devices"
	iRet = alloc_chrdev_region (&m_oSynchronizationDev, 0, DRIVER_DEV_REGISTER_NB, DRV_DEV_NAME);
	// And check if the allocation happened well
	if (iRet < 0) {
		printk(KERN_ERR "%s%s : alloc_chrdev_region failed, err : %d\n", DRV_NAME, __func__, iRet);
		return -ERR_SYNCHRONIZATION_INIT_ALLOC_CHRDEV_REGION;
	}
	
	// Save the dynamically obtained major and minor
	m_uiSynchronizationMajor = MAJOR(m_oSynchronizationDev);
	m_uiSynchronizationMinor = MINOR(m_oSynchronizationDev);
	
	// Initialize the global variable Initialise 'm_oGpioCDev' and associate it with the file_operation 'synchronization_fops'
	cdev_init(&m_oSynchronizationCDev, &synchronization_fops);
    m_oSynchronizationCDev.owner = THIS_MODULE;			// Indicate the module owner
    
    // Add the driver in the system and link it with the file_operations linked in 'm_oSynchronizationCDev' to the node using 'm_oSynchronizationDev'
    iRet = cdev_add(&m_oSynchronizationCDev, m_oSynchronizationDev, DRIVER_DEV_REGISTER_NB);
    // And check if the creation of the link happened well
    if (iRet != 0 ) {
        printk(KERN_ERR "%s%s : Cdev_add failed : %d\n", DRV_NAME, __func__, iRet);
    	iRet = -ERR_SYNCHRONIZATION_INIT_CDEV_ADD;
    	goto out_unregister;
    }
	
	// Now that we've added the device, create a class, so that udev will make the /dev entries (also create an entry in "/sys/class/")
	m_pSynchronizationClass = class_create (THIS_MODULE, DRV_SYS_NAME);
	// And check if the operation happened well
	if (IS_ERR(m_pSynchronizationClass)) {
        printk(KERN_ERR "%s%s : Unable to create class\n", DRV_NAME, __func__ );
        iRet = -ERR_SYNCHRONIZATION_INIT_CLASS_CREATE;
        goto out_cdev_del;
    }

	// Entry creation in "/dev"
	m_pDeviceEntry = device_create (m_pSynchronizationClass, NULL, MKDEV (m_uiSynchronizationMajor, m_uiSynchronizationMinor), "%s", DRV_DEV_NAME);
	// And check if the entry creation happened well
	if (IS_ERR(m_pDeviceEntry)) {
        printk(KERN_ERR "%s%s : Unable to create device : %s\n", DRV_NAME, __func__, DRV_DEV_NAME);
        iRet = -ERR_SYNCHRONIZATION_INIT_DEVICE_CREATE;
        goto out_cdev_del;
    }
    
    // Register proc entry
	m_pPreemptProcEntry = create_proc_read_entry (DRV_PROC_PREEMPT_ENTRY, 0, 0, SynchronizationPreemptionTimeRead, NULL);
	if (IS_ERR(m_pPreemptProcEntry)) {
		printk(KERN_ERR "%s%s : Unable to create the \"proc\" entry : \"%s\"\n", DRV_NAME, __func__, DRV_PROC_PREEMPT_ENTRY);
        iRet = -ERR_SYNCHRONIZATION_INIT_CREATE_PROC_ENTRY;
        goto out_device_del;
    }
	
	// Relay Timer configuration
	iRet = RelayTimerConfiguration();
	// And check if the configuration happened well
	if (iRet < 0)
		goto out_proc_del;
	
	// Gps Software shutdown Gpio configuration
	iRet = GpsSoftwareShutdownGpioConfiguration();
	// And check if the configuration happened well
	if (iRet < 0)
		goto out_proc_del;
	
	// Pps Timer configuration
	iRet = PpsTimerConfiguration ();
	// And check if the configuration happened well
	if (iRet < 0)
		goto out_proc_del;
		
	// Pps timer start
	omap_dm_timer_start(m_pPpsTimerStruct);
	
	// State machine initialization
	m_DriverCurrentState = STATE_Initialization;
	
	// Thread Management
	SecondsIncrementsVerificationThread = kthread_create(SecondsIncrementsVerificationFunction, NULL, "SecondsIncrementsVerificationThread");
	// Launch the thread if we need to
	if (m_bThreadLaunching == 1)
		wake_up_process (SecondsIncrementsVerificationThread);
	
	// Initialize the Gps Off time at 5 minutes
	m_uiGpsTimeToOff = 5 * 60;
	
	// Reset the preemption time array
	memset(m_iPreemptTimeValueTable, 0, PREEMPTION_TIME_VALUE_NUMBER*sizeof(int));
	
	// Switch the Gps to On
	SetGpsStatus (1);
		
	printk(KERN_NOTICE "%s%s : Driver loaded.\n", DRV_NAME, __func__);
	
	goto done;
	
	// Exit shortcuts
	
	out_proc_del :
		// Remove proc entry
		remove_proc_entry(DRV_PROC_PREEMPT_ENTRY, NULL);
	
	out_device_del :
		// Delete the node and the entry of '/dev'
		device_destroy (m_pSynchronizationClass, MKDEV (m_uiSynchronizationMajor, m_uiSynchronizationMinor));
	
	out_cdev_del :
    	// Destroy the driver from the system and destroy the link between the file_operations linked in 'm_oSynchronizationCDev' and the node using 'm_oSynchronizationDev'
    	cdev_del(&m_oSynchronizationCDev);

	out_unregister :
    	// De-allocate the driver 'DRV_DEV_NAME'
    	unregister_chrdev_region(m_oSynchronizationDev, DRIVER_DEV_REGISTER_NB);

	done :
    	return iRet;
}


//----------------------------------------------------------------------------------------------//
//																								//
// Function Name		:	Synchronization_cleanup												//
//																								//
// Description			:	Module destroy function	(rmmod)										//
//																								//
// Return				:   Nothing																//
//																								//
//----------------------------------------------------------------------------------------------//
void __exit Synchronization_cleanup(void)
{
	// Vairiables initialization
	int iCpt;
	void * addr = NULL;
	
	// If the thread should be stop
	if (m_bThreadLaunching == 1)
		// Stop it
		kthread_stop(SecondsIncrementsVerificationThread);
	
	// Stop the Pps Timer
	omap_dm_timer_stop(m_pPpsTimerStruct);
	
	// Release the Pps Timer
	omap_dm_timer_free(m_pPpsTimerStruct);
	
	// Release the Pps Timer interrupt
	free_irq (omap_dm_timer_get_irq(m_pPpsTimerStruct), (void *) m_uiPpsTimerNumber);
	
	// Set the Pps Timer register in previous mode
	for (iCpt = 0; iCpt < TIMER_MUX_PINS_NB; iCpt++) {
		// Register access request
		addr = ioremap(m_uiPpsTimerRegisterAddress[iCpt], 2);
		// If no error occured during the request
		if (addr) {
			// If the mode isn't the same that the previous mode
			if (ioread16(addr) != m_uiPpsTimerPreviousMode[iCpt])
				// Restore previous mode
				iowrite16(m_uiPpsTimerPreviousMode[iCpt], addr);
		
			// Unmap the register access		
			iounmap(addr);
		}
	}
	
	// If the relay timer is launch
	if (m_DriverCurrentState == STATE_Gps_Off)
		// Stop it
		omap_dm_timer_stop(m_pRelayTimerStruct);
	
	// Release the Relay Timer
	omap_dm_timer_free(m_pRelayTimerStruct);
	
	// Release the Relay Timer interrupt
	free_irq (omap_dm_timer_get_irq(m_pRelayTimerStruct), (void *) m_uiRelayTimerNumber);
	
	// Free the Gps Software shutdown Gpio
	gpio_free (m_uiGpsShutdownGpioNumber);
	
	// Set the Gps Software shutdown Gpio register in the right mode
	addr = ioremap(m_uiGpsShutdownGpioRegisterAddress, 2);
	// If no error occured during the request
	if (addr) {
		// If the mode isn't the same that the previous mode
		if (ioread16(addr) != m_uiGpsShutdownGpioPreviousMode)
			// Restore previous mode
			iowrite16(m_uiGpsShutdownGpioPreviousMode, addr);
		
		// Unmap the register access
		iounmap(addr);
	}
	
	// Remove proc entry
	remove_proc_entry(DRV_PROC_PREEMPT_ENTRY, NULL);
	
	// Delete the node and the entry of '/dev'
	device_destroy (m_pSynchronizationClass, MKDEV (m_uiSynchronizationMajor, m_uiSynchronizationMinor));
	    
	// Delete the entry of '/sys/class'
	class_unregister (m_pSynchronizationClass);
    class_destroy (m_pSynchronizationClass);
    
    // Delete the 'm_oSynchronizationCDev' global variable
    cdev_del (&m_oSynchronizationCDev);
    
	// Deallocate the driver and the major/minor number
	unregister_chrdev_region (m_oSynchronizationDev, DRIVER_DEV_REGISTER_NB);

	printk(KERN_NOTICE "%s%s : Driver unloaded.\n", DRV_NAME, __func__);
}

//----------------------------------------------------------------------------------------------//
//																								//
//	Mapping of the 'module_init' and 'module_exit' functions									//
//																								//
//----------------------------------------------------------------------------------------------//

// Link 'insmod' to the 'Synchronization_initialize' function
module_init(Synchronization_initialize);
// Link 'rmmod' to the 'Synchronization_cleanup' function
module_exit(Synchronization_cleanup);
