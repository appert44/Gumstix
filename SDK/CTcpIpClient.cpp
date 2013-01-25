/**
 *	\file	CTcpIpClient.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CTcpIpClient class
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#include "CTcpIpClient.h"

#ifndef CTCPIPCLIENT_RX_BUFFER_SIZE
    #define CTCPIPCLIENT_RX_BUFFER_SIZE	(10*1024)		/**< \brief Maximum size of a handled client receive buffer */
#endif

using namespace std;

/**
 *	\fn 			CTcpIpClient::CTcpIpClient(bool enableAutoReconnectMode,unsigned int NbReconnect)
 *	\brief			Constructor of the CTcpIpClient class.
 *	\param[in]		enableAutoReconnectMode : true for enable , false otherwise.
 *	\param[in]		NbReconnect : number of reconnect tentative.
 */
CTcpIpClient::CTcpIpClient(bool enableAutoReconnectMode,unsigned int NbReconnect)
{
  /* Init */
  m_bMode = enableAutoReconnectMode;
  m_fd = 0;
  m_bConnected = false;
  m_uiNbAttemp = NbReconnect;
  m_bReconnect = false;
}

/**
 *	\fn			CTcpIpClient::~CTcpIpClient()
 *	\brief 		Destructor of the CTcpIpClient class.
 */
CTcpIpClient::~CTcpIpClient()
{
	close(m_fd);
    m_bThreadRunning = false;
}

/**
 *	\fn			int CTcpIpClient::Connect(char* ServerIP, unsigned int ServerPort)
 *	\brief 		create and configure a socket and connect to a server.
 *	\param[in]	ServerIP : Adress IP of the ServerIP
 *	\param[in]	ServerPort : Port of the Server.
 *	\return 	int (-1 for error) (0 otherwise)
 */
int CTcpIpClient::Connect(char* ServerIP, unsigned int ServerPort)
{
	
    /* init */
    int optval, ret, flags;
    struct sockaddr_in socketInfo;
    struct pollfd fds;
    socklen_t optlen = sizeof (optval);	
    const int sockaddr_len = sizeof (struct sockaddr_in);
	struct linger so_linger;

    /* Exit if socket already exist */
    if (m_fd > 0){
		
		cout<<"CTcpIpClient > Can't Connect because the socket is already exist"<<endl;
		return -1;
	
    }
    
    /* Check parameters */
    if (ServerIP == (char *)NULL){
	
		cout<<"CTcpIpClient > Can't connect to nobody !\n";
		return -1;
    
	} if (ServerPort > 65535){
		
		cout<<"CTcpIpClient > Invalid range for port must be between [1-65535]\n"<<ServerPort<< endl;
		return -1;
	
    }
   
   /* Create a socket */
    m_fd= socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_fd < 0) {
	
		close(m_fd);
		m_fd = -1;
		cout<<"CTcpIpClient > CTcpIpClient > Can't create a socket !\n";
		return -1;
    
	}
	
	/*	Enable Keep-Alive on the socket */
    /*	http://tldp.org/HOWTO/html_single/TCP-Keepalive-HOWTO/#checkdeadpeers */
    optval = 1;
    if (setsockopt (m_fd, IPPROTO_TCP, SO_KEEPALIVE, &optval, optlen) < 0) {
	
		cout<<"CTcpIpClient > Error on setsockopt, can't enable SO_KEEPALIVE\n";
		close (m_fd);
		m_fd = -1;
		return -1;
   
	}
	
	// Verifie l'etat de l'option keepalive

	if(getsockopt(m_fd, IPPROTO_TCP, SO_KEEPALIVE, &optval, &optlen) < 0) {
    
		perror("getsockopt()");
		close(m_fd);
		m_fd = -1;
		return -1;
   
	}

	so_linger.l_onoff = 1;
	so_linger.l_linger = 1;
	if(setsockopt(m_fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof so_linger) < 0)
	{
		
		cout <<"Error on setsockopt, can't set SO_LINGER\n";
		close (m_fd);
		m_fd = -1;
		return -1;
		
	}
	
  /*	Set the interval between the last data packet sent (simple ACKs are not considered data)
    and the first keepalive probe; after the connection is marked to need keepalive,
    this counter is not used any further */
    optval = 1;
    if (setsockopt (m_fd, SOL_TCP, TCP_KEEPIDLE, &optval, optlen) < 0) {
	
		cout <<"Error on setsockopt, can't set TCP_KEEPIDLE\n";
		close (m_fd);
		m_fd = -1;
		return -1;
   
	}
    
    /*	Set the number of unacknowledged probes to send before considering the connection dead
    and notifying the application layer  */
    optval = 2;
    if (setsockopt (m_fd, SOL_TCP, TCP_KEEPCNT, &optval, optlen) < 0) {
		
		cout<<"CTcpIpClient > Error on setsockopt, can't set TCP_KEEPCNT\n";
		close (m_fd);
		m_fd= -1;
		return -1;
    
	}
    
    /*	Set the interval between subsequential keepalive probes,
    regardless of what the connection has exchanged in the meantime */
    optval = 1;
    if (setsockopt (m_fd, SOL_TCP, TCP_KEEPINTVL, &optval, optlen) < 0) {
	
		cout<<"CTcpIpClient > Error on setsockopt, can't set TCP_KEEPINTVL\n";
		close (m_fd);
		m_fd = -1;
		return -1;
   
	}
    
    /* Connecting */
    socketInfo.sin_family = PF_INET;
    if (inet_aton (ServerIP, &socketInfo.sin_addr) < 0) {
	
		cout<<"CTcpIpClient > Invalid host address : "<<ServerIP<<endl;
        close (m_fd);
        m_fd = -1;
		return -1;
    
	}
    
    socketInfo.sin_port = htons (ServerPort);
    fcntl (m_fd, F_SETFL, O_NONBLOCK);
    connect (m_fd, (struct sockaddr *) &socketInfo, sizeof (struct sockaddr_in));
    
	/*  When the connection has been established asynchronously,
    poll() shall indicate that the file descriptor for the socket is ready for writing. */
    fds.fd = m_fd;
    fds.events = POLLOUT;
    ret = poll (&fds, 1, 500);
    if (ret <= 0) {
	
		cout<<"CTcpIpClient > Error during the socket connection\n";
		close (m_fd);
		m_fd = -1;
		return -1;
    
	}
    
    /* Check the connection */
    ret = getpeername (m_fd, (struct sockaddr *)&socketInfo, (socklen_t *)&sockaddr_len);
    if (ret < 0) {
		
		cout<<"CTcpIpClient > Error during getpeername\n";
		close (m_fd);
		m_fd = -1;
 		return -1;
    
	}
    
    /*	We are connected to the peer
    *	Set socket in blocking mode  */
    flags = fcntl (m_fd, F_GETFL, NULL);
    fcntl (m_fd, F_SETFL, flags & ~O_NONBLOCK);
    strncpy (m_pcServerIP, ServerIP, 16);
    m_uiServerPort = ServerPort;
    cout <<"CTcpIpClient > Socket now connected to "<< inet_ntoa (socketInfo.sin_addr) << ":" << htons (socketInfo.sin_port) <<endl;
    m_bConnected = true;
    return 0;

}

  /**
   * \fn 			int CTcpIpClient::Close()
   * \brief 		Shutdown and close the socket , stop the thread.
   * \return		int (-1 for error) (0 otherwise)
   */
int CTcpIpClient::Close()
{

    int ret;
    
	/* Check connection */
	if (m_fd < 0 && m_bReconnect == false) {
		
		cout<<"CTcpIpClient > Disconnect call without a valid file descriptor !\n";
		return -1;
    
	}
    
    /* Shutdown the connection, before close it */
    ret = shutdown (m_fd, SHUT_RDWR);
    if (ret < 0) {
		
		cout<<"CTcpIpClient > Error on shutdown\n";
		goto close_socket;
    
	}
    
    /*close thread*/
    m_bThreadRunning = false;
	m_oThreadRxDc.join();
    
	/* Close the connection */
    close_socket:
	close (m_fd);
	m_fd = -1;
	m_bConnected = false;
	cout<<"CTcpIpClient > Close complete"<<endl;	
	UnregisterAllCallBacks(ALL_EVENTS);
	return 0;
}

  /**
   * \fn 			int CTcpIpClient::Reconnect()
   * \brief 		WARNING : Private function , try to reconnect to the server.
   * \return		int (-1 for error) (0 otherwise)
   */
int CTcpIpClient::Reconnect()
{
    int ret;
	unsigned int NbrCo;
    close (m_fd);
    m_fd = -1;
	m_bReconnect = true;

	NbrCo = m_uiNbAttemp;

	cout<<"CTcpIpClient > Trying to reconnect to : " << m_pcServerIP<<" :"<< m_uiServerPort << "\n";
	ret = Connect(m_pcServerIP,m_uiServerPort);

    /*Trying to reconnect only 10 times */
    while (NbrCo != 0 && ret !=0 ) {
	
		sleep (1);
		cout<<"CTcpIpClient > Trying to reconnect to : " << m_pcServerIP<<" :"<< m_uiServerPort << "\n";
		ret = Connect(m_pcServerIP,m_uiServerPort);
		if(NbrCo < INFINITE)
		{	
			-- NbrCo;
		}
	}   
	
    if (ret < 0)
	{
		cout<<"Reconnexion failed"<<endl;
		return -1;
	}
	
	else 
	{
		m_bReconnect = false;
		return 0;
	}
}

  /**  
   * \fn 			int CTcpIpClient::RegisterCallBack (unsigned int uiIdEvent, boost::function<CallBack> oFct, void* pData1 = NULL, void* pData2 = NULL)
   * \brief 		Register a Callback which will be called when the event specified in EnventID will occure. WARNING in this class pData1 is used for return a Client* to the CallBack.
   * \param[in]  	uiIdEvent : 	Integer that indicate the event ID on which the function will be launched.
   * \param[in]  	oFct :			Object which indicates the function to launch when a event occure.
   * \param[in] 	pData1 :		User pointer.
   * \param[in] 	pData2 : 		User pointer.
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpClient::RegisterCallBack (unsigned int uiIdEvent, boost::function<CallBack> oFct ,void* pData1 = NULL , void* pData2 = NULL)
{
    
	bool bTest = false;

	/* Check if the signal slot are empty */
    if ((m_oSigOnRec.num_slots() + m_oSigOnDc.num_slots()) == 0)
    {
		bTest = true;
    }
    
	/* check the IdEvent who's the users wants to associate with his CallBack */
    if (uiIdEvent == EVENT_CTCPIPCLIENT_RECEIVEDATA) {
	
		m_oLastFctId = m_oSigOnRec.connect(oFct);		// Connected the passed function and saved its connection ID

	/* check the IdEvent who's the users wants to associate with his CallBack */
    }else if(uiIdEvent == EVENT_CTCPIPCLIENT_ONDISCONNECT) {

		m_oLastFctId = m_oSigOnDc.connect(oFct); 		// Connected the passed function and saved its connection ID
	
	}else{
		cout<<"CTcpIpClient > Wrong ID Event" << endl;
		return -2;
    }
    
    if(m_oLastFctId.connected() == false) {
		cout<<"CTcpIpClient > Error register Callback " << endl;
		return -1;
	}else {		
		cout<<"CTcpIpClient > CallBack correctement connectée"<<endl;
	}
	
	m_pData1 = pData1;
	m_pData2 = pData2;
    /*If the function is Connected check if the signal slot have only one function associate 
	if is true we can start a thread.*/
    if(bTest == true){ 
		// Indicate to the thread it should run
		m_bThreadRunning = true;
		// And launch the OnRec OnDisc thread
		/* Start the thread */
		boost::thread m_oThread(boost::bind(&CTcpIpClient::OnReceive, this));
/*				cout << "thread started " <<endl;  */  
		return 0;  
	}    
    return 0;
}

  /**  
   * \fn			int CTcpIpClient::UnregisterCallBack(unsigned int uiIdEvent, boost::signals2::connection oIdFct)
   * \brief 		Unregister a previously register callback using its connection Id
   * \param[in] 	uiIdEvent : Integer that indicate the event ID on which the function is launched
   * \param[in]  	oIdFct : Object which indicates the connection ID of the function to unregister
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpClient::UnregisterCallBack (unsigned int uiIdEvent, boost::signals2::connection oIdFct )
{
   
	// Init variables
    std::size_t iNumBefore;
    std::size_t iNumAfter;
    
    /* check the IdEvent who's the users wants to unregister his CallBack */    
    if (uiIdEvent == EVENT_CTCPIPCLIENT_RECEIVEDATA) {
		// Checks if there are some connected functions
		if (m_oSigOnRec.num_slots() == 0) {
			cout<<"CTcpIpClient > No CallBack connected on the Rec Client Signal" << endl;
			return -1;
		}
		// Saved the number of connected function before the deconnection
		iNumBefore = m_oSigOnRec.num_slots();
		// Disconnect the function using its connection ID
		oIdFct.disconnect();
		// Saved the number of connected function after the deconnection
		iNumAfter = m_oSigOnRec.num_slots();

	/* check the IdEvent who's the users wants to unregister his CallBack */	
    }else if (uiIdEvent == EVENT_CTCPIPCLIENT_ONDISCONNECT) {
		// Checks if there are some connected functions
		if (m_oSigOnDc.num_slots() == 0) {
			cout<<"CTcpIpClient > No CallBack connected on the Dc Client Signal" << endl;
			return -1;
		}
		// Saved the number of connected function before the deconnection
		iNumBefore = m_oSigOnDc.num_slots();
		// Disconnect the function using its connection ID
		oIdFct.disconnect();
		// Saved the number of connected function after the deconnection
		iNumAfter = m_oSigOnDc.num_slots();

	}else {
		cout<<"CTcpIpClient > Wrong ID Event" << endl;
		return -2;
    }
    // Checks if the function was well disconnected
    if (iNumBefore == iNumAfter+1) {
		//m_lListOnRec.erase(pItToDelete);
		cout<<"CTcpIpClient > CallBack correctly disconnect" << endl;
		// If there is no longer connected functions to the boost::signals
		if (m_oSigOnDc.num_slots() == 0 && m_oSigOnRec.num_slots() == 0) {
			// Indicate to the thread it should stop
			m_bThreadRunning = false;
			// Wait for the thread end
			m_oThreadRxDc.join();
		}
		return 0;
    }else{
	cout<<"CTcpIpClient > CallBack not correctly disconnect" << endl;
	return -2;
    } 
	
	return 0;
}


  /**  
   * \fn 			int CTcpIpClient::UnregisterAllCallBacks(unsigned int uiIdEvent)
   * \brief 		Unregister a previously register callback using its connection Id
   * \param[in]  	uiIdEvent : Integer that indicate the event ID to empty
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpClient :: UnregisterAllCallBacks(unsigned int uiIdEvent)
{
	/* Check if the users wants to unregister all the CallBack of the all events */
    if (uiIdEvent == ALL_EVENTS) {
		// Checks if there are some connected functions
		if ((m_oSigOnRec.num_slots() == 0) && (m_oSigOnDc.num_slots() == 0)) {
			cout<<"CTcpIpClient > No CallBack Connected" << endl;
			return -1;
		}else {
			// Disconnect all the functions from this specific events
			m_oSigOnRec.disconnect_all_slots();
			m_oSigOnDc.disconnect_all_slots();
			// Checks if all the functions was well disconnected
			if ((m_oSigOnRec.num_slots() == 0) && (m_oSigOnDc.num_slots() == 0)){    
			
				cout<<"CTcpIpClient > All CallBacks  correctly disconnected" << endl;
			
			}else {
			
				cout<<"CTcpIpClient > CallBack not correctly Disconnect" << endl;
				return -2;
			}
		}
	/* Check if the users wants to unregister all the CallBack of the Receive data events */
    }else if (uiIdEvent == EVENT_CTCPIPCLIENT_RECEIVEDATA) {
		// Checks if there are some connected functions
		if (m_oSigOnRec.num_slots() == 0) {
			cout<<"CTcpIpClient > No CallBack actually connected" << endl;
			return -1;
		}else {
			// Disconnect all the functions from this specific events
			m_oSigOnRec.disconnect_all_slots();	
			// Checks if all the functions was well disconnected
			if (m_oSigOnRec.num_slots() == 0){	   
				cout<<"CTcpIpClient > All Rx CallBack has been disconnect" << endl;
			}else {
			cout<<"CTcpIpClient > CallBack Rx has not been correctly disconnect" << endl;
			return -2;
			}
		}
	
	/* Check if the users wants to unregister all the CallBack of the On Disconnect events */
    } else if (uiIdEvent == EVENT_CTCPIPCLIENT_ONDISCONNECT) {
		// Checks if there are some connected functions
		if (m_oSigOnDc.num_slots() == 0) {
			cout<<"CTcpIpClient > No CallBack actually connected" << endl;
			return -1;
		}else {
			// Disconnect all the functions form this specific events
			m_oSigOnDc.disconnect_all_slots();
			// Checks if all the functions was well disconnected
			if (m_oSigOnDc.num_slots() == 0){
				cout<<"CTcpIpClient > All the callBack Dc has been Disconnect" << endl;
			}else{
				cout<<"CTcpIpClient > CallBack Dc has not been correctly disconnect" << endl;
				return -2;
			}
		}	
    }

    // Unknown event
    else {
		cout<<"CTcpIpClient > Wrong ID Event" << endl;
	return -2;
    }
	if((m_oSigOnRec.num_slots() == 0) && (m_oSigOnDc.num_slots() == 0)){
		// Indicate to the thread it should stop
		m_bThreadRunning = false;
		// Wait for the thread end
		m_oThreadRxDc.join();
	}else{
		return 0;
	}
	return 0;
}

   /**
    * \fn 			void CTcpIpClient::OnReceive()
    * \brief 		Method who manage the launch of CallBack when an event ReceiveData/Deconnection occured.
    */
void CTcpIpClient::OnReceive()
{ 
    
    /* Init */
    int nreaded, ret;
    char pcBuffer [CTCPIPCLIENT_RX_BUFFER_SIZE];
    
	struct pollfd fds;
    /* Init poll */
    fds.fd = m_fd;
    fds.events = POLLIN | POLLPRI | POLLHUP | POLLERR;
	
    do{
	
		/* Poll */
		ret = poll (&fds, 1, 1000);
		
		/* Function poll() aims to detected various events on a socket, in our case we will treat three of them :
		POLLIN  : Data other than high-priority data may be read without blocking. 
		POLLPRI : High-priority data may be read without blocking. 
		POLLHUP : The device has been disconnected. 
		poll() blocks the code and wait his time-out (500ms) or an events */
//		printf("Poll ret code = %d and fd.revent = 0x%04X\n", ret, fds.revents);
		/* Error on poll */
		if (ret < 0) {

			/* cout<<"CTcpIpClient > Error on poll, ret code = " << ret << endl;*/
			close (m_fd);
			m_fd = -1;
			m_bConnected = false;
			m_bThreadRunning = false;
			m_oThreadRxDc.join();
			/*if poll() not detect events in time-out he return 0 */
	
		} else if (ret == 0) {
		
			/*  cout<<"CTcpIpClient > Time-Out"<<endl;*/
			/* poll events detected */
	
		} else if (ret > 0) {
	    
			/* Check if is a disconnection */
			if (fds.revents & POLLHUP) {
				/* Connexion lost */
				cout<<"CTcpIpClient > Peer is disconnected !"<<endl;
				/* bool Available prevent to not try to write when devices disconnected */
				m_bConnected = false;
				
				if (m_bMode == true) {		//TODO Modifier la méthode reconnect en int et prévoir un retour -1 (break);
					ret = Reconnect ();
					if(ret < 0)break; 								//if reconnect failed, thread exit.
				} else {
					/* Call the user CallBack Signal slot OnDisconnet */
/*					cout<<"CTcpIpClient > Lancement de la callback OnDisconnect"<<endl;		    */
					CEvent* pEvent = new CEvent();
					m_oSigOnDc(m_pcServerIP, m_uiServerPort, pEvent, m_pData1, m_pData2);
					Close();
					delete(pEvent);
					break;
				}
	    /* Check if is a data incomming */
	    } else if (fds.revents & (POLLIN | POLLPRI)) {
		
			/* Read */
			memset (pcBuffer, 0, CTCPIPCLIENT_RX_BUFFER_SIZE);				//reset the pcBuffer memory
			nreaded = recv (m_fd, pcBuffer, CTCPIPCLIENT_RX_BUFFER_SIZE, 0);		//read the data receive
		
			if (nreaded <= 0) {
		    
				if (nreaded < 0)cout<<"CTcpIpClient > Error on recv, you must reconnnect the socket!\n";
		    
		    else cout<<"CTcpIpClient > Peer is disconnected !\n";
		    /*	When poll send a POLLIN and the recv return 0 byte readed,
		    It's mean that the peer have process to a shutdown on the link */
		    
			m_bConnected = false;
			if (m_bMode == true) {
			
				ret = Reconnect();
				if(ret < 0)break; 							//if reconnect failed, thread exit.
		    
			} else {
			
				/* Call the user CallBack Signal slot OnDisconnet */
/*				cout<<"CTcpIpClient > Lancement de la callback OnDisconnect"<<endl;*/
				CEvent* pEvent = new CEvent();
				m_oSigOnDc(m_pcServerIP, m_uiServerPort, pEvent, m_pData1, m_pData2);
				Close();
				delete(pEvent);
				break;
		    
			}
		} else {
				
				CEvent* pEvent= new CEvent();
				/* Call the user callback signal slot OnReceive */
				m_oSigOnRec(pcBuffer, nreaded, pEvent, m_pData1, m_pData2);	
				delete (pEvent);
		    
			}
		} else {
		    cout<<"CTcpIpClient > Poll event " << fds.revents << " not supported !\n";
	
		}    
		}
    }while(m_bThreadRunning);
    cout<<"CTcpIpClient > thread off"<<endl;
}

  /**  
   * \fn 			int CTcpIpClient::Write (char * pcBuffer, unsigned int uiBufferSize).
   * \brief 		write a data buffer on the Server.
   * \param[in] 	pcBuffer : data buffer who will be send.
   * \param[in] 	uiBufferSize : size of the buffer who will be send.
   * \return 		int (-1 for error) (0 otherwise) 
   */
 int CTcpIpClient::Write (char * pcBuffer, unsigned int uiBufferSize )
 {
    int ret, offset, size;
    struct pollfd pfd;
    /* Init */
    size = uiBufferSize;
    offset = 0;
    pfd.fd = m_fd;
    pfd.events = POLLOUT;
    /* Check parameters */
    if (this == (CTcpIpClient *)NULL) {
		cout<<"CTcpIpClient > NULL CTcpIpClient pointer\n";
		return -1;
    }

    if (size == 0) return 0;
	if (pcBuffer == (char *)NULL) {
		cout<<"CTcpIpClient > NULL Buffer pointer\n";
		return -1;
    }
    do {
		ret = poll (&pfd, 1, 1000);
		if (ret <= 0) {
			/* Error or timeout */
			return -EIO;
		}
//		printf("fd=%d, buffer+offset=%d, size=%d\n", m_fd, pcBuffer + offset, size);		
		ret = send (m_fd, pcBuffer + offset, size, MSG_CONFIRM);
		if (ret < 0) {
			perror (NULL);
			return ret;
		}
		offset += ret;
		size -= ret;
		if (size <= 0)
		return uiBufferSize;
    } while (1);
    if (ret != size) {
	cout<<"CTcpIpClient > Error during sending buffer, socket shutdown !\n";
	Close();
	return -2;
    } 
    return 0;
}


/**
 *	\fn				boost::signals2::connection CTcpIpClient::GetLastFctId()
 *  \brief			Send back the last connected callback connexion Id
 *	\return			The last connected callback connexion Id
 */
boost::signals2::connection CTcpIpClient::GetLastFctId()
{
    return m_oLastFctId;
}

  /**
   * \fn 			bool CTcpIpClient::IsConnected()
   * \brief 		Check if the Client is Connected to the Server/
   * \return 		true if is Connected , false otherwise.
   */
bool CTcpIpClient::IsConnected()
{
    return m_bConnected;
}

  /**
   * \fn			int CTcpIpClient::PrintConfig()
   * \brief 		print the Config of the Client.
   * \return 		int (-1 for error) (0 otherwise) 
   */
int CTcpIpClient::PrintConfig()
{
    cout<<"CTcpIpClient > Reconnexion mode : "<<m_bMode<<endl;
    cout<<"CTcpIpClient > Thread Running : "<<m_bThreadRunning<<endl;
    cout<<"CTcpIpClient > Bool Connected : "<<m_bConnected<<endl;
	cout<<"CTcpIpClient > Nombre de CallBack Connecté au signal ReceiveData : "<<m_oSigOnRec.num_slots()<<endl;
	cout<<"CTcpIpClient > Nombre de CallBack Connecté au signal OnDisconnect : "<<m_oSigOnDc.num_slots()<<endl;
    cout<<"CTcpIpClient > Ip Server : "<<m_pcServerIP<<endl;
    cout<<"CTcpIpClient > Port : "<<m_uiServerPort<<endl;

    return 0;
}
