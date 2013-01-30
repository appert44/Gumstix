/**
 *	\file	ifsttar-synchronization-fct-test-app.cpp
 *
 *	\brief 	This file is used to define the synchronization driver test program
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.2a
 *
 *	\date 13 August, 2012
 *
 */

#include "../../../SDK/PEGASE_2.h"

#define SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME		"Synchronization driver test > "							/**< \brief Prefixe of each prints of the CGPS Example application */

using namespace std;

// Global variable
bool m_bRunning = false;			/**< \brief Boolean used to stop the program */

// Prints strings
const char * sDriverMode[] = {"Mode 1", "Mode 2", "Mode 3"};
const char * sDriverState[] = {"Initialization", "Wait_Gps_Fixed", "Gps_Fixed", "Switch_to_Gps_Off", "STATE_Gps_Off"};
const char * sGpsStatus[] = {"Off", "On"};

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "\nCTRL + C !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when received a kill */
void Term (int signal)
{
	unused(signal);
	
	cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "\nKill received !!" << endl;
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
	
	// cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "A message of " << iBufferSize << " bytes has been sent by the synchronization driver : " << sBuffer << endl;
	
	if (strcmp (sBuffer, "Pps detected") == 0)
		// Print that a Pps has been detected
		printf ("%sA Pps has been detected, and the launch time of this callback function has taken %06d micro-seconds\n\n", SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME, pEvent->GetMicroSecond());
	else if (strcmp (sBuffer, "Pps simulated") == 0)
		// Print that a Pps has been detected
		printf ("%sA Pps has been simulated, and the launch time of this callback function has taken %06d micro-seconds\n\n", SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME, pEvent->GetMicroSecond());
	else
		cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "WARNING : A Pps has been detected but it is impossible to know if it is a real Pps or if it is a simulated Pps" << endl << endl;
}

/** \brief	Demonstration program of using CGPS */
int main()
{
	// Variables initialization
	int iCpt = 0, iRet = 0;
	int iCurrentDriverMode, iCurrentDriverState, iCurrentGpsStatus, iGpsOffTime = 0;
	int iPreviousDriverMode, iPreviousDriverState, iPreviousGpsStatus;
	
	CGPS * pGps = NULL;
	CEvent * pEvent = NULL;
	
	signal (SIGINT, Stop);				// Redirect <Ctrl+C> to Stop fuction
	signal (SIGTERM, Term);				// Redirect kill to Term fuction
	
	m_bRunning = true;
	
	// Initialization
	cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "Synchronization driver test program initialization" << endl << endl;
	
	// Create a new CGPS object
	pGps = new CGPS();
	
	// Initialize gps
	iRet = pGps->Open();
	if (iRet < 0) {
		cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "Fail during the Gps initialization : error " << iRet << endl << endl;
		return -1;
	}
	
	// Add the launch of onPPS when the PPS is detected
	boost::function<CallBack> oOnCallBackPps = &onReceivePps;
	iRet = pGps->RegisterCallback(EVENT_CGPS_ONPPS, oOnCallBackPps);
	if (iRet < 0) {
		cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "Failure during the registration of the callback for the Pps detection : error " << iRet << endl << endl;
		return -2;
	}
	boost::signals2::connection oIdCallbackPps = pGps->GetLastFctId();
	
	// Start
	cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "Synchronization driver test program start" << endl << endl;
	
	// Configure the Gps Off Time
	iRet = pGps->SetGpsOffTime(1);
	if (iRet < 0)
		return -3;
	
	// While we not received a CTRL+C or a kill
	do {
		pEvent = new CEvent();
		
		iCurrentDriverMode = pGps->GetDriverRunningMode();
		if (iCurrentDriverMode < 0)
			return -4;
		iCurrentDriverState = pGps->GetDriverState();
		if (iCurrentDriverState < 0)
			return -5;
		iCurrentGpsStatus = pGps->GetGpsStatus();
		if (iCurrentGpsStatus < 0)
			return -6;
		iGpsOffTime = pGps->GetTimeSinceGpsOff();
		if (iGpsOffTime < 0)
			return -7;
		
		// Informations prints
		printf ("%sDate : %02d/%02d/%04d %02d:%02d:%02d:%06d\n", SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME, pEvent->GetDay(), pEvent->GetMonth(), pEvent->GetYear(), pEvent->GetHour(), pEvent->GetMinute(), pEvent->GetSecond(), pEvent->GetMicroSecond());
		if (iPreviousDriverMode != iCurrentDriverMode)
			cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "The driver mode has just changed for : " << sDriverMode[iCurrentDriverMode - MODE1] << endl;
		if (iPreviousDriverState != iCurrentDriverState)
			cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "The driver state has just changed for : " << sDriverState[iCurrentDriverState] << endl;
		if (iPreviousGpsStatus != iCurrentGpsStatus)
			cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "The gps module status has just changed for : Gps " << sGpsStatus[iCurrentGpsStatus] << endl;
		cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "Driver in " << sDriverMode[iCurrentDriverMode - MODE1] << ", state : " << sDriverState[iCurrentDriverState] << ", Gps " << sGpsStatus[iCurrentGpsStatus] << endl;
		if (!iCurrentGpsStatus)
			cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "The Gps module is Off from " << iGpsOffTime << " seconds" << endl;
		cout << endl;
		
		// Sleep during one second
		sleep(1);
		iCpt++;
		
		if (iCpt == 30) {
			if (iCurrentDriverMode != MODE3 && (pGps->SetDriverRunningMode(MODE3)) < 0)
				return -8;
			else
				cout << SYNCHRONIZATION_DRIVER_TEST_PRINT_NAME << "Driver mode 3 test in progress ..." << endl << endl;
		}
		
		iPreviousDriverMode = iCurrentDriverMode;
		iPreviousDriverState = iCurrentDriverState;
		iPreviousGpsStatus = iCurrentGpsStatus;
		
		delete (pEvent);
	} while (m_bRunning);
	
	// Delete the CGPS object
	delete(pGps);
    
    return 0;    
}
