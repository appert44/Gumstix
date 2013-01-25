/**
 *	\file	CUartExample.cpp
 *
 *	\brief 	This file is used to show an example of using the CUart class
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#include "PEGASE_2.h"

#define CUART_EXAMPLE_PRINT_NAME		"CUart Example > "						/**< \brief Prefixe of each prints of the CUart Example application */

using namespace std;

// Global variables
bool m_bRunning = false;			/**< \brief Boolean used to stop the program */

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout << CUART_EXAMPLE_PRINT_NAME << "\nCTRL + C !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when received a kill */ 
void Term (int signal)
{
	unused(signal);
	
	cout << CUART_EXAMPLE_PRINT_NAME << "Kill received !!" << endl;
	m_bRunning = false;
}

/** \brief	Function N°1 called when a GPS frame is received on the GPS module */
void onUartReceive1(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Variables initialization
	int iRet;
	CUart * pUart = (CUart *) pData1;
	
	unused(pEvent);
	unused(pData2);
	
	// Print the GPS frame which launched the Callback
	cout << CUART_EXAMPLE_PRINT_NAME << "onUartReceive1 : Buffer GPS recu : " << sBuffer << "de taille : " << iBufferSize << endl << endl;
	
	// Respond Hello if the message received is not a Gps message
	if (strncmp (sBuffer, "$GPTXT", 6)) {
		iRet = pUart->Write((char *) "Hello\n", 6);
		if (iRet < 0)
			cout << CUART_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the write on the Uart" << endl << endl;
	}	
}

/** \brief	Function N°2 called when a GPS frame is received on the GPS module */
void onUartReceive2(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	// Print the GPS frame which launched the Callback
	cout << CUART_EXAMPLE_PRINT_NAME << "onUartReceive2 : Buffer GPS recu : " << sBuffer << "de taille : " << iBufferSize << endl << endl;
}

/** \brief	Demonstration program of using CUart */
int main()
{
	// Variables initialization
	int iCpt = 0, iRet;
	
	CUart * pUart = NULL;
	
	signal (SIGINT, Stop);					// Redirect <Ctrl+C> to Stop fuction
	signal (SIGTERM, Term);					// Redirect kill to Term fuction
	
	m_bRunning = true;
	
	// Initialization
	cout << CUART_EXAMPLE_PRINT_NAME << "Initialiazation of the Uart test program" << endl << endl;
		
	// Create the CUart object used to get the Uart data
	pUart = new CUart ((char *) "/dev/ttyO0", B9600, (char *) "8N1", 0, 0, 1);
	
	// Initialize gps
	iRet = pUart->Open();
	if (iRet < 0) {
		cout << CUART_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the initialization of the Uart object" << endl << endl;
		return -1;
	}
	
	pUart->PrintConfig();
	
	// Add the launch of onUartReceive1 when a data is received on the Uart
	boost::function<CallBack> oOnUartReceive1 = &onUartReceive1;
	iRet = pUart->RegisterCallback(EVENT_CUART_RECEIVEDATA, oOnUartReceive1, pUart, NULL);
	if (iRet < 0) {
		cout << CUART_EXAMPLE_PRINT_NAME << "Failure during the registration of the callback 1 for the Uart reception event : error " << iRet << endl << endl;
		return -2;
	}
	boost::signals2::connection oIdCallbackUart1 = pUart->GetLastFctId();
	
	// Add the launch of onUartReceive2 when a data is received on the Uart
	boost::function<CallBack> oOnUartReceive2 = &onUartReceive2;
	iRet = pUart->RegisterCallback(EVENT_CUART_RECEIVEDATA, oOnUartReceive2, pUart, NULL);
	if (iRet < 0) {
		cout << CUART_EXAMPLE_PRINT_NAME << "Failure during the registration of the callback 2 for the Uart reception event : error " << iRet << endl << endl;
		return -3;
	}
	boost::signals2::connection oIdCallbackUart2 = pUart->GetLastFctId();
	
	// Start
	cout << CUART_EXAMPLE_PRINT_NAME << "Start of the Uart test program" << endl << endl;
	
	// While we not received a CTRL+C or a kill
	do {
		// Sleep during one second
		sleep(1);
		iCpt ++;
		
		// Unregister/register some CallBacks after some seconds
		if (iCpt == 10) {
			iRet = pUart->UnregisterCallback(EVENT_CUART_RECEIVEDATA, oIdCallbackUart1);
			if (iRet < 0)
				cout << CUART_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the unregistration of the callback 1 on Uart event" << endl << endl;
		}
		else if (iCpt == 15) {
			iRet = pUart->UnregisterCallback(EVENT_CUART_RECEIVEDATA, oIdCallbackUart2);
			if (iRet < 0)
				cout << CUART_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the unregistration of the callback 1 on Uart event" << endl << endl;
		}
		else if (iCpt == 20) {
			iRet = pUart->RegisterCallback(EVENT_CUART_RECEIVEDATA, oOnUartReceive1, pUart, NULL);
			if (iRet < 0)
				cout << CUART_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the registration of the callback 1 on Uart event" << endl << endl;
				oIdCallbackUart1 = pUart->GetLastFctId();
		}
	} while (m_bRunning);
	
	// Delete the CUart object
	delete(pUart);
    
    return 0;    
}
