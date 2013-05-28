/**
 *	\file	sdk_errors.h
 *
 *	\brief 	This file is used to define the errors returns.
 *	
 *	\author Jonathan Aillet
 *
 *	\version v1.0a
 *
 *	\date 16 July, 2012
 *
 */

#ifndef SDK_ERRORS_H
#define SDK_ERRORS_H

// Booked for the ifsttar drivers errors (1000-1999) (see ifsttar-drivers/drivers_errors.h)


// CEvent (2000-2999)


// CIODevice (3000-3999)
	// int CIODevice :: Open () const
	#define	ERR_CIODEVICE_OPEN_NOT_IMPLEMENTED									3001	/**< \brief The Open method hasn't been implemented in the current class */
	// int CIODevice :: Close () const
	#define	ERR_CIODEVICE_CLOSE_NOT_IMPLEMENTED									3051	/**< \brief The Close method hasn't been implemented in the current class */
	// int CIODevice :: Start () const
	#define	ERR_CIODEVICE_START_NOT_IMPLEMENTED									3101	/**< \brief The Start method hasn't been implemented in the current class */
	// int CIODevice :: Stop () const 
	#define	ERR_CIODEVICE_STOP_NOT_IMPLEMENTED									3151	/**< \brief The Stop method hasn't been implemented in the current class */
	// int CIODevice :: Read (char * sBuffer, unsigned int uiBufferSize) const
	#define	ERR_CIODEVICE_READ_NOT_IMPLEMENTED									3201	/**< \brief The Read method hasn't been implemented in the current class */
	// int CIODevice :: Write (char * sBuffer, unsigned int uiBufferSize) const
	#define	ERR_CIODEVICE_WRITE_NOT_IMPLEMENTED									3251	/**< \brief The Write method hasn't been implemented in the current class */
	// int CIODevice :: PrintConfig () const
	#define	ERR_CIODEVICE_PRINTCONFIG_NOT_IMPLEMENTED							3301	/**< \brief The PrintConfig method hasn't been implemented in the current class */
	// int CIODevice :: RegisterCallBack (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2) const
	#define	ERR_CIODEVICE_REGISTERCALLBACK_NOT_IMPLEMENTED						3351	/**< \brief The RegisterCallBack method hasn't been implemented in the current class */
	// int CIODevice :: UnregisterCallBack (unsigned int uiEventId, boost::signals2::connection oIdFct) const
	#define	ERR_CIODEVICE_UNREGISTERCALLBACK_NOT_IMPLEMENTED					3401	/**< \brief The UnregisterCallBack method hasn't been implemented in the current class */
	// int CIODevice :: UnregisterAllCallBacks (unsigned int uiEventId) const
	#define	ERR_CIODEVICE_UNREGISTERALLCALLBACKS_NOT_IMPLEMENTED				3451	/**< \brief The UnregisterAllCallBacks method hasn't been implemented in the current class */
	// bool CIODevice :: IsOpenned () const
	#define	ERR_CIODEVICE_ISOPENNED_NOT_IMPLEMENTED								3501	/**< \brief The IsOpenned method hasn't been implemented in the current class */
	// unsigned int CIODevice :: GetOpenMode () const
	#define	ERR_CIODEVICE_GETOPENMODE_NOT_IMPLEMENTED							3551	/**< \brief The GetOpenMode method hasn't been implemented in the current class */
	// boost::signals2::connection CIODevice :: GetLastFctId() const
	#define	ERR_CIODEVICE_GATLASTFCTID_NOT_IMPLEMENTED							3601	/**< \brief The GetLastFctId method hasn't been implemented in the current class */

// CGPIO (4000-4999)
	// int CGPIO :: Open()
	#define	ERR_CGPIO_OPEN_SPECIFIED_PATH										4001	/**< \brief The specified Gpio path is wrong */
	#define	ERR_CGPIO_OPEN_GPIO_NOT_USEABLE										4002	/**< \brief Thie Gpio isn't useable */
	#define	ERR_CGPIO_OPEN_GPIO_DRIVER_OPEN										4003	/**< \brief The connexion with the driver failed */
	#define	ERR_CGPIO_OPEN_SET_DIRECTION										4004	/**< \brief An error occure during the Gpio direction setting */
	#define	ERR_CGPIO_OPEN_SET_OUTPUT_VALUE										4005	/**< \brief An error occure during the Gpio default output value setting */
	#define	ERR_CGPIO_OPEN_SPECIFIED_EVENT										4006	/**< \brief The specified Gpio active event is wrong */
	#define	ERR_CGPIO_OPEN_SPECIFIED_DEBOUNCE_DELAY								4007	/**< \brief The specified Gpio debounce delay is wrong */
	#define	ERR_CGPIO_OPEN_SET_DEBOUNCE_DELAY									4008	/**< \brief An error occure during the Gpio debounce delay setting */
	#define	ERR_CGPIO_OPEN_SET_INTERRUPT_MODE									4009	/**< \brief An error occure during the Gpio interrupt mode setting */
	// int CGPIO :: Read()
	#define	ERR_CGPIO_READ_WRONG_MODE											4051	/**< \brief The IT mode is set while the I/O mode is needed */
	#define	ERR_CGPIO_READ_SET_DIRECTION										4052	/**< \brief An error occure during the Gpio direction setting */
	#define	ERR_CGPIO_READ_GPIO_DRIVER_READ										4053	/**< \brief An error occure during the Gpio read (in the Gpio driver) */
	#define	ERR_CGPIO_READ_WRONG_READ_VALUE										4054	/**< \brief The driver send back a wrong value during the read */
	// int CGPIO :: Write (bool bValue)
	#define	ERR_CGPIO_WRITE_WRONG_MODE											4101	/**< \brief The IT mode is set while the I/O mode is needed */
	#define	ERR_CGPIO_WRITE_SET_DIRECTION										4102	/**< \brief An error occure during the Gpio direction setting */
	#define	ERR_CGPIO_WRITE_GPIO_DRIVER_WRITE									4103	/**< \brief An error occure during the Gpio write (in the Gpio driver) */
	// int CGPIO :: SetDelay (unsigned int uiDelay)
	#define	ERR_CGPIO_SETDELAY_SPECIFIED_DEBOUNCE_DELAY							4151	/**< \brief The specified Gpio debounce delay is wrong */
	#define	ERR_CGPIO_SETDELAY_SET_DEBOUNCE_DELAY								4152	/**< \brief An error occured during the Gpio debounce delay setting */
	// int CGPIO :: RegisterCallback (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2)
	#define	ERR_CGPIO_REGISTERCALLBACK_WRONG_MODE								4204	/**< \brief The IO mode is set while the IT mode is needed */
	#define	ERR_CGPIO_REGISTERCALLBACK_THREAD_MANAGING							4201	/**< \brief An error occurred in the thread managing (the callback hasn't been registered) */
	#define	ERR_CGPIO_REGISTERCALLBACK_REGISTERING								4202	/**< \brief An error occurred during the callback registering (using 'connect' function) */
	#define	ERR_CGPIO_REGISTERCALLBACK_SPECIFIED_EVENT							4203	/**< \brief The event specified during the method call is not recognized in this class */
	// int CGPIO :: UnregisterCallback(unsigned int uiEventId, boost::signals2::connection oFctId)
	#define	ERR_CGPIO_UNREGISTERCALLBACK_WRONG_MODE								4254	/**< \brief The IO mode is set while the IT mode is needed */
	#define	ERR_CGPIO_UNREGISTERCALLBACK_NO_CALLBACK							4251	/**< \brief No callback are currently connected to the specified event */
	#define	ERR_CGPIO_UNREGISTERCALLBACK_UNREGISTERING							4252	/**< \brief An error occurred during the callback unregistering (using 'disconnect' function) */
	#define	ERR_CGPIO_UNREGISTERCALLBACK_SPECIFIED_EVENT						4253	/**< \brief The event specified during the method call is not recognized in this class */
	// int CGPIO :: UnregisterAllCallbacks (unsigned int uiEventId)
	#define	ERR_CGPIO_UNREGISTERALLCALLBACKS_WRONG_MODE							4304	/**< \brief The IO mode is set while the IT mode is needed */
	#define	ERR_CGPIO_UNREGISTERALLCALLBACKS_NO_CALLBACK						4301	/**< \brief No callback are currently connected to the specified event */
	#define	ERR_CGPIO_UNREGISTERALLCALLBACKS_UNREGISTERING						4302	/**< \brief An error occurred during the callbacks unregistering (using 'disconnect_all_slots') */
	#define	ERR_CGPIO_UNREGISTERALLCALLBACKS_SPECIFIED_EVENT					4303	/**< \brief The event specified during the method call is not recognized in this class */

// CGPS (5000-5999)
	// int CGPS :: Open ()
	#define	ERR_CGPS_OPEN_GPIO_CONFIGURATION									5001	/**< \brief An error occurred during the software shutdown gpio configuration */
	#define	ERR_CGPS_OPEN_UART_CREATION											5002	/**< \brief An error occurred during the CUart object creation */
	#define	ERR_CGPS_OPEN_UART_OPENING											5003	/**< \brief An error occurred during the CUart object opening */
	#define	ERR_CGPS_OPEN_UART_CALLBACK_REGISTRATION							5004	/**< \brief An error occurred during the CUart object registration */
	#define	ERR_CGPS_OPEN_GPS_RECONFIGURATION_FROM_DEFAULT						5005	/**< \brief An error occurred during the Gps module reconfiguration from its default configuration */
	#define	ERR_CGPS_OPEN_SYNCHRONIZATION_DRIVER_LINK							5005	/**< \brief An error occurred during the synchronization driver link opening */
	// int CGPS :: GpsReconfigurationFromDefault ()
	#define	ERR_CGPS_GPSRECONFIGURATIONFROMDEFAULT_UART_RECONFIGURATION			5051	/**< \brief An error occurred during the CUart reconfiguration */
	#define	ERR_CGPS_GPSRECONFIGURATIONFROMDEFAULT_GPS_RECONFIGURATION			5052	/**< \brief An error occurred during the Gps module reconfiguration */
	#define	ERR_CGPS_GPSRECONFIGURATIONFROMDEFAULT_SAVE_NEW_GPS_CONFIGURATION 	5053	/**< \brief An error occurred during the Gps module new configuration saving */
	// int CGPS :: GetUbxFrameSize ()
	#define ERR_CGPS_GETUBXFRAMESIZE_FRAME_TOO_LONG								5101	/**< \brief The Ubx frame we want to send is too long */
	// int CGPS :: SendUbxFrame ()
	#define ERR_CGPS_SENDUBXFRAME_FRAME_TOO_LONG								5151	/**< \brief The Ubx frame we want to send is too long */
	#define ERR_CGPS_SENDUBXFRAME_NOT_UBX_FRAME									5152	/**< \brief The frame we want to send is not an Ubx frame */
	#define ERR_CGPS_SENDUBXFRAME_WRONG_CLASS_ID								5153	/**< \brief The Ubx frame Id we want to send is wrong */
	// int CGPS :: GpsUbxConfiguration ()
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_GBS_RECONFIGURATION				5201	/**< \brief The Ubx Gbs Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_GGA_RECONFIGURATION				5202	/**< \brief The Ubx Gga Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_GLL_RECONFIGURATION				5203	/**< \brief The Ubx Gll Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_GRS_RECONFIGURATION				5204	/**< \brief The Ubx Grs Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_GSA_RECONFIGURATION				5205	/**< \brief The Ubx Gsa Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_GST_RECONFIGURATION				5206	/**< \brief The Ubx Gst Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_GSV_RECONFIGURATION				5207	/**< \brief The Ubx Gsv Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_RMC_RECONFIGURATION				5208	/**< \brief The Ubx Rmc Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_VTG_RECONFIGURATION				5209	/**< \brief The Ubx Vtg Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_NMEA_ZDA_RECONFIGURATION				5210	/**< \brief The Ubx Zda Nmea reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_DDC_PORT_RECONFIGURATION				5211	/**< \brief The DDC (I2C) port reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_SPI_PORT_RECONFIGURATION				5212	/**< \brief The SPI port reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_USB_PORT_RECONFIGURATION				5213	/**< \brief The USB port reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_UART1_PORT_RECONFIGURATION				5214	/**< \brief The Uart 1 port reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_UART2_PORT_RECONFIGURATION				5215	/**< \brief The Uart 2 port reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_ANTENNA_RECONFIGURATION				5216	/**< \brief The antenna reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_POWER_MANAGEMENT_RECONFIGURATION		5217	/**< \brief The power management reconfiguration frame haven't been sent correctly */
	#define ERR_CGPS_GPSUBXCONFIGURATION_RECEIVER_MANAGER_RECONFIGURATION		5218	/**< \brief The recevier manager reconfiguration frame haven't been sent correctly */
	// int CGPS :: SetCallbackUtcHour (unsigned int uiHour, unsigned int uiMinute, unsigned int uiSecond)
	#define ERR_CGPS_SETCALLBACKUTCHOUR_SPECIFIED_HOUR							5251	/**< \brief The specified Utc hour is wrong */
	// int CGPS :: RegisterCallback(unsigned int uiIdEvent, boost::function<CallBack> oFct, void * pData1, void * pData2)
	#define	ERR_CGPS_REGISTERCALLBACK_THREAD_MANAGING							5301	/**< \brief An error occurred in the thread managing (the callback hasn't been registered) */
	#define	ERR_CGPS_REGISTERCALLBACK_REGISTERING								5302	/**< \brief An error occurred during the callback registering (using 'connect' function) */
	#define	ERR_CGPS_REGISTERCALLBACK_SPECIFIED_EVENT							5303	/**< \brief The event specified during the method call is not recognized in this class */
	// int CGPS :: UnregisterCallback(unsigned int uiIdEvent, boost::signals2::connection oIdFct)
	#define	ERR_CGPS_UNREGISTERCALLBACK_NO_CALLBACK								5351	/**< \brief No callback are currently connected to the specified event */
	#define	ERR_CGPS_UNREGISTERCALLBACK_UNREGISTERING							5352	/**< \brief An error occurred during the callback unregistering (using 'disconnect' function) */
	#define	ERR_CGPS_UNREGISTERCALLBACK_SPECIFIED_EVENT							5353	/**< \brief The event specified during the method call is not recognized in this class */
	// int CGPS :: UnregisterAllCallbacks (unsigned int uiIdEvent)
	#define	ERR_CGPS_UNREGISTERALLCALLBACKS_NO_CALLBACK							5401	/**< \brief No callback are currently connected to the specified event */
	#define	ERR_CGPS_UNREGISTERALLCALLBACKS_UNREGISTERING						5402	/**< \brief An error occurred during the callbacks unregistering (using 'disconnect_all_slots') */
	#define	ERR_CGPS_UNREGISTERALLCALLBACKS_SPECIFIED_EVENT						5403	/**< \brief The event specified during the method call is not recognized in this class */
	// int CGPS :: GetDriverRunningMode ()
	#define	ERR_CGPS_GETDRIVERRUNNINGMODE_GET_DRIVER_RUNNING_MODE				5451	/**< \brief An error occured during the communication with the synchronization driver */
	// int CGPS :: GetDriverState ()
	#define	ERR_CGPS_GETDRIVERSTATE_GET_DRIVER_STATE							5501	/**< \brief An error occured during the communication with the synchronization driver */
	// int CGPS :: GetGpsStatus ()
	#define	ERR_CGPS_GETGPSSTATUS_GET_GPS_STATUS								5551	/**< \brief An error occured during the communication with the synchronization driver */
	// int CGPS :: GetTimeSinceGpsOff ()
	#define	ERR_CGPS_GETTIMESINCEGPSOFF_GET_TIME_SINCE_GPS_OFF					5601	/**< \brief An error occured during the communication with the synchronization driver */
	// iint CGPS :: GetGpsOffTime ()
	#define	ERR_CGPS_GETGPSOFFTIME_GET_GPS_OFF_TIME								5651	/**< \brief An error occured during the communication with the synchronization driver */
	// int CGPS :: GetTemperature ()
	#define	ERR_CGPS_GETTEMPERATURE_GET_TEMPERATURE								5701	/**< \brief An error occured during the communication with the synchronization driver */
	// int CGPS :: GetCaptureTimerClock ()
	#define	ERR_CGPS_GETCAPTURETIMERCLOCK_GET_CAPTURE_TIMER_CLOCK				5751	/**< \brief An error occured during the communication with the synchronization driver */
	// int CGPS :: GetRelayTimerClock ()
	#define	ERR_CGPS_GETRELAYTIMERCLOCK_GET_CAPTURE_TIMER_CLOCK					5801	/**< \brief An error occured during the communication with the synchronization driver */
	// int CGPS :: SetDriverRunningMode (unsigned int uiDriverRunningMode)
	#define	ERR_CGPS_SETDRIVERRUNNINGMODE_SPECIFIED_MODE						5851	/**< \brief The mode specified during this methos call is wrong */
	#define	ERR_CGPS_SETDRIVERRUNNINGMODE_SET_DRIVER_RUNNING_MODE				5852	/**< \brief An error occured during the communication with the synchronization driver */
	// int CGPS :: SetGpsOffTime (unsigned int uiGpsOffTime)
	#define	ERR_CGPS_SETGPSOFFTIME_SPECIFIED_GPS_OFF_TIME						5901	/**< \brief The Gps Off time specified during this methos call is wrong */
	#define	ERR_CGPS_SETGPSOFFTIME_SET_GPS_OFF_TIME								5902	/**< \brief An error occured during the communication with the synchronization driver */

// CMessagePump (6000-6999)


// CTcpIpClient (7000-7999)


// CTcpIpServer (8000-8999)


// CSpiPort (9000-9999)


// CUart (10000-10999)
	// int CUart :: Open()
	#define	ERR_CUART_OPEN_UART_OPEN											10001	/**< \brief An error occurred during the Uart opening */
	#define	ERR_CUART_OPEN_UART_CONFIGURATION									10002	/**< \brief An error occurred during the Uart configuration */
	// int CUart :: Configuration()
	#define	ERR_CUART_CONFIGURATION_SPECIFIED_FORMAT							10051	/**< \brief The specified format is wrong */
	#define	ERR_CUART_CONFIGURATION_SET_OUTPUT_BAUDRATE							10052	/**< \brief An error occurred during the output baudrate settup */
	#define	ERR_CUART_CONFIGURATION_SET_INPUT_BAUDRATE							10053	/**< \brief An error occurred during the input baudrate settup */
	#define	ERR_CUART_CONFIGURATION_CLEAN_UART_BUFFER							10054	/**< \brief The internal uart buffer can not be well cleaned */
	#define	ERR_CUART_CONFIGURATION_APPLY_CONFIGURATION							10055	/**< \brief An error occurred during applying the new configuration */
	// int CUart :: Write (char * sBuffer, unsigned int iBufferSize)
	#define	ERR_CUART_WRITE_SPECIFIED_BUFFER									10101	/**< \brief The passed buffer is empty */
	#define	ERR_CUART_WRITE_UART_WRITE											10102	/**< \brief An error occured during the write on the Uart port */
	// int CUart :: RegisterCallback (unsigned int iEventID, boost::function<CallBack> oFct, void * pData1, void * pData2)
	#define	ERR_CUART_REGISTERCALLBACK_THREAD_MANAGING							10151	/**< \brief An error occurred in the thread managing (the callback hasn't been registered) */
	#define	ERR_CUART_REGISTERCALLBACK_REGISTERING								10152	/**< \brief An error occurred during the callback registering (using 'connect' function) */
	#define	ERR_CUART_REGISTERCALLBACK_SPECIFIED_EVENT							10153	/**< \brief The event specified during the method call is not recognized in this class */
	// int CUart :: UnregisterCallback (unsigned int iEventID, boost::signals2::connection oIdFct)
	#define	ERR_CUART_UNREGISTERCALLBACK_NO_CALLBACK							10201	/**< \brief No callback are currently connected to the specified event */
	#define	ERR_CUART_UNREGISTERCALLBACK_UNREGISTERING							10202	/**< \brief An error occurred during the callbacks unregistering (using 'disconnect' function) */
	#define	ERR_CUART_UNREGISTERCALLBACK_SPECIFIED_EVENT						10203	/**< \brief The event specified during the method call is not recognized in this class */
	// int CUart :: UnregisterAllCallbacks (unsigned int iEventID)
	#define	ERR_CUART_UNREGISTERALLCALLBACKS_NO_CALLBACK						10251	/**< \brief No callback are currently connected to the specified event */
	#define	ERR_CUART_UNREGISTERALLCALLBACKS_UNREGISTERING						10252	/**< \brief An error occurred during the callbacks unregistering (using 'disconnect_all_slots') */
	#define	ERR_CUART_UNREGISTERALLCALLBACKS_SPECIFIED_EVENT					10253	/**< \brief The event specified during the method call is not recognized in this class */

#endif // SDK_ERRORS_H
