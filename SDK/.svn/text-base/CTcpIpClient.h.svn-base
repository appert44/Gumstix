/**
 *	\file	CTcpIpClient.h
 *
 *	\brief 	This header file is used to declare the CTcpIpClient class and all its methods.
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#ifndef CTcpIpClient_H
#define CTcpIpClient_H

#include "CIODevice.h"

/**
 *	\class	CTcpIpClient
 *
 *	\brief 	The CTcpIpClient TODO : to complete
 *
 *	TODO : to complete.
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 15 May, 2012
 *
 */
class CTcpIpClient : virtual public CIODevice
{
	private:
	/* Attributes */
	/**************/
	bool 									m_bMode;							/**< \brief Mode true for reconnection enable , false otherwise */
	bool				 					m_bThreadRunning;					/**< \brief ThreadRunning true for thread running , false otherwise */
	bool									m_bConnected;						/**< \brief Connected true for Connected , false otherwise */
	bool									m_bReconnect;						/**< \brief Reconnect true for Reconnect try , false otherwise */
	
	unsigned int 							m_uiServerPort;						/**< \brief Port Server */
	unsigned int							m_uiNbAttemp;						/**< \brief Number of reconnection */
	
	char									m_pcServerIP[16];					/**< \brief IP Server */
		
	boost::thread							m_oThreadRxDc;						/**< \brief Thread */
	boost::signals2::signal<CallBack>		m_oSigOnRec;						/**< \brief Signal boost of Reception data */
	boost::signals2::signal<CallBack>		m_oSigOnDc;							/**< \brief Disconnect Signal boost */
	
	/* Methods  */
	/************/
	void OnReceive();  
	int Reconnect();
		
	public:
	/* Methods  */
	/************/
	// Constructor
	CTcpIpClient (bool enableAutoReconnectMode, unsigned int NbReconnect);  
	// Destructor
	virtual ~CTcpIpClient();

	// Client's Method
	int Connect (char * ServerIP, unsigned int ServerPort );				// Connect to the Server
	virtual int Close ();													// Close the connection
	virtual int Write (char * pcBuffer, unsigned int uiBufferSize );		// Write to the server
	
	// Managing the registration of callbacks
	virtual int RegisterCallBack (unsigned int uiIdEvent, boost::function<CallBack> oFct , void* pData1 , void* pData2);	// Register the CallBack
	virtual int UnregisterCallBack (unsigned int uiIdEvent, boost::signals2::connection oIdFct );  			// Unregister the CallBack
	virtual int UnregisterAllCallBacks(unsigned int uiIdEvent);				// Unregister all the CallBack

	// Accessor
	virtual int PrintConfig ();												// Print the config
	bool IsConnected ();													// Check if Connected
	virtual boost::signals2::connection GetLastFctId(); 					// Get the last ID of the boost function Register

};

#endif // CTcpIpClient_H
