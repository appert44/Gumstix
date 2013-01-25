/**
 *	\file	CGPS.h
 *
 *	\brief 	This header file is used to declare the CGPS class and all its methods.
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.2a
 *
 *	\date 25 April, 2012
 *
 */

#ifndef CGPS_H
#define CGPS_H

#include "CUart.h"

/**
 *	\def	NMEA_BUFFER_SIZE
 *	\brief	NMEA Rx buffer size
 *	
 *	NMEA 0183 standard messages vary in length, but each message is limited to 79 characters or less.
 */
#define NMEA_BUFFER_SIZE	128

/**
 *	\def	UBX_BUFFER_SIZE
 *	\brief	UBX Tx buffer size
 *	
 *	UBX messages vary in length, but the messages which we will use are shorter than 256 characters.
 */
#define UBX_BUFFER_SIZE	256

/**
 *	\def	QUALITY_MIN
 *	\brief	Minimum quality to consider that the hour and the position are valid
 */
#define QUALITY_MIN	1


#define UBX 		0xB562						/**< \brief UBX Mask */


#define CFG_PRT		0x0600 						/**< \brief Port Configuration Mask */
#define CFG_USB		0x061B						/**< \brief USB Configuration Mask */
#define CFG_MSG		0x0601						/**< \brief Message (frame) rate Configuration Mask */
#define CFG_INF		0x0602						/**< \brief Information Message Configuration Mask */
#define CFG_NAV5	0x0624						/**< \brief Navigation Engine Settings Mask */
#define CFG_NAVX5	0x0623						/**< \brief Navigation Engine Expert Settings Mask */
#define CFG_DAT		0x0606						/**< \brief Stantard Datum Configuration Mask */
#define CFG_RATE	0x0608						/**< \brief Navigation/Measurement Rate Settings Mask */
#define CFG_SBAS	0x0616						/**< \brief SBAS (Satellite Based Augmentation Systems) Configuration Mask */
#define CFG_NMEA	0x0617						/**< \brief NMEA Protocol Configuration Mask */
#define CFG_TMODE	0x061D						/**< \brief Time Mode Settings Mask */
#define CFG_ESFGWT	0x0629						/**< \brief Gyro+wheel Tick Sol (GWT) - LEA-6R Settings Mask */
#define CFG_TP		0x0607						/**< \brief TimePulse Parameters Mask */
#define CFG_TP5		0x0631						/**< \brief TimePulse Parameters 5 Mask */
#define CFG_RXM		0x0611						/**< \brief RXM (Receiver Manager : Power Mode) Configuration Mask */
#define CFG_PM		0x0632						/**< \brief Power Management Configuration Mask */
#define CFG_PM2		0x063B						/**< \brief Extended Power Management Configuration Mask */
#define CFG_RINV	0x0634						/**< \brief Remote Inventory Configuration Mask */
#define CFG_ANT		0x0613						/**< \brief Antenna Control Settings Mask */

#define CFG_CFG		0x0609						/**< \brief Clear, Save and Load configurations mask */

#define CFG_ACK		0x0501						/**< \brief Acknowledge */
#define CFG_NACK	0x0500						/**< \brief Not-Acknowledge */


#define DEF_DTM		0xF00A						/**< \brief DTM (Datum Reference) NMEA Frame Mask */
#define DEF_GPQ		0xF040						/**< \brief GPQ (Poll Message) NMEA Frame Mask */
#define DEF_TXT		0xF041						/**< \brief TXT (Text Transmission) NMEA Frame Mask */

#define DEF_GBS		0xF009						/**< \brief GBS (GNSS Satellite Fault Detection) NMEA Frame Mask */
#define DEF_GGA		0xF000						/**< \brief Gga (Global Positioning System Fix Data) NMEA Frame Mask */
#define DEF_GLL		0xF001						/**< \brief GLL (Latitude and Longitude, with Time of Position Fix and Status) NMEA Frame Mask */
#define DEF_GRS		0xF006						/**< \brief GRS (GNSS Range Residuals) NMEA Frame Mask */
#define DEF_GSA		0xF002						/**< \brief Gsa (GNSS DOP and Active Satellites) NMEA Frame Mask */
#define DEF_GST		0xF007						/**< \brief Gst (GNSS Pseudo Range Error Statistics) NMEA Frame Mask */
#define DEF_GSV		0xF003						/**< \brief GSV (GNSS Satellites in View) NMEA Frame Mask */
#define DEF_RMC		0xF004						/**< \brief Rmc (Recommended Minimum Data) NMEA Frame Mask */
#define DEF_VTG		0xF005						/**< \brief VTG (Course over Ground and Ground Speed) NMEA Frame Mask */
#define DEF_ZDA		0xF008						/**< \brief Zda (Time and Date) NMEA Frame Mask */

/**
 *	\class CGPS
 *
 *	\brief 	The CGPS class allows to decode and get the GPS frame coming from a serial port.
 *
 *	It allows to configure a serial port for a GPS, decode some specific frame, reconfigure the GPS sends frame , and register a function to call on a specific event.
 *
 *	Event on which CGPS can call a fonction (callback) :
 *		- On PPS
 *		- On Day Change
 *		- On Hour Change
 *		- On a Specific UTC Hour (Hour, Minute, Second)
 *		- On a Receive of GPS Data
 *		- On a Receive of a Gga frame
 *		- On a Receive of a Zda frame
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.1a
 *
 *	\date 25 May, 2012
 *
 */
class CGPS : public CIODevice 
{
	private :
	/* Private Structures  */
	/***********************/
	
	/**
	 *	\struct GpsConfig
	 *	\brief	Structure which can contains the reconfiguration message to send to the GPS module.
	 *		
	 *  	The UBX reconfiguration message is divided in differents areas :
	 *			- 2 bytes which indicates that's a UBX transmission.
	 *			- One byte which indicate the message class (defines the basic subset of the message).
	 *			- One byte which indicate the message Id.
	 *			- 2 bytes which indicates the lengths of the payload area (Unsigned 16-Bit integer in Little Endian Format).
	 *			- Payload which is a variable length field. It contain the information sent to the GPS (ask for actual configuration, reconfiguration, ...).
	 *			- 2 checksums bytes.
	 *
	 *		Here is a list of defaults configurations of the most used configuration (DO NOT DELETE !!) :
	 *			Antenna default settings (CFG-ANT) :
	 *			b5 62 06 13 04 00 1b 00 8b a9 6c f8
	 *			
	 *			Datum default settings (CFG-DAT) :
	 *			b5 62 06 06 34 00 00 00 57 47 53 38 34 00 00 00 00 40 a6 54 58 41 88 6d 74 96 1d a4 72 40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 e2 a5
	 *			
	 *			UBX Information default Message (CFG-INF) : 
	 *			b5 62 06 02 0a 00 00 00 00 00 00 00 00 00 00 00 12 e6
	 *			
	 *			NMEA Information default Message (CFG-INF) : 
	 *			b5 62 06 02 0a 00 01 00 00 00 87 87 87 87 87 87 3d 03
	 *			
	 *			Gga message default configuration (CFG-MSG) :
	 *			b5 62 06 01 08 00 f0 00 01 01 01 01 01 01 05 38
	 *			
	 *			Navigation engine default settings (CFG-NAV5) :
	 *			b5 62 06 24 24 00 ff ff 00 03 00 00 00 00 10 27 00 00 05 00 fa 00 fa 00 64 00 2c 01 00 3c 00 00 00 00 00 00 00 00 00 00 00 00 4c 1c
	 *			
	 *			Navigation engine expert default settings (CFG-NAVX5) :
	 *			b5 62 06 23 28 00 00 00 ff ff 03 00 00 00 03 02 03 10 07 00 00 01 00 00 43 06 00 00 00 00 01 01 00 00 00 64 78 00 00 00 00 00 00 00 00 00 99 9a
	 *			
	 *			NMEA protocol default configuration (CFG-NMEA) :
	 *			b5 62 06 17 04 00 00 23 00 02 46 54
	 *			
	 *			Extended Power Management default configuration (CFG-PM2) :
	 *			b5 62 06 3b 2c 00 01 06 00 00 00 90 02 00 e8 03 00 00 10 27 00 00 00 00 00 00 02 00 00 00 2c 01 00 00 4f c1 03 00 86 02 00 00 fe 00 00 00 64 40 01 00 95 c5
	 *			
	 *			Power Management default configuration (CFG-PM) :
	 *			b5 62 06 32 18 00 00 06 00 00 04 90 00 00 e8 03 00 00 10 27 00 00 00 00 00 00 02 00 00 00 0e 0a
	 *			
	 *			DDC (I2C) port default configuration (CFG-PRT) :
	 *			b5 62 06 00 14 00 00 00 00 00 84 00 00 00 00 00 00 00 07 00 07 00 00 00 00 00 ac ea
	 *			
	 *			Uart 0 port default configuration (CFG-PRT) :
	 *			b5 62 06 00 14 00 01 00 00 00 c0 08 00 00 80 25 00 00 07 00 07 00 00 00 00 00 96 cd
	 *			
	 *			Uart 1 port default configuration (CFG-PRT) :
	 *			b5 62 06 00 14 00 02 00 00 00 c0 08 00 00 80 25 00 00 00 00 00 00 00 00 00 00 89 7f
	 *			
	 *			USB port default configuration (CFG-PRT) :
	 *			b5 62 06 00 14 00 03 00 00 00 00 00 00 00 00 00 00 00 07 00 07 00 00 00 00 00 2b e6
	 *			
	 *			SPI port default configuration (CFG-PRT) :
	 *			b5 62 06 00 14 00 04 00 00 00 00 32 00 00 00 00 00 00 07 00 07 00 00 00 00 00 5e e8
	 *			
	 *			Navigation/Measurement Rate default Settings (CFG-RATE) :
	 *			b5 62 06 08 06 00 e8 03 01 00 01 00 01 39
	 *			
	 *			Default contents of Remote Inventory (CFG-RINV) :
	 *			b5 62 06 34 18 00 00 4e 6f 74 69 63 65 3a 20 6e 6f 20 64 61 74 61 20 73 61 76 65 64 21 00 f9 d0
	 *			
	 *			Receiver Manager default configuration (CFG-RXM) :
	 *			b5 62 06 11 02 00 08 00 21 91
	 *			
	 *			Contents of SBAS default Configuration (CFG-SBAS) :
	 *			b5 62 06 16 08 00 01 03 03 00 51 62 06 00 e4 2f
	 *			
	 *			Timepulse defaults Parameters for Timepulse 1 (CFG-TP5) :
	 *			b5 62 06 31 20 00 00 00 00 00 32 00 00 00 40 42 0f 00 40 42 0f 00 00 00 00 00 a0 86 01 00 00 00 00 00 f7 00 00 00 c9 97
	 *			
	 *			Timepulse defaults Parameters for Timepulse 2 (CFG-TP5) :
	 *			b5 62 06 31 20 00 01 00 00 00 32 00 00 00 04 00 00 00 01 00 00 00 48 e8 01 00 a0 86 01 00 00 00 00 00 fe 00 00 00 e5 41
	 *			
	 *			Timepulse defaults Parameters (CFG-TP) :
	 *			b5 62 06 07 14 00 40 42 0f 00 a0 86 01 00 01 01 00 00 32 00 00 00 00 00 00 00 0d 6c 
	 *			
	 *			USB default configuration (CFG-USB) :
	 *			b5 62 06 1b 6c 00 46 15 a6 01 00 00 00 00 64 00 00 01 75 2d 62 6c 6f 78 20 41 47 20 2d 20 77 77 77 2e 75 2d 62 6c 6f 78 2e 63 6f 6d 00 00 00 00 00 00 75 2d 62 6c 6f 78 20 36 20 20 2d 20 20 47 50 53 20 52 65 63 65 69 76 65 72 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 50 dc
	 *
	 */
	struct GpsConfig {
		uint16_t ui16Sync;									/**< \brief UBX identificator */
		uint16_t ui16ClassId;								/**< \brief Message class and Message Id */
		uint16_t ui16Lenght;								/**< \brief Length of the payload area */
		char sPayloadAndChecksums[UBX_BUFFER_SIZE];			/**< \brief Payload area and chechsums */
	};
	
	public :
	/* Public Structures  */
	/**********************/
	
	/**
	 *	\struct	Gga
	 *	\brief	Structure which can contains the contents of a Gga frame.
	 *		
	 *  	Advantage of Gga : Essential fix data which provide 3D location and accuracy data.
	 *
	 *		Example of a Gga frame :
	 *		$GPGga,142849.00,4709.37314,N,00138.29601,W,1,06,2.73,00030,M,048,M,,*7D	(No DGPS)
	 */
	struct Gga {
		unsigned int uiHour;			/**< \brief Hour (UTC) */
		unsigned int uiMinute;			/**< \brief Minute */
		unsigned int uiSecond;			/**< \brief Second */
		unsigned int uiMilliSecond;		/**< \brief Millisecond */
		double dLatitude;				/**< \brief Latitude */
		double dLongitude;				/**< \brief Longitude */
		char cLatitudeRef;				/**< \brief Latitude reference */
		char cLongitudeRef;				/**< \brief Longitude reference */
		double dHDOP;					/**< \brief Horizontal Dillution Of Precision */
		double dAltitude;				/**< \brief Altitude */
		double dMSL;					/**< \brief Correction of the height of the geoid */
		unsigned int uiDGpsId;			/**< \brief Identification of the DGPS station */
		double dDGpsTime;				/**< \brief Number of seconds since the last DGPS update */
		unsigned int uiQuality;			/**< \brief GPS Quality Indicator : 0 = No GPS, 1 = GPS, 2 = DGPS */
		unsigned int uiNbSat;			/**< \brief Number of Satellites used to get these data */
	};

	/**
	 *	\struct	Zda
	 *	\brief	Structure which can contains the contents of a Zda frame.
	 *	
	 *		The Zda message comes out 100-500 msec after the PPS.
	 *  	Advantage of Zda : allow to get all the time information in one frame
	 *
	 *		Example of a Zda frame :
	 *		$GPZda,201530.00,04,07,2002,00,00*60
	 */
	struct Zda {
		unsigned int uiHour;			/**< \brief Hour (UTC) */
		unsigned int uiMinute;			/**< \brief Minute */
		unsigned int uiSecond;			/**< \brief Second */
		unsigned int uiMilliSecond;		/**< \brief Millisecond */
		unsigned int uiDay;				/**< \brief Day */
		unsigned int uiMonth;			/**< \brief Month */
		unsigned int uiYear;			/**< \brief Year */
	};

	/**
	 *	\struct	Rmc
	 *	\brief	Structure which can contains the contents of a Rmc frame.
	 *	
	 *		Advantages of Rmc : contains the Recommended Minimum data which come from a GPS
	 *  	
	 *		Example of a Rmc frame :
	 *		$GPRmc,143818.50,A,4709.37401,N,00138.29284,W,0.015,,220312,,,D*65
	 */
	struct Rmc {
		unsigned int uiHour;			/**< \brief Hour (UTC) */
		unsigned int uiMinute;			/**< \brief Minute */
		unsigned int uiSecond;			/**< \brief Second */
		unsigned int uiMilliSecond;		/**< \brief Millisecond */
		double dLatitude;				/**< \brief Latitude */
		double dLongitude;				/**< \brief Longitude */
		char cLatitudeRef;				/**< \brief Latitude reference */
		char cLongitudeRef;				/**< \brief Longitude reference */
		double dSpeed;					/**< \brief Spead */
		double dCape;					/**< \brief Cape */
		unsigned int uiDay;				/**< \brief Day */
		unsigned int uiMonth;			/**< \brief Month */
		unsigned int uiYear;			/**< \brief Year */
	};
	
	/**
	 *	\struct	Gsa
	 *	\brief	Structure which can contains the contents of a Gsa frame.
	 *	
	 *		Advantages of Gsa : identifies the GPS position fix mode, the Satellite Vehicles used for navigation, and the DOP value
	 *  	
	 *		Example of a Gsa frame :
	 *		$GPGsa,A,3,19,28,14,18,27,22,31,29,,,,,1.7,1.0,1.3*35
	 */
	struct Gsa {
		char c3DMode;					/**< \brief 3DMode : A->Automatic, M->Manual (force 2D or 3D operation) */
		unsigned int uiMode;			/**< \brief Mode */
		unsigned int uiID01;			/**< \brief ID of the first satellite vehicles used in position fix */
		unsigned int uiID02;			/**< \brief ID of the second satellite vehicles used in position fix */
		unsigned int uiID03;			/**< \brief ID of the third satellite vehicles used in position fix */
		unsigned int uiID04;			/**< \brief ID of the forth satellite vehicles used in position fix */
		unsigned int uiID05;			/**< \brief ID of the fifth satellite vehicles used in position fix */
		unsigned int uiID06;			/**< \brief ID of the sixth satellite vehicles used in position fix */
		unsigned int uiID07;			/**< \brief ID of the seventh satellite vehicles used in position fix */
		unsigned int uiID08;			/**< \brief ID of the eighth satellite vehicles used in position fix */
		unsigned int uiID09;			/**< \brief ID of the nineth satellite vehicles used in position fix */
		unsigned int uiID10;			/**< \brief ID of the tenth satellite vehicles used in position fix */
		unsigned int uiID11;			/**< \brief ID of the eleventh satellite vehicles used in position fix */
		unsigned int uiID12;			/**< \brief ID of the twelfth satellite vehicles used in position fix */
		double dPDOP;					/**< \brief Positional Dilution of Precision */
		double dHDOP;					/**< \brief Horizontal Dilution of Precision */
		double dVDOP;					/**< \brief Vertical Dilution of Precision */
	};

	/**
	 *	\struct	Gst
	 *	\brief	Structure which can contains the contents of a Gst frame.
	 *	
	 *		Advantages of Gst : it is used to support Receiver Autonomous Integrity Monitoring (RAIM)
	 *  	
	 *		Example of a Gst frame :
	 *		$GPGst,220320.0,1.3,0.8,0.5,166.1,0.8,0.5,1.6,*4F
	 */
	struct Gst {
		unsigned int uiHour;			/**< \brief Hour (UTC) */
		unsigned int uiMinute;			/**< \brief Minute */
		unsigned int uiSecond;			/**< \brief Second */
		unsigned int uiMilliSecond;		/**< \brief Millisecond */
		double dRMS;					/**< \brief sigma */
		double dSDoSMajAoEE;			/**< \brief Standard Deviation of Semi-Major Axis of Error Ellipse in meters */
		double dSDoSMinAoEE;			/**< \brief Standard Deviation of Semi-Minor Axis of Error Ellipse in meters */
		double dOoSMajAoEE;				/**< \brief	Orientation of Semi-Minor Axis of Error Ellipse in dergrees from True North */
		double dSDoLatE;				/**< \brief Standard Deviation of Latitude Error in meters */
		double dSDoLongE;				/**< \brief Standard Deviation of Longitude Error in meters */
		double dSDoAltE;				/**< \brief Standard Deviation of Altitude Error in meters */
	};
	
	private :
  	/* Attributes */
	/**************/
	bool m_bActivateGbsFrame;		/**< \brief GPS Gbs frames activation boolean */
	bool m_bActivateGgaFrame;		/**< \brief GPS Gga frames activation boolean */
	bool m_bActivateGllFrame;		/**< \brief GPS GGL frames activation boolean */
	bool m_bActivateGrsFrame;		/**< \brief GPS GRS frames activation boolean */
	bool m_bActivateGsaFrame;		/**< \brief GPS Gsa frames activation boolean */
	bool m_bActivateGstFrame;		/**< \brief GPS Gst frames activation boolean */
	bool m_bActivateGsvFrame;		/**< \brief GPS GSV frames activation boolean */
	bool m_bActivateRmcFrame;		/**< \brief GPS Rmc frames activation boolean */
	bool m_bActivateVtgFrame;		/**< \brief GPS Vtg frames activation boolean */
	bool m_bActivateZdaFrame;		/**< \brief GPS Zda frames activation boolean */
	
	bool m_bPrintGga; 				/**< \brief GDA frames print boolean */
	bool m_bPrintZda; 				/**< \brief Zda frames print boolean */
	bool m_bPrintRmc; 				/**< \brief Rmc frames print boolean */
	bool m_bPrintGst; 				/**< \brief Gst frames print boolean */
	bool m_bPrintGsa; 				/**< \brief Gsa frames print boolean */
	
	bool m_bRtsCts;					/**< \brief RTS/CTS mode boolean */
	bool m_bXonXoff;				/**< \brief XOn/XOff mode boolean */
	
	bool m_bHasBeenReconfigure;		/**< \brief Boolean which is activated when the Gps module has already been reconfigurate */
	
	bool m_bDayChange;				/**< \brief Boolean which indicates if the event on day change will be launched on the next Pps signal */
	bool m_bHourChange;				/**< \brief Boolean which indicates if the event on hour change will be launched on the next Pps signal */
	bool m_bOnSpecHour;				/**< \brief Boolean which indicates if the event on specific hour can appear */
	
	bool m_bThreadRunning;			/**< \brief Boolean which indicates if the thread should run or stop. */
	
	bool m_bSynchronizationDriverConnected;		/**< \brief Boolean which indicates if the class is correctly connected to the synchronization driver. */
	bool m_bGgaDataReceived;		/**< \brief Boolean which indicates if a Gga data has been received since the last Pps signal. */
	bool m_bZdaDataReceived;		/**< \brief Boolean which indicates if a Zda data has been received since the last Pps signal. */
	
	unsigned int m_uiBaud;			/**< \brief Integer that indicates the baudrate of the GPS device */
	unsigned int m_uiSpecUtcHour;		/**< \brief Attributes that specifies the declenchement hour for the specific hour event */
	unsigned int m_uiSpecUtcMinute;		/**< \brief Attributes that specifies the declenchement minute for the specific hour event */
	unsigned int m_uiSpecUtcSecond;		/**< \brief Attributes that specifies the declenchement second for the specific hour event */
	
	char * m_sDevicePath;			/**< \brief Path of the serial port used to communicate with the gps module */
	char * m_sFormat;				/**< \brief Format of the serial communication of the GPS */
	
	Gga GgaFrame;					/**< \brief Content of the last decoded Gga frame */
	Zda ZdaFrame;					/**< \brief Content of the last decoded Zda frame */
	Rmc RmcFrame;					/**< \brief Content of the last decoded Rmc frame */
	Gsa GsaFrame;					/**< \brief Content of the last decoded Gsa frame */
	Gst GstFrame;					/**< \brief Content of the last decoded Gst frame */
	
	GpsConfig GpsConfigFrame;		/**< \brief Structure used to send a reconfiguration frame to the uBlox Gps module. */
	
	CUart * m_pSerial;				/**< \brief Pointer to the Uart used in the CGPS class */
	
	/** \brief Thread which will be launched to detect when a Pps signal is received. */
	boost::thread						m_oPpsThread;
	
	/** \brief Connexion Id of GpsNmeaReception */
	boost::signals2::connection 		m_oIdCallbackUart;
	
	// Callback signals
	boost::signals2::signal<CallBack> 	m_oSigOnPps;			/**< \brief Signal which allow to launch the callback(s) connected on the On Pps event */
	boost::signals2::signal<CallBack> 	m_oSigOnDayChange;		/**< \brief Signal which allow to launch the callback(s) connected on the On Day Change event */
	boost::signals2::signal<CallBack> 	m_oSigOnHourChange;		/**< \brief Signal which allow to launch the callback(s) connected on the On Hour Change event */
	boost::signals2::signal<CallBack> 	m_oSigOnSpecUTCHour;	/**< \brief Signal which allow to launch the callback(s) connected on the On a Specific UTC Hour event */
	boost::signals2::signal<CallBack> 	m_oSigOnGpsData;		/**< \brief Signal which allow to launch the callback(s) connected on the On a Receive of GPS Data event */
	boost::signals2::signal<CallBack> 	m_oSigOnGgaData;		/**< \brief Signal which allow to launch the callback(s) connected on the On a Receive of a Gga frame event */
	boost::signals2::signal<CallBack> 	m_oSigOnZdaData;		/**< \brief Signal which allow to launch the callback(s) connected on the On a Receive of a Zda frame */
	
	/* Methods   */
	/*************/

	// Method who communicate continously with the synchronization driver to know when a Pps is received and manage the launch of the OnPps CallBacks
	void PpsDetection ();

	// Ubx Checksum verification method
	bool UbxVerifyChecksum (char * sBuffer, int iBufferSize);
	
	// Ubx checksum generation method
	bool AddUbxChecksum (char * sBuffer, int iBufferSize);
	
	// Send back the size of the Ubx frame to send
	int GetUbxFrameSize ();
	
	// Send the message currently in the GpsConfigFrame structure
	int SendUbxFrame ();
	
	// Reconfigure the GPS module using Ubx frame
	int GpsUbxConfiguration ();
	
	// Ubx data reception and decodage on Gps serial link method
	void GpsUbxReception (char * sBuffer, int iBufferSize);
	
	// Nmea Checksum verification method
	bool NmeaVerifyChecksum (char * sBuffer, int iBufferSize);
	
	// Check method and decoding of the data received on the serial link
	void GpsNmeaCheckAndDecode (char * sBuffer, int iBufferSize);
	
	// Nmea data reception on Gps serial link method
	void GpsNmeaReception (char * sBuffer, int iBufferSize);
	
	// Gps data reception method
	void GpsReception (char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2);
	
	// Frame decoding and printing methods
	void GpsDecodeGga (char * sBuffer, int iBufferSize);
	void GpsDecodeZda (char * sBuffer, int iBufferSize);
	void GpsDecodeRmc (char * sBuffer, int iBufferSize);
	void GpsDecodeGst (char * sBuffer, int iBufferSize);
	void GpsDecodeGsa (char * sBuffer, int iBufferSize);
	
	public :
	/* Methods  */
	/************/
	// Constructor
	CGPS (const char * sDevicePath = (char *) "/dev/ttyO0", unsigned int uiBaud = B115200, char * sFormat = (char *) "8N1", bool bRtsCts = false, bool bXonXoff = false);
	// Destructor
	~CGPS ();
	
	// Initialization of a CGPS object
	virtual int Open ();
	
	// Reconfigure the Gps module from its default configuration
	int GpsReconfigurationFromDefault ();
	
	// Methods which enable/disable frame displays when they're received
	void SetPrintGga (bool bPrintGga);
	void SetPrintZda (bool bPrintZda);
	void SetPrintRmc (bool bPrintRmc);
	void SetPrintGst (bool bPrintGst);
	void SetPrintGsa (bool bPrintGsa);
	
	// Method which set the hour when the OnSpecHourCallback will be launched
	int SetCallbackUtcHour (unsigned int uiHour, unsigned int uiMinute, unsigned int uiSeconde);
	
	// Managing the registration of callbacks depending on the event indicated
	virtual int RegisterCallback (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1 = NULL, void * pData2 = NULL);			// Register a Callback which will be called when the event specified in EnventID will occur
	virtual int UnregisterCallback (unsigned int uiEventId, boost::signals2::connection oIdfct);	// Unregister a previously register callback using its connection Id
	virtual int UnregisterAllCallbacks (unsigned int uiEventId);		// Unregister all previously register callback on a specific event
	virtual boost::signals2::connection GetLastFctId ();			// Send back the last connected callback connexion Id
	int TotalNumberOfConnectedCallbacks ();							// Method which return the total number of connected callback(s)

	// Send back the altitude contents in the last received Gga frame
	double GetAltitude ();				// Send back the current altitude.
	
	// Send back informations about the precision of the localisation
	double GetSDoSMinAoEE ();			// Send back the Standard deviation of semi-minor axis of error ellipse in meters.
	double GetSDoSMajAoEE ();			// Send back the Standard deviation of semi-major axis of error ellipse in meters.
	double GetVPrec ();					// Send back the GPS vertical precision en meters.
	double GetHPrec ();					// Send back the GPS horizontal precision en meters.
	
	// Methods which allow to get the driver paramaters
	int GetDriverRunningMode ();		// Get the driver running mode
	int GetDriverState ();				// Get the driver state machine state
	int GetGpsStatus ();				// Get the gps module status (On/Off)
	int GetTimeSinceGpsOff ();			// Get the time since the gps module is Off
	int GetGpsOffTime ();				// Get the time during which the Gps should be turned off
	int GetTemperature ();				// Get the temperature (using the quartz error drift)
	int GetCaptureTimerClock ();		// Get the capture timer clock
	int GetRelayTimerClock ();			// Get the relay timer clock
	
	// Methods which allow to set the driver paramaters
	int SetDriverRunningMode (unsigned int uiDriverRunningMode);		// Set the driver running mode
	int SetGpsOffTime (unsigned int uiGpsOffTime);						// Set the time during which the Gps should be turned off
	
	//	Method which print the configuration of the Gps
	virtual int PrintConfig ();
};

#endif //CGPS_H
