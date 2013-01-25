/**
 *	\file	App2.cpp
 *
 *	\brief 	Demonstration program of an application who send data to a server in a specific hour
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
bool m_bRunning = true;			/**< \brief Boolean used to stop the program */

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout << "\nCTRL + C !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when received a kill */
void Term (int signal)
{
	unused(signal);
	
	cout << "Kill reçu !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when a data is received on the Client */
void OnReceiveCallBackClient(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	cout << "My callback called: " << sBuffer <<", size : "<<iBufferSize<< endl;
}

/** \brief	Function called when the Client is disconnected */
void OnDeconnectionCallBackClient(char* pcServerIp, int iServerPort, CEvent* pEvent, void* pData1, void* pData2)
{
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	cout << "You have been disconnected of the server : Ip server : "<< pcServerIp <<", Server port : "<< iServerPort  << endl;
}

/** \brief	Function called when the Client is disconnected */
void onSpecHour(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	char sBufferToSend[1024];
	memset(sBufferToSend,0,1024);
	
	// Cast the Zda * object passed in pData1
	CGPS::Zda * ZdaFrame = (CGPS::Zda *) pData1;
	
	unused(sBuffer);
	unused(pEvent);
	unused(iBufferSize);
	
	// Cast the CTcpIpClient* object passed in pData2
	CTcpIpClient * m_pClient = (CTcpIpClient *) pData2;
	// Print the hour using the Zda * object
	sprintf (sBufferToSend,"Nous venons d'atteindre l'heure indiqué pour le lancement de la Callback, il est maintenant %02d:%02d:%02d\n",
		ZdaFrame->uiHour,
		ZdaFrame->uiMinute,
		ZdaFrame->uiSecond);
	
	// Send the content of the GGA * object
	m_pClient->Write(sBufferToSend, strlen(sBufferToSend));	
}


/**
 *	\brief	Demonstration program of an application who send data to a server in a specific hour
 *	
 *	\param[in] argc	: number of argument
 *	\param[in] argv : pointer of argument (here argv[1] = IP and argv[2] = Port
 */
int main (int argc, char ** argv)
{
	
	int ret = 0;
	int Port;
	
	if(argc != 3)
	{
		cout<<"you need to take argument on execution : ./Example IP Port"<<endl;
		return -1;
	}
	
	CTcpIpClient * m_pClient = NULL;
	CGPS * m_pGps = NULL;

	Port = atoi(argv[2]);
	
	// Register CallBack need a boost function on his prototype 
	boost::function<CallBack> oOnCallBackRx = &OnReceiveCallBackClient;								//Convert the User function Receive to boost function
	boost::function<CallBack> oOnCallBackDc = &OnDeconnectionCallBackClient;	
	boost::function<CallBack> oOnCallBackSpecHour = &onSpecHour;

	m_pClient = new CTcpIpClient(true,INFINITE);
	m_pClient->Connect(argv[1],Port);

	ret = m_pClient->RegisterCallBack(EVENT_CTCPIPCLIENT_RECEIVEDATA ,oOnCallBackRx, (void*)m_pClient , NULL);				//Register Rx CallBack
	if(ret < 0)
	{
		cout<<"CTcpIpClient > Register failed"<<endl;
		
		return -1;
	}

	boost::signals2::connection oIdCallbackRx = m_pClient->GetLastFctId();


	m_pGps = new CGPS();

	m_pGps->Open();
	
	// Set the Hour of launch of the SpecHourCallback
	m_pGps->SetCallbackUtcHour(14,46,0);

	ret = m_pGps->RegisterCallback(EVENT_CGPS_ONSPECUTCHOUR, oOnCallBackSpecHour, NULL, (void*)m_pClient);
	if(ret < 0)
	{
		cout<<"CGPS > Register failed"<<endl;
		
		return -1;
	}
	
	boost::signals2::connection oIdCallbackSpecHour = m_pGps->GetLastFctId();	
	
	do
	{
		sleep(1);
	
	}while(m_bRunning);

	return 0;

}
