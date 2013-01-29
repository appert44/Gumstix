/**
 *	\file	CGPIO.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CGPIO class
 *	
 *	\author Jonathan Aillet
 *
 *	\version v1.0a
 *
 *	\date 28 June, 2012
 *
 */

#include "CGPIO.h"

using namespace std;

/**
 *	\fn		CGPIO :: CGPIO (char * sDevicePath, bool bDirection, bool bDefaultValue)
 *	\brief	Constructor of the CGPIO class for the Input/Output mode
 *
 *  \param[in]	sDevicePath : 		Path of the choosen gpio
 *  \param[in]	bDirection :		Direction of the Gpio (INPUT or OUTPUT)
 *  \param[in]	bDefaultValue : 	Choose the default OUTPUT value
 */
CGPIO :: CGPIO (char * sDevicePath, bool bDirection, bool bDefaultValue)
{
	// I/O Mode
	m_bMode = IO_MODE;
	
	// Save the configuration
	m_bDirection = bDirection;
	m_bDefaultValue = bDefaultValue;
	m_uiActiveEvent = 0;
	m_uiDelay = 0;
	
	// Init the used attributes
	m_fd = -1;
	m_bThreadRunning = false;
	
	// Saved the path of the serial port which will be used to read GPS frame
	m_sGpioPath = strdup(sDevicePath);
	
	// Initialisation of the usable Gpio
	SetAuthorizeGpio();
	
	cout << CGPIO_PRINT_NAME << "CGPIO class object has been created (in Input/Output mode)" << endl;
}

/**
 *	\fn		CGPIO :: CGPIO (char * sDevicePath, unsigned int uiActiveEvent)
 *	\brief	Constructor of the CGPIO class for the Input/Output mode
 *
 *  \param[in]	sDevicePath : 		Path of the choosen gpio
 *  \param[in]	uiActiveEvent :		Choose the events which will be detected
 */
CGPIO :: CGPIO (char * sDevicePath, unsigned int uiActiveEvent)
{
	// IT Mode
	m_bMode = IT_MODE;
	
	// Save the configuration
	m_bDirection = 0;
	m_bDefaultValue = 0;	
	m_uiActiveEvent = uiActiveEvent;
	m_uiDelay = 0;
	
	// Init the used attributes
	m_fd = -1;
	m_bThreadRunning = false;
	
	// Saved the path of the serial port which will be used to read GPS frame
	m_sGpioPath = strdup(sDevicePath);
	
	// Initialisation of the usable Gpio
	SetAuthorizeGpio();
	
	cout << CGPIO_PRINT_NAME << "CGPIO class object has been created (in Interrupt mode)" << endl;
}

/** 
 *	\fn		CGPIO :: ~CGPIO()
 *	\brief	Destructor of the CGPIO class
 */
CGPIO :: ~CGPIO()
{	
	// Unregister all the callbacks connected to the CUart object
	UnregisterAllCallbacks(ALL_EVENTS);
	
	// Closure of the Gpio
	if (m_fd != -1) {
		close(m_fd);
	}
	
	// Deallocation of the dynamicly allocated memory
	if (m_sGpioPath != NULL) {
		free(m_sGpioPath);
	}
	
	cout << CGPIO_PRINT_NAME << "CGPIO class object has been destroyed" << endl;
}

/**
 *	\fn		int CGPIO :: Open()
 *	\brief	Launch initialization and configuration of a CGPIO object
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CGPIO :: Open()
{
	// Check if the path of the gpio is correct
	if (strncmp(m_sGpioPath, "/dev/gpio", 9))
		return -ERR_CGPIO_OPEN_SPECIFIED_PATH;
	
	// Check if the Gpio is useable
	if (SearchOnTable (GetuiGpioNum()) == false)
		return -ERR_CGPIO_OPEN_GPIO_NOT_USEABLE;
	
	// Open the gpio port and check if the open happened well
	m_fd = open(m_sGpioPath, O_RDWR);
	if (m_fd < 0)
		return -ERR_CGPIO_OPEN_GPIO_DRIVER_OPEN;
	
	// Check the Gpio open mode
	if (m_bMode == IO_MODE) {
		// Check the direction (Driver direction is Input)
		if (m_bDirection == OUTPUT) {
			// Set the direction as Output and check if the direction is correctly set
			if (ioctl(m_fd, GPIO_IOCTL_SET_DIRECTION, OUTPUT) < 0)
				return -ERR_CGPIO_OPEN_SET_DIRECTION;			
		}
		// Check the the Output default value (Driver default value is 0)
		if (m_bDefaultValue == 1) {
			// Set the default value to 1
			if (ioctl(m_fd, GPIO_IOCTL_SET_OUTPUT_VALUE, m_bDefaultValue) < 0)
				return -ERR_CGPIO_OPEN_SET_OUTPUT_VALUE;
		}
	}
	else {
		// Check the active event
		if (m_uiActiveEvent > 0x0000000F)
			return -ERR_CGPIO_OPEN_SPECIFIED_EVENT;		
		// Check the debounce delay
		else if (m_uiDelay > 500)
			return -ERR_CGPIO_OPEN_SPECIFIED_DEBOUNCE_DELAY;
		// Check if the definition of the debounce delay happened well
		else if (ioctl (m_fd, GPIO_IOCTL_SET_DEBOUNCE_DELAY, m_uiDelay) < 0)
			return -ERR_CGPIO_OPEN_SET_DEBOUNCE_DELAY;
		
		// Set the Gpio in IT mode and specified the active event
		if (ioctl (m_fd, GPIO_IOCTL_SET_INTERRUPT, m_uiActiveEvent) < 0)
			return -ERR_CGPIO_OPEN_SET_INTERRUPT_MODE;
	}
	
	cout << CGPIO_PRINT_NAME << "Opening of Gpio : " << GetuiGpioNum() << " successful" << endl;
	
	return 0;
}

/**
 *	\fn		int CGPIO :: Read()
 *	\brief	Read the current input value of the pin (only in Input/Output Mode)
 *
 *	\return The value of the Gpio
 */
int CGPIO :: Read()
{
	// Variables initialization
	unsigned int uiValue = -1;
	char sRxBuffer[2];
	
	memset (sRxBuffer, '\0', 2);
	
	// Check if we are in the right mode to read the value (IO mode)
	if (m_bMode == IT_MODE)
		return -ERR_CGPIO_READ_WRONG_MODE;
	else if (m_bDirection == OUTPUT) {
		// Set the Gpio direction as Input if the current direction is Output 
		if (ioctl(m_fd, GPIO_IOCTL_SET_DIRECTION, INPUT) < 0)
			return -ERR_CGPIO_READ_SET_DIRECTION;
	}
	// Read the value
	if (read(m_fd, sRxBuffer, 2) < 0)
		return -ERR_CGPIO_READ_GPIO_DRIVER_READ;
	
	// Convert the value to a integer (a boolean)
	uiValue = atoi (sRxBuffer);
	
	// If the value readed is invalid
	if (uiValue > 1)
		return -ERR_CGPIO_READ_WRONG_READ_VALUE;
	else
		return uiValue;
}

/**
 *	\fn		int CGPIO :: Write (bool bValue)
 *	\brief	Read the current input value of the pin (only in Input/Output Mode)
 *
 *	\return A negative number if an error occured, 2 otherwise
 */
int CGPIO :: Write (bool bValue)
{
	// Variable initialization
	char sTxBuffer[2];
	
	// Formatting the buffer to send for the write
	sprintf(sTxBuffer, "%d", bValue);
	sTxBuffer[1] = '\0';
	
	// Check if we are in the right mode to read the value (IO mode)
	if (m_bMode == IT_MODE)
		return -ERR_CGPIO_WRITE_WRONG_MODE;
	else if (m_bDirection == INPUT) {
		// Set the Gpio direction as Output if the current direction is Input
		if (ioctl(m_fd, GPIO_IOCTL_SET_DIRECTION, OUTPUT))
			return -ERR_CGPIO_WRITE_SET_DIRECTION;
	}
	// Write the value
	if (write(m_fd, sTxBuffer, 2) != 2)
		return -ERR_CGPIO_WRITE_GPIO_DRIVER_WRITE;
	
	return 2;
}

/**
 *	\fn		void CGPIO :: OnGpioEvent()
 *	\brief	Method who manage the launch of callbacks when an event is detected on the Gpio
 */
void CGPIO :: OnGpioEvent()
{
	// Variables initialization
	int iNreaded, iCpt = 0, iRet;
	char sRxBuffer[2];
	GeoDatationData oGpioGDData;
	
	struct pollfd oPollFds;
	CEvent * pEvent = NULL;
	
	// While we haven't said to the thread to stop
	do {
		// Poll initialization
		oPollFds.fd = m_fd;
		oPollFds.events = POLLIN | POLLPRI;
		
		// Launch the poll (which remplace a blockinck reading)
		iRet = poll (&oPollFds, 1, 500);		
		
		// Check why the poll has been terminated
		if (iRet > 0) {
			// Reset the reception buffer
			memset (sRxBuffer, 0, sizeof(sRxBuffer));
			
			// Read the data (and check how many bytes has been received)
			iNreaded = read (m_fd, sRxBuffer, sizeof(sRxBuffer));
			
			// Get the GeoDatation data of this event
			iRet = ioctl(m_fd, GPIO_IOCTL_GET_LAST_IT_GD_DATA, &oGpioGDData);
			
			// Check if the GeoDatation data of this event are correctly getted
			if (iRet < 0) {
				printf("%sError during the recovery of the GPIO GDData object\n", CGPIO_PRINT_NAME);
				// Create the CEvent object without using the driver GeoDatation informations
				pEvent = new CEvent;
			}
			else {
				// Create the CEvent object using the driver GeoDatation informations
				pEvent = new CEvent (&oGpioGDData);				
				printf ("%sContent of GPIO GDData : %02d/%02d/%04d, %02d:%02d:%02d:%06d\n\n", CGPIO_PRINT_NAME, pEvent->GetDay(), pEvent->GetMonth(), pEvent->GetYear(), pEvent->GetHour(), pEvent->GetMinute(), pEvent->GetSecond(), pEvent->GetMicroSecond());
			}
			
			cout << CGPIO_PRINT_NAME << "An event has been detected on the Gpio " << GetuiGpioNum() << ", the actual value of the Gpio is " << sRxBuffer << endl;
			printf("%sEvent seen at %02d/%02d/%04d, %02d:%02d:%02d:%06d\n\n", CGPIO_PRINT_NAME, pEvent->GetDay(), pEvent->GetMonth(), pEvent->GetYear(), pEvent->GetHour(), pEvent->GetMinute(), pEvent->GetSecond(), pEvent->GetMicroSecond());
			
			// Launch the connected callbacks
			m_oSigOnGpioEvent(sRxBuffer, iNreaded, pEvent, m_pData1, m_pData2);
			
			// Increment the variable which indicates how many read has been done
			iCpt++;
						
			// Delete the previously created CEvent object
			delete(pEvent);
		}
		else {
			// Tell if the poll timeout has been reached
			cout << CGPIO_PRINT_NAME << "'Poll' timeout reached" << endl;
		}
	} while (m_bThreadRunning);
}

/**
 *	\fn		void CGPIO :: SetAuthorizeGpio()
 *	\brief	Method which set the authorize Gpios
 */
void CGPIO :: SetAuthorizeGpio()
{
	// Useable GPIOs (which we want to use on the PEGASE2 final version
	m_puiAuthorizedGpio[0] = 12;
	m_puiAuthorizedGpio[1] = 13;
	m_puiAuthorizedGpio[2] = 14;
	m_puiAuthorizedGpio[3] = 17;
	m_puiAuthorizedGpio[4] = 18;
	m_puiAuthorizedGpio[5] = 19;
	m_puiAuthorizedGpio[6] = 20;
	m_puiAuthorizedGpio[7] = 21;
	m_puiAuthorizedGpio[8] = 22;
	m_puiAuthorizedGpio[9] = 23;
	m_puiAuthorizedGpio[10] = 70;
	m_puiAuthorizedGpio[11] = 71;
	m_puiAuthorizedGpio[12] = 72;
	m_puiAuthorizedGpio[13] = 73;
	m_puiAuthorizedGpio[14] = 74;
	m_puiAuthorizedGpio[15] = 75;
	m_puiAuthorizedGpio[16] = 76;
	m_puiAuthorizedGpio[17] = 77;
	m_puiAuthorizedGpio[18] = 78;
	m_puiAuthorizedGpio[19] = 79;
	m_puiAuthorizedGpio[20] = 80;
	m_puiAuthorizedGpio[21] = 81;
	
	// For the PEGASE2 prototype board
	m_puiAuthorizedGpio[22] = 64;	// LED 1
	m_puiAuthorizedGpio[23] = 65;	// LED 2
	m_puiAuthorizedGpio[24] = 67;	// On/Off GPS
	
	m_puiAuthorizedGpio[25] = 147;	// PPS (Tobi and PEGASE2)
	
	m_puiAuthorizedGpio[26] = 69;	// External GPIO
	m_puiAuthorizedGpio[37] = 86;	// External GPIO
	
	m_puiAuthorizedGpio[28] = 171;	// SPI 1 : CLK
	m_puiAuthorizedGpio[29] = 172;	// SPI 1 : MOSI
	m_puiAuthorizedGpio[30] = 173;	// SPI 1 : MISO
	m_puiAuthorizedGpio[31] = 174;	// SPI 1 : CS0
	m_puiAuthorizedGpio[32] = 175;	// SPI 1 : CS1
	
	m_puiAuthorizedGpio[33] = 184;	// I2C 3 : SCL
	m_puiAuthorizedGpio[34] = 185;	// I2C 3 : SDA
}

/**
 *	\fn		unsigned int CGPIO :: GetuiGpioNum ()
 *	\brief	Method which return the Gpio number currently used
 *
 *	\return The Gpio number currently used
 */
unsigned int CGPIO :: GetuiGpioNum ()
{
	// Variables initialization
	int iCpt;
	char sGpio[3];
	
	memset(sGpio, '\0', sizeof(sGpio));
	
	// Get the Gpio number using its path
	for (iCpt = 0; iCpt < 3; iCpt++)
		sGpio[iCpt] = m_sGpioPath[iCpt+9];
	
	return atoi(sGpio);
}

/**
 *	\fn		bool CGPIO :: SearchOnTable (unsigned int uiGpioNum)
 *	\brief	Method which return if the gpio is usable
 *
 *	\return A boolean which indicates if the Gpio is usable
 */
bool CGPIO :: SearchOnTable (unsigned int uiGpioNum)
{
	// Variable initialization
	int iCpt;
	
	// Check if this gpio can be used
	for (iCpt = 0; iCpt < MAX_GPIO_NB; iCpt++) {
		if (uiGpioNum == m_puiAuthorizedGpio[iCpt])
			return true;
	}
	
	return false;
}

/**
 *	\fn		int CGPIO :: SetDelay (unsigned int uiDelay)
 *	\brief	Method which allow to set the debounce value (500 mS max.)
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CGPIO :: SetDelay (unsigned int uiDelay)
{
	// Check if the delay is 'admissible'
	if (uiDelay > 500)
		return -ERR_CGPIO_SETDELAY_SPECIFIED_DEBOUNCE_DELAY;
	
	m_uiDelay = uiDelay;
	
	// Passing the new debounce delay to the driver and check if the opperation happened well
	if (ioctl (m_fd, GPIO_IOCTL_SET_DEBOUNCE_DELAY, m_uiDelay) < 0)
		return -ERR_CGPIO_SETDELAY_SET_DEBOUNCE_DELAY;
	
	return 0;
}

/**
 *	\fn		int CGPIO :: RegisterCallback (unsigned int uuiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2)
 *  \brief	Register a Callback which will be called when the event specified in EnventID will occure
 *
 *  \param[in]	uuiEventId : 		Integer that indicate the event ID on which the function will be launched
 *  \param[in]	oFct : 			Object which indicates the function to launch when a event occure
 *  \param[in] 	pData1 : 		User pointer.
 *  \param[in] 	pData2 : 		User pointer.
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CGPIO :: RegisterCallback (unsigned int uuiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2)
{	
	// Variable initialization
	bool bThreadLaunching = false;
	
	// Check if we are in the right mode to read the value (IT mode)
	if (m_bMode == IO_MODE)
		return -ERR_CGPIO_REGISTERCALLBACK_WRONG_MODE;
	
	// If the boost::signal is empty
	if (m_oSigOnGpioEvent.num_slots() == 0) {
		if (m_bThreadRunning) {
			cout << CGPIO_PRINT_NAME << "Error, the thread is already launched while no callbacks are connected" << endl;
			// Indicate to the thread it should stop
			m_bThreadRunning = false;
			// Wait for the thread end
			m_oThreadEvent.join();
			return -ERR_CGPIO_REGISTERCALLBACK_THREAD_MANAGING;
		}
		
		// Indicate that the thread should be launch
		bThreadLaunching = true;
	}
	
	// Check the event
	if (uuiEventId == EVENT_CGPIO_EVENT) {
		// Connect the passed function
		m_oLastFctId = m_oSigOnGpioEvent.connect(oFct);
		
		// Checks if the function was well connected
		if(m_oLastFctId.connected()) {
			cout << CGPIO_PRINT_NAME << "Callback correctly registered" << endl;
			if (bThreadLaunching) {
				// Indicate to the thread it should run
				m_bThreadRunning = true;
				// And launch the data reader thread
				boost::thread m_oThreadEvent(boost::bind(&CGPIO::OnGpioEvent, this));
			}
			
			// User parameters
			m_pData1 = pData1;
			m_pData2 = pData2;
			
			return 0;
		}
		else {
			cout << CGPIO_PRINT_NAME <<  "The callback could not be correctly registered" << endl;
			return -ERR_CGPIO_REGISTERCALLBACK_REGISTERING;
		}
	}
	// Unknown event
	else {
		cout << "Wrong event ID" << endl;
		return -ERR_CGPIO_REGISTERCALLBACK_SPECIFIED_EVENT;
	}
}

/**
 *	\fn		int CGPIO :: UnregisterCallback(unsigned int uuiEventId, boost::signals2::connection oFctId)
 *  \brief	Unregister a previously register callback using its connection Id
 *
 *  \param[in]	uuiEventId : 		Integer that indicate the event ID on which the function is launched
 *  \param[in]	oFctId : 		Object which indicates the connection ID of the function to unregister
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CGPIO :: UnregisterCallback(unsigned int uuiEventId, boost::signals2::connection oFctId)
{
	// Variables initialization
	int iNumBefore, iNumAfter;
	
	// Check if we are in the right mode to read the value (IT mode)
	if (m_bMode == IO_MODE)
		return -ERR_CGPIO_UNREGISTERCALLBACK_WRONG_MODE;
	
	// Check the event
	if (uuiEventId == EVENT_CGPIO_EVENT) {
		// Checks if there are some connected functions
		if (m_oSigOnGpioEvent.num_slots() == 0) {
			cout << CGPIO_PRINT_NAME << "No Callbacks are connected on the Gpio specified event" << endl;
			return -ERR_CGPIO_UNREGISTERCALLBACK_NO_CALLBACK;
		}
		
		// Saved the number of connected function before the deconnexion
		iNumBefore = m_oSigOnGpioEvent.num_slots();
		
		// Disconnect the function using its connexion ID
		oFctId.disconnect();
		
		// Saved the number of connected function after the deconnexion
		iNumAfter = m_oSigOnGpioEvent.num_slots();
	}
	// Unknown event
	else {
		cout << CGPIO_PRINT_NAME << "Wrong event ID" << endl;
		return -ERR_CGPIO_UNREGISTERCALLBACK_SPECIFIED_EVENT;
	}
	
	// Checks if the function was well disconnected
	if (iNumBefore == iNumAfter+1) {
		// If there is no longer connected functions to the boost::signals
		if (iNumAfter == 0) {			
			cout << CGPIO_PRINT_NAME << "Callback correctly unregistered" << endl;
			
			// Indicate to the thread it should stop
			m_bThreadRunning = false;
			// Wait for the thread end
			m_oThreadEvent.join();
		}
		
		return 0;
	}
	else {
		cout << CGPIO_PRINT_NAME << "The callback could not be correctly unregistered" << endl;
		return -ERR_CGPIO_UNREGISTERCALLBACK_UNREGISTERING;
	}
}

/**
 *	\fn		int CGPIO :: UnregisterAllCallbacks (unsigned int uuiEventId)
 *  \brief	Unregister all previously register callback on a specific event
 *
 *  \param[in]	uuiEventId : 		Integer that indicate the event ID to empty
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CGPIO :: UnregisterAllCallbacks (unsigned int uuiEventId)
{
	// Check if we are in the right mode to read the value (IT mode)
	if (m_bMode == IO_MODE)
		return -ERR_CGPIO_UNREGISTERALLCALLBACKS_WRONG_MODE;
	
	// Check the event
	if (uuiEventId == ALL_EVENTS || uuiEventId == EVENT_CGPIO_EVENT) {
		// Checks if there are some connected functions
		if (m_oSigOnGpioEvent.num_slots() == 0) {
			cout << CGPIO_PRINT_NAME << "No callbacks are currently connected" << endl;
			return -ERR_CGPIO_UNREGISTERALLCALLBACKS_NO_CALLBACK;
		}
		else {
			// Disconnect all the functions form this specific events
			m_oSigOnGpioEvent.disconnect_all_slots();
			
			// Checks if all the functions was well disconnected
			if (m_oSigOnGpioEvent.num_slots() == 0)
			{
				cout << CGPIO_PRINT_NAME << "Every Callbacks correctly disconnected" << endl;
				
				// Indicate to the thread it should stop
				m_bThreadRunning = false;
				// Wait for the thread end
				m_oThreadEvent.join();
				
				return 0;
			}
			else {
				cout << CGPIO_PRINT_NAME << "The callbacks could not be correctly unregistered" << endl;
				return -ERR_CGPIO_UNREGISTERALLCALLBACKS_UNREGISTERING;
			}
		}
	}
	// Unknown event
	else {
		cout << CGPIO_PRINT_NAME << "Wrong event ID" << endl;
		return -ERR_CGPIO_UNREGISTERALLCALLBACKS_SPECIFIED_EVENT;
	}
}

/**
 *	\fn		boost::signals2::connection CGPIO :: GetLastFctId ()
 *  \brief	Send back the last connected callback connexion Id
 *
 *	\return	The last connected callback connexion Id
 */
boost::signals2::connection CGPIO :: GetLastFctId ()
{
	return m_oLastFctId;
}

/**
 *	\fn		int CGPIO :: PrintConfig()
 *	\brief	Method which print the configuration of the serial port
 */
int CGPIO :: PrintConfig()
{
	cout << endl;
	
	// Check the mode
	if (m_bMode == IO_MODE) {
		// Print the Gpio number
		cout << CGPIO_PRINT_NAME << "Gpio " << GetuiGpioNum() << " in ";
		
		// Check the direction
		if (m_bDirection == INPUT)
			// Print the Gpio mode
			cout << "Input mode" << endl;
		else
			// Print the Gpio mode
			cout << "Output mode" << endl;
	}
	else
		// Print the Gpio number, mode, and delay
		printf ("%sGpio %d in Interrupt mode with a debounce delay of %d ms\n", CGPIO_PRINT_NAME, GetuiGpioNum(), m_uiDelay);
	
	// Print the Gpio path
	cout << CGPIO_PRINT_NAME << "Gpio Path : " << m_sGpioPath << endl;
	
	return 0;
}
