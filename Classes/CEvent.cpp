/**
 *	\file	CEvent.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CEvent class
 *	
 *	\author Jonathan Aillet
 *
 *	\version v1.0a
 *
 *	\date 28 June, 2012
 *
 */

#include "CEvent.h"

using namespace std;

/**
 *	\fn		CEvent :: CEvent ()
 *	\brief 	Constructor of the CEvent class (with the possibilitie to set the µsecond).
 */
CEvent :: CEvent ()
{
	// Variables initialization
	bool bValidData = false;
	int iRet = 0;
	float fMicroSecond = 0;
	
	// Open the link with the synchronization driver and check if the open happened well
	m_iSynchronizationFd = open("/dev/synchronization", O_RDWR);
	if (m_iSynchronizationFd >= 0)
		bValidData = true;
	
	// If the link is correctly openned
	if (bValidData) {
		// Get the GeoDatation informations form the driver
		iRet = ioctl(m_iSynchronizationFd, SYNC_IOCTL_GET_OFFICIAL_GD_DATA, &m_oCurrentGeoDatationData);
		// If an error occure during the get of the GeoDatation informations
		if (iRet < 0) {
			// Get the GeoDatation informations from the system time (using time.h)
			GetSystemTime();
			cout << CEVENT_PRINT_NAME << "WARNING : An error occurred during the communication with the synchronization driver, the system hour is currently used (the microsecond is incorrect)" << endl << endl;
		}
		else {
			m_iDriverMode = m_oCurrentGeoDatationData.iDriverMode;
			m_iQuartzError = m_oCurrentGeoDatationData.iQuartzError;
			
			if (m_iDriverMode == MODE2) {
				fMicroSecond = m_oCurrentGeoDatationData.uiMicroSecond * (1000000 / (1000000 + m_iQuartzError));
				m_uiMicroSecond = (unsigned int) round(fMicroSecond);
			}
			else	
				m_uiMicroSecond = m_oCurrentGeoDatationData.uiMicroSecond;		// µsecond
			
			// Get and convert time
			m_tDate = m_oCurrentGeoDatationData.oData.oTimeTTimeInSeconds;	// Time in seconds since the 01/01/1970
			ConvertTime (UTC_TIME);
	
			// Init location data
			m_dLatitude = m_oCurrentGeoDatationData.oData.dLatitude;			// latitude
			m_dLongitude = m_oCurrentGeoDatationData.oData.dLongitude;			// latitude reference
			m_cLatitudeRef = m_oCurrentGeoDatationData.oData.cLatitudeRef;		// longitude
			m_cLongitudeRef = m_oCurrentGeoDatationData.oData.cLongitudeRef;	// longitude reference
	
			// Init quality information
			m_dNbSat = m_oCurrentGeoDatationData.oData.uiNbSat;				// satellite number			
			m_iQuality = m_oCurrentGeoDatationData.oData.iQuality;			// quality
		}
	}
	else {
		// Get the GeoDatation informations from the system time (using time.h)
		GetSystemTime();
		cout << CEVENT_PRINT_NAME << "WARNING : An error occurred during the opening of the synchronization driver link, the system hour is currently used (the microsecond is incorrect)" << endl << endl;
	}
	
	// cout << CEVENT_PRINT_NAME << "CEvent class object has been created" << endl;
	// printf("%sCEvent date : %02d/%02d/%04d, %02d:%02d:%02d:%06d\n", CEVENT_PRINT_NAME, m_uiDay, m_uiMonth, m_uiYear, m_uiHour, m_uiMinute, m_uiSecond, m_uiMicroSecond);
	// cout << endl;
}

/**
 *	\fn		CEvent :: CEvent (GeoDatationData * GDData)
 *	\brief 	Constructor of the CEvent class using a GeoDatationData structure.
 */
CEvent :: CEvent (GeoDatationData * GDData)
{
	// Variable initialization
	float fMicroSecond = 0;
	
	memcpy (&m_oCurrentGeoDatationData, GDData, sizeof(GeoDatationData));
	
	m_iDriverMode = m_oCurrentGeoDatationData.iDriverMode;
	m_iQuartzError = m_oCurrentGeoDatationData.iQuartzError;
	
	if (m_iDriverMode == 2) {
		fMicroSecond = m_oCurrentGeoDatationData.uiMicroSecond * (1000000 / (1000000 + m_iQuartzError));
		m_uiMicroSecond = (unsigned int) round(fMicroSecond);
	}
	else	
		m_uiMicroSecond = m_oCurrentGeoDatationData.uiMicroSecond;		// µsecond
	
	// Get and convert time
	m_tDate = m_oCurrentGeoDatationData.oData.oTimeTTimeInSeconds;	// Time in seconds since the 01/01/1970
	ConvertTime (UTC_TIME);

	// Init location data
	m_dLatitude = m_oCurrentGeoDatationData.oData.dLatitude;			// latitude
	m_dLongitude = m_oCurrentGeoDatationData.oData.dLongitude;			// latitude reference
	m_cLatitudeRef = m_oCurrentGeoDatationData.oData.cLatitudeRef;		// longitude
	m_cLongitudeRef = m_oCurrentGeoDatationData.oData.cLongitudeRef;	// longitude reference

	// Init quality information
	m_dNbSat = m_oCurrentGeoDatationData.oData.uiNbSat;				// satellite number			
	m_iQuality = m_oCurrentGeoDatationData.oData.iQuality;			// quality
	
	// cout << CEVENT_PRINT_NAME << "CEvent class object has been created" << endl;
	// printf("%sCEvent date : %02d/%02d/%04d, %02d:%02d:%02d:%06d\n", CEVENT_PRINT_NAME, m_uiDay, m_uiMonth, m_uiYear, m_uiHour, m_uiMinute, m_uiSecond, m_uiMicroSecond);
	// cout << endl;
}

/**
 *	\fn		CEvent :: ~CEvent()
 *	\brief 	Destructor of the CEvent class.
 */		
CEvent :: ~CEvent()
{
	// Variable initialization
	int iRet;
	
	if (m_iSynchronizationFd >= 0) {
		iRet = close(m_iSynchronizationFd);
		if (iRet < 0)
			cout << CEVENT_PRINT_NAME << "WARNING : An error occurred during the closing of the serial port link : " << iRet << endl << endl;
	}
		
	// cout << CEVENT_PRINT_NAME << "CEvent class object has been destroyed" << endl << endl;
}

/**
 *	\fn		void CEvent :: ConvertTime (bool bTimeZone)
 *	\brief 	Convert the time and set the time attributes using the time_t variable : m_tDate
 */		
void CEvent :: ConvertTime (bool bTimeZone)
{
	if (bTimeZone == UTC_TIME)
		m_oTime = *gmtime(&m_tDate);
	else
		m_oTime = *localtime(&m_tDate);

	// Init timing data (with the current time)
	m_uiSecond = m_oTime.tm_sec;
	m_uiMinute = m_oTime.tm_min;
	m_uiHour = m_oTime.tm_hour;
	m_uiDay = m_oTime.tm_mday;
	m_uiMonth = m_oTime.tm_mon+1;
	m_uiYear = m_oTime.tm_year+1900;
}

/**
 *	\fn		int CEvent :: GetSystemTime()
 *	\brief 	Get the system time using the time.h library.
 */		
int CEvent :: GetSystemTime()
{
	// Gets the current time
	time (&m_tDate);
	
	ConvertTime (LOCAL_TIME);
	
	m_uiMicroSecond = 0;
	
	// IFSTTAR Location
	m_dLatitude = 04709.37501;
	m_dLongitude = 00138.29299;
	m_cLatitudeRef = 'N';
	m_cLongitudeRef = 'W';
	
	// Init quality information (which are currently unavailable)
	m_dNbSat = 0;
	m_iQuality = 0;
	
	return 0;
}

/*
 *	Accesor for the decoded informations.
 *	Year, month, day, hour, minute, second, number of satellite, quality, ...
 */

/**
 *	\fn 	unsigned int CEvent :: GetYear ()
 *	\brief 	Send back the Year of the event.
 *
 *	\return	The year of the event.
 */
unsigned int CEvent :: GetYear ()
{
	return m_uiYear;
}

/**
 *	\fn 	unsigned int CEvent :: GetMonth ()
 *	\brief 	Send back the Month of the event.
 *
 *	\return The Month of the event.
 */
unsigned int CEvent :: GetMonth ()
{
	return m_uiMonth;
}


/**
 *	\fn		unsigned int CEvent :: GetDay ()
 *	\brief	Send back the Day of the event.
 *
 *	\return	The Day of the event.
 */
unsigned int CEvent :: GetDay ()
{
	return m_uiDay;
}


/**
 *	\fn		unsigned int CEvent :: GetHour ()
 *	\brief	Send back the Hour of the event (UTC).
 *
 *	\return The Hour of the event (UTC).
 */
unsigned int CEvent :: GetHour ()
{
	return m_uiHour;
}


/**
 *	\fn		unsigned int CEvent :: GetMinute ()
 *	\brief	Send back the Minute of the event.
 *
 *	\return The Minute of the event.
 */
unsigned int CEvent :: GetMinute ()
{
	return m_uiMinute;
}


/**
 *	\fn		unsigned int CEvent :: GetSecond ()
 *	\brief	Send back the Second of the event.
 *
 *	\return	The second of the event.
 */
unsigned int CEvent :: GetSecond ()
{
	return m_uiSecond;
}


/**
 *	\fn		unsigned int CEvent :: GetMicroSecond ()
 *	\brief	Send back the µSecond of the event.
 *
 *	\return	The µSecond of the event.
 */
unsigned int CEvent :: GetMicroSecond ()
{
	return m_uiMicroSecond;
}


/**
 *	\fn		unsigned int CEvent :: GetTimeInSeconds()
 *	\brief	Send back the time in Second of the event.
 *
 *  Time in second is : 3600 X Hour + 60 X Minute + Second.
 *  This method is used to have the time of the event in seconds (UTC).
 *
 *	\return The time in seconds of the event.
 */
unsigned int CEvent :: GetTimeInSeconds()
{
	return m_uiHour*3600+m_uiMinute*60+m_uiSecond;
}


/**
 *	\fn		double CEvent :: GetLatitude ()
 *	\brief 	Send back the raw latitude data of the event sent by the GPS.
 *
 *	\return The raw latitude data of the event.
 */
double CEvent :: GetLatitude ()
{
	return m_dLatitude;
}

/**
 *	\fn		double CEvent :: GetPreciseLatitude ()
 *	\brief	Send back the latitude data of the event using the format : DD.HHXXXX.
 *	
 *	With :	
 *			- DD : degrees.
 *			- HH : hundredths of degrees (0-99).
 *			- XXXX : rest of the decimale part of the degrees (0-9999).
 *
 *	This is the format most currently used.
 *
 *	\return The latitude data of the event using the format : DD.HHXXXX.
 */
double CEvent :: GetPreciseLatitude ()
{
	double 	DecLatitude = 0.0;
	double	ResteLatitude = 0.0;
	int 	EntierLatitude = 0;
		
	EntierLatitude = (int)floor(m_dLatitude / 100.0 );
	ResteLatitude =  (m_dLatitude - (EntierLatitude * 100));
	DecLatitude = (double) ((double) EntierLatitude + (ResteLatitude / 60.0));
	return DecLatitude;
}

/**
 *	\fn		unsigned int CEvent :: GetLatitudeDeg ()
 *	\brief	Send back the degrees of the latitude of the event using the format : DD.
 *
 *	With : 	
 *			- DD : degrees
 *
 *	\return	The degrees of the latitude of the event using the format : DD.
 */
unsigned int CEvent :: GetLatitudeDeg ()
{
	return (int) floor(m_dLatitude / 100.0);
}

/**
 *	\fn		unsigned int CEvent :: GetLatitudeMin ()
 *	\brief	Send back the minutes of the latitude of the event using the format : MM.
 *
 *	With :	
 *			- MM : minutes (between 0 and 59).
 *
 *	\return	The minutes of the latitude of the event using the format : MM.
 */
unsigned int CEvent :: GetLatitudeMin ()
{
	return (int) floor(((m_dLatitude/100) - GetLatitudeDeg ())*100);
}

/**
 *	\fn		unsigned int CEvent :: GetLatitudeSec ()
 *	\brief	Send back the seconds of the latitude of the event using the format : SS.
 *
 *	With :	
 *			- SS : seconds (between 0 and 59).
 *
 *	\return	The seconds of the latitude of the event using the format : SS.
 */
unsigned int CEvent :: GetLatitudeSec ()
{
	double dDecMinutes = 0, dSecondes = 0;
	
	dDecMinutes = (m_dLatitude - floor(m_dLatitude))*100;	
	dSecondes = dDecMinutes/100*60;
	
	return floor(dSecondes);
}

/**
 *	\fn		unsigned int CEvent :: GetLatitudeHSec ()
 *	\brief	Send back the hundredths of a seconds of the latitude of the event using the format : HH.
 *
 *	With :	
 *			- HH : hundredths of a seconds (between 0 and 99).
 *
 *	\return	The hundredths of a seconds of the latitude of the event using the format : HH.
 */
unsigned int CEvent :: GetLatitudeHSec ()
{
	double dDecMinutes = 0, dSecondes = 0;
	
	dDecMinutes = (m_dLatitude - floor(m_dLatitude))*100;	
	dSecondes = dDecMinutes/100*60;
	
	return (dSecondes - floor(dSecondes))*100;	
}

/**
 *	\fn		char CEvent :: GetLatitudeRef ()
 *	\brief	Send back the latitude reference of the event : N or S (North or South).
 *
 *	\return	The latitude reference of the event.
 */
char CEvent :: GetLatitudeRef ()
{
	return m_cLatitudeRef;
}

/**
 *	\fn		double CEvent :: GetLongitude ()
 *	\brief	Send back the raw longitude data of the event sent by the GPS.
 *
 *	\return	The raw longitude data of the event sent by the GPS.
 */
double CEvent :: GetLongitude ()
{
	return m_dLongitude;
}

/**
 *	\fn		double CEvent :: GetPreciseLongitude ()
 *	\brief	Send back the longitude data of the event using the format : DD.HHXXXX.
 *
 *	With :	
 *			- DD : degrees.
 *			- HH : hundredths of degrees (0-99).
 *			- XXXX : rest of the decimale part of the degrees (0-9999).
 *
 *	This is the format most currently used.
 *
 *	\return The longitude data of the event using the format : DD.HHXXXX.
 */
double CEvent :: GetPreciseLongitude ()
{
	double 	DecLongitude = 0.0;
	double	ResteLongitude = 0.0;
	int 	EntierLongitude = 0;
		
	EntierLongitude = (int)floor(m_dLongitude / 100.0 );
	ResteLongitude =  (m_dLongitude - (EntierLongitude * 100));
	DecLongitude = (double) ((double) EntierLongitude + (ResteLongitude / 60.0));
	return DecLongitude;
}

/**
 *	\fn		unsigned int CEvent :: GetLongitudeDeg ()
 *	\brief	Send back the degrees of the longitude of the event using the format : DD.
 *
 * 	With :
 *			- DD : degrees.
 *
 *	\return	The degrees of the longitude of the event using the format : DD.
 */
unsigned int CEvent :: GetLongitudeDeg ()
{
	return (int) floor(m_dLongitude / 100.0);
}


/**
 *	\fn		unsigned int CEvent :: GetLongitudeMin ()
 *	\brief	Send back the minutes of the longitude of the event using the format : MM.
 *
 *	With :
 *			- MM : minutes (between 0 and 59).
 *
 *	\return	The minutes of the longitude of the event using the format : MM.
 */
unsigned int CEvent :: GetLongitudeMin ()
{
	return (int) floor(((m_dLongitude/100) - GetLongitudeDeg ())*100);
}

/**
 *	\fn		unsigned int CEvent :: GetLongitudeSec ()
 *	\brief	Send back the seconds of the longitude of the event using the format : SS.
 *
 *	With :
 *			- SS : seconds (between 0 and 59).
 *
 *	\return	The seconds of the longitude of the event using the format : SS.
 */
unsigned int CEvent :: GetLongitudeSec ()
{
	double dDecMinutes = 0, dSecondes = 0;
	
	dDecMinutes = (m_dLongitude - floor(m_dLongitude))*100;	
	dSecondes = dDecMinutes/100*60;
	
	return floor(dSecondes);
}

/**
 *	\fn		unsigned int CEvent :: GetLongitudeHSec ()
 *	\brief  Send back the hundredths of a seconds of the longitude of the event using the format : HH.
 *
 *	With :
 *			- HH : hundredths of a seconds (between 0 and 99).
 *
 *	\return	The hundredths of a seconds of the longitude of the event using the format : HH.
 */
unsigned int CEvent :: GetLongitudeHSec ()
{
	double dDecMinutes = 0, dSecondes = 0;
	
	dDecMinutes = (m_dLongitude - floor(m_dLongitude))*100;	
	dSecondes = dDecMinutes/100*60;
	
	return (dSecondes - floor(dSecondes))*100;	
}

/**
 *	\fn		char CEvent :: GetLongitudeRef ()
 *	\brief	Send back the longitude reference of the event : W or E (West or East).
 *
 *	\return	The longitude reference of the event : W or E (West or East).
 */
char CEvent :: GetLongitudeRef ()
{
	return m_cLongitudeRef;
}

/**
 *	\fn		unsigned int CEvent :: GetNbSat()
 *	\brief	Send back the number of satellites used to obtain the other informations about the event.
 *
 *	\return The number of satellites used to obtain the other informations about the event
 */
unsigned int CEvent :: GetNbSat()
{
	return m_dNbSat;
}

/**
 *	\fn		unsigned int CEvent :: GetQuality ()
 *	\brief	Send back the quality of the GPS signal at the moment of the event.
 *
 *	\return The quality of the GPS signal at the moment of the event.
 */
unsigned int CEvent :: GetQuality ()
{
	return m_iQuality;
}

/**
 *	\fn		int CEvent :: GetQuartzError ()
 *	\brief	Send back the timer quartz error of the synchronization driver.
 *
 *	\return The timer quartz error of the synchronization driver.
 */
int CEvent :: GetQuartzError ()
{
	return m_iQuartzError;
}

/**
 *	\fn		int CEvent :: GetSynchronizationDriverMode ()
 *	\brief	Send back the running mode of the synchronization driver.
 *
 *	\return The running mode of the synchronization driver.
 */
int CEvent :: GetSynchronizationDriverMode ()
{
	return m_iDriverMode;
}
