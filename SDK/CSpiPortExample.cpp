/*-------------------------------------------------------------------
//TODO THIS CLASS HAS NOT BEEN TESTED!!!
-------------------------------------------------------------------*/


/**
 *	\file	CSpiPortExample.cpp
 *
 *	\brief 	This file is used to show an example of using the CSpiPort class
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 8 June, 2012
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
	
	cout<<"\nCClientTcpIP >CTRL + C !!" << endl;
	mRunning = false;
}

/** \brief	Function called when received a kill */ 
void Term (int signal)
{
	unused(signal);
	
	cout << "\n CClientTcpIP >  Kill!!" << endl;
	mRunning = false;
}

/** \brief	Function called when data has been receive on the socket */
void OnReceiveCallBack(char * sBuffer, int iBufferSize, CEvent* pEvent, void* pData1, void* pData2)
{
	unused(sBuffer);
	unused(iBufferSize);	
	unused(pEvent);
	unused(pData1);
	unused(pData2);
    
    cout << "My callback called: " << sBuffer <<", size : "<<iBufferSize<< endl;
}


/** \brief	Demonstration program of using CSpiPort */
int main()
{
	/* Init */
	CSpiPort * pSPI = NULL;																	//Init pointer on object CClientTcpIP
	int iRet;
	unsigned int uiSampleSize = 16;							// 8 / 16 / 32 CAN SPI
	unsigned int uiSpiChannel = 1;							// 1-4
	unsigned int uiSampleCountbyCB = 5000;					// 160 - ???(10k)
	unsigned int uiSampleFrequency = 5000;					// 1 - 100k
	unsigned int uiCsNumber = 1;							// Depend of SpiChannel (4 cs for ch 1 , 2 cs for ch 2 , 1 cs for ch 1 , 1 cs for ch 1) 

    signal (SIGINT, Stop);																			// Redirect <Ctrl+C> to Stop fuction
    signal (SIGTERM, Term);																			// Redirect kill to Term fuction
    
     mRunning = true;
 
	pSPI = new CSpiPort(uiSampleSize,uiSpiChannel,uiSampleCountbyCB,uiSampleFrequency,uiCsNumber);	//Create Client Object true for Reconnect mode 
    if (pSPI == NULL) {
		cout<<"CSpiPort > Error on CSpiPort_New\n";
		return -1;
	}

	/* Init CallBack Function */
	/*  Register CallBack need a boost function on his prototype */
    boost::function<CallBack> oOnCallBackRx = &OnReceiveCallBack;							//Convert the User function Receive to boost function

    iRet = pSPI->RegisterCallBack(EVENT_CSPIPORT_RECEIVEDATA ,oOnCallBackRx, pSPI, NULL);	//Register Rx CallBack
    if(iRet < 0){
		cout<<"CSpiPort > Register failed"<<endl;
		delete pSPI;
		return iRet;
    }

    /*WARNING if you will unregister a function , you need to get his Id because boost connect return only the last Id of the function register */
    boost::signals2::connection oIdCallbackClientTCP1 = pSPI->GetLastFctId();			//Get the Id of function connected for unregister later.

 	iRet = pSPI->Open();
	if(iRet < 0){
		cout<<"CSpiPort > Open Failed"<<endl;
		delete pSPI;
		return iRet;
	}
	
	iRet = pSPI->Start();
	if(iRet < 0){
		cout<<"CSpiPort > Start Failed"<<endl;	 
		delete pSPI;
		return iRet;
	}
	
	do {
		sleep(1);
		cout<<"CSpiPort > main"<<endl;
	} while(mRunning);

	pSPI->Stop();
	pSPI->Close();
	delete(pSPI);

	return 0;
}
