/**
 *	\file	CGPIO.h
 *
 *	\brief 	This header file is used to declare the CGPIO class and all its methods.
 *	
 *	\author Jonathan Aillet
 *
 *	\version v1.0a
 *
 *	\date 28 June, 2012
 *
 */

#ifndef CGPIO_H
#define CGPIO_H

#include "CIODevice.h"

/**
 *	\def	MAX_GPIO_NB
 *	\brief	Define which set the maximum number of useable GPIO (define in the m_puiAuthorizedGpio table).
 */
#define MAX_GPIO_NB	64

/**
 *	\class CGPIO
 *
 *	\brief 	The CGPIO class allows to open, read, and write on a GPIO using its entry in '/dev'
 *
 *	It also allows to register a function to be called when a event occure on a GPIO port.
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.1a
 *
 *	\date 15 May, 2012
 *
 */
class CGPIO : public CIODevice
{
	private :
	/* Attributes */
	/**************/
	bool 			m_bThreadRunning;							/**< \brief Boolean which indicates if the thread should run or stop. */
	bool			m_bMode;									/**< \brief Boolean which indicates the using mode of the Gpio (I/O Mode, or IT Mode). */
	bool			m_bDirection;								/**< \brief Boolean which indicates the direction of th Gpio in I/O Mode. */
	bool			m_bDefaultValue;							/**< \brief Boolean which indicates the default OUTPUT value */
	unsigned int	m_uiActiveEvent;							/**< \brief Integer which indicates on which events the Gpio should respond. */
	unsigned int	m_uiDelay;									/**< \brief Integer which indicates the minimum delay between two events. */
	
	unsigned int	m_puiAuthorizedGpio[MAX_GPIO_NB];			/**< \brief Array which will contain a list of the Usable Gpio */
	
	char *			m_sGpioPath;								/**< \brief Buffer which contains the path of the Gpio currently used. */
	
	/**	\brief Thread which will be launched to read the pin of the Gpio and to launch the callbacks. */
	boost::thread						m_oThreadEvent;
	
	/**	\brief Last Connexion Id */
	boost::signals2::connection 		m_oIdGpioCallback;
	
	//	Callback(s) signal(s)
	boost::signals2::signal<CallBack> 	m_oSigOnGpioEvent;		/**< \brief Signal which allow to launch the callback(s) connected on the Gpio event */
	
	/* Methods  */
	/************/
	// Method which manage the launch of callbacks when an event is detected on the Gpio
	void OnGpioEvent (void);
	
	// Method which return the Gpio number currently used
	unsigned int GetuiGpioNum();
	
	// Method which set the authorize Gpios
	void SetAuthorizeGpio();
	
	// Method which return if the gpio is usable
	bool SearchOnTable (unsigned int uiGpioNum);
	
	public :
	/* Methods  */
	/************/
	//	Constructor for Input/Output Mode
	CGPIO (char * sDevicePath, bool bDirection, bool bDefaultValue);
	//	Constructor for Interrupt Mode
	CGPIO (char * sDevicePath, unsigned int uiActiveEvent);
	//	Destructor
	~CGPIO();
	
	//	Initialization of a CGPIO object
	virtual int Open();
	
	// Read the input bit of the Gpio (in I/O Mode only)
	int Read();
	
	//	Define the output bit of the Gpio (in I/O Mode only)
	int Write (bool bValue);
	
	//	Managing the registration of callbacks depending on the event indicated
	virtual int RegisterCallback (unsigned int uuiEventId, boost::function<CallBack> oFct, void * pData1 = NULL, void * pData2 = NULL);	// Register a Callback which will be called when the event specified in EnventID will occur, return -1 in IO mode, the first call of this function launch the blocking read thread
	virtual int UnregisterCallback (unsigned int uuiEventId, boost::signals2::connection oFctId);	// Unregister a previously register callback using its connection Id, return -1 in IO mode
	virtual int UnregisterAllCallbacks (unsigned int uuiEventId);									// Unregister all previously register callback on a specific event, return -1 in IO mode
	virtual boost::signals2::connection GetLastFctId();												// Send back the last connected callback connexion Id
	
	// Method which allow to set the debounce value
	int SetDelay (unsigned int uiDelay);
	
	//	Method which print the configuration of the serial port
	virtual int PrintConfig();
};
#endif //CGPIO_H
