/**
 *	\file	CTcpIpServerExample.cpp
 *
 *	\brief 	This file is used to show an example of using the ServerTcpIP class
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 25 April, 2012
 *
 */

#include "PEGASE_2.h"

using namespace std;

// Global variable
int mRunning = false;			/**< \brief Boolean used to stop the program */

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout << "\nCTRL + C !!" << endl;
	mRunning = false;
}

/** \brief	Function called when received a kill */
void Term (int signal)
{
	unused(signal);
	
	cout << "Kill reçu !!" << endl;
	mRunning = false;
}

/** \brief	Function called when a connection has been detected on the listenning socket. */
void OnConnectionCallBack(char * sBuffer, int iBufferSize, CEvent* pEvent, void * pData1, void* pData2)
{
	CTcpIpServer::CConnectedClient * pClient = (CTcpIpServer::CConnectedClient *) pData1;
	
	unused(pEvent);
	unused(pData2);
	unused(sBuffer);
	unused(iBufferSize);
	
	cout<<"Un client s'est connecté : Ip Client : "<< pClient->GetClientAdressIP() <<", port du Client : "<< pClient->GetClientPort() << endl;
}

/** \brief	Function called when data has been receive on the socket CTcpIpServer::CConnectedClient* */
void OnReceiveCallBack(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	CTcpIpServer * pServer = (CTcpIpServer *) pData2;
	CTcpIpServer::CConnectedClient * pExpediteur = (CTcpIpServer::CConnectedClient *) pData1;
	
	unused(pEvent);
	
    cout << "My callback called: " << sBuffer << ", de taille: " << iBufferSize << endl;
    
	pServer->Write (pExpediteur, sBuffer, iBufferSize);
	
	/* Quit if receive 'exit' from a client */
	if (strncmp ("exit", sBuffer, 4) == 0) {
		pServer->Write (pExpediteur, (char *) "Bye bye !\n", sizeof("Bye bye !\n")-1);
		mRunning = false;
	}
}

/** \brief	Function called when a deconnection has been detected on the socket CTcpIpServer::CConnectedClient* */
void OnDeconnectionCallBack(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	CTcpIpServer::CConnectedClient * pClient = (CTcpIpServer::CConnectedClient *) pData1;
	
	unused(pEvent);
	unused(pData2);
	unused(sBuffer);
	unused(iBufferSize);
	
	cout<<"CTcpIpServer >Un Client s'est déconnecté du serveur : Ip du Client : "<< pClient->GetClientAdressIP() <<", port du Client : "<< pClient->GetClientPort() << endl;
}

/** \brief	Demonstration program of using CTcpIpServer */
int main()
{
	int iRet;
	CTcpIpServer * pServer = NULL;
	 
    signal (SIGINT, Stop);									// Redirect <Ctrl+C> to Stop fuction
    signal (SIGTERM, Term);									// Redirect kill to Term fuction
	
	pServer = new CTcpIpServer(1234);
	
	mRunning = true;

	/* Init CallBack Function */
    boost::function<CallBack> oOnCallBackRx = &OnReceiveCallBack;
    boost::function<CallBack> oOnCallBackDc = &OnDeconnectionCallBack;
    boost::function<CallBack> oOnCallBackCo = &OnConnectionCallBack;

	
	iRet = pServer->RegisterCallBack(EVENT_CTCPIPSERVER_RECEIVEDATA ,oOnCallBackRx, NULL, pServer);		//Register Rx CallBack
    if(iRet < 0) {
		cout<<"CTcpIpServer > Register failed"<<endl;
		delete pServer;
		return -1;
	}
	
    boost::signals2::connection oIdCallbackServerTCP1 = pServer->GetLastFctId();			//Get the Id of function connected
 
    
    iRet = pServer->RegisterCallBack(EVENT_CTCPIPSERVER_ONDISCONNECT ,oOnCallBackDc , NULL, NULL);		//Register Dc CallBack
    if(iRet < 0) {
		cout<<"CTcpIpServer > Register failed"<<endl;
		delete pServer;
		return -1;
    }
    
    boost::signals2::connection oIdCallbackServerTCP2 = pServer->GetLastFctId();			//Get the Id of function connected

	
	iRet = pServer->RegisterCallBack(EVENT_CTCPIPSERVER_ONCONNECT ,oOnCallBackCo, NULL, NULL);		//Register Dc CallBack
    if(iRet < 0) {
		cout<<"CTcpIpServer > Register failed"<<endl;
		delete pServer;
		return -1;
    }
    
    boost::signals2::connection oIdCallbackServerTCP3 = pServer->GetLastFctId();			//Get the Id of function connected
	
	iRet = pServer->Start();
	if(iRet < 0) {
		cout<<"CTcpIpServer > Started failed"<<endl;
		return -1;
	}
	
// 	iRet = pServer->UnregisterCallBack(EVENT_CTCPIPSERVER_RECEIVEDATA,oIdCallbackServerTCP1);	 
// 	if(iRet < 0) {
// 		cout<<"CTcpIpServer >Unregister Failed"<<endl;
// 		delete pServer;
// 		return -1;
// 	}


// 	iRet = pServer->UnregisterAllCallBacks(ALL_EVENTS);
// 	if(iRet < 0) {
// 		cout<<"CTcpIpServer >Unregister All Failed"<<endl;
// 		delete pServer;
// 		return -1;
// 	}

	pServer->PrintConfig();

	do {
		sleep(1);
		cout<<"CTcpIpServer > Nombre de client connectée : "<<pServer->GetNbClientsConnected()<<endl;
		// pServer->Write(NULL,"toto",4);
	} while(mRunning);
	
	pServer->Stop();
	delete(pServer);

	return 0;
}
