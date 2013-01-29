/**
 *	\file	CTcpIpServer.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CTcpIpServer class
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#include "CTcpIpServer.h"

#ifndef CTCPIPSERVER_LISTEN_QUEUE_SIZE
	#define CTCPIPSERVER_LISTEN_QUEUE_SIZE	(512)			/**< \brief Maximum number of connected client */
#endif

#ifndef CTCPIPSERVER_RX_BUFFER_SIZE
	#define CTCPIPSERVER_RX_BUFFER_SIZE	(10*1024)			/**< \brief Maximum size of a handled Server receive buffer */
#endif

using namespace std;

/**
 *	\fn 		CTcpIpServer::CConnectedClient::CConnectedClient()
 *	\brief		Constructor of the CConnectedClient class.
 */
CTcpIpServer::CConnectedClient::CConnectedClient()
{
	m_sAdress = NULL;
	m_fdClient = -1;
	m_uiClientPort = 0;
}

/**
 *	\fn 		CTcpIpServer::CConnectedClient::~CConnectedClient()
 *	\brief		Destructor of the CConnectedClient class.
 */
CTcpIpServer::CConnectedClient::~CConnectedClient()
{
	close(m_fdClient);
	m_sAdress = NULL;
	m_fdClient = -1;
	m_uiClientPort = 0;
}

  /**  
   * \fn 			CConnectedClient* CTcpIpServer::FindClient(int fd)
   * \brief 		Find the CConnectedClient* with the fd specified
   * \param[in] 	fd : files descriptor of the CConnectedClient*
   * \return 		CConnectedClient* the CConnectedClient* who have been find.
   */
CTcpIpServer::CConnectedClient* CTcpIpServer::FindClient(int fd)
{
	
	ClientList::iterator it;
	CConnectedClient* ClientSearch  = NULL;
	
	if(fd <= 0)
	{
		cout<<"CTcpIpServer > Bad files descriptor \n";
	}
	
	for ( it=m_Client_list.begin() ; it != m_Client_list.end(); it++ )
	{	
		ClientSearch = *it;
		if(ClientSearch->GetFdClient() == fd) 
		{
			break;
		}
	}
	
	return ClientSearch;
	
}

/**
 *	\fn			char* CTcpIpServer::CConnectedClient::GetClientAdressIP()
 *	\brief 		Assesor : Return the Client* IP adress
 *	\return 	char* (IP adress)
 */
char* CTcpIpServer::CConnectedClient::GetClientAdressIP()
{
	return m_sAdress;
}


/**
 *	\fn			unsigned int CTcpIpServer::CConnectedClient::GetClientPort()
 *	\brief 		Assesor : Return the Client* Port.
 *	\return 	unsigned int (port of the Client socket).
 */
unsigned int CTcpIpServer::CConnectedClient::GetClientPort()
{
	return m_uiClientPort;
}


/**
 *	\fn			int CTcpIpServer::CConnectedClient::GetFdClient()
 *	\brief 		Assesor : Return the Client* fd.
 *	\return 	int (file descriptor of the Client socket)
 */
int CTcpIpServer::CConnectedClient::GetFdClient()
{
	return m_fdClient;
}

/**
 *	\fn			int CTcpIpServer::CConnectedClient::SetClientAdressIP(char* sIPClientAdress)
 *	\brief 		Assesor : Set the Client* IP adress.
 *  \param		sIPClientAdress : IP who is set.
 *	\return 	int (-1 for error) (0 otherwise)
 */
int CTcpIpServer::CConnectedClient::SetClientAdressIP(char* sIPClientAdress)
{
	if (sIPClientAdress == (char*) NULL)
	{
		cout<<"CTcpIpServer > Bad IP Adress"<<endl;
		return -1;
	}
	
	m_sAdress = sIPClientAdress;
	
	return 0;
}


/**
 *	\fn			int CTcpIpServer::CConnectedClient::SetClientPort(unsigned int uiPort)
 *	\brief 		Assesor : Set the Client* Port.
 *  \param		uiPort : Port who is set.
 *	\return 	int (-1 for error) (0 otherwise)
 */
int CTcpIpServer::CConnectedClient::SetClientPort(unsigned int uiPort)
{
	//TODO verifier les ports a utiliser
	m_uiClientPort = uiPort;
	return 0;
}


/**
 *	\fn			int CTcpIpServer::CConnectedClient::SetFdClient(int iFdClient)
 *	\brief 		Assesor : Set the Client* fd.
 *  \param		iFdClient : fd who is Set.
 *	\return 	int (-1 for error) (0 otherwise)
 */
int CTcpIpServer::CConnectedClient::SetFdClient(int iFdClient)
{
	if(iFdClient < 0)
	{
		cout<<"CTcpIpServer > Bad fd client"<<endl;
		return -1;
	}
	
	m_fdClient = iFdClient;
	return 0;
}

/**
 *	\fn			CTcpIpServer::CTcpIpServer(unsigned int uiPort)
 *	\brief 		Constructor of the CTcpIpServer class.
 *	\param[in] uiPort : listening port of the server
 */
CTcpIpServer::CTcpIpServer (unsigned int uiPort)
{
	m_uiPort = uiPort;
	m_fd = -1;
}

/**
 *	\fn			CTcpIpServer::~CTcpIpServer()
 *	\brief 		Destructor of the CTcpIpServer class.
 */
CTcpIpServer::~CTcpIpServer()
{
	close(m_fd);
}

/**
 *	\fn			int CTcpIpServer::Start()
 *	\brief 		create and configure a socket and start a the listen on it.
 *	\return 	int (-1 for error) (0 otherwise)
 */
int CTcpIpServer::Start ()
{
	int ret;
	int sock_opt;
	struct sockaddr_in addr_serv;
	
	/* Check for multiple call */
	if (m_fd >= 0) {
		cout<<"CTcpIpServer > Listen socket already opened.\n"<<endl;
		return 0;
	}

	/* Init a socket */
	m_fd = socket (PF_INET, SOCK_STREAM, 0);
	if (m_fd < 0) {
		cout<<"CTcpIpServer > Can't create the socket\n"<<endl;
		return -1;
	}


	/*	Configure SO_REUSEADDR
	 *	if sock_opt = 1 - Allow bind to re-use a sowket in TIME_WAIT mode
	 *	if sock_opt = 0 - Doesn't
	 */
	sock_opt = 1;	
	ret = setsockopt (m_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt, sizeof (sock_opt));
	if (ret != 0) {
		cout<<"CTcpIpServer > Error on setsockopt with command SO_REUSEADDR and arg= "<<sock_opt<< endl;
	}

	/* Bind */
	addr_serv.sin_family = AF_INET;	 
   	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
   	addr_serv.sin_port = htons (m_uiPort);
   	ret = bind (m_fd, (struct sockaddr *) &addr_serv, sizeof(addr_serv));
	if (ret < 0) {
		cout<<"CTcpIpServer > Error on bind\n"<<endl;
		close(m_fd);
		m_fd = -1;
		return -1;
	}

	/* Listen */	
	ret = listen (m_fd, CTCPIPSERVER_LISTEN_QUEUE_SIZE);
	if (ret < 0) {
		cout<<"CTcpIpServer > Error on listen\n"<<endl;
		close(m_fd);
		m_fd = -1;
		return -1;
	}

	return 0;
}

  /**
   * \fn 			int CTcpIpServer::Stop()
   * \brief 		Shutdown and close the socket , stop the thread.
   * \return		int (-1 for error) (0 otherwise)
   */
int CTcpIpServer::Stop()
{
	int ret;
	
	/* Check for unstarted listen socket */
	if (m_fd < 0) {
		cout<<"CTcpIpServer > Warning listen socket may be not started !\n"<<endl;
	}
	
	/* Stop the listen socket */
    /* Shutdown the connection, before close it */
    ret = shutdown (m_fd, SHUT_RDWR);
    if (ret < 0) {
		
		cout<<"CClientTcpIP > Error on shutdown\n";
		goto close_socket;
    
	}

 	/* Stop all active connection */
	m_Client_list.clear();
	
	/* Stop Thread */	
	m_bThreadRunning = false;
	// Wait for the thread end
	m_oThread.join();	

	close_socket:
	close(m_fd);
	m_fd = -1;
	cout<<"CClientTcpIP > Close complete"<<endl;	
	UnregisterAllCallBacks(ALL_EVENTS);
	
	return 0;
}

  /**
   * \fn 			unsigned int CTcpIpServer::GetNbClientsConnected()
   * \brief 		Get the number of CConnectedClient* from the m_Client_list
   * \return 		unsigned int 
   */
unsigned int CTcpIpServer::GetNbClientsConnected()
{
	return (unsigned int)m_Client_list.size();
}

  /**  
   * \fn 			int CTcpIpServer::RegisterCallBack (unsigned int uiIdEvent, boost::function<CallBack> oFct, void* pData1 = NULL, void* pData2 = NULL)
   * \brief 		Register a Callback which will be called when the event specified in EnventID will occure. WARNING in this class pData1 is used for return a Client* to the CallBack.
   * \param[in]  	uiIdEvent : 	Integer that indicate the event ID on which the function will be launched.
   * \param[in]  	oFct :			Object which indicates the function to launch when a event occure.
   * \param[in] 	pData1 :		Client*.
   * \param[in] 	pData2 : 		User pointer.
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpServer::RegisterCallBack (unsigned int uiIdEvent, boost::function<CallBack> oFct, void* pData1 = NULL, void* pData2 = NULL)
{

	if(pData1 != NULL)
	{
		cout<<"pData1 only used for a Client* on this Class"<<endl;
	}
	
    bool bTest = false;

    /* Check if the signal slot are empty */
    if ((m_oSigOnRec.num_slots() + m_oSigOnDc.num_slots() + m_oSigOnCo.num_slots())== 0)
    {
		bTest = true;
    }
    /* check the IdEvent who's the users wants to associate with his CallBack */
    if (uiIdEvent == EVENT_CTCPIPSERVER_RECEIVEDATA) {
		// Connected the passed function and saved its connection ID
		m_oLastFctId = m_oSigOnRec.connect(oFct);

	/* check the IdEvent who's the users wants to associate with his CallBack */
	}else if(uiIdEvent == EVENT_CTCPIPSERVER_ONDISCONNECT) {
		// Connected the passed function and saved its connection ID
		m_oLastFctId = m_oSigOnDc.connect(oFct);

	}else if(uiIdEvent == EVENT_CTCPIPSERVER_ONCONNECT) {
		// Connected the passed function and saved its connection ID
		m_oLastFctId = m_oSigOnCo.connect(oFct);
		
	}else{
		cout<<"CTcpIpServer > Wrong IDEVENT" << endl;
		return -2;
    }
   
	if(m_oLastFctId.connected() == false) {
		cout<<"CTcpIpServer > Error register Callback " << endl;
		return -1;
	}else {		
		cout<<"CTcpIpServer > CallBack correctement connectée"<<endl;
	}
    
    pData1 = NULL;
	m_pData2 = pData2;
    
    /*If the function is Connected check if the signal slot have only one function associate 
	if is true we can start a thread.*/
    if(bTest == true){ 
		// Indicate to the thread it should run
		m_bThreadRunning = true;
		// And launch the OnRec OnDisc thread
		/* Start the thread */
		boost::thread m_oThread(boost::bind(&CTcpIpServer::OnReceive, this));
/*				cout << "thread started " <<endl;  */   
	}    
    return 0;
}

  /**  
   * \fn			int CTcpIpServer::UnregisterCallBack(unsigned int uiIdEvent, boost::signals2::connection oIdFct)
   * \brief 		Unregister a previously register callback using its connection Id
   * \param[in] 	uiIdEvent : Integer that indicate the event ID on which the function is launched
   * \param[in]  	oIdFct : Object which indicates the connection ID of the function to unregister
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpServer::UnregisterCallBack (unsigned int uiIdEvent, boost::signals2::connection oIdFct)
{
	// Init variables
    std::size_t iNumBefore;
    std::size_t iNumAfter;
    
    /* check the IdEvent who's the users wants to unregister his CallBack */    
    if (uiIdEvent == EVENT_CTCPIPSERVER_RECEIVEDATA) {
		// Checks if there are some connected functions
		if (m_oSigOnRec.num_slots() == 0) {
			cout<<"CTcpIpServer > Aucune Callback n'est actuelement connectée sur l'évenement de reception des données sur le serveur" << endl;
			return -1;
		}
		// Saved the number of connected function before the deconnection
		iNumBefore = m_oSigOnRec.num_slots();
		// Disconnect the function using its connection ID
		oIdFct.disconnect();
		// Saved the number of connected function after the deconnection
		iNumAfter = m_oSigOnRec.num_slots();

	/* check the IdEvent who's the users wants to unregister his CallBack */	
    }
    else if (uiIdEvent == EVENT_CTCPIPSERVER_ONDISCONNECT) {
		// Checks if there are some connected functions
		if (m_oSigOnDc.num_slots() == 0) {
			cout<<"CTcpIpServer > Aucune Callback n'est actuelement connectée sur l'évenement de déconnexion sur le serveur" << endl;
			return -1;
		}
		// Saved the number of connected function before the deconnection
		iNumBefore = m_oSigOnDc.num_slots();
		// Disconnect the function using its connection ID
		oIdFct.disconnect();
		// Saved the number of connected function after the deconnection
		iNumAfter = m_oSigOnDc.num_slots();
    }
    else if (uiIdEvent == EVENT_CTCPIPSERVER_ONCONNECT) {
		// Checks if there are some connected functions
		if (m_oSigOnCo.num_slots() == 0) {
			cout<<"CTcpIpServer > Aucune Callback n'est actuelement connectée sur l'évenement de connexion sur le serveur" << endl;
			return -1;
		}
		// Saved the number of connected function before the deconnection
		iNumBefore = m_oSigOnCo.num_slots();
		// Disconnect the function using its connection ID
		oIdFct.disconnect();
		// Saved the number of connected function after the deconnection
		iNumAfter = m_oSigOnCo.num_slots();
    }
    // Unknown event
	else {
		cout<<"CTcpIpServer > Erreur dans l'ID de l'évenement" << endl;
		return -2;
    }
    // Checks if the function was well disconnected
    if (iNumBefore == iNumAfter+1) {
		//m_lListOnRec.erase(pItToDelete);
		cout<<"CTcpIpServer > Callback correctement déconnecté" << endl;
		// If there is no longer connected functions to the boost::signals
		if (m_oSigOnDc.num_slots() == 0 && m_oSigOnRec.num_slots() == 0 && m_oSigOnCo.num_slots() == 0 ) {
			// Indicate to the thread it should stop
			m_bThreadRunning = false;
			// Wait for the thread end
			m_oThread.join();
		}
		return 0;
    }else{
	cout<<"CTcpIpServer > La Callback n'a pas pu être correctement déconnecté" << endl;
	return -2;
    } 
	
	return 0;
}

  /**  
   * \fn 			int CTcpIpServer::UnregisterAllCallBacks(unsigned int uiIdEvent)
   * \brief 		Unregister a previously register callback using its connection Id
   * \param[in]  	uiIdEvent : Integer that indicate the event ID to empty
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpServer::UnregisterAllCallBacks(unsigned int uiIdEvent)
{
	/* Check if the users wants to unregister all the CallBack of the all events */
    if (uiIdEvent == ALL_EVENTS) {
		// Checks if there are some connected functions
		if ((m_oSigOnRec.num_slots() == 0) && (m_oSigOnDc.num_slots() == 0) && (m_oSigOnCo.num_slots() == 0)) {
			cout<<"CTcpIpServer > Aucune Callback n'est actuelement connectée" << endl;
			return -1;
		}else {
			// Disconnect all the functions from this specific events
			m_oSigOnRec.disconnect_all_slots();
			m_oSigOnDc.disconnect_all_slots();
			m_oSigOnCo.disconnect_all_slots();
			// Checks if all the functions was well disconnected
			if ((m_oSigOnRec.num_slots() == 0) && (m_oSigOnDc.num_slots() == 0) && (m_oSigOnCo.num_slots() == 0)){    
				cout<<"CTcpIpServer > Toutes les callbacks ont bien été deconnectées" << endl;
				}else {
			
					cout<<"CTcpIpServer > Les callbacks n'ont pas été correctement deconnectées" << endl;
					return -2;
			}
		}
	/* Check if the users wants to unregister all the CallBack of the Receive data events */
    }else if (uiIdEvent == EVENT_CTCPIPSERVER_RECEIVEDATA) {
		// Checks if there are some connected functions
		if (m_oSigOnRec.num_slots() == 0) {
			cout<<"CTcpIpServer > Aucune Callback n'est actuelement connectée" << endl;
			return -1;
		}else {
			// Disconnect all the functions from this specific events
			m_oSigOnRec.disconnect_all_slots();	
			// Checks if all the functions was well disconnected
			if (m_oSigOnRec.num_slots() == 0){	   
				cout<<"CTcpIpServer > Toutes les callbacks Rx ont bien été deconnectées" << endl;
			}else {
			cout<<"CTcpIpServer > Les callbacks n'ont pas été correctement deconnectées" << endl;
			return -2;
			}
		}
	
	/* Check if the users wants to unregister all the CallBack of the On Disconnect events */
    } else if (uiIdEvent == EVENT_CTCPIPSERVER_ONDISCONNECT) {
		// Checks if there are some connected functions
		if (m_oSigOnDc.num_slots() == 0) {
			cout<<"CTcpIpServer > Aucune Callback n'est actuelement connectée" << endl;
			return -1;
		}else {
			// Disconnect all the functions form this specific events
			m_oSigOnDc.disconnect_all_slots();
			// Checks if all the functions was well disconnected
			if (m_oSigOnDc.num_slots() == 0){
				cout<<"CTcpIpServer > Toutes les callbacks Dc ont bien été deconnectées" << endl;
			}else{
				cout<<"CTcpIpServer > Les callbacks n'ont pas été correctement deconnectées" << endl;
				return -2;
			}
		}	
    }
    
     else if (uiIdEvent == EVENT_CTCPIPSERVER_ONCONNECT) {
		// Checks if there are some connected functions
		if (m_oSigOnCo.num_slots() == 0) {
			cout<<"CTcpIpServer > Aucune Callback n'est actuelement connectée" << endl;
			return -1;
		}else {
			// Disconnect all the functions form this specific events
			m_oSigOnCo.disconnect_all_slots();
			// Checks if all the functions was well disconnected
			if (m_oSigOnCo.num_slots() == 0){
				cout<<"CTcpIpServer > Toutes les callbacks Co ont bien été deconnectées" << endl;
			}else{
				cout<<"CTcpIpServer > Les callbacks n'ont pas été correctement deconnectées" << endl;
				return -2;
			}
		}	
    }
    // Unknown event
    else {
		cout<<"CTcpIpServer > Erreur dans l'ID de l'évenement" << endl;
	return -2;
    }
	if((m_oSigOnRec.num_slots() == 0) && (m_oSigOnDc.num_slots() == 0) && (m_oSigOnCo.num_slots() == 0)){
		// Indicate to the thread it should stop
		m_bThreadRunning = false;
		// Wait for the thread end
		m_oThread.join();
	}else{
		return 0;
	}
	return 0;
}


   /**
    * \fn 			void CTcpIpServer::OnReceive()
    * \brief 		Method who manage the launch of CallBack when an event Connection/ReceiveData/Deconnection occured.
    */
void CTcpIpServer::OnReceive()
{

	char buffer_rx[CTCPIPSERVER_RX_BUFFER_SIZE];
	int i = 0, ret = 0, readed = 0;
	int pfd_size;
	char* sIPClientAdress;
	int iFdClient;
	unsigned int uiClientPort;
	struct pollfd *pfd = NULL;
	struct sockaddr_in addr_client;			/* Information sur un client lors de sa connection */
	unsigned int size_addr_client = sizeof(struct sockaddr_in);
	int flag ;								/* flag de sortie de boucle */
	ClientList::iterator it;
	CConnectedClient* cursor = NULL;
	CConnectedClient* welcome = NULL;


	while(m_bThreadRunning)
	{
		//Check if the pfd isn't NULL
		if (pfd != (struct pollfd *) NULL)
		free (pfd);

		//Allocation of the stuct pfd (size of struct pollfd * number of CConnectedClient)
		pfd_size = 1 + (int) m_Client_list.size();
		pfd = (struct pollfd *) malloc (pfd_size * sizeof (struct pollfd));
		if (pfd == (struct pollfd *) NULL) {
			cout<<"CTcpIpServer > Error on pfd allocation\n";
			continue;
		}

		//Add the fd socket server on the pollfd struct and defined the event on this
		pfd[0].fd = m_fd;
		pfd[0].events = POLLIN | POLLPRI;

		i=0;
		
		//Add the fd CConnectedClient* on the pollfd struct and defined the event on this
		for ( it=m_Client_list.begin() ; it != m_Client_list.end(); it++ )
		{
			cursor = *it;
			pfd[i+1].fd = cursor->GetFdClient(); 
			pfd[i+1].events = POLLIN | POLLPRI | POLLHUP | POLLERR;
			i++;
		}

		flag = 0;
		do
		{
			ret = poll (pfd, pfd_size, 5000 /* ms */);
			if (ret < 0) {
				/* Error on poll */
				cout<<"CTcpIpServer > Erreur sur le poll\n";
			} else if (ret == 0) {
				/* Time-out */
				cout<<"CTcpIpServer > Timeout !\n";
			// POLLIN | POLLPRI | POLLHUP | POLLERR received
			} else {
				/* check all the list */
				for (i=0; i<pfd_size; i++) {
//  					printf("Poll ret code = %d and fd.revent = 0x%04X , i = %d\n", ret, pfd[i].revents, i);

					//if the events is POLLIN or POLLPRI
					if (pfd[i].revents & (POLLIN | POLLPRI)) {
						//check if is the server socket
						if (i == 0) {

							/* if POLLIN/POLLPRI on the fd server -> Connection request */
							welcome = new CConnectedClient();
							iFdClient = accept(m_fd, (struct sockaddr *) &addr_client, &size_addr_client);
							welcome->SetFdClient(iFdClient);
							
							//check if we get the good fd client
							if (welcome->GetFdClient() < 0) {
								/* Error on accept */
								delete (welcome);
							} else {

								/* Getting information about the new client */
								sIPClientAdress = strdup(inet_ntoa(addr_client.sin_addr));
								welcome->SetClientAdressIP(sIPClientAdress);
								uiClientPort = ntohs(addr_client.sin_port);
								welcome->SetClientPort(uiClientPort);
								cout<<"CTcpIpServer > Connection de IP= "<<welcome->GetClientAdressIP()<<" : "<<welcome->GetClientPort()<<endl;
								
								/* Add Client to the list */
								if (welcome == (CConnectedClient *) NULL)break;
								m_Client_list.push_front(welcome);
						
								/* Launch of OnConnection Callback(s) */
								CEvent* pEvent = new CEvent();
								m_oSigOnCo(NULL,0,pEvent,welcome,m_pData2);
								delete (pEvent);
						
							}
								flag = 1;
						//if POLLIN/POLLPRI on other fd -> data received							
						} else {

							/* Socket client : Data available */
							memset (buffer_rx, '\0', CTCPIPSERVER_RX_BUFFER_SIZE);
							readed = recv (pfd[i].fd, buffer_rx, CTCPIPSERVER_RX_BUFFER_SIZE, 0);
							//if read failed
							if (readed <= 0) {
								/* Find the client who read is failed*/
								CConnectedClient* bye = FindClient (pfd[i].fd);
								
								/* Launch of OnDeconnection Callback(s) */
								CEvent* pEvent = new CEvent();
								m_oSigOnDc(NULL,0,pEvent,bye,m_pData2);
								delete (pEvent);
								
								//delete the client (list and memory)
								DelClient (bye);
								flag = 1;
								
							//if read worked	
							} else {
								
								//find the client
								CConnectedClient* tmp = FindClient (pfd[i].fd);
								
								//Launch the Reception CallBack(s)
								CEvent* pEvent = new CEvent();
								m_oSigOnRec(buffer_rx,readed,pEvent,tmp,m_pData2);
								delete (pEvent);
								
							}
						}
					//check if event is POLLERR/POLLHUP (deconnection)
					} else if (pfd[i].revents & (POLLERR | POLLHUP)) {
						/* Connection IP lost */
						
						//TODO evenement de déconnection non détecté.
						cout<<"CTcpIpServer > Connection lost"<<endl;
						
						//find the client
						CConnectedClient* bye = FindClient (pfd[i].fd);
						
						//launch the Deconnection CallBack(s)
						CEvent* pEvent = new CEvent();
						m_oSigOnDc(NULL,0,pEvent,bye,m_pData2);
						delete (pEvent);
						
						//Delete the client (list and memory)
						DelClient (bye);
 						
 						flag = 1;
					}
				}
			}
			
		}while(!flag);

	}	
	cout<<"CTcpIpServer > thread end"<<endl;
	Stop();
}

  /**  
   * \fn 			int CTcpIpServer::Write ( CConnectedClient* to ,char * pcBuffer, unsigned int uiBuffer_size).
   * \brief 		write a data buffer on the CConnectedClient* specified if the CConnectedClient* is NULL that will do a broadcast.
   * \param[in]  	to : The Client specified for the write send. (NULL for broadcast on all ClientSocket).
   * \param[in]  	pcBuffer : data buffer who will be send.
   * \param[in] 	uiBuffer_size : size of the buffer who will be send.
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpServer::Write ( CConnectedClient* to ,char * pcBuffer, unsigned int uiBuffer_size)	//TODO ne gere pas l'envoi de donnée en boucle infini sans sleep.
{

	int ret, offset, size;
	struct pollfd pfd;
	ClientList::iterator it;

	/* Init */
	size = uiBuffer_size;
	offset = 0;
	pfd.events = POLLOUT;
	
	//check parameter
	if (pcBuffer == (char *)NULL) {
		cout<<"CTcpIpServer > NULL Buffer pointer\n";
		return -1;
    }
    
    if(uiBuffer_size <= 0)
	{
		cout<<"CTcpIpServer > Size Can't be <=0 \n";
		return -1;
	}
    
    //if the CConnectedClient* is NULL -> write on broadcast
	if (to == (CConnectedClient *) NULL){
		for ( it=m_Client_list.begin() ; it != m_Client_list.end(); it++ ) {
		
			to = *it;
			pfd.fd = to->GetFdClient();
			cout<<"CTcpIpServer > Destinataire @IP :"<<to->GetClientAdressIP()<< " Port : "<<to->GetClientPort()<<" fd = "<<to->GetFdClient()<<endl;
			offset = 0;
			
			do {
				ret = poll (&pfd, 1, 1000);
				if (ret <= 0) {
					/* Error or timeout */
					return -EIO;
				}
				ret = send (to->GetFdClient(), pcBuffer + offset, uiBuffer_size, MSG_DONTWAIT);
				if(ret < 0){
					cout<<"CTcpIpServer > Error on sending message to : "<<to->GetClientAdressIP()<<" Port : "<<to->GetClientPort()<<endl;
					close (to->GetFdClient());
					DelClient (to);
				}
				offset += ret;
				size -= ret;
				if (size <= 0) break;
			
			}while(1);
		}
	//if CConnectedClient* isn't NULL -> write only to this
	} else {

		do {
			ret = poll (&pfd, 1, 1000);
			if (ret <= 0) {
				/* Error or timeout */
				return -EIO;
			}
			ret = send(to->GetFdClient(), pcBuffer + offset, uiBuffer_size,MSG_DONTWAIT);
                        if (ret != (int) uiBuffer_size) {
				cout<<"CTcpIpServer > Ret = "<<ret<<", buffer_size = "<<uiBuffer_size<<endl;
				cout<<"CTcpIpServer > Error on sending message to "<<to->GetClientAdressIP()<<" Port : "<<to->GetClientPort()<<endl;
				close (to->GetFdClient());
				DelClient (to);
				return -1;
			}

			offset += ret;
			size -= ret;
			if (size <= 0) return uiBuffer_size;
		
		}while(1);
	 }
return uiBuffer_size;
}

  /**  
   * \fn 			void CTcpIpServer::DelClient(CConnectedClient* Client)
   * \brief 		Delete the CConnectedClient* from the m_Client_list.
   * \param[in] 	Client 
   */
void CTcpIpServer::DelClient(CConnectedClient* Client)
{
	
	if(Client == (CConnectedClient*) NULL)
	{
		cout<<"CTcpIpServer > NULL pointer CConnectedClient"<<endl;
	}
	
	/* Empty list */
	if((int)m_Client_list.size() == 0)
	{
		cout<<"CTcpIpServer > Empty List"<<endl;
		return;
	}
	
	m_Client_list.remove(Client);
	delete (Client);
	
}

  /**
   * \fn			int CTcpIpServer::PrintConfig()
   * \brief 		print the Config of the Server
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpServer::PrintConfig()
{
	
	cout<<"CTcpIpServer > Nombre de CallBack Connecté au signal ReceiveData : "<<m_oSigOnRec.num_slots()<<endl;
	cout<<"CTcpIpServer > Nombre de CallBack Connecté au signal OnDisconnect : "<<m_oSigOnDc.num_slots()<<endl;
	cout<<"CTcpIpServer > Nombre de CallBack Connecté au signal OnConnect : "<<m_oSigOnCo.num_slots()<<endl;
	cout<<"CTcpIpServer > listening Port : "<<m_uiPort<<endl;
	return 0;
}

  /**
   * \fn 			int CTcpIpServer::SetPort(unsigned int uiPort)
   * \brief 		Set the listening port
   * \param 		uiPort : listening port
   * \return		int (-1 for error) (0 otherwise) 
   */
int CTcpIpServer::SetPort(unsigned int uiPort)
{
	if(uiPort > 65535)
	{
		cout<<"CTcpIpServer > Bad Port"<<endl;
		return -1;
	}
	m_uiPort = uiPort;
	return 0;
}

/**
 *	\fn				boost::signals2::connection CTcpIpServer :: GetLastFctId ()
 *  \brief			Send back the last connected callback connexion Id
 *	\return			The last connected callback connexion Id
 */
boost::signals2::connection CTcpIpServer::GetLastFctId()
{
	return m_oLastFctId;
}
