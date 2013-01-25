/**
 *	\file	CGPS.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CGPS class
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.2a
 *
 *	\date 25 May, 2012
 *
 */

#include "CGPS.h"

using namespace std;

/**
 *	\fn		CGPS :: CGPS(const char * sDevicePath, unsigned int iBaud, char * sFormat, bool bRtsCts, bool bXonXoff)
 *	\brief	Constructor of the CGPS class
 *
 *	\param[in]	sDevicePath :		Path of the serial port on which the GPS is connected
 *	\param[in]	iBaud :				Choose the baudrate : B4800, B9600, B115200, ...
 *  \param[in]	sFormat : 			Typically "8N1"
 *  \param[in]	bRtsCts :			Typically no : 0 (or false)
 *	\param[in]	bXonXoff :			Typically no : 0 (or false)
 */
CGPS :: CGPS(const char * sDevicePath, unsigned int iBaud, char * sFormat, bool bRtsCts, bool bXonXoff)
{	
	// Init print booleans
	m_bPrintGga = false;
	m_bPrintZda = false;
	m_bPrintRmc = false;
	
	// Init frame activation booleans
	m_bActivateGbsFrame = false;
	m_bActivateGgaFrame = true;
	m_bActivateGllFrame = false;
	m_bActivateGrsFrame = false;
	m_bActivateGsaFrame = true;
	m_bActivateGstFrame = true;
	m_bActivateGsvFrame = false;
	m_bActivateRmcFrame = false;
	m_bActivateVtgFrame = false;
	m_bActivateZdaFrame = true;
	
	// Init the thread launching boolean
	m_bThreadRunning = false;
	
	// Init the hour which will launch the SpecHourCallback boolean
	m_bOnSpecHour = false;
	
	// Init the reconfiguration indicator boolean
	m_bHasBeenReconfigure = false;
	
	// Init the booleans used to communicate with the Synchronization driver
	m_bSynchronizationDriverConnected = false;
	m_bGgaDataReceived = false;
	m_bZdaDataReceived = false;
	
	m_uiSpecUtcHour = 10;
	m_uiSpecUtcMinute = 0;
	m_uiSpecUtcSecond = 0;

	// Init Zda
	ZdaFrame.uiDay = 20;
	ZdaFrame.uiMonth = 12;
	ZdaFrame.uiYear = 1989;
	ZdaFrame.uiHour = 13;
	ZdaFrame.uiMinute = 28;
	ZdaFrame.uiSecond = 22;
	ZdaFrame.uiMilliSecond = 75;

	// Init Gga
	GgaFrame.uiHour = 13;
	GgaFrame.uiMinute = 28;
	GgaFrame.uiSecond = 22;
	GgaFrame.uiMilliSecond = 62;
	GgaFrame.dLatitude = 0;
	GgaFrame.dLongitude = 0;
	GgaFrame.cLatitudeRef = '\0';
	GgaFrame.cLongitudeRef = '\0';
	GgaFrame.dHDOP = 0;
	GgaFrame.dAltitude = 0;
	GgaFrame.dMSL = 0;
	GgaFrame.uiDGpsId  = 0;
	GgaFrame.dDGpsTime  = 0;
	GgaFrame.uiQuality = 0;
	GgaFrame.uiNbSat = 0;
	
	// Init Rmc
	RmcFrame.uiHour = 13;
	RmcFrame.uiMinute = 28;
	RmcFrame.uiSecond = 22;
	RmcFrame.uiMilliSecond = 75;
	RmcFrame.dLatitude = 0;
	RmcFrame.dLongitude = 0;
	RmcFrame.cLatitudeRef = '\0';
	RmcFrame.cLongitudeRef = '\0';
	RmcFrame.dSpeed = 0;
	RmcFrame.dCape = 0;
	RmcFrame.uiDay  = 20;
	RmcFrame.uiMonth  = 12;
	RmcFrame.uiYear = 89;
	
	// Saved the serial information about the GPS
	m_sDevicePath = strdup(sDevicePath);
	m_uiBaud = iBaud;
	m_sFormat = strdup(sFormat);
	m_bRtsCts = bRtsCts;
	m_bXonXoff = bXonXoff;
	
	// Init empty pointer
	m_pSerial = NULL;
	
	cout << CGPS_PRINT_NAME << "CGPS class object has been created" << endl;
}

/**
 *	\fn		CGPS :: ~CGPS()
 *	\brief	Destructor of the CGPS class
 */
CGPS :: ~CGPS()
{
	// Unregister all the callbacks connected to the CGPS object
	UnregisterAllCallbacks(ALL_EVENTS);
	
	if (m_pSerial != NULL)
		delete(m_pSerial);
	
	// Deallocation of the dynamicly allocated memory
	if (m_sDevicePath != NULL)
		free(m_sDevicePath);
	if (m_sFormat != NULL)
		free(m_sFormat);
	
	// Close the link with the synchonization driver
	if (m_fd >= 0)
		close (m_fd);
		
	cout << CGPS_PRINT_NAME << "CGPS class object has been destroyed" << endl;
}

/**
 *	\fn		int CGPS :: Open ()
 *	\brief	Initialization of a CGPS object
 *
 *	\return	A negative number if an error occured, 0 otherwise
 */
int CGPS :: Open ()
{
	// Variable initialization
	int iRet = 0, iGpsShutdownGpioFd;
	
	cout << CGPS_PRINT_NAME << "CGPS class object initialization in progress ..." << endl;
	
	m_fd = open("/dev/synchronization", O_RDWR);
	// If the Ifsttar Synchronization dirver is loaded
	if (m_fd > 0)
		m_bSynchronizationDriverConnected = true;
	else {
		m_bSynchronizationDriverConnected = false;
		
		// Configure the Gps software shutdown Gpio
		iGpsShutdownGpioFd = open("/dev/gpio067", O_RDWR);
		// If the Ifsttar Gpio driver isn't loaded
		if (iGpsShutdownGpioFd < 0) {
			// Muxing th Gpio output pin
			system("devmem2 0x480020D6 h 0x010C");
			// Test if the access to the gpio is already configured
			iRet = open("/sys/class/gpio/gpio67/direction", O_RDWR);
			if (iRet < 0) {			
				// Configure the access to the gpio
				iRet = system("echo 67 > /sys/class/gpio/export");
				if (iRet < 0) {
					cout << CGPS_PRINT_NAME << "Error on software shutdown gpio configuration (add of the gpio in /sys/class/gpio)" << endl;
					return -ERR_CGPS_OPEN_GPIO_CONFIGURATION;
				}
			}
			close (iRet);
			// Configure the gpio direction
			iRet = system("echo out > /sys/class/gpio/gpio67/direction");
			if (iRet < 0) {
				cout << CGPS_PRINT_NAME << "Error on software shutdown gpio configuration (set out direction)" << endl;
				return -ERR_CGPS_OPEN_GPIO_CONFIGURATION;
			}
			// Configure the gpio value
			iRet = system("echo 1 > /sys/class/gpio/gpio67/value");
			if (iRet < 0) {
				cout << CGPS_PRINT_NAME << "Error on software shutdown gpio configuration (set the value to 1)" << endl;
				return -ERR_CGPS_OPEN_GPIO_CONFIGURATION;
			}
		}
		else {
			// Configure the Gpio direction
			iRet = ioctl(iGpsShutdownGpioFd, GPIO_IOCTL_SET_DIRECTION, OUTPUT);
			if (iRet < 0) {
				cout << CGPS_PRINT_NAME << "Error on software shutdown gpio configuration (set out direction with ifsttar driver)" << endl;
				return -ERR_CGPS_OPEN_GPIO_CONFIGURATION;
			}
			// Configure the Gpio value
			iRet = ioctl(m_fd, GPIO_IOCTL_SET_OUTPUT_VALUE, 1);
			if (iRet < 0) {
				cout << CGPS_PRINT_NAME << "Error on software shutdown gpio configuration (set the value to 1)" << endl;
				return -ERR_CGPS_OPEN_GPIO_CONFIGURATION;
			}
		}
	}
	
	// Create the Uart object used to communicate with the Gps module
	m_pSerial = new CUart (m_sDevicePath, m_uiBaud, m_sFormat, m_bRtsCts, m_bXonXoff, 1);
	
	// Test if the uart creation happened well
	if (m_pSerial == NULL) {
		cout << CGPS_PRINT_NAME << "Error on Uart construction" << endl;
		return -ERR_CGPS_OPEN_UART_CREATION;
	}
	
	// Initialize the reconfiguration Uart object
	iRet = m_pSerial->Open();
	if (iRet < 0) {
		cout << CGPS_PRINT_NAME << "Error " << iRet << " on Uart initialization" << endl;
		return -ERR_CGPS_OPEN_UART_OPENING;
	}

	// Add the launch of the GpsReception method when a data is received on the Uart
	boost::function<CallBack> oOnCallBack = boost::bind(&CGPS::GpsReception, this, _1, _2, _3, _4, _5);
	if (m_pSerial->RegisterCallback(EVENT_CUART_RECEIVEDATA, oOnCallBack, NULL, NULL) != 0) {
		cout << CGPS_PRINT_NAME << "Faillure during the registration of reconfiguration CUart reception callback" << endl;
		return -ERR_CGPS_OPEN_UART_CALLBACK_REGISTRATION;
	}

	// Get an Id to disconect the GpsReception method later
	m_oIdCallbackUart = m_pSerial->GetLastFctId();
	
	if (!m_bHasBeenReconfigure) {
		// Reconfigure the Gps module from its default configuration
		iRet = GpsReconfigurationFromDefault ();
		if (iRet < 0) {
			cout << CGPS_PRINT_NAME << "Error " << iRet << " on gps module reconfiguration from its default configuration" << endl;
			return -ERR_CGPS_OPEN_GPS_RECONFIGURATION_FROM_DEFAULT;
		}
	}
	
	if (!m_bSynchronizationDriverConnected) {
		cout << CGPS_PRINT_NAME << "WARNING : Failed to contact the synchronization driver, error " << m_fd << endl;
		return ERR_CGPS_OPEN_SYNCHRONIZATION_DRIVER_LINK;
	}
	
	return 0;
}

/**
 *	\fn		int CGPS :: GpsReconfigurationFromDefault ()
 *	\brief	Reconfigure the Gps module from its default configuration
 *
 *	\return	A negative number if an error occured, 0 otherwise
 */
int CGPS :: GpsReconfigurationFromDefault ()
{
	// Variable initialization
	int iRet = 0;
	
	// Initialize the GpsConfigFrame
	GpsConfigFrame.ui16Sync = htobe16(UBX);				// Little Endian (host) to Big Endian
	GpsConfigFrame.ui16ClassId = 0;
	GpsConfigFrame.ui16Lenght = 0;
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);
	
	// Reconfigure the Uart to communicate with the Gps module (with its default configuration)
	iRet = m_pSerial->ChangePortConfiguration (B9600, m_sFormat, m_bRtsCts, m_bXonXoff, 1);
	if (iRet < 0) {
		printf("%sFaillure during the Uart reconfiguration, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSRECONFIGURATIONFROMDEFAULT_UART_RECONFIGURATION;
	}
	
	// Launch the Gps module reconfiguration
	iRet = GpsUbxConfiguration ();
	if (iRet < 0) {
		printf("%sFaillure during the Gps reconfiguration, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSRECONFIGURATIONFROMDEFAULT_GPS_RECONFIGURATION;
	}
	
	// Wait for Gps acknowledges
	sleep(2);
	
	// Reconfigure the Uart to communicate with the Gps module (with its new configuration)
	iRet = m_pSerial->ChangePortConfiguration (m_uiBaud, m_sFormat, m_bRtsCts, m_bXonXoff, 1);
	if (iRet < 0) {
		printf("%sFaillure during the Uart reconfiguration, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSRECONFIGURATIONFROMDEFAULT_UART_RECONFIGURATION;
	}
	
	// Wait for Gps acknowledge
	sleep(2);
	
	// Save configuration
	GpsConfigFrame.ui16ClassId = htobe16(CFG_CFG);		// Configuration management message Id
	GpsConfigFrame.ui16Lenght = 13;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send

	// Complete the buffer to send saying that the all configuration must be saved
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c%c%c%c%c%c", 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0xFF);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSRECONFIGURATIONFROMDEFAULT_SAVE_NEW_GPS_CONFIGURATION;
	}
	
	m_bHasBeenReconfigure = true;
	
	return 0;
}

/**
 *	\fn		bool CGPS :: NmeaVerifyChecksum (char * sBuffer, int iBufferSize)
 *	\brief	Nmea Checksum verification method
 *
 *  \param[in]	sBuffer : 			Buffer which contains the message to check
 *  \param[in]	iBufferSize : 		Integer that indicate the size of the message to check
 *
 *	\return	A boolean which indicates if the Checksum is valid
 */
bool CGPS :: NmeaVerifyChecksum (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	unsigned char ucChecksum = 0;
	unsigned int uiIndex = 1;
	char sSum[3];
	
	// End of the method if the frame sent doesn't exist
	if (sBuffer ==  NULL)
		return true;

	// End of the method if the frame is empty
	if (iBufferSize <=  0)
		return true;

	// End of the method if the frame doesn't had an '*'
    if (strchr(sBuffer, '*') == NULL)
        return true;
	
	// While the '*' isn't reached
	do {
		// XOR operation
		ucChecksum ^= sBuffer[uiIndex];
		// Next character
		uiIndex ++;

		// End of the method if frame is badly formed
		if ((int) uiIndex >= iBufferSize)
			return true;

	} while (sBuffer[uiIndex] != '*');

	// End of the method if the frame is too long
	if (((int) uiIndex + 2) >= iBufferSize)
		return true;
	
	// If the calculate Checksum is the same as in the end of the GPS trame
	(void) snprintf (sSum, sizeof(sSum), "%02X", ucChecksum);
	if (sSum[0] == toupper (sBuffer[uiIndex+1]) && sSum[1] == toupper(sBuffer[uiIndex+2]))
			// Send back a 0 to tell that the Checksum is good
			return false;
	// Otherwise, send back a 1 to tell that the Checksum is wrong
	else	return true;
}

/**
 *	\fn		bool CGPS :: UbxVerifyChecksum (char * sBuffer, int iBufferSize)
 *	\brief	Ubx Checksum verification method
 *
 *  \param[in]	sBuffer : 			Buffer which contains the message to check
 *  \param[in]	iBufferSize : 		Integer that indicate the size of the message to check
 *
 *	\return	A boolean which indicates if the Checksum is valid
 */
bool CGPS :: UbxVerifyChecksum (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	int iCpt;
	unsigned char ucChecksumA = 0, ucChecksumB = 0;
	
	// Check the checksum of the frame using the verification method described in the documentation of the uBlox Gps module
	for (iCpt = 2; iCpt < iBufferSize-2; iCpt++) {
		ucChecksumA += sBuffer[iCpt];
		ucChecksumB += ucChecksumA;
	}
	
	// If the checksum is valid
	if (ucChecksumA == sBuffer[iBufferSize-2] && ucChecksumB == sBuffer[iBufferSize-1])
		return true;
	// Otherwise
	else
		return false;
}

/**
 *	\fn		bool CGPS :: AddUbxChecksum (char * sBuffer, int iBufferSize)
 *	\brief	Ubx Checksum generation method
 *
 *	\return	A boolean which indicates if the operation went well
 */
bool CGPS :: AddUbxChecksum (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	int iCpt;
	unsigned char ucChecksumA = 0, ucChecksumB = 0;
	
	// Generate the checksum of the frame using the verification method described in the documentation of the uBlox Gps module
	for (iCpt = 2; iCpt < iBufferSize-2; iCpt++) {
		ucChecksumA += sBuffer[iCpt];
		ucChecksumB += ucChecksumA;
	}
	
	// Add the checksum at the end of the buffer
	sBuffer[iBufferSize-2] = ucChecksumA;
	sBuffer[iBufferSize-1] = ucChecksumB;
	
	// printf("%s Checksum A : %02x, Checksum B : %02x\n", CGPS_PRINT_NAME, ucChecksumA, ucChecksumB);
	
	return true;
}

/**
 *	\fn		int CGPS :: GetUbxFrameSize ()
 *	\brief	Send back the size of the Ubx frame to send.
 *
 *	\return	The size of the Ubx frame to send
 */
int CGPS :: GetUbxFrameSize ()
{
	// Check if the frame is not too long
	if (GpsConfigFrame.ui16Lenght > UBX_BUFFER_SIZE)
		return -ERR_CGPS_GETUBXFRAMESIZE_FRAME_TOO_LONG;
	// Otherwise
	else
		// Send back the size of the frame, which is the size of buffer + 8 (Ubx Id (2) + message class + message id + length of the payload (2) + checksum (2))
		return GpsConfigFrame.ui16Lenght+8;
}

/**
 *	\fn		int CGPS :: SendUbxFrame ()
 *	\brief	Send the message currently in the GpsConfigFrame structure
 *
 *	\return	A negative number if an error occured, 0 otherwise
 */
int CGPS :: SendUbxFrame ()
{
	// Check if the frame is not too long
	if (GpsConfigFrame.ui16Lenght > UBX_BUFFER_SIZE)
		return -ERR_CGPS_SENDUBXFRAME_FRAME_TOO_LONG;
	// Check if its an Ubx frame
	else if (be16toh(GpsConfigFrame.ui16Sync) != UBX)
		return -ERR_CGPS_SENDUBXFRAME_NOT_UBX_FRAME;
	// Check if the frame have an Id
	else if (GpsConfigFrame.ui16ClassId == 0)
		return -ERR_CGPS_SENDUBXFRAME_WRONG_CLASS_ID;
	
	// Add the checksum to the frame
	AddUbxChecksum ((char *) &GpsConfigFrame, GetUbxFrameSize());
	
	// Send it
	m_pSerial->Write((char *) &GpsConfigFrame, GetUbxFrameSize());
	
	return 0;
}

/**
 *	\fn		int CGPS :: GpsUbxConfiguration ()
 *	\brief	Reconfigure the GPS module
 *
 *	\return	A negative number if an error occured, 0 otherwise
 */
int CGPS :: GpsUbxConfiguration ()
{
	// Variables initialization
	int iRet, iCpt;
	bool bActualFrame;
	uint16_t ui16ClassId;
	
	// Configurations of the ouputs Nmea frames using GpsConfigFrame
	for (iCpt = 0; iCpt < 10; iCpt++) {
		switch (iCpt) {
			// For Gbs frame
			case 0 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateGbsFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_GBS;
				break;
			// For Gga frame
			case 1 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateGgaFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_GGA;
				break;
			// For Gll frame
			case 2 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateGllFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_GLL;
				break;
			// For Grs frame
			case 3 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateGrsFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_GRS;
				break;
			// For Gsa frame
			case 4 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateGsaFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_GSA;
				break;
			// For Gst frame
			case 5 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateGstFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_GST;
				break;
			// For Gsv frame
			case 6 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateGsvFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_GSV;
				break;
			// For Rmc frame
			case 7 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateRmcFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_RMC;
				break;
			// For Vtg frame
			case 8 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateVtgFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_VTG;
				break;
			// For Zda frame
			case 9 :
				// Check if the Nmea frame must be activated
				bActualFrame = m_bActivateZdaFrame;
				// Copy the Nmea frame Id in the GpsConfigFrame structure
				ui16ClassId = DEF_ZDA;
				break;
		}
		
		// Configure the GpsConfigFrame
		GpsConfigFrame.ui16ClassId = htobe16(CFG_MSG);		// Nmea frame reconfiguration message Id
		GpsConfigFrame.ui16Lenght = 8;						// Size of the frame
		memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
		
		// Deactivate the frame if its boolean is set to 'false'
		if (bActualFrame == false)
			// Complete the buffer to send saying that frame must be deactivated on all the communication port
			sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c", (ui16ClassId & 0xff00) >> 8, (ui16ClassId & 0x00ff), 0, 0, 0, 0, 0, 0);
		// Activate it otherwise
		else
			// Complete the buffer to send saying that frame must be activated only on the serial communication port (Uart 1)
			sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c", (ui16ClassId & 0xff00) >> 8, (ui16ClassId & 0x00ff), 0, 1, 0, 0, 0, 0);
	
		// Send the reconfiguration frame
		iRet = SendUbxFrame ();
		// And check if the operation went well
		if (iRet != 0) {
			printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
			return -(ERR_CGPS_GPSUBXCONFIGURATION_NMEA_GBS_RECONFIGURATION+iCpt);
		}
	}
	
	// De-activate DDC (I2C) port
	GpsConfigFrame.ui16ClassId = htobe16(CFG_PRT);		// Port reconfiguration message Id
	GpsConfigFrame.ui16Lenght = 20;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
	
	// Complete the buffer to send saying that the communication must be disabled on the DDC (I2C) port
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0, 0, 0, 2, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSUBXCONFIGURATION_DDC_PORT_RECONFIGURATION;
	}
	
	// De-activate SPI port
	GpsConfigFrame.ui16ClassId = htobe16(CFG_PRT);		// Port reconfiguration message Id
	GpsConfigFrame.ui16Lenght = 20;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
	
	// Complete the buffer to send saying that the communication must be disabled on the SPI port
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 4, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSUBXCONFIGURATION_SPI_PORT_RECONFIGURATION;
	}
	
	// De-activate USB port
	GpsConfigFrame.ui16ClassId = htobe16(CFG_PRT);		// Port reconfiguration message Id
	GpsConfigFrame.ui16Lenght = 20;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
	
	// Complete the buffer to send saying that the communication must be disabled on the USB port
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 3, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSUBXCONFIGURATION_USB_PORT_RECONFIGURATION;
	}
	
	// De-activate Uart 2 port
	GpsConfigFrame.ui16ClassId = htobe16(CFG_PRT);		// Port reconfiguration message Id
	GpsConfigFrame.ui16Lenght = 20;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
	
	// Complete the buffer to send saying that the communication must be disabled on the Uart 2 port
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 2, 0, 0, 0, 0xc0, 8, 0, 0, 0x80, 0x25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSUBXCONFIGURATION_UART2_PORT_RECONFIGURATION;
	}
	
	// TODO : vérifier antenne active
	// Reconfigure the antenna settings
	GpsConfigFrame.ui16ClassId = htobe16(CFG_ANT);		// Antenna reconfiguration message Id
	GpsConfigFrame.ui16Lenght = 4;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
	
	// Complete the buffer to send saying to reconfigure correctly the antenna settings
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c", 0x17, 0, 0x8b, 0xa9);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSUBXCONFIGURATION_ANTENNA_RECONFIGURATION;
	}
	
	// TODO : à améliorer (consommation du Gps en veille)
	// Reconfigure the Extended Power Management settmings
	GpsConfigFrame.ui16ClassId = htobe16(CFG_PM2);		// Extended power management reconfiguration message Id
	GpsConfigFrame.ui16Lenght = 44;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
	
	// Complete the buffer to send saying to reconfigure correctly the extended power management settings
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x01, 0x06, 0x00, 0x00, 0x60, 0x90, 0x02, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x2c, 0x01, 0x00, 0x00, 0x4f, 0xc1, 0x03, 0x00, 0x86, 0x02, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSUBXCONFIGURATION_POWER_MANAGEMENT_RECONFIGURATION;
	}
	
	// Reconfigure the Receiver Manager settmings
	GpsConfigFrame.ui16ClassId = htobe16(CFG_RXM);		// Receiver manager reconfiguration message Id
	GpsConfigFrame.ui16Lenght = 2;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
	
	// Complete the buffer to send saying to reconfigure correctly the receiver manager settings
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c", 0x08, 0x01);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSUBXCONFIGURATION_RECEIVER_MANAGER_RECONFIGURATION;
	}
	
	// Reconfigure Uart 1 port
	GpsConfigFrame.ui16ClassId = htobe16(CFG_PRT);		// Port reconfiguration message Id
	GpsConfigFrame.ui16Lenght = 20;						// Size of the frame
	memset(GpsConfigFrame.sPayloadAndChecksums, '\0', UBX_BUFFER_SIZE);		// Reset the buffer to send
	
	// Complete the buffer to send saying that the communication must be enable at 115200 bauds on the Uart 1 port
	sprintf(GpsConfigFrame.sPayloadAndChecksums, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 1, 0, 0, 0, 0xc0, 8, 0, 0, 0, 0xc2, 1, 0, 7, 0, 7, 0, 0, 0, 0, 0);
	
	// Send the reconfiguration frame
	iRet = SendUbxFrame ();
	// And check if the operation went well
	if (iRet != 0) {
		printf("%sFail during the Gps reconfiguration frame send, error %d\n", CGPS_PRINT_NAME, iRet);
		return -ERR_CGPS_GPSUBXCONFIGURATION_UART1_PORT_RECONFIGURATION;
	}
	
	return 0;
}

/**
 *	\fn		void CGPS :: GpsDecodeGga (char * sBuffer, int iBufferSize)
 *	\brief	Gga frame decoding method
 *
 *  \param[in]	sBuffer : 			Buffer which contains the message to decode
 *  \param[in]	iBufferSize : 		Integer that indicate the size of the message to decode
 */
void CGPS :: GpsDecodeGga (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	unsigned int uiTime;
	unsigned char ucIndex = 0;
	char * sDebut, * sFin, * sOrigBuffer;
	const char * sGps[] = {"No GPS", "GPS", "DGPS", "Bad syntax"};
		
	static CEvent * pEvent = NULL;
	
	Gga GgaFrameTemp;

	// cout << CGPS_PRINT_NAME << "Decoding Nmea-Gga data" << endl;

	// Initialization
	memset (&GgaFrameTemp, 0, sizeof (Gga));
	sOrigBuffer = strdup(sBuffer);
	pEvent = new CEvent;

	// Data extraction
	sDebut = sBuffer;
	sFin = sBuffer;
	// While the the end of the frame is not reached
	while (* sFin != '\0') {
		// If we detect the end of a field
		if (* sFin == ',' || *sFin == '*') {
			* sFin = '\0';

			// Token treatment
			if (ucIndex == 1) {
				// Hour, minute, second, milliseond
				uiTime = atoi(sDebut);
				GgaFrameTemp.uiHour = uiTime / 10000;
				GgaFrameTemp.uiMinute = (uiTime / 100) % 100;
				GgaFrameTemp.uiSecond = uiTime % 100;
				GgaFrameTemp.uiMilliSecond = atoi(sFin-2)*10;
			} else if (ucIndex == 2)
				// Latitude
				GgaFrameTemp.dLatitude = atof (sDebut);
			else if (ucIndex == 3)
				// Latitude reference
				GgaFrameTemp.cLatitudeRef = *sDebut;
			else if (ucIndex == 4)
				// Longitude
				GgaFrameTemp.dLongitude = atof (sDebut);
			else if (ucIndex == 5)
				// Longitude reference
				GgaFrameTemp.cLongitudeRef = *sDebut;
			else if (ucIndex == 6)
				// Quality
				GgaFrameTemp.uiQuality = atoi(sDebut);
			else if (ucIndex == 7)
				// Satellite number
				GgaFrameTemp.uiNbSat = atoi(sDebut);
			else if (ucIndex == 8)
				// Horizontal Dilution of Precision
				GgaFrameTemp.dHDOP = atof (sDebut);
			else if (ucIndex == 9)
				// Altitude
				GgaFrameTemp.dAltitude = atof (sDebut);
			else if (ucIndex == 11)
				// MSL
				GgaFrameTemp.dMSL = atof (sDebut);
			else if (ucIndex == 13)
				// DGPS Time
				GgaFrameTemp.dDGpsTime = atof (sDebut);
			else if (ucIndex == 14) {
				// DGPS Id
				GgaFrameTemp.uiDGpsId = atoi (sDebut);
				break;
			}
			// End of the token treatment

			ucIndex ++;
			sDebut = sFin + 1;
		}
		sFin++;
	}
	
	// Check if the hour has change	
	if (GgaFrame.uiHour != GgaFrameTemp.uiHour) {
		m_bHourChange = true;
		
		// Check if the day has change
		if (GgaFrameTemp.uiHour == 0)
			m_bDayChange = true;
	}
	
	// Copy the data into the class
	(void) memcpy (&GgaFrame, &GgaFrameTemp, sizeof (Gga));
	
	// If the display is set, print the content of the Gga frame
	if (m_bPrintGga) {
		printf ("%sGga Frame : \n\tHour : %02d:%02d:%02d\n\tSatellite number : %d\n\tQuality : %s\n\tPosition : %.5f %c, %.5f %c\n\tHDOP : %f\n\tAltitude : %f\n\tMSL : %f\n\n",
			CGPS_PRINT_NAME,
			GgaFrame.uiHour,
			GgaFrame.uiMinute,
			GgaFrame.uiSecond,
			GgaFrame.uiNbSat,
			(GgaFrame.uiQuality > 2) ?  sGps[3] :  sGps[GgaFrame.uiQuality],
			GgaFrame.dLatitude, GgaFrame.cLatitudeRef,
			GgaFrame.dLongitude, GgaFrame.cLongitudeRef,
			GgaFrame.dHDOP,
			GgaFrame.dAltitude,
			GgaFrame.dMSL);
	}
	
	// Launch the Gga callback(s)
	m_oSigOnGgaData(sOrigBuffer, iBufferSize, pEvent, (void *) &GgaFrame, m_pData2);
	
	// If the driver is not connected
	if (!m_bSynchronizationDriverConnected) {		
		// If the hour has change, launch the OnHourChange callback(s)
		if (m_bHourChange) {
			m_oSigOnHourChange(NULL, 0, pEvent, (void *) &GgaFrame, m_pData2);
			m_bHourChange = false;
		}

		// If the day has change, launch the OnDayChange callback(s)
		if (m_bDayChange) {
			m_oSigOnDayChange(NULL, 0, pEvent, (void *) &GgaFrame, m_pData2);
			m_bDayChange = false;
			m_bOnSpecHour = true;
		}

		// If the hour specified hour is reached, launch the OnSpecUTCHour callback(s)
		if (GgaFrame.uiHour == m_uiSpecUtcHour && GgaFrame.uiMinute == m_uiSpecUtcMinute && GgaFrame.uiSecond == m_uiSpecUtcSecond && m_bOnSpecHour == 1) {
			m_oSigOnSpecUTCHour(NULL, 0, pEvent, (void *) &GgaFrame, m_pData2);
			m_bOnSpecHour = false;
		}
	}
	
	// Delete the previously created CEvent
	delete pEvent;
	
	// Say that we have received a Gga frame since the last Pps
	m_bGgaDataReceived = true;
}

/**
 *	\fn		void CGPS :: SetPrintGga (bool bPrintGga)
 *	\brief	Method which enable/disable the display of Gga frame when its received
 *
 *	\param[in]	bPrintGga :		Boolean wich indicates if we should print the Gga frame (0 for disable, 1 for enable)
 */
void CGPS :: SetPrintGga (bool bPrintGga)
{
	m_bPrintGga = bPrintGga;
}

/**
 *	\fn		void CGPS :: GpsDecodeZda (char * sBuffer, int iBufferSize)
 *	\brief	Zda frame decoding method
 *
 *  \param[in]	sBuffer : 			Buffer which contains the message to decode
 *  \param[in]	iBufferSize : 		Integer that indicate the size of the message to decode
 */
void CGPS :: GpsDecodeZda (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	unsigned int uiTime;
	char * sDebut, * sFin, * sOrigBuffer;
	unsigned char ucIndex = 0;
	
	static CEvent * pEvent = NULL;
	
	Zda ZdaFrameTemp;

	// cout << CGPS_PRINT_NAME << "Decoding Nmea-Zda data" << endl;

	// Initialization
	memset (&ZdaFrameTemp, 0, sizeof (Zda));
	sOrigBuffer = strdup(sBuffer);
	pEvent = new CEvent;

	// Data extraction
	sDebut = sBuffer;
	sFin = sBuffer;
	// While the the end of the frame is not reached
	while (* sFin != '\0') {
		// If we detect the end of a field
		if (* sFin == ',' || * sFin == '*') {
			* sFin = '\0';

			// Token treatment
			if (ucIndex == 1) {
				// Hour, minute, second, milliseond
				uiTime = atoi(sDebut);
				ZdaFrameTemp.uiHour = uiTime / 10000;
				ZdaFrameTemp.uiMinute = (uiTime / 100) % 100;
				ZdaFrameTemp.uiSecond = uiTime % 100;
				ZdaFrameTemp.uiMilliSecond = atoi(sFin-2)*10;
			} else if (ucIndex == 2)
				// Day
				ZdaFrameTemp.uiDay = atoi(sDebut);
			else if (ucIndex == 3)
				// Month
				ZdaFrameTemp.uiMonth = atoi(sDebut);
			else if (ucIndex == 4) {
				// Year
				ZdaFrameTemp.uiYear = atoi(sDebut);
				break;
			}
			// End of the token treatment

			ucIndex ++;
			sDebut = sFin + 1;
		}
		sFin++;
	}
	
	// Copy the data into the class
	(void) memcpy (&ZdaFrame, &ZdaFrameTemp, sizeof (Zda));
	
	// If the display is set, print the content of the Zda frame
	if (m_bPrintZda) {
		printf ("%sZda frame : \n\tThe %02d/%02d/%04d, at %02d:%02d:%02d\n\n",
			CGPS_PRINT_NAME,
			ZdaFrame.uiDay,
			ZdaFrame.uiMonth,
			ZdaFrame.uiYear,
			ZdaFrame.uiHour,
			ZdaFrame.uiMinute,
			ZdaFrame.uiSecond);
	}
	
	// Launch the Zda callback(s)
	m_oSigOnZdaData(sOrigBuffer, iBufferSize, pEvent, (void *) &ZdaFrame, m_pData2);
	
	// Delete the previously created CEvent
	delete pEvent;
	
	// Say that we have received a Zda frame since the last Pps
	m_bZdaDataReceived = true;
}

/**
 *	\fn		void CGPS :: SetPrintZda(bool bPrintZda)
 *	\brief	Method which enable/disable the display of Zda frame when its received
 *
 *	\param[in]	bPrintZda :	Boolean wich indicates if we should print the Zda frame (0 for disable, 1 for enable)
 */
void CGPS :: SetPrintZda(bool bPrintZda)
{
	m_bPrintZda = bPrintZda;
}

/**
 *	\fn		void CGPS :: GpsDecodeRmc (char * sBuffer, int iBufferSize)
 *	\brief	Rmc frame decoding method
 *
 *  \param[in]	sBuffer : 			Buffer which contains the message to decode
 *  \param[in]	iBufferSize : 		Integer that indicate the size of the message to decode
 */
void CGPS :: GpsDecodeRmc (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	unsigned int uiTime, uiDate;
	char * sDebut, * sFin, * sOrigBuffer;
	unsigned char ucIndex = 0;
	
	Rmc RmcFrameTemp;
	
	unused(iBufferSize);

	// cout << CGPS_PRINT_NAME << "Decoding Nmea-Rmc data" << endl;

	// Initialization
	memset (&RmcFrameTemp, 0, sizeof (Rmc));
	sOrigBuffer = strdup(sBuffer);

	// Data extraction
	sDebut = sBuffer;
	sFin = sBuffer;
	// While the the end of the frame is not reached
	while (* sFin != '\0') {
		// If we detect the end of a field
		if (*sFin == ',' || *sFin == '*') {
			*sFin = '\0';

			// Token treatment
			if (ucIndex == 1) {
				// Hour, minute, second, millisecond
				uiTime = atoi(sDebut);
				RmcFrameTemp.uiHour = uiTime / 10000;
				RmcFrameTemp.uiMinute = (uiTime / 100) % 100;
				RmcFrameTemp.uiSecond = uiTime % 100;
				RmcFrameTemp.uiMilliSecond = atoi(sFin-2)*10;
			} else if (ucIndex == 3)
				// Latitude
				RmcFrameTemp.dLatitude = atof (sDebut);
			else if (ucIndex == 4)
				// Latitude reference
				RmcFrameTemp.cLatitudeRef = *sDebut;
			else if (ucIndex == 5)
				// Longitude
				RmcFrameTemp.dLongitude = atof (sDebut);
			else if (ucIndex == 6)
				// Longitude reference
				RmcFrameTemp.cLongitudeRef = *sDebut;
			else if (ucIndex == 7)
				// Speed
				RmcFrameTemp.dSpeed = atof (sDebut);
			else if (ucIndex == 8)
				// Cape
				RmcFrameTemp.dCape = atof (sDebut);
			else if (ucIndex == 9) {
				// Day, month, year
				uiDate = atoi(sDebut);
				RmcFrameTemp.uiDay = uiDate / 10000;
				RmcFrameTemp.uiMonth = (uiDate / 100) % 100;
				RmcFrameTemp.uiYear = uiDate % 100;
				break;
			}
			// End of the token treatment

			ucIndex ++;
			sDebut = sFin + 1;
		}
		sFin++;
	}
	
	// Copy the data into the class
	(void) memcpy (&RmcFrame, &RmcFrameTemp, sizeof (Rmc));
	
	// If the display is set, print the content of the Rmc frame
	if (m_bPrintRmc) {
		printf ("%sRmc frame : \n\tHour : %02d:%02d:%02d\n\tDate : %02d/%02d/%02d\n\tPosition : %.5f%c, %.5f%c\n\tSpeed : %.5f\n\tCap : %.5f\n\n",
			CGPS_PRINT_NAME,
			RmcFrame.uiHour,
			RmcFrame.uiMinute,
			RmcFrame.uiSecond,
			RmcFrame.uiDay,
			RmcFrame.uiMonth,
			RmcFrame.uiYear,
			RmcFrame.dLatitude, RmcFrame.cLatitudeRef,
			RmcFrame.dLongitude, RmcFrame.cLongitudeRef,
			RmcFrame.dSpeed,
			RmcFrame.dCape);
	}
}

/**
 *	\fn		void CGPS :: SetPrintRmc(bool bPrintRmc)
 *	\brief	Method which enable/disable the display of Rmc frame when its received
 *
 *	\param[in]	bPrintRmc :	Boolean wich indicates if we should print the Rmc frame (0 for disable, 1 for enable)
 */
void CGPS :: SetPrintRmc(bool bPrintRmc)
{
	m_bPrintRmc = bPrintRmc;
}

/**
 *	\fn		void CGPS :: GpsDecodeGsa (char * sBuffer, int iBufferSize)
 *	\brief	Gsa frame decoding method
 *
 *	\author Jean Gueganno
 *
 *	\date 09 May, 2012
 *
 *  \param[in]	sBuffer : 			Buffer which contains the message to decode
 *  \param[in]	iBufferSize : 		Integer that indicate the size of the message to decode
 */
void CGPS :: GpsDecodeGsa (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	int iIndex = 0;
	char * sDebut, * sFin, * sOrigBuffer;
	
	Gsa GsaFrameTemp;

	unused(iBufferSize);

	// Initialization
	memset(&GsaFrameTemp, 0, sizeof(Gsa));
	sOrigBuffer = strdup(sBuffer);

	// cout << CGPS_PRINT_NAME << "Decoding Nmea-Gsa data" << endl;
	
	// Data extraction
	sDebut = sBuffer;
	sFin = sBuffer;
	// While the the end of the frame is not reached
	while (* sFin != '\0') {
		// If we detect the end of a field
		if (*sFin == ',' || *sFin == '*') {
			
			// Token treatment
			if(iIndex == 1)
				// 3D mode
				GsaFrameTemp.c3DMode = *sDebut;
			else if(iIndex == 2) 
				// Mode
				GsaFrameTemp.uiMode = atoi(sDebut);
			else if(iIndex == 3)
				// ID of the first satellite vehicles used in position fix
				GsaFrameTemp.uiID01 = atoi(sDebut);
			else if(iIndex == 4)
				// ID of the second satellite vehicles used in position fix
				GsaFrameTemp.uiID02 = atoi(sDebut);
			else if(iIndex == 5)
				// ID of the third satellite vehicles used in position fix
				GsaFrameTemp.uiID03 = atoi(sDebut);
			else if(iIndex == 6)
				// ID of the forth satellite vehicles used in position fix
				GsaFrameTemp.uiID04 = atoi(sDebut);
			else if(iIndex == 7)
				// ID of the fifth satellite vehicles used in position fix
				GsaFrameTemp.uiID05 = atoi(sDebut);
			else if(iIndex == 8)
				// ID of the sixth satellite vehicles used in position fix
				GsaFrameTemp.uiID06 = atoi(sDebut);
			else if(iIndex == 9)
				// ID of the seventh satellite vehicles used in position fix
				GsaFrameTemp.uiID07 = atoi(sDebut);
			else if(iIndex == 10)
				// ID of the eighth satellite vehicles used in position fix
				GsaFrameTemp.uiID08 = atoi(sDebut);
			else if(iIndex == 11)
				// ID of the nineth satellite vehicles used in position fix
				GsaFrameTemp.uiID09 = atoi(sDebut);
			else if(iIndex == 12)
				// ID of the tenth satellite vehicles used in position fix
				GsaFrameTemp.uiID10 = atoi(sDebut);
			else if(iIndex == 13)
				// ID of the eleventh satellite vehicles used in position fix
				GsaFrameTemp.uiID11 = atoi(sDebut);
			else if(iIndex == 14)
				// ID of the twelfth satellite vehicles used in position fix
				GsaFrameTemp.uiID12 = atoi(sDebut);
			else if(iIndex == 15)
				// Positional Dilution of Precision
				GsaFrameTemp.dPDOP = atof(sDebut);
			else if(iIndex == 16)
				// Horizontal Dilution of Precision
				GsaFrameTemp.dHDOP = atof(sDebut);
			else if(iIndex == 17) {
				// Vertical Dilution of Precision
				GsaFrameTemp.dVDOP = atof(sDebut);
				break;
			}
			// End of the token treatment
			
			iIndex++;
			sDebut = sFin + 1;
		}
		sFin++;
	}
   
	// Copy the data into the class
	(void) memcpy (&GsaFrame, &GsaFrameTemp, sizeof (Gsa));
	
	// If the display is set, print the content of the Gsa frame
	if (m_bPrintGsa) {
		cout << CGPS_PRINT_NAME << "Gsa frame :" << endl
			<< "\t3D functionnement mode		: "	<< GsaFrame.c3DMode	<< endl
			<< "\tFunctionnement mode			: "	<< GsaFrame.uiMode		<< endl
			<< "\tFirst seen satellite ID		: "	<< GsaFrame.uiID01		<< endl
			<< "\tSecond seen satellite ID		: "	<< GsaFrame.uiID02		<< endl
			<< "\tThird seen satellite ID		: "	<< GsaFrame.uiID03		<< endl
			<< "\tFourth seen satellite ID		: "	<< GsaFrame.uiID04		<< endl
			<< "\tFith seen satellite ID		: "	<< GsaFrame.uiID05		<< endl
			<< "\tSixth seen satellite ID		: "	<< GsaFrame.uiID06		<< endl
			<< "\tSeventh seen satellite ID		: "	<< GsaFrame.uiID07		<< endl
			<< "\tEighth seen satellite ID		: "	<< GsaFrame.uiID08		<< endl
			<< "\tNineth seen satellite ID		: "	<< GsaFrame.uiID09		<< endl
			<< "\tTenth seen satellite ID		: "	<< GsaFrame.uiID10	<< endl
			<< "\tEleventh seen satellite ID		: "	<< GsaFrame.uiID11	<< endl
			<< "\tTwelfth seen satellite ID		: "	<< GsaFrame.uiID12	<< endl
			<< "\tPDOP							: "	<< GsaFrame.dPDOP		<< endl
			<< "\tHDOP							: "	<< GsaFrame.dHDOP		<< endl
			<< "\tVDOP							: "	<< GsaFrame.dVDOP		<< endl;
	}
	
}

/**
 *	\fn		void CGPS :: SetPrintGsa(bool bPrintGsa)
 *	\brief	Method which enable/disable the display of Gsa frame when its received
 *
 *	\author Jean Gueganno
 *
 *	\date 09 May, 2012
 *
 *	\param[in]	bPrintGsa :	Boolean wich indicates if we should print the Gsa frame (0 for disable, 1 for enable)
 */
void CGPS :: SetPrintGsa(bool bPrintGsa)
{
	m_bPrintGsa = bPrintGsa;
}

/**
 *	\fn		void CGPS :: GpsDecodeGst (char * sBuffer, int iBufferSize)
 *	\brief	Gst frame decoding method
 *
 *	\author Jean Gueganno
 *
 *	\date 09 May, 2012
 *
 *  \param[in]	sBuffer : 			Buffer which contains the message to decode
 *  \param[in]	iBufferSize : 		Integer that indicate the size of the message to decode
 */
void CGPS :: GpsDecodeGst (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	int iIndex = 0;
	unsigned int uiTime;
	char * sDebut, * sFin, * sOrigBuffer;
	
	Gst GstFrameTemp;
	
	unused(iBufferSize);

	// Initialization
	memset(&GstFrameTemp, 0, sizeof(Gst));

	sOrigBuffer = strdup(sBuffer);
	
	// cout << CGPS_PRINT_NAME << "Decoding Nmea-Gst data" << endl;

	// Data extraction
	sDebut = sBuffer;
	sFin = sBuffer;
	// While the the end of the frame is not reached
	while (* sFin != '\0') {
		// If we detect the end of a field
		if (*sFin == ',' || *sFin == '*') {
		
			// Token treatment
			if (iIndex == 1) {
				// Hour, minute, second, millisecond
				uiTime = atoi(sDebut);
				GstFrameTemp.uiHour = uiTime / 10000;
				GstFrameTemp.uiMinute = (uiTime / 100) % 100;
				GstFrameTemp.uiSecond = uiTime % 100;
				GstFrameTemp.uiMilliSecond = atoi(sFin-2)*10;
			}
			else if (iIndex == 2)
				// Sigma
				GstFrameTemp.dRMS = atof(sDebut);
			else if (iIndex == 3)
				// Standard Deviation of Semi-Major Axis of Error Ellipse in meters
				GstFrameTemp.dSDoSMajAoEE = atof(sDebut);
			else if (iIndex == 4)
				// Standard Deviation of Semi-Minor Axis of Error Ellipse in meters
				GstFrameTemp.dSDoSMinAoEE = atof(sDebut);
			else if (iIndex == 5)
				// Orientation of Semi-Minor Axis of Error Ellipse in dergrees from True North
				GstFrameTemp.dOoSMajAoEE = atof(sDebut);
			else if (iIndex == 6)
				// Standard Deviation of Latitude Error in meters
				GstFrameTemp.dSDoLatE = atof(sDebut);
			else if (iIndex == 7)
				// Standard Deviation of Longitude Error in meters
				GstFrameTemp.dSDoLongE = atof(sDebut);
			else if (iIndex == 8) {
				// Standard Deviation of Altitude Error in meters
				GstFrameTemp.dSDoAltE = atof(sDebut);
				break;
			}
			// End of the token treatment
			
			iIndex++;
			sDebut = sFin + 1;
		}
		sFin++;
	}

	// Copy the data into the class
	(void) memcpy (&GstFrame, &GstFrameTemp, sizeof (Gst));

	// If the display is set, print the content of the Gst frame
	if (m_bPrintGst) {
		cout << CGPS_PRINT_NAME << "Trame Gst :" << endl
			<< "\tHour : "														<< GstFrame.uiHour			<< endl
			<< "\tMinutes : "													<< GstFrame.uiMinute		<< endl
			<< "\tSeconds : "													<< GstFrame.uiSecond		<< endl
			<< "\tMilliseconds : "												<< GstFrame.uiMilliSecond	<< endl
			<< "\tSigma : "														<< GstFrame.dRMS			<< endl
			<< "\tStandard deviation of semi-major axis of error ellipse : "	<< GstFrame.dSDoSMajAoEE	<< endl
			<< "\tStandard deviation of semi-minor axis of error ellipse : "	<< GstFrame.dSDoSMinAoEE	<< endl
			<< "\tOrientation of semi-major axis of error ellipse : "			<< GstFrame.dOoSMajAoEE	<< endl
			<< "\tStandard deviation of latitude error : "						<< GstFrame.dSDoLatE		<< endl
			<< "\tStandard deviation of longitude error : "						<< GstFrame.dSDoLongE		<< endl
			<< "\tStandard deviation of altitude error : "						<< GstFrame.dSDoAltE		<< endl;
	}
}

/**
 *	\fn		void CGPS :: SetPrintGst(bool bPrintGst)
 *	\brief	Method which enable/disable the display of Gst frame when its received
 *
 *	\author Jean Gueganno
 *
 *	\date 09 May, 2012
 *
 *	\param[in]	bPrintGst :	Boolean wich indicates if we should print the Gst frame (0 for disable, 1 for enable)
 */
void CGPS :: SetPrintGst(bool bPrintGst)
{
	m_bPrintGst = bPrintGst;
}

/**
 *	\fn		int CGPS :: SetCallbackUtcHour (unsigned int uiHour, unsigned int uiMinute, unsigned int uiSecond)
 *	\brief	Method which set the hour when the OnSpecUTCHourCallback will be launched
 *
 *	\param[in]	uiHour :		Integer which indicates the hour at which to launch the onUTCHour callback
 *	\param[in]	uiMinute :		Integer which indicates the minute at which to launch the onUTCHour callback
  *	\param[in]	uiSecond :		Integer which indicates the second at which to launch the onUTCHour callback
 */
int CGPS :: SetCallbackUtcHour (unsigned int uiHour, unsigned int uiMinute, unsigned int uiSecond)
{
	// Check if the specifed hour is an hour (hour : 0-23, minute : 0-59, second : 0-59)
	if (uiHour > 23 || uiMinute > 59 || uiSecond > 59){
		cout << CGPS_PRINT_NAME << "Error during the setting of the specific hour used to launch the callback" << endl << endl;
		return -ERR_CGPS_SETCALLBACKUTCHOUR_SPECIFIED_HOUR;
	}
	else {
		// Copy the specified hour, minute, and second to be the hour on which the on specified Utc hour callback will be launch
		m_uiSpecUtcHour = uiHour;
		m_uiSpecUtcMinute = uiMinute;
		m_uiSpecUtcSecond = uiSecond;
		
		// Say that the callback can be launched
		m_bOnSpecHour = true;
		
		printf ("%sThe called hour of speciefied hour callback has been changed, it will now happen at %02d:%02d:%02d\n\n", CGPS_PRINT_NAME, m_uiSpecUtcHour, m_uiSpecUtcMinute, m_uiSpecUtcSecond);
		return 0;
	}
}

/**
 *	\fn		void CGPS :: GpsNmeaCheckAndDecode (char * sBuffer, int iBufferSize)
 * 	\brief	Check method and decoding of the data received on the serial link
 */
void CGPS :: GpsNmeaCheckAndDecode (char * sBuffer, int iBufferSize)
{
	// Verify Checksum
	if (NmeaVerifyChecksum (sBuffer, iBufferSize)) {
		cout << CGPS_PRINT_NAME << "Bad Checksum !" << endl;
		return;
	}

	// Search the good method to decode data
	if (strncmp (sBuffer, "$GPGGA", 6) == 0) {
		// Gga frame
		GpsDecodeGga (sBuffer, iBufferSize);
		// cout << CGPS_PRINT_NAME << "Data decoded (Gga) !" << endl << endl;
	}
	else if (strncmp (sBuffer, "$GPZDA", 6) == 0) {
		// Zda frame
		GpsDecodeZda (sBuffer, iBufferSize);
		// cout << CGPS_PRINT_NAME << "Data decoded (Zda) !" << endl << endl;
	}
	else if (strncmp (sBuffer, "$GPRMC", 6) == 0) {
		// Rmc frame
		GpsDecodeRmc (sBuffer, iBufferSize);
		// cout << CGPS_PRINT_NAME << "Data decoded (Rmc) !" << endl << endl;
	}
	else if (strncmp (sBuffer, "$GPGSA", 6) == 0) {
		// Gsa frame
		GpsDecodeGsa (sBuffer, iBufferSize);
		// cout << CGPS_PRINT_NAME << "Data decoded (Gsa) !" << endl << endl;
	}
	else if (strncmp (sBuffer, "$GPGST", 6) == 0) {
		// Gst frame
		GpsDecodeGst (sBuffer, iBufferSize);
		// cout << CGPS_PRINT_NAME << "Data decoded (Gst) !" << endl << endl;
	}
	else if (strncmp (sBuffer, "$GPTXT", 6) == 0)
		// Txt frame
		cout << CGPS_PRINT_NAME << "Text Nmea message receiced from the Gps module : " << sBuffer << endl << endl;
	else
		cout << CGPS_PRINT_NAME << "That isn't a decodable Nmea trame : " << sBuffer  << endl << endl;
}

/**
 *	\fn		void CGPS :: GpsNmeaReception (char * sBuffer, int iBufferSize)
 *	\brief	Data received on GPS serial link method (connected method to CUart callback)
 *
 *	\param[in]	sBuffer :		Pointer of received data (retuned by the CUart object)
 *	\param[in]	iBufferSize :	Size of received data
 */
void CGPS :: GpsNmeaReception (char * sBuffer, int iBufferSize)
{
	// Variable initialization
	int iRet = 0;
	static CEvent * pEvent;
	
	time_t oTimeTTimeInSeconds;
	struct tm oTMTime;
		
	GpsData oGpsDataToSend;
	
	// Initialization
	pEvent = new CEvent;
	
	// Launch the GPS data callback(s)
	m_oSigOnGpsData(sBuffer, iBufferSize, pEvent, m_pData1, m_pData2);

	// Check and decode
	GpsNmeaCheckAndDecode(sBuffer, iBufferSize);
	
	// If a Gga and a Zda frame have been received, and if the synchronization driver is connected
	if (m_bGgaDataReceived && m_bZdaDataReceived && m_bSynchronizationDriverConnected) {
		// Get the system time informations
		time(&oTimeTTimeInSeconds);
		oTMTime = *localtime(&oTimeTTimeInSeconds);
		
		// Copy the time send by the Gps module in tm structure
		oTMTime.tm_year = ZdaFrame.uiYear-1900;
		oTMTime.tm_mon = ZdaFrame.uiMonth-1;
		oTMTime.tm_mday = ZdaFrame.uiDay;
		oTMTime.tm_hour = ZdaFrame.uiHour;
		oTMTime.tm_min = ZdaFrame.uiMinute;
		oTMTime.tm_sec = ZdaFrame.uiSecond;
		
		// Convert the GMT time send by the Gps module in a GMT time in seconds (remove the local time difference)
		oTimeTTimeInSeconds = mktime (&oTMTime) + oTMTime.tm_gmtoff;
		
		// Copy the data to be sent to the synchronization driver
		oGpsDataToSend.oTimeTTimeInSeconds = oTimeTTimeInSeconds;
		oGpsDataToSend.dLatitude = GgaFrame.dLatitude;
		oGpsDataToSend.dLongitude = GgaFrame.dLongitude;
		oGpsDataToSend.cLatitudeRef = GgaFrame.cLatitudeRef;
		oGpsDataToSend.cLongitudeRef =  GgaFrame.cLongitudeRef;
		oGpsDataToSend.iQuality = GgaFrame.uiQuality;
		oGpsDataToSend.uiNbSat = GgaFrame.uiNbSat;
		
		// Send the data
		iRet = ioctl(m_fd, SYNC_IOCTL_SET_TEMPORARY_GPS_DATA, &oGpsDataToSend);
		// Check if everithing went well
		if (iRet < 0)
			cout << CGPS_PRINT_NAME << "WARNING : Error " << iRet << " during the send of the data to the synchronization driver" << endl;
	}
	
	// Delete the previously created CEvent
	delete pEvent;
}

/**
 *	\fn		void CGPS :: GpsUbxReception (char * sBuffer, int iBufferSize)
 *	\brief	Data received on GPS serial link method (connected method to CUart callback)
 *
 *	\param[in]	sBuffer :		Pointer of received data (retuned by the CUart object)
 *	\param[in]	iBufferSize :	Size of received data
 */
void CGPS :: GpsUbxReception (char * sBuffer, int iBufferSize)
{
	// Variables initialization
	bool bAck = false;
	
	uint16_t ui16ClassId;
	
	// Check if the chechsum of the Ubx frame is valid
	if (UbxVerifyChecksum(sBuffer, iBufferSize)) {
		// If it's a acknowledge response frame
		if (sBuffer[2] == (CFG_ACK & 0xff00) >> 8) {			// Acknowledge frame					
			// Get the Class Id
			ui16ClassId = (sBuffer[6] << 8) + sBuffer[7];				
			
			// Verified the acknowledge type						
			if (sBuffer[3] == (CFG_ACK & 0x00ff))				// Message acknowledge 
				bAck = true;
			else if (sBuffer[3] == (CFG_NACK & 0x00ff))			// Message not acknowledge 
				bAck = false;
			
			// Check the ClassId
			switch (ui16ClassId) {
				// If it's a frame configuration acknowledge
				case CFG_MSG :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of frame configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of frame configuration received" << endl << endl;
					break;
				
				// If it's a port configuration acknowledge
				case CFG_PRT :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of port configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of port configuration received" << endl << endl;
					break;
				
				// If it's a receiver manager configuration acknowledge
				case CFG_RXM :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of receiver manager configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of receiver manager configuration received" << endl << endl;
					break;
				
				// If it's a Navigation/Measurement Rate configuration acknowledge
				case CFG_RATE :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of Navigation/Measurement Rate configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of Navigation/Measurement Rate configuration received" << endl << endl;
					break;
				
				// If it's a Nmea protocol configuration acknowledge
				case CFG_NMEA :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of Nmea protocol configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of Nmea protocol configuration received" << endl << endl;
					break;
				
				// If it's a Time Mode configuration acknowledge
				case CFG_TMODE :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of Time Mode configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of Time Mode configuration received" << endl << endl;
					break;
				
				// If it's a TimePulse parameters acknowledge
				case CFG_TP :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of TimePulse parameters received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of TimePulse parameters received" << endl << endl;
					break;
				
				// If it's a TimePulse extended parameters acknowledge
				case CFG_TP5 :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of TimePulse extended parameters received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of TimePulse extended parameters received" << endl << endl;
					break;
				
				// If it's a Power Management configuration acknowledge
				case CFG_PM :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of Power Management configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of Power Management configuration received" << endl << endl;
					break;
				
				// If it's a Power Management extended configuration acknowledge
				case CFG_PM2 :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of Power Management extended configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of Power Management extended configuration received" << endl << endl;
					break;
				
				// If it's a Antenna Control configuration acknowledge
				case CFG_ANT :
					// Check the acknoledge type
					if (bAck)
						cout << CGPS_PRINT_NAME << "Acknowledge of Antenna Control configuration received" << endl << endl;
					else
						cout << CGPS_PRINT_NAME << "Not-Acknowledge of Antenna Control configuration received" << endl << endl;
					break;
				
				// If it's not handled acknowledge
				default :
					cout << CGPS_PRINT_NAME << "Not handled Acknowledge" << endl << endl;
					break;
			}
		}
		// Ubx frame not handled
		else
			cout << CGPS_PRINT_NAME << "Ubx frame not handled" << endl << endl;
	}
	else
		cout << CGPS_PRINT_NAME << "Ubx frame checksum invalid" << endl << endl;
}

/**
 *	\fn		void CGPS :: GpsReception (char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
 *	\brief	Data received on GPS serial link method (connected method to CUart callback)
 *
 *	\param[in]	sBuffer :		Pointer of received data (retuned by the CUart object)
 *	\param[in]	iBufferSize :	Size of received data
 *	\param[in]	pEvent :		Pointer of a CEvent object which contains informations about the event (timing and location information)
 *	\param[in]	pData1 :		Optionnal Private pointer 1 -> empty
  *	\param[in]	pData2 :		Optionnal Private pointer 2 -> empty
 */
void CGPS :: GpsReception (char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Variables initialization
	bool bEndFrame, bUbxFrame, bNmeaFrame;
	int iCpt1 = 0, iCpt2, iBufferToTreatSize;
	char sBufferToTreat[UBX_BUFFER_SIZE];
	
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	// cout << CGPS_PRINT_NAME << "Data received : " << sBuffer  << "ans its size is : " << iBufferSize << endl;
	
	// While the end of hte buffer is not reached
	while (iCpt1 != iBufferSize-1) {
		// Reset the buffer used to get the frame
		memset(sBufferToTreat, '\0', UBX_BUFFER_SIZE);
		iCpt2=0;
		
		// Reset the booleans which indicates the frame treatement state
		bEndFrame = false;
		bUbxFrame = false;
		bNmeaFrame = false;
		
		// While the end of the frame has not been detectd
		while (iCpt1 < iBufferSize && bEndFrame != true) {
			// If it's the begin a Nmea frame
			if (sBuffer[iCpt1] == '$')
				bNmeaFrame = true;
			
			// If it's the begin a Ubx frame
			if (iCpt1+1 < iBufferSize && (unsigned char) sBuffer[iCpt1] == 0xB5 && sBuffer[iCpt1+1] == 0x62)
				bUbxFrame = true;
			
			// Copy the current charactere of the frame
			sBufferToTreat[iCpt2++] = sBuffer[iCpt1++];
			
			// Check if the end of the frame is reached
			if (sBuffer[iCpt1] == '$' || sBuffer[iCpt1] == '\n' || (iCpt1+1 < iBufferSize && (unsigned char) sBuffer[iCpt1] == 0xB5 && sBuffer[iCpt1+1] == 0x62)) {
				bEndFrame = true;
				iBufferToTreatSize = iCpt2;
			}
			
			// Maybe TODO : Rebuild data in a local buffer in case if canonical mode don't work fine !
		}
		
		// If it's a Ubx frame	
		if (bEndFrame && bUbxFrame) {
			// Print the frame
			printf("%sUbx frame received : ", CGPS_PRINT_NAME);
			for (iCpt2=0; iCpt2 < iBufferToTreatSize; iCpt2++)
				printf("%02x ", sBufferToTreat[iCpt2]);
			printf("\n");
		
			// Decode the frame
			GpsUbxReception (sBufferToTreat, iBufferToTreatSize);
		}
		// If it's a Nmea frame
		else if (bEndFrame && bNmeaFrame) {
			// Print the frame
			// printf("%sNmea frame received : %s\n\n", CGPS_PRINT_NAME, sBufferToTreat);
			// Decode the frame
			GpsNmeaReception (sBufferToTreat, iBufferToTreatSize);
		}
		// If the frame has no know begining or if its the end of the reception buffer
		else if ((bEndFrame && !bUbxFrame && !bNmeaFrame) || iCpt1 == iBufferSize) {
			// Print the frame
			printf("%sUnknow or uncomplete frame received ", CGPS_PRINT_NAME);
			/*for (iCpt2=0; iCpt2 < iBufferToTreatSize; iCpt2++)
				printf("%02x ", sBufferToTreat[iCpt2]);*/
			printf ("\n%sEnd of the frame treatement\n\n", CGPS_PRINT_NAME);
			break;
		}
		else
			printf("%sError during the treatement\n\n", CGPS_PRINT_NAME);
		
		// printf("%sCharacters left : %d\n\n", CGPS_PRINT_NAME, iBufferSize-iCpt1);
	}
}

/**
 *	\fn		int CGPS :: RegisterCallback(unsigned int uiIdEvent, boost::function<CallBack> oFct, void * pData1, void * pData2)
 *  \brief	Register a Callback which will be called when the event specified in EnventID will occur
 *
 *  \param[in]	uiIdEvent : 	Integer that indicate the event ID on which the function will be launched
 *  \param[in]	oFct : 			Object which indicates the function to launch when a event occure
 *  \param[in] 	pData1 : 		WARNING in this class pData1 is used for return a structure that contains data decoding of the tram to the CallBack.
 *  \param[in] 	pData2 : 		User pointer.
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CGPS :: RegisterCallback(unsigned int uiIdEvent, boost::function<CallBack> oFct, void * pData1, void * pData2)
{	
	// Variable initialization
	bool b_ThreadLaunching = false;
	
	// Check the event
	switch (uiIdEvent) {

		// OnPps event
		case EVENT_CGPS_ONPPS :
			// If the boost::signal is empty
			if (m_oSigOnPps.num_slots() == 0) {
				if (m_bThreadRunning) {
					cout << CGPS_PRINT_NAME << "Error, the thread is already launched while no callbacks are connected" << endl;
					// Indicate to the thread it should stop
					m_bThreadRunning = false;
					// Wait for the thread end
					m_oPpsThread.join();
					return -ERR_CGPS_REGISTERCALLBACK_THREAD_MANAGING;
				}
				else {
					// Indicate that the thread should be launch
					b_ThreadLaunching = true;
				}
			}
			
			// Connect the passed function an d saved its connexion ID
			m_oLastFctId = m_oSigOnPps.connect(oFct);
			
			break;
		
		// OnDayChange event
		case EVENT_CGPS_ONDAYCHANGE : 
			// Connected the passed function and saved its connexion ID
			m_oLastFctId = m_oSigOnDayChange.connect(oFct);
			break;
		
		// OnHourChange event
		case EVENT_CGPS_ONHOURCHANGE : 
			// Connected the passed function and saved its connexion ID
			m_oLastFctId = m_oSigOnHourChange.connect(oFct);
			break;
		
		// OnSpecifiedUtcHour event
		case EVENT_CGPS_ONSPECUTCHOUR :
			// Connected the passed function and saved its connexion ID
			m_oLastFctId = m_oSigOnSpecUTCHour.connect(oFct);
			break;
		
		// OnGpsFrameReceived event
		case EVENT_CGPS_RECEIVEGPSDATA : 
			// Connected the passed function and saved its connexion ID
			m_oLastFctId = m_oSigOnGpsData.connect(oFct);
			break;
		
		// OnGgaFrameReceived event
		case EVENT_CGPS_RECEIVEGGADATA :
			// Connected the passed function and saved its connexion ID
			m_oLastFctId = m_oSigOnGgaData.connect(oFct);
			break;
		
		// OnZdaFrameReceived event
		case EVENT_CGPS_RECEIVEZDADATA :
			// Connected the passed function and saved its connexion ID
			m_oLastFctId = m_oSigOnZdaData.connect(oFct);
			break;
		
		// Unknown event
		default :
			cout << CGPS_PRINT_NAME << "Wrong event ID" << endl;
			return -ERR_CGPS_REGISTERCALLBACK_SPECIFIED_EVENT;
	}
	
	// Checks if the function was well connected
	if(m_oLastFctId.connected()) {
		// If pData1 has been sepcified by the user
		if (pData1 != NULL)
			cout << CGPS_PRINT_NAME << "pData1 couldn't be used in this Class" << endl;
		
		// Copy pData2 if pData2 has been sepcified by the user
		m_pData2 = pData2;
		
		// If the PpsDetection thread should be launched
		if (b_ThreadLaunching && m_bSynchronizationDriverConnected) {
			// Indicate to the thread it should run
			m_bThreadRunning = true;
			// And launch the data reader thread
			boost::thread m_oPpsThread(boost::bind(&CGPS::PpsDetection, this));
		}
		
		cout << CGPS_PRINT_NAME << "Callback correctly registered" << endl;
		
		return 0;
	}
	else {
		cout << CGPS_PRINT_NAME << "The callback could not be correctly registered" << endl;
		return -ERR_CGPS_REGISTERCALLBACK_REGISTERING;	
	}
}

/**
 *	\fn		int CGPS :: UnregisterCallback(unsigned int uiIdEvent, boost::signals2::connection oIdFct)
 *  \brief	Unregister a previously register callback using its connection Id
 *
 *  \param[in]	uiIdEvent : 		Integer that indicate the event ID on which the function is launched
 *  \param[in]	oIdFct : 		Object which indicates the connection ID of the function to unregister
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CGPS :: UnregisterCallback(unsigned int uiIdEvent, boost::signals2::connection oIdFct)
{
	// Variables initialization
	int iNumBefore, iNumAfter;
	
	// Check the event
	switch (uiIdEvent) {

		// OnPps event
		case EVENT_CGPS_ONPPS :
			// Checks if there are some connected functions
			if (m_oSigOnPps.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No Callbacks are currently connected on the Pps detection event" << endl;
				return -ERR_CGPS_UNREGISTERCALLBACK_NO_CALLBACK;
			}
			
			// Saved the number of connected function before the deconnexion
			iNumBefore = m_oSigOnPps.num_slots();
			
			// Disconnect the function using its connexion ID
			oIdFct.disconnect();
			
			// Saved the number of connected function after the deconnexion
			iNumAfter = m_oSigOnPps.num_slots();
			
			// If the signal is empty
			if (m_oSigOnPps.num_slots() == 0) {
				// Indicate to the thread it should stop
				m_bThreadRunning = false;
				// Wait for the thread end
				m_oPpsThread.join();
			}
			
			break;
		
		// OnDayChange event
		case EVENT_CGPS_ONDAYCHANGE : 
			// Checks if there are some connected functions
			if (m_oSigOnDayChange.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No Callbacks are currently connected on the day change event" << endl;
				return -ERR_CGPS_UNREGISTERCALLBACK_NO_CALLBACK;
			}
			
			// Saved the number of connected function before the deconnexion
			iNumBefore = m_oSigOnDayChange.num_slots();
			
			// Disconnect the function using its connexion ID
			oIdFct.disconnect();
			
			// Saved the number of connected function after the deconnexion
			iNumAfter = m_oSigOnDayChange.num_slots();
			break;
		
		// OnHourChange event
		case EVENT_CGPS_ONHOURCHANGE : 
			// Checks if there are some connected functions
			if (m_oSigOnHourChange.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No Callbacks are currently connected on the hour change event" << endl;
				return -ERR_CGPS_UNREGISTERCALLBACK_NO_CALLBACK;
			}
			
			// Saved the number of connected function before the deconnexion
			iNumBefore = m_oSigOnHourChange.num_slots();
			
			// Disconnect the function using its connexion ID
			oIdFct.disconnect();
			
			// Saved the number of connected function after the deconnexion
			iNumAfter = m_oSigOnHourChange.num_slots();
			break;
		
		// OnSpecifiedUtcHour event
		case EVENT_CGPS_ONSPECUTCHOUR :
			// Checks if there are some connected functions
			if (m_oSigOnSpecUTCHour.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No Callbacks are currently connected on the specified hour event" << endl;
				return -ERR_CGPS_UNREGISTERCALLBACK_NO_CALLBACK;
			}
			
			// Saved the number of connected function before the deconnexion
			iNumBefore = m_oSigOnSpecUTCHour.num_slots();
			
			// Disconnect the function using its connexion ID
			oIdFct.disconnect();
			
			// Saved the number of connected function after the deconnexion
			iNumAfter = m_oSigOnSpecUTCHour.num_slots();
			break;
		
		// OnGpsFrameReceived event
		case EVENT_CGPS_RECEIVEGPSDATA : 
			// Checks if there are some connected functions
			if (m_oSigOnGpsData.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No Callbacks are currently connected on the Gps receive data event" << endl;
				return -ERR_CGPS_UNREGISTERCALLBACK_NO_CALLBACK;
			}
			
			// Saved the number of connected function before the deconnexion
			iNumBefore = m_oSigOnGpsData.num_slots();
			
			// Disconnect the function using its connexion ID
			oIdFct.disconnect();
			
			// Saved the number of connected function after the deconnexion
			iNumAfter = m_oSigOnGpsData.num_slots();
			break;
		
		// OnGgaFrameReceived event
		case EVENT_CGPS_RECEIVEGGADATA :
			// Checks if there are some connected functions
			if (m_oSigOnGgaData.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No Callbacks are currently connected on the Gga receive data event" << endl;
				return -ERR_CGPS_UNREGISTERCALLBACK_NO_CALLBACK;
			}
			
			// Saved the number of connected function before the deconnexion
			iNumBefore = m_oSigOnGgaData.num_slots();
			
			// Disconnect the function using its connexion ID
			oIdFct.disconnect();
			
			// Saved the number of connected function after the deconnexion
			iNumAfter = m_oSigOnGgaData.num_slots();
			break;
		
		// OnZdaFrameReceived event
		case EVENT_CGPS_RECEIVEZDADATA :
			// Checks if there are some connected functions
			if (m_oSigOnZdaData.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No Callbacks are currently connected on the Zda receive data event" << endl;
				return -ERR_CGPS_UNREGISTERCALLBACK_NO_CALLBACK;
			}
			
			// Saved the number of connected function before the deconnexion
			iNumBefore = m_oSigOnZdaData.num_slots();
			
			// Disconnect the function using its connexion ID
			oIdFct.disconnect();
			
			// Saved the number of connected function after the deconnexion
			iNumAfter = m_oSigOnZdaData.num_slots();
			break;
		
		// Unknown event
		default :
			cout << CGPS_PRINT_NAME << "Wrong event ID" << endl;
			return -ERR_CGPS_UNREGISTERCALLBACK_SPECIFIED_EVENT;
	}
	
	// Checks if the function was well disconnected
	if (iNumBefore == iNumAfter+1) {
		cout << CGPS_PRINT_NAME << "Callback correctly unregistered" << endl;
		return 0;
	}
	else {
		cout << CGPS_PRINT_NAME << "The callback could not be correctly unregistered" << endl;
		return -ERR_CGPS_UNREGISTERCALLBACK_UNREGISTERING;
	}
}

/**
 *	\fn		int CGPS :: TotalNumberOfConnectedCallbacks()
 *	\brief	Method which return the total number of connected callback(s)
 *
 *	\return	The total number of connected callback(s)
 */
int CGPS :: TotalNumberOfConnectedCallbacks()
{
	return m_oSigOnPps.num_slots() + m_oSigOnDayChange.num_slots() + m_oSigOnHourChange.num_slots() + m_oSigOnSpecUTCHour.num_slots() + m_oSigOnGpsData.num_slots() + m_oSigOnGgaData.num_slots() + m_oSigOnZdaData.num_slots();
}

/**
 *	\fn		int CGPS :: UnregisterAllCallbacks (unsigned int uiIdEvent)
 *  \brief	Unregister all previously register callback on a specific event
 *
 *  \param[in]	uiIdEvent : 		Integer that indicate the event ID to empty
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CGPS :: UnregisterAllCallbacks (unsigned int uiIdEvent)
{
	// Variable initalization
	int iFinalNumber;
	
	// Check the event
	switch (uiIdEvent) {

		// If we want to disconnect all the funtion from all known events
		case ALL_EVENTS :			
			// Checks if there are some connected functions
			if (TotalNumberOfConnectedCallbacks() == 0) {
				cout << CGPS_PRINT_NAME << "No callbacks are currently connected" << endl;
				return -ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK;
			}
			
			// Disconnect all the functions form all the events
			m_oSigOnPps.disconnect_all_slots();
			m_oSigOnDayChange.disconnect_all_slots();
			m_oSigOnHourChange.disconnect_all_slots();
			m_oSigOnSpecUTCHour.disconnect_all_slots();
			m_oSigOnGpsData.disconnect_all_slots();
			m_oSigOnGgaData.disconnect_all_slots();
			m_oSigOnZdaData.disconnect_all_slots();
			
			if (m_oSigOnPps.num_slots() == 0) {
				// Indicate to the thread it should stop
				m_bThreadRunning = false;
				// Wait for the thread end
				m_oPpsThread.join();
			}
			
			// Saved the number of connected function after the deconnexion
			iFinalNumber = TotalNumberOfConnectedCallbacks();
			
			break;
		
		// OnPps event
		case EVENT_CGPS_ONPPS :
			// Checks if there are some connected functions
			if (m_oSigOnPps.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No callbacks are currently connected" << endl;
				return -ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK;
			}
			
			// Disconnect all the functions form this specific events
			m_oSigOnDayChange.disconnect_all_slots();
			
			if (m_oSigOnPps.num_slots() == 0) {
				// Indicate to the thread it should stop
				m_bThreadRunning = false;
				// Wait for the thread end
				m_oPpsThread.join();
			}
			
			// Saved the number of connected function after the deconnexion
			iFinalNumber = m_oSigOnPps.num_slots();
			
			break;
		
		// OnDayChange event
		case EVENT_CGPS_ONDAYCHANGE : 
			// Checks if there are some connected functions
			if (m_oSigOnDayChange.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No callbacks are currently connected" << endl;
				return -ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK;
			}
			
			// Disconnect all the functions form this specific events
			m_oSigOnDayChange.disconnect_all_slots();
			
			// Saved the number of connected function after the deconnexion
			iFinalNumber = m_oSigOnDayChange.num_slots();
			
			break;
		
		// OnHourChange event
		case EVENT_CGPS_ONHOURCHANGE : 
			// Checks if there are some connected functions
			if (m_oSigOnHourChange.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No callbacks are currently connected" << endl;
				return -ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK;
			}
			
			// Disconnect all the functions form this specific events
			m_oSigOnHourChange.disconnect_all_slots();
			
			// Saved the number of connected function after the deconnexion
			iFinalNumber = m_oSigOnHourChange.num_slots();
			
			break;
		
		// OnSpecifiedUtcHour event
		case EVENT_CGPS_ONSPECUTCHOUR :
			// Checks if there are some connected functions
			if (m_oSigOnSpecUTCHour.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No callbacks are currently connected" << endl;
				return -ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK;
			}
			
			// Disconnect all the functions form this specific events
			m_oSigOnSpecUTCHour.disconnect_all_slots();
			
			// Saved the number of connected function after the deconnexion
			iFinalNumber = m_oSigOnSpecUTCHour.num_slots();
			
			break;
		
		// OnGpsFrameReceived event
		case EVENT_CGPS_RECEIVEGPSDATA : 
			// Checks if there are some connected functions
			if (m_oSigOnGpsData.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No callbacks are currently connected" << endl;
				return -ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK;
			}
			
			// Disconnect all the functions form this specific events
			m_oSigOnGpsData.disconnect_all_slots();
			
			// Saved the number of connected function after the deconnexion
			iFinalNumber = m_oSigOnGpsData.num_slots();
			
			break;
		
		// OnGgaFrameReceived event
		case EVENT_CGPS_RECEIVEGGADATA :
			// Checks if there are some connected functions
			if (m_oSigOnGgaData.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No callbacks are currently connected" << endl;
				return -ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK;
			}
			
			// Disconnect all the functions form this specific events
			m_oSigOnGgaData.disconnect_all_slots();
			
			// Saved the number of connected function after the deconnexion
			iFinalNumber = m_oSigOnGgaData.num_slots();
			
			break;
		
		// OnZdaFrameReceived event
		case EVENT_CGPS_RECEIVEZDADATA :
			// Checks if there are some connected functions
			if (m_oSigOnZdaData.num_slots() == 0) {
				cout << CGPS_PRINT_NAME << "No callbacks are currently connected" << endl;
				return -ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK;
			}
			
			// Disconnect all the functions form this specific events
			m_oSigOnZdaData.disconnect_all_slots();
			
			// Saved the number of connected function after the deconnexion
			iFinalNumber = m_oSigOnZdaData.num_slots();
			
			break;
		
		// Unknown event
		default :
			cout << CGPS_PRINT_NAME << "Wrong event ID" << endl;
			return -ERR_CGPS_UNREGISTERALLCALLBACKS_SPECIFIED_EVENT;
		
	}
	
	// Checks if all the functions was well disconnected
	if (iFinalNumber == 0) {
		cout << CGPS_PRINT_NAME << "Every Callbacks correctly disconnected" << endl;
		return 0;
	}
	else {
		cout << CGPS_PRINT_NAME << "The callbacks could not be correctly unregistered" << endl;
		return -ERR_CGPS_UNREGISTERALLCALLBACKS_UNREGISTERING;
	}
}

/**
 *	\fn		boost::signals2::connection CGPS :: GetLastFctId ()
 *  \brief	Send back the last connected callback connexion Id
 *
 *	\return	The last connected callback connexion Id
 */
boost::signals2::connection CGPS :: GetLastFctId ()
{
	return m_oLastFctId;
}

/**
 *	\fn		void CGPS :: PpsDetection()
 *  \brief	Method who communicate continously with the synchronization driver to know when a Pps is received and manage the launch of the OnPps CallBacks
 */
void CGPS :: PpsDetection()
{
	// Init variables
	int iCpt = 0, iRet, iNreaded, iFd;
	char sRxBuffer[20];
	
	struct pollfd oFds;
	
	struct rtc_time oRTTime;
	
	CEvent * pEvent = NULL;
	
	// While we haven't said to the thread to stop
	do {
		// Init poll		
		oFds.fd = m_fd;
		oFds.events = POLLIN;
		oFds.revents = 0;
	
		// Launch the poll (which remplace a blocking reading)
		iRet = poll (&oFds, 1, 50);
		
		// Check why the poll has been finished
		if (iRet > 0) {
			// Create a new CEvent object
			pEvent = new CEvent(/*0*/);
			
			// printf("%sRevents contents : 0x%04x\n\n", CGPS_PRINT_NAME, oFds.revents);
			
			// Reset the reception buffer
			memset (sRxBuffer, 0, sizeof(sRxBuffer));
			
			// Read the string send back by the driver
			iNreaded = read (m_fd, sRxBuffer, sizeof(sRxBuffer));
			
			// cout << CGPS_PRINT_NAME << "A Pps has been detected" << endl;
			
			// Launch the connected callbacks
			m_oSigOnPps(sRxBuffer, iNreaded, pEvent, m_pData1, m_pData2);
			
			// Increment the variable which indicates how many Pps has been detected
			iCpt++;
			
			// Fix system hour
			if (GgaFrame.uiQuality >= QUALITY_MIN && (m_bGgaDataReceived || m_bZdaDataReceived)) {
				// If we have received a Zda frame
				if (m_bZdaDataReceived) {
					// We can update the day, the month, and the year
					oRTTime.tm_mday = ZdaFrame.uiDay;				// day of the month
					oRTTime.tm_mon = ZdaFrame.uiMonth - 1;			// month
					oRTTime.tm_year = ZdaFrame.uiYear - 1900;		// year
				}
				
				// If we have received a Gga frame
				if (m_bGgaDataReceived) {
					// We can update the hour, the minute, and the second
					oRTTime.tm_sec = GgaFrame.uiSecond;			// seconds
					oRTTime.tm_min = GgaFrame.uiMinute;			// minutes
					oRTTime.tm_hour = GgaFrame.uiHour;			// hours
				}
				
				// Open the link to the harware system clock driver
				iFd = open("/dev/rtc", O_RDONLY);
				// If the openning not happened well
				if (iFd <= 0)
					cout << CGPS_PRINT_NAME << "WARNING : Error " << iRet << " during the openning of the link with the hardware clock" << endl;
				else {
					// Copy the new UTC time in hardware time
					iRet = ioctl(iFd, RTC_SET_TIME, &oRTTime);
					// If the copy not happenned well
					if (iRet < 0) {
						cout << CGPS_PRINT_NAME << "WARNING : Error " << iRet << " during the setting of the new hardware time" << endl;
						
						// Copy hardware time to system time
						system("/sbin/hwclock --hctosys --utc");
					}
					
					// Close the link to the harware system clock driver
					close(iFd);
				}
			}
			
			// Reset the frame reception booleans
			m_bGgaDataReceived = false;
			m_bZdaDataReceived = false;
			
			// If the hour has change, launch the OnHourChange callback(s)
			if (m_bHourChange) {
				m_oSigOnHourChange(NULL, 0, pEvent, (void *) &GgaFrame, m_pData2);
				m_bHourChange = false;
			}
	
			// If the day has change, launch the OnDayChange callback(s)
			if (m_bDayChange) {
				m_oSigOnDayChange(NULL, 0, pEvent, (void *) &GgaFrame, m_pData2);
				m_bDayChange = false;
				m_bOnSpecHour = true;
			}
	
			// If the hour specified hour is reached, launch the OnSpecUTCHour callback(s)
			if (GgaFrame.uiHour == m_uiSpecUtcHour && GgaFrame.uiMinute == m_uiSpecUtcMinute && GgaFrame.uiSecond == m_uiSpecUtcSecond && m_bOnSpecHour == 1) {
				m_oSigOnSpecUTCHour(NULL, 0, pEvent, (void *) &GgaFrame, m_pData2);
				m_bOnSpecHour = false;
			}
			
			// Delete the previously created CEvent object
			delete(pEvent);
		}
		else {
			// Tell if the poll timeout has been reached
			// cout << CGPS_PRINT_NAME << "'Poll' timeout reached" << endl;
		}
	} while (m_bThreadRunning);
}

/*
 *	Accesor for the decoded informations
 *	Altitude, location precision informations, ...
 */

/**
 *	\fn		double CGPS :: GetAltitude ()
 *  \brief	Send back the current altitude
 *
 *	\return	The current altitude
 */
double CGPS :: GetAltitude ()
{
	return GgaFrame.dAltitude;
}

/**
 *	\fn		double CGPS :: GetSDoSMinAoEE ()
 *  \brief	Send back the Standard deviation of semi-minor axis of error ellipse in meters.
 *
 *	\author Jean Gueganno
 *
 *	\date 09 May, 2012
 *
 *	\return	The GPS the Standard deviation of semi-minor axis of error ellipse in meters
 */
double CGPS :: GetSDoSMinAoEE ()
{
	return GstFrame.dSDoSMinAoEE;
}

/**
 *	\fn		double CGPS :: GetSDoSMajAoEE ()
 *  \brief	Send back the Standard deviation of semi-major axis of error ellipse in meters.
 *
 *	\author Jean Gueganno
 *
 *	\date 09 May, 2012
 *
 *	\return	The GPS the Standard deviation of semi-major axis of error ellipse in meters
 */
double CGPS :: GetSDoSMajAoEE ()
{
	return GstFrame.dSDoSMajAoEE;
}

/**
 *	\fn		double CGPS :: GetHPrec ()
 *  \brief	Send back the GPS horizontal precision en meters.
 *
 *	\author Jean Gueganno
 *
 *	\date 09 May, 2012
 *
 *	\return	The GPS the GPS horizontal precision en meters
 */
double CGPS :: GetHPrec ()
{
	return GstFrame.dRMS * GsaFrame.dHDOP;
}

/**
 *	\fn		double CGPS :: GetVPrec ()
 *  \brief	Send back the GPS vertical precision en meters.
 *
 *	\author Jean Gueganno
 *
 *	\date 09 May, 2012
 *
 *	\return	The GPS GPS vertical precision en meters
 */
double CGPS :: GetVPrec ()
{
	return GstFrame.dRMS * GsaFrame.dVDOP;
}

/*
 *	Accesor for the driver informations
 *	Running mode, state, ...
 */

/**
 *	\fn		int CGPS :: GetDriverRunningMode ()
 *  \brief	Get the driver running mode
 *
 *	\return	A negative number if an error occured, the driver running mode otherwise.
 */
int CGPS :: GetDriverRunningMode ()
{
	int iRet = 0, iCurrentDriverMode = 0;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_GET_DRIVER_MODE, &iCurrentDriverMode);
	if (iRet < 0)
		return -ERR_CGPS_GETDRIVERRUNNINGMODE_GET_DRIVER_RUNNING_MODE;
	else
		return iCurrentDriverMode;
}

/**
 *	\fn		int CGPS :: GetDriverState ()
 *  \brief	Get the driver state machine state
 *
 *	\return	A negative number if an error occured, the driver state machine state otherwise.
 */
int CGPS :: GetDriverState ()
{
	int iRet = 0, iCurrentDriverState = 0;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_GET_DRIVER_STATE, &iCurrentDriverState);
	if (iRet < 0)
		return -ERR_CGPS_GETDRIVERSTATE_GET_DRIVER_STATE;
	else
		return iCurrentDriverState;
}

/**
 *	\fn		int CGPS :: GetGpsStatus ()
 *  \brief	Get the gps module status (On/Off)
 *
 *	\return	A negative number if an error occured, the gps module status (On/Off) otherwise.
 */
int CGPS :: GetGpsStatus ()
{
	int iRet = 0, iCurrentGpsStatus = 0;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_GET_GPS_STATUS, &iCurrentGpsStatus);
	if (iRet < 0)
		return -ERR_CGPS_GETGPSSTATUS_GET_GPS_STATUS;
	else
		return iCurrentGpsStatus;
}

/**
 *	\fn		int CGPS :: GetTimeSinceGpsOff ()
 *  \brief	Get the time since the gps module is Off
 *
 *	\return	A negative number if an error occured, the time since the gps module is Off otherwise.
 */
int CGPS :: GetTimeSinceGpsOff ()
{
	int iRet = 0, iGpsTimeSinceGpsOff = 0;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_GET_TIME_SINCE_GPS_OFF, &iGpsTimeSinceGpsOff);
	if (iRet < 0)
		return -ERR_CGPS_GETTIMESINCEGPSOFF_GET_TIME_SINCE_GPS_OFF;
	else
		return iGpsTimeSinceGpsOff;
}

/**
 *	\fn		int CGPS :: GetGpsOffTime ()
 *  \brief	Get the time during which the Gps should be turned off
 *
 *	\return	A negative number if an error occured, the time during which the Gps should be turned off otherwise.
 */
int CGPS :: GetGpsOffTime ()
{
	int iRet = 0, iGpsOffTime = 0;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_GET_GPS_OFF_TIME, &iGpsOffTime);
	if (iRet < 0)
		return -ERR_CGPS_GETGPSOFFTIME_GET_GPS_OFF_TIME;
	else
		return iGpsOffTime;
}

/**
 *	\fn		int CGPS :: GetTemperature ()
 *  \brief	Get the temperature (using the quartz error drift)
 *
 *	\return	A negative number if an error occured, the time during which the Gps should be turned off otherwise.
 */
int CGPS :: GetTemperature ()
{
	int iRet = 0, iTemperature = 0;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_GET_TEMPERATURE, &iTemperature);
	if (iRet < 0)
		return -ERR_CGPS_GETTEMPERATURE_GET_TEMPERATURE;
	else
		return iTemperature;
}

/**
 *	\fn		int CGPS :: GetCaptureTimerClock ()
 *  \brief	Get the capture timer clock
 *
 *	\return	A negative number if an error occured, the capture timer clock otherwise.
 */
int CGPS :: GetCaptureTimerClock ()
{
	int iRet = 0, iCaptureTimerClock = 0;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_GET_CAPTURE_TIMER_CLOCK, &iCaptureTimerClock);
	if (iRet < 0)
		return -ERR_CGPS_GETCAPTURETIMERCLOCK_GET_CAPTURE_TIMER_CLOCK;
	else
		return iCaptureTimerClock;
}

/**
 *	\fn		int CGPS :: GetRelayTimerClock ()
 *  \brief	Get the relay timer clock
 *
 *	\return	A negative number if an error occured, the relay timer clock otherwise.
 */
int CGPS :: GetRelayTimerClock ()
{
	int iRet = 0, iRelayTimerClock = 0;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_GET_RELAY_TIMER_CLOCK, &iRelayTimerClock);
	if (iRet < 0)
		return -ERR_CGPS_GETRELAYTIMERCLOCK_GET_CAPTURE_TIMER_CLOCK;
	else
		return iRelayTimerClock;
}

/**
 *	\fn		int CGPS :: SetDriverRunningMode (unsigned int uiDriverRunningMode)
 *  \brief	Set the driver running mode
 *
 *	\return	A negative number if an error occured, 0 otherwise.
 */
int CGPS :: SetDriverRunningMode (unsigned int uiDriverRunningMode)
{
	int iRet = 0;
	
	if (uiDriverRunningMode < MODE1 || uiDriverRunningMode > MODE3)
		return -ERR_CGPS_SETDRIVERRUNNINGMODE_SPECIFIED_MODE;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_SET_DRIVER_MODE, uiDriverRunningMode);
	if (iRet < 0)
		return -ERR_CGPS_SETDRIVERRUNNINGMODE_SET_DRIVER_RUNNING_MODE;
	else
		return 0;
}

/**
 *	\fn		int CGPS :: SetGpsOffTime (unsigned int uiGpsOffTime)
 *  \brief	Set the time during which the Gps should be turned off
 *
 *	\return	A negative number if an error occured, 0 otherwise.
 */
int CGPS :: SetGpsOffTime (unsigned int uiGpsOffTime)
{
	int iRet = 0;
	
	if (uiGpsOffTime > GPS_MAXIMUM_OFF_TIME)
		return -ERR_CGPS_SETGPSOFFTIME_SPECIFIED_GPS_OFF_TIME;
	
	iRet = ioctl (m_fd, SYNC_IOCTL_SET_GPS_OFF_TIME, uiGpsOffTime);
	if (iRet < 0)
		return -ERR_CGPS_SETGPSOFFTIME_SET_GPS_OFF_TIME;
	else
		return 0;
}

/**
 *	\fn		void CGPS :: PrintConfig()
 *	\brief	Method which print the configuration of the gps module
 */
int CGPS :: PrintConfig()
{
	// Variable initialization
	char sBuffer[64];
	const char * sFrame[] = {"deactivate", "activate"};
	
	cout << endl;
	
	// Print the Uart configuration using the 'stty -F' system calls
	sprintf(sBuffer, "stty -F %s", m_sDevicePath);
	cout << CGPS_PRINT_NAME << endl;
	system (sBuffer);
	
	cout << endl;
	
	// If the Gps module has been reconfigured
	if (m_bHasBeenReconfigure) {
		// Print the frames which have been activated
		printf("%sThe Gps module have ben reconfigured\n\n", CGPS_PRINT_NAME);
		printf("%sNmea frames informations :\n\tGbs frames %s\n\tGga frames %s\n\tGgl frames %s\n\tGrs frames %s\n\tGsa frames %s\n\tGst frames %s\n\tGsv frames %s\n\tRmc frames %s\n\tVtg frames %s\n\tZda frames %s\n\n",
			CGPS_PRINT_NAME,
			sFrame[m_bActivateGbsFrame],
			sFrame[m_bActivateGgaFrame],
			sFrame[m_bActivateGllFrame],
			sFrame[m_bActivateGrsFrame],
			sFrame[m_bActivateGsaFrame],
			sFrame[m_bActivateGstFrame],
			sFrame[m_bActivateGsvFrame],
			sFrame[m_bActivateRmcFrame],
			sFrame[m_bActivateVtgFrame],
			sFrame[m_bActivateZdaFrame]);
	}
	else
		printf("%sThe Gps module haven't ben reconfigured\n\n", CGPS_PRINT_NAME);
	
	// Print which prints are activated
	printf("%sPrint frame informations :\n\tGga frames %s\n\tZda frames %s\n\tRmct frames %s\n\tGst frames %s\n\tGsa frames %s\n\n",
		CGPS_PRINT_NAME,
		sFrame[m_bPrintGga],
		sFrame[m_bPrintZda],
		sFrame[m_bPrintRmc],
		sFrame[m_bPrintGst],
		sFrame[m_bPrintGsa]);
	
	// Print the information about the specifice UTC hour callback
	printf("%sThe specific Utc hour callback will be launched at %02d:%02d:%02d\n\n", CGPS_PRINT_NAME, m_uiSpecUtcHour, m_uiSpecUtcMinute, m_uiSpecUtcSecond);
	
	// Print the information about the synchronization driver
	printf("%sThe synchronization is currently ", CGPS_PRINT_NAME);
	if (m_bSynchronizationDriverConnected)
		printf("online\n\n");
	else
		printf("offline\n\n");
	
	return 0;
}
