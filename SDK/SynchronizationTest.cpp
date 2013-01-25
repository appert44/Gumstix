/**
 *	\file	SynchronizationTest.cpp
 *
 *	\brief 	This file contains the test application for demonstrating the 
 *			efficiency of the synchronisation bewtween some PEGASE2 boards.
 *	
 *	\author Jonathan Aillet
 *
 *	\version v1.0a
 *
 *	\date 28 June, 2012
 *
 */
 
#include "PEGASE_2.h"

#define SYNCHRONIZATION_TEST_PRINT_NAME		"Synchronization Test > "			/**< \brief Prefixe of each prints of the Synchronization test application */

using namespace std;

// Global variable
bool m_bRunning = false;			/**< \brief Boolean used to stop the program */

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout << SYNCHRONIZATION_TEST_PRINT_NAME << "CTRL + C !!" << endl << endl;
	m_bRunning = false;
}

/** \brief	Function called when received a kill */
void Term (int signal)
{
	unused(signal);
	
	cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Kill received !!" << endl << endl;
	m_bRunning = false;
}

/** \brief	Function called when a GGA frame is received on the GPS module */
void onReceiveGGA(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	unused(sBuffer);
	unused(iBufferSize);
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Gga frame received" << endl << endl;
}

/** \brief	Function called when a ZDA frame is received on the GPS module */
void onReceiveZDA(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	unused(sBuffer);
	unused(iBufferSize);
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Zda frame received" << endl << endl;
}

/** \brief	Function called when an event is detected on the Gpio 185 */
void onGpio185Event (char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	int iValue = atoi (sBuffer);
	
	unused(iBufferSize);
	unused(pData1);
	unused(pData2);
	
	// Print the current value of the gpio and the moment when the value has changed
	printf ("%sThe interrupt has been activated on the Gpio 185, the Gpio value is now %d\n", SYNCHRONIZATION_TEST_PRINT_NAME, iValue);
	printf ("%sInterruption triggered at %02d:%02d:%02d:%06d %02d/%02d/%04d\n\n", SYNCHRONIZATION_TEST_PRINT_NAME, pEvent->GetHour(), pEvent->GetMinute(), pEvent->GetSecond(), pEvent->GetMicroSecond(), pEvent->GetDay(), pEvent->GetMonth(), pEvent->GetYear());
}

/** \brief	Synchronization demonstration program */
int main()
{
	// Variables initialization 	
	int iRet = 0, iCpt = 0;
	
	signal (SIGINT, Stop);			// Redirect <Ctrl+C> to Stop fuction
	signal (SIGTERM, Term);			// Redirect kill to Term fuction
	
	CGPIO * pGpio184;
	CGPIO * pGpio185;
	
	CGPS * pGps;
	
	m_bRunning = true;
	
	// Initialization
	cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Synchronization test program initialization" << endl << endl;

	
	// Create a new CGPS object
	pGps = new CGPS();
	
	// Initialize GPS
	iRet = pGps->Open();
	if (iRet < 0) {
		cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Fail to initialize the Gps, error : " << iRet << endl << endl;
		return -2;
	}
	
	// Create the GPIO objetcs
	pGpio184 = new CGPIO((char *) "/dev/gpio184", OUTPUT, 0);
	pGpio185 = new CGPIO((char *) "/dev/gpio185", IRQF_TRIGGER_RISING);
	
	// Initialize GPIOs
	iRet = pGpio184->Open();
	if (iRet < 0) {
		cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Fail to initialize the Gpio 184, error : " << iRet << endl << endl;
		return -3;
	}
	
	iRet = pGpio185->Open();
	if (iRet < 0) {
		cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Fail to initialize the Gpio 185, error : " << iRet << endl << endl;
		return -4;
	}
	
	// Add the launch of onReceiveGGA when a GGA frame is received
	/*boost::function<CallBack> oOnCallBackGGA = &onReceiveGGA;
	iRet = pGps->RegisterCallback(EVENT_CGPS_RECEIVEGGADATA, oOnCallBackGGA, NULL, NULL);
	if (iRet < 0) {
		cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Fail to register the callback on a Gga frame receive, error : " << iRet << endl << endl;
		return -5;
	}
	boost::signals2::connection oIdCallbackGga = pGps->GetLastFctId();*/
	
	// Add the launch of onReceiveGGA when a ZDA frame is received
	/*boost::function<CallBack> oOnCallBackZDA = &onReceiveZDA;
	iRet = pGps->RegisterCallback(EVENT_CGPS_RECEIVEZDADATA, oOnCallBackZDA, NULL, NULL);
	if (iRet < 0) {
		cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Fail to register the callback on a Zda frame receive, error : " << iRet << endl << endl;
		return -6;
	}
	boost::signals2::connection oIdCallbackZda = pGps->GetLastFctId();*/
	
	// Add the launch of the function onGpio185Event when an event is detected on the Gpio 185
	boost::function<CallBack> oOnCallBackGpio185Event = &onGpio185Event;
	iRet = pGpio185->RegisterCallback(EVENT_CGPIO_EVENT, oOnCallBackGpio185Event, NULL, NULL);
	if (iRet < 0) {
		cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Fail to register the callback on a event of the Gpio 185, error : " << iRet << endl << endl;
		return -7;
	}
	boost::signals2::connection oIdCallbackGpio185 = pGpio185->GetLastFctId();
	
	// Set the debounce delay
	iRet = pGpio185->SetDelay(50);
	if (iRet < 0) {
		cout << SYNCHRONIZATION_TEST_PRINT_NAME << "Fail to configure debounce, error : " << iRet << endl << endl;
		return -8;
	}
	
	// Start
	printf("%sProgram start, synchronization driver in run mode 1 : without quartz error correction\n\n", SYNCHRONIZATION_TEST_PRINT_NAME);
	
	// While we not received a CTRL+C or a kill
	do {
		// Write a 1 on the Gpio 184
		iRet = pGpio184->Write(1);
		if (iRet < 0)
			printf("%sError %d during the write of a 1 on the Gpio 184\n", SYNCHRONIZATION_TEST_PRINT_NAME, iRet);
		
		// Wait during 50 ms
		usleep (50000);
		
		// Write a 0 on the Gpio 184
		iRet = pGpio184->Write(0);
		if (iRet < 0)
			printf("%sError %d during the write of a 0 on the Gpio 184\n", SYNCHRONIZATION_TEST_PRINT_NAME, iRet);
		
		// Wait during 1,1 s
		sleep(1);
		usleep (100000);
		
		// If we have change the Gpio value 20 times
		if (iCpt == 20) {
			// Change the mode of the synchronization driver
			iRet = pGps->SetDriverRunningMode(MODE2);
			
			// Check if it happened well
			if (iRet <= 0)
				printf("%sError when the mode of the synchronization driver has been changed\n\n", SYNCHRONIZATION_TEST_PRINT_NAME);
			else
				printf("%sSynchronization driver in run mode 2 : with quartz error correction\n\n", SYNCHRONIZATION_TEST_PRINT_NAME);
		}
		
		iCpt++;
	} while (m_bRunning);
	
	// Delete the CGPS object
	delete(pGps);
	delete(pGpio184);
	delete(pGpio185);
    
    return 0;    
}
