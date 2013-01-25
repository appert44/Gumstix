/**
 *	\file	CGPIOExample.cpp
 *
 *	\brief 	This file is used to show an example of using the CGPIO class
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#include "PEGASE_2.h"

#define CGPIO_EXAMPLE_PRINT_NAME		"CGPIO Example > "						/**< \brief Prefixe of each prints of the CGPIO Example application */

using namespace std;

// Global variable
bool m_bRunning = false;

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout << CGPIO_EXAMPLE_PRINT_NAME << "\nCTRL + C !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when received a kill */
void Term (int signal)
{
	unused(signal);
	
	cout << CGPIO_EXAMPLE_PRINT_NAME << "Kill received !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when an event is detected on the Gpio 175 */
void onGpio175Event (char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Variable initialization
	int iValue = atoi (sBuffer);
	
	unused(iBufferSize);
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	// Print the current value of the gpio
	printf ("%sThe interrupt has been activated on the Gpio 175, the Gpio value is now %d\n\n", CGPIO_EXAMPLE_PRINT_NAME, iValue);
}

/** \brief	Function called when an event is detected on the Gpio wired on the PPS */
void onGpioPPSEvent (char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Variable initialization
	int iValue = atoi (sBuffer);
	
	unused(iBufferSize);
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	// Print the current value of the gpio
	printf ("%sThe interrupt has been activated on the PPS Gpio, the Gpio value is now %d\n\n", CGPIO_EXAMPLE_PRINT_NAME, iValue);
}

/** \brief	Function which read the input value of the Gpio passed in parameter every 1 mS and print a message if the value has changed until the program received a CTRL+C */
void * GpioReadThread (void * ptr)
{
	// Variables initialization
	static int iReadVal = -1;
	static int iPrevReadVal = -1;
	
	CGPIO * pGpio = (CGPIO *) ptr;
	
	// While we haven't said to thread to stop (using the boolean m_bRunning)
	do
	{
		usleep(1000);
		
		// Read the current value of the Gpio and check if the read happened well
		iReadVal = pGpio->Read();
		if (iReadVal < 0) {
			printf("%sError %d during the Gpio 184 reading, thread stopping\n\n", CGPIO_EXAMPLE_PRINT_NAME, iReadVal);
			break;
		}
		
		// If the Gpio value has changed
		if (iReadVal != iPrevReadVal) {
			// We print the new value
			printf("%sThe value of the Gpio 184 just changed, it is now %d\n\n", CGPIO_EXAMPLE_PRINT_NAME, iReadVal);
			iPrevReadVal = iReadVal;
		}
		
	} while(m_bRunning);
	
	pthread_exit (NULL);
}


/** \brief	Demonstration program of using CGPS */
int main()
{
	// Variables initialization	
	int iCpt = 0, iToWrite = 0, iRet;
	
	CGPIO * pGpio184;					// I2C 3 - SCL
	CGPIO * pGpio185;					// I2C 3 - SDA
	CGPIO * pGpio175;					// SPI 1 - CS1
	
	CGPIO * pGpio147;					// PPS Tobi&GPS/Prototype PEGASE2
	
	CGPIO * pGpio171;					// LED2 Prototype PEGASE2
	CGPIO * pGpio65;					// LED1 Prototype PEGASE2
	
	//pthread_t tReadThread;
	
	signal (SIGINT, Stop);					// Redirect <Ctrl+C> to Stop fuction
	signal (SIGTERM, Term);					// Redirect kill to Term fuction
	
	m_bRunning = true;
	
	// Start
	cout << CGPIO_EXAMPLE_PRINT_NAME << "Test Gpio program start" << endl << endl;
	
	// Create new CGPIOs objects
	pGpio184 = new CGPIO((char *) "/dev/gpio184", INPUT, 0);
	pGpio185 = new CGPIO((char *) "/dev/gpio185", OUTPUT, 0);
	pGpio175 = new CGPIO((char *) "/dev/gpio175", IRQF_TRIGGER_RISING);
	
	pGpio147 = new CGPIO((char *) "/dev/gpio147", IRQF_TRIGGER_RISING);
	

	pGpio171 = new CGPIO((char *) "/dev/gpio171",OUTPUT, 0);
	pGpio65 = new CGPIO((char *) "/dev/gpio065", OUTPUT, 0);
	

	// Initialize GPIOs
/*	iRet = pGpio184->Open();
	if (iRet < 0) {
		cout << CGPIO_EXAMPLE_PRINT_NAME << "Failure during the initialization of the Gpio 184 : error " << iRet << endl << endl;
		return -1;
	}
	
	iRet = pGpio185->Open();
	if (iRet < 0) {
		cout << CGPIO_EXAMPLE_PRINT_NAME << "Failure during the initialization of the Gpio 185 : error " << iRet << endl << endl;
		return -2;
	}*/
	
	iRet = pGpio175->Open();
	if (iRet < 0) {
		cout << CGPIO_EXAMPLE_PRINT_NAME << "Failure during the initialization of the Gpio 175 : error " << iRet << endl << endl;
		return -3;
	}
	
	/*iRet = pGpio147->Open();
	if (iRet < 0) {
		cout << CGPIO_EXAMPLE_PRINT_NAME << "Failure during the initialization of the Gpio 147 : error " << iRet << endl << endl;
		return -4;
	}*/
	
	iRet = pGpio171->Open();
	if (iRet < 0) {
		cout << CGPIO_EXAMPLE_PRINT_NAME << "Failure during the initialization of the Gpio 64 : error " << iRet << endl << endl;
		return -5;
	}
	
/*	iRet = pGpio65->Open();
	if (iRet < 0) {
		cout << CGPIO_EXAMPLE_PRINT_NAME << "Failure during the initialization of the Gpio 65 : error " << iRet << endl << endl;
		return -6;
	}*/
	
	// Add the launch of the function onGpio175Event when an event is detected on the Gpio 175
	boost::function<CallBack> oOnCallBackGpio175Event = &onGpio175Event;
	iRet = pGpio175->RegisterCallback(EVENT_CGPIO_EVENT, oOnCallBackGpio175Event, NULL, NULL);
	if (iRet < 0) {
		cout << CGPIO_EXAMPLE_PRINT_NAME << "Failure during the registration of the callback for the Gpio 175 : error " << iRet << endl << endl;
		return -7;
	}
	boost::signals2::connection oIdCallbackGpio175Event = pGpio175->GetLastFctId();
	
	// Add the launch of the function onGpioPPSEvent when an event is detected on the Gpio 147 (PPS)
/*	boost::function<CallBack> oOnCallBackGpioPPSEvent = &onGpioPPSEvent;
	iRet = pGpio147->RegisterCallback(EVENT_CGPIO_EVENT, oOnCallBackGpioPPSEvent, NULL, NULL);
	if (iRet < 0) {
		cout << CGPIO_EXAMPLE_PRINT_NAME << "Failure during the registration of the callback for the Gpio 147 : error " << iRet << endl << endl;
		return -8;
	}
	boost::signals2::connection oIdCallbackGpioPPSEvent = pGpio147->GetLastFctId();
	
	// Launch the reading thread
	if(pthread_create(&tReadThread, NULL, GpioReadThread, (void *) pGpio184) != 0) {
		printf("%sError during the thread launching\n", CGPIO_EXAMPLE_PRINT_NAME);
		return -9;
	}*/
	
	// While we not received a CTRL+C or a kill
	do {
		// Wait 1 second
		iCpt++;
		sleep(1);
		
		// Write values on the Gpios
		iRet = pGpio171->Write(iToWrite);
		if (iRet < 0)
			cout << CGPIO_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the write on the Gpio 64" << endl << endl;
	/*	iRet = pGpio65->Write(iToWrite);
		if (iRet < 0)
			cout << CGPIO_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the write on the Gpio 65" << endl << endl;
		iRet = pGpio185->Write(iToWrite);
		if (iRet < 0)
			cout << CGPIO_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the write on the Gpio 185" << endl << endl;
		*/
		// Check which value we'll need to write the next time
		if (iToWrite == 0)
			iToWrite = 1;
		else
			iToWrite = 0;
		
		// Unregister some CallBacks after some seconds
	/*	if (iCpt == 10) {
			iRet = pGpio147->UnregisterCallback(EVENT_CGPIO_EVENT, oIdCallbackGpioPPSEvent);
			if (iRet < 0)
				cout << CGPIO_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the unregistration of the callback on Pps event" << endl << endl;
		}
		else if (iCpt == 60) {
			iRet = pGpio175->UnregisterCallback(EVENT_CGPIO_EVENT, oIdCallbackGpio175Event);
			if (iRet < 0)
				cout << CGPIO_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the unregistration of the callback on Gpio 175 event" << endl << endl;
		}*/
	} while (m_bRunning);
	
	// Delete the CGPIO objects
/*	delete(pGpio184);
	delete(pGpio185);*/
	delete(pGpio175);
	
/*	delete(pGpio147);
	*/
	delete(pGpio171);
	//delete(pGpio65);
    
    return 0;    
}
