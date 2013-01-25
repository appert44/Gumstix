/*-------------------------------------------------------------------
//TODO THIS CLASS HAS BEEN NOT TESTED!!!
-------------------------------------------------------------------*/


#ifndef CSpiPort_H
#define CSpiPort_H
/**
 *	\file	CSpiPort.h
 *
 *	\brief 	This header file is used to declare the CSpiPort class and all its methods.
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 8 June, 2012
 *
 */

#include "CIODevice.h"

/**
 *	\class CSpiPort
 *
 *	\brief 	The CSpiPort allows to create and configure a Server Tcp-IP
 *
 *	It allows to configure, read, write, and register a function to call when a data is received and on a SPI Port.
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 8 June, 2012
 *
 */
class CSpiPort : public CIODevice
{
public:

	/* Constructor */
    CSpiPort(unsigned int uiSampleCountbyCB, unsigned int uiSampleSize, unsigned int uiSampleFrequency , unsigned int uiSpiChannel, unsigned int uiCsNumber);
	/* Destructor */
	~CSpiPort();
	
	/* SPI Port's methods */
	virtual int Open();
	virtual int Close();
	virtual int Start();																//start the listen socket
	virtual int Stop();																	//Stop the server running
	virtual int Write (char * pcBuffer, unsigned int uiBuffer_size);   					//Write to a CConnectedClient* (NULL for broadcast)


	
	/* Managing the registration of callbacks */
	virtual int RegisterCallBack(unsigned int uiIdEvent, boost::function<CallBack> oFct, void* pData1, void* pData2);	//Register the CallBack
	virtual int UnregisterCallBack(unsigned int uiIdEvent, boost::signals2::connection oIdFct);							//unregister the CallBack
	virtual int UnregisterAllCallBacks(unsigned int uiIdEvent);															//unregister all the CallBack
	
	/* Accessor */
	virtual boost::signals2::connection GetLastFctId();											//Get the last ID of the boost function Register
	virtual int PrintConfig();																	//print the config


private:
  
	/* Private methods */
	void OnReceive();												/**< \brief thread method */

	/* Attribut */

	unsigned int						m_uiSampleCountbyCB;		/**< \brief Number of sample on Callback */
	unsigned int						m_uiSampleSize;				/**< \brief Size of the Sample of the ADC */
	unsigned int						m_uiSampleFrequency;		/**< \brief Sample Frequency */
	unsigned int						m_uiSpiChannel; 			/**< \brief Channel of the Spi */
	unsigned int						m_uiCsNumber;				/**< \brief Cs of the SpiChannel */
	bool 								m_bThreadRunning;			/**< \brief bool for thread run */

	boost::thread						m_oThreadRx;				/**< \brief thread */
	boost::signals2::signal<CallBack> 	m_oSigOnRec;				/**< \brief Signal boost of Reception data */


};

#endif // CSpiPort_H
