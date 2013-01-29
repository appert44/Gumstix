/**
 *	\file	CUart.h
 *
 *	\brief 	This header file is used to declare the CUart class and all its methods.
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#ifndef CUART_H
#define CUART_H

#include "CIODevice.h"


/**
 *	\class CUart
 *
 *	\brief 	The CUart class allows to use a serial port.
 *
 *	It allows to configure, read, write, and register a function to call when a data is received.
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */
class CUart : public CIODevice 
{
	private :
	/* Attributes */
	/**************/
	int				m_iNumUart;					/**< \brief Number of the selected Serial port. */
	bool 			m_bThreadRunning;			/**< \brief Boolean which indicates if the thread should run or stop. */
	unsigned int 	m_uiBaud;					/**< \brief Uart baudrate configuration attribute. */
	char * 			m_sFormat;					/**< \brief Uart format configuration attribute (Ex : 8N1). */
	bool 			m_bRtsCts;					/**< \brief Uart Rts/Cts configuration attribute. */
	bool 			m_bXonXoff;					/**< \brief Uart Xon/Xoff configuration attribute. */
	bool 			m_bCanonicalReading;		/**< \brief Uart canonical reading configuration attribute. */
	
	char * 			m_sSerialPath;				/**< \brief Buffer which contains the path of the serial link currently used. */
	char * 			m_sRxBuffer;				/**< \brief Recepetion buffer. */
	
	struct termios 	m_oOrigUartConf;			/**< \brief Uart original saved configuration. */
	
	/** \brief Thread which will be launched to read the data received on the Uart and to launch the callbacks. */
	boost::thread						m_oThreadRx;
	
	// Callback(s) signal(s)
	boost::signals2::signal<CallBack> 	m_oSigOnRec;		/**< \brief Callback signal which is called when a data is received on the serial link. */
	
	/* Methods  */
	/************/
	// Configure the serial port with the configuration sent to the constructor
	int Configuration ();
	
	//	Method who manage the launch of callbacks when data are received on the serial link
	void OnReceive ();
	
	public :
	/* Methods  */
	/************/
 	//	Constructor of the CUart class
	CUart (const char * sDevicePath, unsigned int iBaud, char * sFormat, bool bRtsCts, bool bXonXoff, bool bCanonicalReading);
	//	Destructor of the CUart class
	~CUart ();
	
	// Initialization of a CUart object
	virtual int Open ();

	//	Send data using the serial link
	virtual int Write (char * sBuffer, unsigned int uiBufferSize);
	
	// Change the serial port configuration
	int ChangePortConfiguration (unsigned int iBaud, char * sFormat, bool bRtsCts, bool bXonXoff, bool bCanonicalReading);

	//	Managing the registration of callbacks
	virtual int RegisterCallback (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1 = NULL, void * pData2 = NULL);			// Register a Callback which will be called when the event specified in EnventID will occur
	virtual int UnregisterCallback (unsigned int uiEventId, boost::signals2::connection oIdfct);	// Unregister a previously register callback using its connection Id
	virtual int UnregisterAllCallbacks (unsigned int uiEventId);									// Unregister all previously register callback on a specific event
	virtual boost::signals2::connection GetLastFctId ();								// Send back the last connected callback connexion Id

	//	Method which print the configuration of the serial port
	virtual int PrintConfig ();
};
#endif //CUART_H
