/**
 *	\file	CGPSExample.cpp
 *
 *	\brief 	This file is used to show an example of using the CGPS class
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.2a
 *
 *	\date 25 May, 2012
 *
 */

#include "PEGASE_2.h"

#define CGPS_EXAMPLE_PRINT_NAME		"CGPS Example > "							/**< \brief Prefixe of each prints of the CGPS Example application */

using namespace std;

// Global variable
bool m_bRunning = false;			/**< \brief Boolean used to stop the program */

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout << CGPS_EXAMPLE_PRINT_NAME << "\nCTRL + C !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when received a kill */
void Term (int signal)
{
	unused(signal);
	
	cout << CGPS_EXAMPLE_PRINT_NAME << "\nKill received !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when a GPS frame is received on the GPS module */
void onReceivePps(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	unused(sBuffer);
	unused(iBufferSize);
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	// Print that a Pps has been detected
	cout << CGPS_EXAMPLE_PRINT_NAME << "A Pps has been detected" << endl << endl;
}

/** \brief	Function called when a GPS frame is received on the GPS module */
void onReceiveGPS(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	// Print the GPS frame which launched the Callback
	cout << CGPS_EXAMPLE_PRINT_NAME << "Gps frame received : " << sBuffer << " and its size is : " << iBufferSize << endl << endl;
}

/** \brief	Function called when a Gga frame is received on the GPS module */
void onReceiveGga(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Variable initialization
	char * gps[] = {(char *) "No GPS",(char *) "GPS",(char *) "DGPS",(char *) "Bad syntax"};
	
	unused(pEvent);
	unused(pData1);
	unused(pData2);
	
	// Cast the CGPS::Gga * object passed in pData1
	CGPS::Gga * GgaFrame = (CGPS::Gga *) pData1;
	
	// Print the Gga frame which launched the Callback
	cout << CGPS_EXAMPLE_PRINT_NAME << "Gga frame received : " << sBuffer << " and its size is : " << iBufferSize << endl << endl;
	// Print the content of the CGPS::Gga * object
	printf ("%sGga frame content : \n\tHour : %02d:%02d:%02d\n\tNumber of satellite : %d\n\tQuality : %s\n\tPosition : %.5f%c, %.5f%c\n\tHDOP : %f\n\tAltitude : %f\n\tMSL : %f\n\n\n",
		CGPS_EXAMPLE_PRINT_NAME,
		GgaFrame->uiHour,
		GgaFrame->uiMinute,
		GgaFrame->uiSecond,
		GgaFrame->uiNbSat,
		(GgaFrame->uiQuality > 2) ? gps[3] : gps[GgaFrame->uiQuality],
		GgaFrame->dLatitude, GgaFrame->cLatitudeRef,
		GgaFrame->dLongitude, GgaFrame->cLongitudeRef,
		GgaFrame->dHDOP,
		GgaFrame->dAltitude,
		GgaFrame->dMSL);
}

/** \brief	Function called when a Zda frame is received on the GPS module */
void onReceiveZda(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Cast the CGPS::Zda * object passed in pData1
	CGPS::Zda * trame_Zda = (CGPS::Zda *) pData1;
	
	unused(pEvent);
	unused(pData2);
	
	// Print the CGPS::Zda frame which launched the Callback
	cout << CGPS_EXAMPLE_PRINT_NAME << "Zda frame received : " << sBuffer << " and its size is : " << iBufferSize << endl << endl;
	// Print the content of the CGPS::Zda * object
	printf ("%sZda frame content : \n\tThe %02d/%02d/%04d, at %02d:%02d:%02d\n\n",
		CGPS_EXAMPLE_PRINT_NAME,
		trame_Zda->uiDay,
		trame_Zda->uiMonth,
		trame_Zda->uiYear,
		trame_Zda->uiHour,
		trame_Zda->uiMinute,
		trame_Zda->uiSecond);
}

/** \brief	Function called when the day change */
void onDayChange(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Cast the CGPS::Zda * object passed in pData1
	CGPS::Zda * trame_Zda = (CGPS::Zda *) pData1;
	
	unused(sBuffer);
	unused(iBufferSize);
	unused(pEvent);
	unused(pData2);
	
	// Print the new date using the CGPS::Zda * object
	printf ("%sThe day has changed, the day is now %02d/%02d/%04d\n\n\n",
		CGPS_EXAMPLE_PRINT_NAME,
		trame_Zda->uiDay,
		trame_Zda->uiMonth,
		trame_Zda->uiYear);
}

/** \brief	Function called when the uiHour change */
void onHourChange(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Cast the CGPS::Zda * object passed in pData1
	CGPS::Zda * trame_Zda = (CGPS::Zda *) pData1;
	
	unused(sBuffer);
	unused(iBufferSize);
	unused(pEvent);
	unused(pData2);
	
	// Print the new uiHour using the CGPS::Zda * object
	printf ("%sThe hour has changed, the hour is now %02d:%02d:%02d\n\n\n",
		CGPS_EXAMPLE_PRINT_NAME,
		trame_Zda->uiHour,
		trame_Zda->uiMinute,
		trame_Zda->uiSecond);
}

/** \brief	Function called when the specified uiHour is reached */
void onSpecHour(char * sBuffer, int iBufferSize, CEvent * pEvent, void * pData1, void * pData2)
{
	// Cast the CGPS::Zda * object passed in pData1
	CGPS::Gga * GgaFrame = (CGPS::Gga *) pData1;
	
	unused(sBuffer);
	unused(iBufferSize);
	unused(pEvent);
	unused(pData2);
	
	// Print the uiHour using the CGPS::Zda * object
	printf ("%sWe have reached the specified hour to launch this callback, the hour is now %02d:%02d:%02d\n\n\n",
		CGPS_EXAMPLE_PRINT_NAME,
		GgaFrame->uiHour,
		GgaFrame->uiMinute,
		GgaFrame->uiSecond);
}

/** \brief	Demonstration program of using CGPS */
int main()
{
	// Variables initialization
	int iCpt = 0, iRet = 0;
	
	CGPS * pGps = NULL;
	// CGPIO * pSoftwareShutdownGpio;
	CEvent * pEvent = NULL;
	
	signal (SIGINT, Stop);				// Redirect <Ctrl+C> to Stop fuction
	signal (SIGTERM, Term);				// Redirect kill to Term fuction
	
	m_bRunning = true;
	
	// Initialization
	cout << CGPS_EXAMPLE_PRINT_NAME << "Gps test program initialization" << endl << endl;
	
	// Create a new CGPS object
	pGps = new CGPS();
	//pSoftwareShutdownGpio = new CGPIO((char *) "/dev/gpio067", OUTPUT, 1);
	
	// Initialize gps
	iRet = pGps->Open();
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Fail during the Gps initialization : error " << iRet << endl << endl;
		return -1;
	}
	
	// Initialize GPIO
	/*iRet = pSoftwareShutdownGpio->Open();
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Fail during the Gps software shutdown Gpio initialization : error " << iRet << endl << endl;
		return -2;
	}*/
	
	// Set the uiHour of launch of the SpecuiHourCallback
	iRet = pGps->SetCallbackUtcHour(10,0,0);
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Fail during the specified hour callback configuration : error " << iRet << endl << endl;
		return -3;
	}
	
	// Enable the display of the content of the decoded frame when they are received
	//pGps->SetPrintGPGga(true);			// For Gga frame
	//pGps->SetPrintGPZda(true);			// For Zda frame
	//pGps->SetPrintGPRmc(true);			// For RMC frame
	//pGps->SetPrintGst(true);				// For GST frame
	//pGps->SetPrintGsa(true);				// For GSA frame
	
	// Add the launch of onPPS when the PPS is detected
	boost::function<CallBack> oOnCallBackPps = &onReceivePps;
	iRet = pGps->RegisterCallback(EVENT_CGPS_ONPPS, oOnCallBackPps);
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Failure during the registration of the callback for the Pps detection : error " << iRet << endl << endl;
		return -4;
	}
	boost::signals2::connection oIdCallbackPps = pGps->GetLastFctId();
	
	// Add the launch of onDayChange when the day change
	boost::function<CallBack> oOnCallBackDayChange = &onDayChange;
	iRet = pGps->RegisterCallback(EVENT_CGPS_ONDAYCHANGE, oOnCallBackDayChange);
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Failure during the registration of the day change callback : error " << iRet << endl << endl;
		return -5;
	}
	boost::signals2::connection oIdCallbackDayChange = pGps->GetLastFctId();
	
	// Add the launch of onuiHourChange when the hour change
	boost::function<CallBack> oOnCallBackuiHourChange = &onHourChange;
	iRet = pGps->RegisterCallback(EVENT_CGPS_ONHOURCHANGE, oOnCallBackuiHourChange);
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Failure during the registration of the hour change callback : error " << iRet << endl << endl;
		return -6;
	}
	boost::signals2::connection oIdCallbackuiHourChange = pGps->GetLastFctId();
	
	// Add the launch of onSpecuiHour when the specified hour is reached
	boost::function<CallBack> oOnCallBackSpecHour = &onSpecHour;
	iRet = pGps->RegisterCallback(EVENT_CGPS_ONSPECUTCHOUR, oOnCallBackSpecHour);
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Failure during the registration of the specified hour callback : error " << iRet << endl << endl;
		return -7;
	}
	boost::signals2::connection oIdCallbackSpecuiHour = pGps->GetLastFctId();
	
	// Add the launch of onReceiveGPS when a GPS frame is received
	boost::function<CallBack> oOnCallBackGPS = &onReceiveGPS;
	iRet = pGps->RegisterCallback(EVENT_CGPS_RECEIVEGPSDATA, oOnCallBackGPS);
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Failure during the registration of Gps data reception callback : error " << iRet << endl << endl;
		return -8;
	}
	boost::signals2::connection oIdCallbackGPS = pGps->GetLastFctId();
	
	// Add the launch of onReceiveGga when a Gga frame is received
	boost::function<CallBack> oOnCallBackGga = &onReceiveGga;
	iRet = pGps->RegisterCallback(EVENT_CGPS_RECEIVEGGADATA, oOnCallBackGga);
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Failure during the registration of Gga data reception callback : error " << iRet << endl << endl;
		return -9;
	}
	boost::signals2::connection oIdCallbackGga = pGps->GetLastFctId();
	
	// Add the launch of onReceiveGga when a Zda frame is received
	boost::function<CallBack> oOnCallBackZda = &onReceiveZda;
	iRet = pGps->RegisterCallback(EVENT_CGPS_RECEIVEZDADATA, oOnCallBackZda, NULL, pGps);
	if (iRet < 0) {
		cout << CGPS_EXAMPLE_PRINT_NAME << "Failure during the registration of Zda data reception callback : error " << iRet << endl << endl;
		return -10;
	}
	boost::signals2::connection oIdCallbackZda = pGps->GetLastFctId();
	
	// Start
	cout << CGPS_EXAMPLE_PRINT_NAME << "Gps test program start" << endl << endl;
	
	// While we not received a CTRL+C or a kill
	do {
		pEvent = new CEvent();
		
		// Print the latitude
		printf("%sLatitude  : %f DEG %c -> %d DEG %d'%d.%d\" %c\n\n", CGPS_EXAMPLE_PRINT_NAME, pEvent->GetPreciseLatitude(), pEvent->GetLatitudeRef(), pEvent->GetLatitudeDeg(), pEvent->GetLatitudeMin(), pEvent->GetLatitudeSec(), pEvent->GetLatitudeHSec(), pEvent->GetLatitudeRef());
		
		// Print the longitude
		printf("%sLongitude : %f DEG %c -> %d DEG %d'%d.%d\" %c\n\n\n", CGPS_EXAMPLE_PRINT_NAME, pEvent->GetPreciseLongitude(), pEvent->GetLongitudeRef(), pEvent->GetLongitudeDeg(), pEvent->GetLongitudeMin(), pEvent->GetLongitudeSec(), pEvent->GetLongitudeHSec(), pEvent->GetLongitudeRef());
		
		// Sleep during one second
		sleep(1);
		iCpt++;
		
		// Unregistering the Gga callback at the fifteenth second
		if (iCpt == 10) {
			iRet = pGps->UnregisterCallback(EVENT_CGPS_RECEIVEGGADATA, oIdCallbackGga);
			if (iRet < 0)
				cout << CGPS_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the unregistration of Gga data reception callback" << endl << endl;
		}
		// Unregistering all the callbacks at the twentyth second
		else if (iCpt == 20 ) {
			iRet = pGps->UnregisterAllCallbacks(ALL_EVENTS);
			if (iRet < 0)
				cout << CGPS_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the unregistration of all the Gps callback" << endl << endl;
		}
		/*// Shuting down the GPS module
		else if (iCpt == 30 ) {
			iRet = pSoftwareShutdownGpio->Write(0);
			if (iRet < 0)
				cout << CGPS_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the shutting down of the Gps module" << endl << endl;
		}
		// Power on the GPS module
		else if  (iCpt == 45 ) {
			iRet = pSoftwareShutdownGpio->Write(1);
			if (iRet < 0)
				cout << CGPS_EXAMPLE_PRINT_NAME << "Error " << iRet << " during the power on of the Gps module" << endl << endl;
		}*/
		
		delete (pEvent);
	} while (m_bRunning);
	
	// Delete the CGPS object
	delete(pGps);
	// Delete the GPIO object
	//delete(pSoftwareShutdownGpio);
    
    return 0;    
}
