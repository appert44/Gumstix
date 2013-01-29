/**
 *	\file	CTcpIpServer.h
 *
 *	\brief 	This header file is used to declare the CTcpIpServer class and all its methods.
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#ifndef CTcpIpServer_H
#define CTcpIpServer_H

#include "CIODevice.h"

/**
 *	\class CTcpIpServer
 *
 *	\brief 	The CTcpIpServer allows to create and configure a Server Tcp-IP
 *
 *	It allows to configure, read, write, and register a function to call when a data is received and on a Client Connection/deconnection
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

class CTcpIpServer : public CIODevice
{
	public:
	
		/**
		 *	\class CConnectedClient
		 *
		 *	\brief 	The CConnectedClient Class is the client list who is connected to the Server
		 *	
		 *	\author Théophile Marcadé
		 *
		 *	\version v0.1a
		 *
		 *	\date 24 April, 2012
		 *
		 */
		class CConnectedClient
		{

			public :
		
				/* Constructor */
				CConnectedClient();
				/* Destructor */	
				~CConnectedClient();
		
				char* GetClientAdressIP();
				unsigned int GetClientPort();
				int GetFdClient();
		
				int SetClientAdressIP(char*);
				int SetClientPort(unsigned int);
				int SetFdClient(int);
		
			private :
				/* Attribut */
				char* m_sAdress;															/**< \brief  IP CConnectedClient* */
				unsigned int m_uiClientPort;														/**< \brief  Port CConnectedClient* */
				int m_fdClient;																/**< \brief fd CConnectedClient* */
		
		};
		
		typedef std::list<CConnectedClient*> ClientList;
		
		/* Constructor */
		CTcpIpServer (unsigned int uiPort);
		/* Destructor */
		~CTcpIpServer();
	
		/* Server's methods */
		virtual int Start();																											//start the listen socket
		virtual int Stop();																												//Stop the server running
		int Write (CConnectedClient* to ,char * pcBuffer, unsigned int uiBuffer_size);   												//Write to a CConnectedClient* (NULL for broadcast)
		virtual int PrintConfig();																										//print the config

	
		/* Managing the registration of callbacks */
		virtual int RegisterCallBack (unsigned int uiIdEvent, boost::function<CallBack> oFct, void* pData1, void* pData2);				//Register the CallBack
		virtual int UnregisterCallBack (unsigned int uiIdEvent, boost::signals2::connection oIdFct);									//unregister the CallBack
		virtual int UnregisterAllCallBacks(unsigned int uiIdEvent);																		//unregister all the CallBack
	
		/* Accessor */
		int SetPort (unsigned int Port);																								//Set the server port
		unsigned int GetNbClientsConnected();																							//Get the Number of CConnectedClient* who's connected
		virtual boost::signals2::connection GetLastFctId();																				//Get the last ID of the boost function Register

	private:
	  
		/* Private methods */
		void OnReceive();												/**< \brief thread method */
		void AddClient(CConnectedClient*);								/**< \brief Add a CConnectedClient* to the m_Client_list */
		void DelClient(CConnectedClient*);								/**< \brief Delete a CConnectedClient* from the m_Client_list */
		CConnectedClient* FindClient(int);								/**< \brief Find a CConnectedClient* from the m_Client_list */

		/* Attribut */
		bool 								m_bThreadRunning;			/**< \brief bool thread running false for close the thread */
		unsigned int 						m_uiPort;					/**< \brief Listening Port */
		ClientList							m_Client_list;				/**< \brief CConnectedClient* list */
		boost::thread						m_oThread;					/**< \brief thread */
		boost::signals2::signal<CallBack> 	m_oSigOnRec;				/**< \brief Signal boost of Reception data */
		boost::signals2::signal<CallBack> 	m_oSigOnDc;					/**< \brief Signal boost of Déconnection */
		boost::signals2::signal<CallBack>   m_oSigOnCo;					/**< \brief Signal boost of Connection */


};

#endif // CTcpIpServer_H
