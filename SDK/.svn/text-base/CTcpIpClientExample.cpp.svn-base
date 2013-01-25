/**
 *	\file	CTcpIpClientExample.cpp
 *
 *	\brief 	This file is used to show an example of using the CTcpIpClient class
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#include "PEGASE_2.h"

using namespace std;

// Global variable
bool mRunning = false;			/**< \brief Boolean used to stop the program */

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout<<"\nCTcpIpClient >CTRL + C !!" << endl;
	mRunning = false;
}

/** \brief	Function called when received a kill */ 
void Term (int signal)
{
	unused(signal);
	
	cout << "\n CTcpIpClient >  Kill!!" << endl;
	mRunning = false;
}

/** \brief	Function called when data has been receive on the socket  */
void OnReceiveCallBack(char* sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
    unused(pEvent);
    unused(pData1);
    unused(pData2);
    
    cout << "My callback called: " << sBuffer <<", size : "<<iBufferSize<< endl;
}

/** \brief	Function called when a deconnection has been detected  */
void OnDeconnectionCallBack(char* pcServerIp, int iServerPort, CEvent* pEvent, void* pData1, void* pData2)
{
	unused(pEvent);
    unused(pData1);
    unused(pData2);
    
    cout << "You have been disconnected of the server : Ip server : "<< pcServerIp <<", Server port : "<< iServerPort << endl;
}

/** \brief	Demonstration program of using CTcpIpClient */
int main()
{
	
	/* Init */
	CTcpIpClient* pClient = NULL;																	//Init pointer on object CTcpIpClient
	int ret;
    mRunning = true;
/* 	string write;*/
    
    signal (SIGINT, Stop);																		// Redirect <Ctrl+C> to Stop fuction
    signal (SIGTERM, Term);																		// Redirect kill to Term fuction
    
    char ServerIp[16] = "192.168.0.25";														//Select the Ip port of the Server
		
    pClient = new CTcpIpClient(true , INFINITE);													//Create Client Object true for Reconnect mode 
    if (pClient == NULL) {
	
		cout<<"CTcpIpClient > Error on CTcpIpClient_New\n";
		return -1;
    
	}
    
    /* Init CallBack Function */
	/*  Register CallBack need a boost function on his prototype */
    boost::function<CallBack> oOnCallBackRx = &OnReceiveCallBack;								//Convert the User function Receive to boost function
    boost::function<CallBack> oOnCallBackDc = &OnDeconnectionCallBack;							//Convert the User function Deconnection to boost function
    
	
	ret = pClient->Connect(ServerIp,1234);														//Connect to the Server, Set the port the Server listens
    if(ret < 0)
    {
		
		cout<<"CTcpIpClient > Connexion failed check your Server."<<endl;
		delete pClient;
		return -1;
    
	}
    
	sleep(1);
    ret = pClient->RegisterCallBack(EVENT_CTCPIPCLIENT_RECEIVEDATA ,oOnCallBackRx, pClient, NULL);				//Register Rx CallBack
    if(ret < 0)
    {
	
		cout<<"CTcpIpClient > Register failed"<<endl;
		delete pClient;
		return -1;
    
    }
    /*WARNING if you will unregister a function , you need to get his Id because boost connect return only the last Id of the function register */
    boost::signals2::connection oIdCallbackClientTCP1 = pClient->GetLastFctId();			//Get the Id of function connected for unregister later.
    
    ret = pClient->RegisterCallBack(EVENT_CTCPIPCLIENT_ONDISCONNECT ,oOnCallBackDc, pClient, NULL);				//Register Dc CallBack
    if(ret < 0)
    {
	
		cout<<"CTcpIpClient > Register failed"<<endl;
		delete pClient;
		return -1;
    
    }
    /*WARNING if you will unregister a function ,
	you need to get his Id because boost connect return only the last Id of the function register */
    boost::signals2::connection oIdCallbackClientTCP2 = pClient->GetLastFctId();					//Get the Id of function connected for unregister later.
	
	ret = pClient->PrintConfig();
    if(ret < 0)
    {

		cout<<"CTcpIpClient > print config failed"<<endl;
		delete pClient;
		return -1;
    
	}
	
//     ret = pClient->UnregisterCallBack(EVENT_CTCPIPCLIENT_RECEIVEDATA,oIdCallbackClientTCP1);		 //Unregister the CallBack Rx
// 	if(ret < 0){
// 		
// 		cout<<"CTcpIpClient > Unregister Failed"<<endl;
// 		delete pClient;
// 		return -1;
// 	    
// 	}


	
	
    do
	{
		sleep(1);

		if(pClient->IsConnected() == true)														//Only enable write when the socket is connected to the server
		{
			ret = pClient->Write((char*)"abcd",4);
			if(ret < 0){
				cout<<"CTcpIpClient > write failed"<<endl;
				break;
			}
		}

	}while(mRunning);
	    
	
	ret = pClient->Close();
	if(ret < 0){
		cout<<"CTcpIpClient > Close not complete"<<endl;
	}
	
	delete pClient;
	
	return 0;
}
