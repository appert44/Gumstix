/**
 *	\file	CUart.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CUart class
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#include "CUart.h"

#ifndef CUART_RX_BUFFER_SIZE
	#define CUART_RX_BUFFER_SIZE	(10*1024)		/**< \brief Maximum size of a handled Uart receive buffer */
#endif

using namespace std;

/**
 *	\fn		CUart :: CUart (const char * sDevicePath, unsigned int iBaud, char * sFormat, bool bRtsCts, bool bXonXoff, bool bCanonicalReading)
 *	\brief	Constructor of the CUart class
 *
 *  \param[in]	sDevicePath : 		Path of the choosen serial port
 *  \param[in]	iBaud :				choose B4800, B9600, B115200, ...
 *  \param[in]	sFormat : 			typically "8N1"
 *  \param[in]	bRtsCts :			typically no : 0
3 *	\param[in]	bXonXoff :			typically no : 0
 *	\param[in]	bCanonicalReading :	typically yes : 1 to obtain an IT (Callback) at each ASCII end of line ("\n" or "\r")
 */
CUart :: CUart (const char * sDevicePath, unsigned int iBaud, char * sFormat, bool bRtsCts, bool bXonXoff, bool bCanonicalReading)
{
	// Save the configuration
	m_uiBaud = iBaud;
	m_sFormat = strdup(sFormat);
	m_bRtsCts = bRtsCts;
	m_bXonXoff = bXonXoff;
	m_bCanonicalReading = bCanonicalReading;
	
	// Init the used attributes
	m_fd = -1;
	m_bThreadRunning = false;
	
	// Saved the path of the serial port which will be used to read GPS frame
	m_sSerialPath = strdup(sDevicePath);
	
	// Init empty pointer
	m_sRxBuffer = NULL;
	
	cout << CUART_PRINT_NAME << "CUart class object has been created" << endl;
}

/** 
 *	\fn		CUart :: ~CUart ()
 *	\brief	Destructor of the CUart class
 */
CUart :: ~CUart ()
{
	// Variable initialization
	int iRet;
	
	// Unregister all the callbacks connected to the CUart object
	UnregisterAllCallbacks(ALL_EVENTS);
	
	// Clean Uart buffer
	iRet = tcflush (m_fd, TCIOFLUSH);
	if (iRet != 0)
		cout << CUART_PRINT_NAME << "Erreur on tcflush" << endl;

	// Restoration of the original configuration of the Uart
	iRet = tcsetattr (m_fd, TCSANOW, &m_oOrigUartConf);
	if (iRet != 0)
		cout << CUART_PRINT_NAME << "Can't apply uart configuration" << endl;
	
	// Closure of the Uart
	if (m_fd != -1)
		close(m_fd);
	
	// Deallocation of the allocated dynamic memory
	if (m_sFormat != NULL)
		free(m_sFormat);
	if (m_sRxBuffer != NULL)
		free(m_sRxBuffer);
	
	cout << CUART_PRINT_NAME << "CUart class object has been destroyed" << endl;
}

/**
 *	\fn		int CUart :: Open ()
 *	\brief	Launch initialization and configuration of a CUart object
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CUart :: Open ()
{
	// Open the serial port
	m_fd = open(m_sSerialPath,  O_NOCTTY | O_RDWR);
	
	// Test if the opening happened well
	if(m_fd < 0) {
		cout << CUART_PRINT_NAME << "Error during the opening of the Uart : " << m_sSerialPath << endl;
		return -ERR_CUART_OPEN_UART_OPEN;
	}
	
	// Allocation of the reception buffer
	m_sRxBuffer = (char *) malloc (CUART_RX_BUFFER_SIZE * sizeof (char));
	
	// Initiates and verifies that the configuration has been correctly applied
	if (Configuration() < 0) {
		cout << CUART_PRINT_NAME << "Error during the configuration of the UART : " << m_sSerialPath << endl;
		return -ERR_CUART_OPEN_UART_CONFIGURATION;
	}
	
	cout << CUART_PRINT_NAME << "Opening of the Uart : " << m_sSerialPath << " successful" << endl;
	
	return 0;
}

/**
 *	\fn		int CUart :: Configuration ()
 *	\brief	Configure the serial port
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CUart :: Configuration ()
{
	// Variables initialization
	int iRet;
	char sBuffer[64];
	
	struct termios oUartConf;
	
	memset (sBuffer, 0, sizeof(sBuffer));

	// Check if the format is good
	if (strlen (m_sFormat) != 3) {
		cout << CUART_PRINT_NAME << "Bad format, use [5-8][NOE][1-2] format, ex : 8N1, 7O2, 8N2" << endl;
		return -ERR_CUART_CONFIGURATION_SPECIFIED_FORMAT;
	}
	
	// Saved the original configuration
	tcgetattr (m_fd, &m_oOrigUartConf);
	
	// Set a basic configuration (solve the board default configuration problem)
	// TODO : Ameliorer et comprendre (cf. ticket 177 pegase)
	sprintf(sBuffer, "stty -F %s 9600", m_sSerialPath);
	system (sBuffer);

	// Get this configuration
	tcgetattr (m_fd, &oUartConf);

	// Set output baudrate
	iRet = cfsetospeed (&oUartConf, m_uiBaud);
	if (iRet != 0) {
		cout << CUART_PRINT_NAME << "Error on cfsetospeed !" << endl;		
		return -ERR_CUART_CONFIGURATION_SET_OUTPUT_BAUDRATE;
	}
	
	// Set input baudrate
	iRet = cfsetispeed (&oUartConf, m_uiBaud);
	if (iRet != 0) {
		cout << CUART_PRINT_NAME << "Error on cfsetispeed !" << endl;		
		return -ERR_CUART_CONFIGURATION_SET_INPUT_BAUDRATE;
	}

	// Set data length
	oUartConf.c_cflag &= ~CSIZE;
	switch (m_sFormat[0]) {
		case '5':
			oUartConf.c_cflag |= CS5; break;
		case '6':
			oUartConf.c_cflag |= CS6; break;
		case '7':
			oUartConf.c_cflag |= CS7; break;
		case '8':
		default:
			oUartConf.c_cflag |= CS8; break;
	}
	
	// Set parity
	switch (m_sFormat[1]) {
		default:
		case 'N':
			oUartConf.c_cflag &= ~PARENB; break;
		case 'O':
			oUartConf.c_cflag |= (PARENB | PARODD); break;
		case 'E':
			oUartConf.c_cflag &= ~PARODD;
			oUartConf.c_cflag |= PARENB; break;
	}

	// Set bit stop length
	switch (m_sFormat[2]) {
		default:
		case '1':
			oUartConf.c_cflag &= ~CSTOPB; break;
		case '2':
			oUartConf.c_cflag |= CSTOPB; break;
	}
	
	// Config rts-cts
	if (m_bRtsCts)
		oUartConf.c_cflag |= CRTSCTS;
	else
		oUartConf.c_cflag &= ~CRTSCTS;

	// Config Xon-Xoff
	if (m_bXonXoff)
		oUartConf.c_iflag |= (IXON | IXOFF | IXANY);
	else
		oUartConf.c_iflag &= ~(IXON | IXOFF | IXANY);
	
	// Canonical reading
	if (m_bCanonicalReading)
		oUartConf.c_lflag |= ICANON;
	else
		oUartConf.c_lflag &= ~ICANON;
	
	// CR NL config
	// TODO : evaluer l'influence
	oUartConf.c_iflag |= IGNCR;

	// No echo
	oUartConf.c_lflag &= ~(ECHO | ECHOE | ECHOK);

	// Enable reception
	oUartConf.c_cflag |= (CREAD | CLOCAL);

	// Clean Uart buffer
	iRet = tcflush (m_fd, TCIOFLUSH);
	if (iRet != 0) {
		cout << CUART_PRINT_NAME << "Erreur on tcflush" << endl;
		return -ERR_CUART_CONFIGURATION_CLEAN_UART_BUFFER;
	}

	// Apply
	iRet = tcsetattr (m_fd, TCSANOW, &oUartConf);
	if (iRet != 0) {
		cout << CUART_PRINT_NAME << "Can't apply uart configuration !" << endl;
		return -ERR_CUART_CONFIGURATION_APPLY_CONFIGURATION;
	}

	return (m_fd);
}

/**
 *	\fn		int CUart :: Write (char * sBuffer, unsigned int uiBufferSize)
 *	\brief	Write data using the serial link
 *
 *  \param[in]	sBuffer : 			Buffer which contains the message to Write
 *  \param[in]	uiBufferSize : 		Integer that indicate the size of the message to Write
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CUart :: Write (char * sBuffer, unsigned int uiBufferSize)
{
	// Init variable
	int iRet;
	// int iCpt;
		
	// Check if the input buffer is empty
	if (uiBufferSize == 0) {
		cout << CUART_PRINT_NAME << "Empty buffer, nothing to Write !" << endl;
		return 0;
	}
	
	// Check if the input buffer pointer is NULL
	if (sBuffer == NULL) {
		cout << CUART_PRINT_NAME << "NULL buffer pointer" << endl;
		return -ERR_CUART_WRITE_SPECIFIED_BUFFER;
	}
	
	// Print the buffer to send
	/*printf("%sBuffer to send :", CUART_PRINT_NAME);
	for (iCpt = 0; iCpt < (int) uiBufferSize; iCpt++)
		printf("%02x ", sBuffer[iCpt]);
	printf("\n\n");*/
	
	// Launch and check if the write went well
	iRet = write (m_fd, sBuffer, uiBufferSize);
	if (iRet < 0) {	
		cout << CUART_PRINT_NAME << "Error " << iRet << " during the write" << endl;		
		return -ERR_CUART_WRITE_UART_WRITE;
	}
	
	// cout << CUART_PRINT_NAME << "Write went well" << endl << endl;
	
	return 0;
}

/**
 *	\fn		int CUart :: ChangePortConfiguration (unsigned int iBaud, char * sFormat, bool bRtsCts, bool bXonXoff, bool bCanonicalReading)
 *	\brief	Constructor of the CUart class
 *
 *  \param[in]	iBaud :				choose B4800, B9600, B115200, ...
 *  \param[in]	sFormat : 			typically "8N1"
 *  \param[in]	bRtsCts :			typically no : 0
 *	\param[in]	bXonXoff :			typically no : 0
 *	\param[in]	bCanonicalReading :	typically yes : 1 to obtain an IT (Callback) at each ASCII end of line ("\n" or "\r")
 */
int CUart :: ChangePortConfiguration (unsigned int iBaud, char * sFormat, bool bRtsCts, bool bXonXoff, bool bCanonicalReading)
{
	// Save the configuration
	m_uiBaud = iBaud;
	m_sFormat = strdup(sFormat);
	m_bRtsCts = bRtsCts;
	m_bXonXoff = bXonXoff;
	m_bCanonicalReading = bCanonicalReading;
		
	// Initiates and verifies that the configuration has been correctly applied
	if (Configuration() < 0) {
		cout << CUART_PRINT_NAME << "Error during the configuration of the UART : " << m_sSerialPath << endl;
		return -ERR_CUART_OPEN_UART_CONFIGURATION;
	}
	
	return 0;
}

/**
 *	\fn		int CUart :: RegisterCallback (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2)
 *  \brief	Register a Callback which will be called when the event specified in EnventID will occure
 *
 *  \param[in]	uiEventId : 	Integer that indicate the event ID on which the function will be launched
 *  \param[in]	oFct : 			Object which indicates the function to launch when a event occure
 *  \param[in] 	pData1 : 		User pointer 1
 *  \param[in] 	pData2 : 		User pointer 2
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CUart :: RegisterCallback (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2)
{
	// Variable initialization
	bool b_ThreadLaunching = false;
	
	// If the boost::signal is empty
	if (m_oSigOnRec.num_slots() == 0) {
		if (m_bThreadRunning) {
			cout << CUART_PRINT_NAME << "Error, the thread is already launched while no callbacks are connected" << endl;
			// Indicate to the thread it should stop
			m_bThreadRunning = false;
			// Wait for the thread end
			m_oThreadRx.join();
			return -ERR_CUART_REGISTERCALLBACK_THREAD_MANAGING;
		}
		
		// Indicate that the thread should be launch
		b_ThreadLaunching = true;
	}
	
	// Check the event 
	if (uiEventId == EVENT_CUART_RECEIVEDATA) {
		// Connect the passed function
		m_oLastFctId = m_oSigOnRec.connect(oFct);
		
		// Checks if the function was well connected
		if(m_oLastFctId.connected()) {
			cout << CUART_PRINT_NAME << "Callback correctly registered" << endl;
			if (b_ThreadLaunching) {
				// Indicate to the thread it should run
				m_bThreadRunning = true;
				// And launch the data reader thread
				boost::thread m_oThreadRx(boost::bind(&CUart::OnReceive, this));
			}
			
			// User parameters
			m_pData1 = pData1;
			m_pData2 = pData2;
			
			return 0;
		}
		else {
			cout << CGPIO_PRINT_NAME <<  "The callback could not be correctly registered" << endl;
			return -ERR_CUART_REGISTERCALLBACK_REGISTERING;
		}
	}
	// Unknown event
	else {
		cout << CUART_PRINT_NAME << "Wrong event ID" << endl;
		return -ERR_CUART_REGISTERCALLBACK_SPECIFIED_EVENT;
	}
}

/**
 *	\fn		int CUart :: UnregisterCallback (unsigned int uiEventId, boost::signals2::connection oIdFct)
 *  \brief	Unregister a previously register callback using its connection Id
 *
 *  \param[in]	uiEventId : 	Integer that indicate the event ID on which the function is launched
 *  \param[in]	oIdFct : 		Object which indicates the connection ID of the function to unregister
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CUart :: UnregisterCallback (unsigned int uiEventId, boost::signals2::connection oIdFct)
{
	// Variable initialization
	int iNumBefore, iNumAfter;
	
	// Check the event
	if (uiEventId == EVENT_CUART_RECEIVEDATA) {
		// Checks if there are some connected functions
		if (m_oSigOnRec.num_slots() == 0) {
			cout << CUART_PRINT_NAME << "No Callbacks are connected on Uart data reception event" << endl;
			return -ERR_CUART_UNREGISTERCALLBACK_NO_CALLBACK;
		}
		
		// Saved the number of connected function before the deconnexion
		iNumBefore = m_oSigOnRec.num_slots();
		
		// Disconnect the function using its connexion ID
		oIdFct.disconnect();
		
		// Saved the number of connected function after the deconnexion
		iNumAfter = m_oSigOnRec.num_slots();
	}
	// Unknown event
	else {
		cout << CUART_PRINT_NAME << "Wrong event ID" << endl;
		return -ERR_CUART_UNREGISTERCALLBACK_SPECIFIED_EVENT;
	}
	
	// Checks if the function was well disconnected
	if (iNumBefore == iNumAfter+1) {
		// If there is no longer connected functions to the boost::signals
		cout << CUART_PRINT_NAME << "Callback correctly unregistered" << endl;
		
		// If there is no longer connected functions to the boost::signals
		if (m_oSigOnRec.num_slots() == 0) {
			// Indicate to the thread it should stop
			m_bThreadRunning = false;
			// Wait for the thread end
			m_oThreadRx.join();
		}
		
		return 0;
	}
	else {
		cout << CUART_PRINT_NAME << "The callback could not be correctly unregistered" << endl;
		return -ERR_CUART_UNREGISTERCALLBACK_UNREGISTERING;
	}
}

/**
 *	\fn		int CUart :: UnregisterAllCallbacks (unsigned int uiEventId)
 *  \brief	Unregister all previously register callback on a specific event
 *
 *  \param[in]	uiEventId : 	Integer that indicate the event ID to empty
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CUart :: UnregisterAllCallbacks (unsigned int uiEventId)
{
	// Check the event
	if (uiEventId == ALL_EVENTS || uiEventId == EVENT_CUART_RECEIVEDATA) {
		// Checks if there are some connected functions
		if (m_oSigOnRec.num_slots() == 0) {
			cout << CUART_PRINT_NAME << "No callbacks are currently connected" << endl;
			return -ERR_CUART_UNREGISTERALLCALLBACKS_NO_CALLBACK;
		}
		else {
			// Disconnect all the functions form this specific events
			m_oSigOnRec.disconnect_all_slots();
			
			// Checks if all the functions was well disconnected
			if (m_oSigOnRec.num_slots() == 0)
			{
				// cout << CUART_PRINT_NAME << "Every Callbacks correctly disconnected" << endl;
				
				// Indicate to the thread it should stop
				m_bThreadRunning = false;
				// Wait for the thread end
				m_oThreadRx.join();
				
				return 0;
			}
			else {
				cout << CUART_PRINT_NAME << "The callbacks could not be correctly unregistered" << endl;
				return -ERR_CUART_UNREGISTERALLCALLBACKS_UNREGISTERING;
			}
		}
	}
	// Unknown event
	else {
		cout << CUART_PRINT_NAME << "Wrong event ID" << endl;
		return -ERR_CUART_UNREGISTERALLCALLBACKS_SPECIFIED_EVENT;
	}	
}

/**
 *	\fn		boost::signals2::connection CUart :: GetLastFctId ()
 *  \brief	Send back the last connected callback connexion Id
 *
 *	\return	The last connected callback connexion Id
 */
boost::signals2::connection CUart :: GetLastFctId ()
{
	return m_oLastFctId;
}

/**
 *	\fn		void CUart :: OnReceive()
 *	\brief	Method who manage the launch of callbacks when data are received on the serial link
 */
void CUart :: OnReceive()
{
	// Variables initialization
	int iNreaded, iRet;
	
	struct pollfd oPollFds;
	CEvent * pEvent = NULL;
	
	// While we haven't said to the thread to stop
	do {
		// Poll initialization		
		oPollFds.fd = m_fd;
		oPollFds.events = POLLIN;
		oPollFds.revents = 0;
	
		// Launch the poll (which remplace a blocking reading)
		iRet = poll (&oPollFds, 1, 50);
		
		// Check why the poll has been finished
		if (iRet > 0) {
			// If it's for a rising edge, that's means a data has been received on the uart
			if (oPollFds.revents | POLLIN) {
				// Create a new CEvent object
				pEvent = new CEvent;
				
				// Reset the reception buffer
				memset (m_sRxBuffer, 0, CUART_RX_BUFFER_SIZE);
				
				// Read the data (and check how many bytes has been received)
				iNreaded = read (m_fd, m_sRxBuffer, CUART_RX_BUFFER_SIZE);
				
				// cout << CUART_PRINT_NAME << "A data has been readed on the Uart : " << m_sRxBuffer << "and its size is : " << iNreaded << endl;
				
				// Launch the connected callbacks
				m_oSigOnRec(m_sRxBuffer, iNreaded, pEvent, m_pData1, m_pData2);
				
				// Delete the previously created CEvent object
				delete(pEvent);
			}
		}
		else {
			// Tell if the poll timeout has been reached
			// cout << CUART_PRINT_NAME << "'Poll' timeout reached" << endl;
		}
	} while (m_bThreadRunning);
}

/**
 *	\fn		void CUart :: PrintConfig()
 *	\brief	Method which print the configuration of the serial port
 */
int CUart :: PrintConfig()
{
	// Variable initialization
	char sBuffer[64];
	
	cout << endl;
	
	// Print the Uart configuration using the 'stty -F' system calls
	sprintf(sBuffer, "stty -F %s", m_sSerialPath);
	system (sBuffer);
	
	cout << endl;
	
	return 0;
}
