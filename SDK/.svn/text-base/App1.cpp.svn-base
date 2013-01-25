/**
 *	\file	App1.cpp
 *
 *	\brief 	Demonstration program of an application who receive data by a gps and send it to a server
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
void OnReceiveCallBackClient (char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	cout << "Ma CallBack called  "<< sBuffer << " size : " << iBufferSize << endl;
}

/** \brief	Function called when the Client is disconnected */
void OnDeconnectionCallBackClient(char * pcServerIp, int iServerPort, CEvent * pEvent, void * pData1, void * pData2)
{
    unused(pEvent);
	unused(pData1);
	unused(pData2);
    
    cout << "You have been disconnected of the server : Ip server : "<< pcServerIp <<", Server port : "<< iServerPort  << endl;
}

/** \brief	Function called when a Gga frame is received on the GPS module */
void onReceiveGga(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	char sBufferToSend[10240];
	memset(sBufferToSend,0,10240);
	
	unused(pEvent);
	
	// Cast the Gga * object passed in pData1
	CGPS::Gga * pGgaFrame = (CGPS::Gga *) pData1;
	// Cast the CTcpIpClient* object passed in pData2
	CTcpIpClient* m_pClient = (CTcpIpClient*) pData2;
	
	// Print the Gga frame which launched the Callback
	cout << "Buffer Gga reçu : " << sBuffer << " de taille : " << iBufferSize << endl << endl;
	

	sprintf(sBufferToSend, "Trame Gga : \n\tHeure : %02d:%02d:%02d\n\tNombre de satellite : %d\n\tQualite : %d\n\tPosition : %.5f%c, %.5f%c\n\tHDOP : %f\n\tAltitude : %f\n\tMSL : %f\n\tDGPS : %d - %f\n\n",
		pGgaFrame->uiHour,
		pGgaFrame->uiMinute,
		pGgaFrame->uiSecond,
		pGgaFrame->uiNbSat,
		pGgaFrame->uiQuality,
		pGgaFrame->dLatitude, pGgaFrame->cLatitudeRef,
		pGgaFrame->dLongitude, pGgaFrame->cLongitudeRef,
		pGgaFrame->dHDOP,
		pGgaFrame->dAltitude,
		pGgaFrame->dMSL,
		pGgaFrame->uiDGpsId, pGgaFrame->dDGpsTime);

		// Send the content of the Gga * object
		m_pClient->Write(sBufferToSend, strlen(sBufferToSend));
}


/**
 *	\brief	Demonstration program of an application who receive data by a gps and send it to a server
 *	
 *	\param[in] 	argc : 		Number of argument
 *	\param[in] 	argv : 		Pointer of argument (here argv[1] = IP and argv[2] = Port)
 */
int main (int argc, char ** argv)
{
	int ret = 0;
	unsigned int Port;
	
	if(argc != 3)
	{
		cout<<"you need to take argument on execution : ./Example IP Port"<<endl;
		return -1;
	}
	
	Port = atoi(argv[2]);
	
	CTcpIpClient * m_pClient = NULL;
	CGPS* m_pGps = NULL;

	// Register CallBack need a boost function on his prototype 

	boost::function<CallBack> oOnCallBackRx = &OnReceiveCallBackClient;								//Convert the User function Receive to boost function
	boost::function<CallBack> oOnCallBackDc = &OnDeconnectionCallBackClient;	
	boost::function<CallBack> oOnCallBackGga = &onReceiveGga;

	m_pClient = new CTcpIpClient (true, INFINITE);
	m_pClient->Connect(argv[1], Port);

	ret = m_pClient->RegisterCallBack(EVENT_CTCPIPCLIENT_RECEIVEDATA ,oOnCallBackRx, (void*)m_pClient , NULL);				//Register Rx CallBack
	if(ret < 0)
	{
		cout<<"CTcpIpClient > Register failed"<<endl;
		
		return -1;
	}

	boost::signals2::connection oIdCallbackRx = m_pClient->GetLastFctId();


	m_pGps = new CGPS();

	m_pGps->Open();

	ret = m_pGps->RegisterCallback(EVENT_CGPS_RECEIVEGGADATA, oOnCallBackGga, NULL, (void*)m_pClient);
	if(ret < 0)
	{
		cout<<"CGPS > Register failed"<<endl;
		
		return -1;
	}
	boost::signals2::connection oIdCallbackGga = m_pGps->GetLastFctId();	
	
	do {
		sleep(1);
	} while(m_bRunning);

	return 0;
}
