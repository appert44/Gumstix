/**
 *	\file	CEvent.h
 *
 *	\brief 	This header file is used to declare the CEvent class and all its methods
 *	
 *	\author Jonathan Aillet
 *
 *	\version v1.0a
 *
 *	\date 28 June, 2012
 *
 */

#ifndef CEVENT_H
#define CEVENT_H

#include "common.h"

#define UTC_TIME	0		/**< \brief UTC time Id (for the 'ConvertTime' function) */
#define LOCAL_TIME	1		/**< \brief Local time Id (for the 'ConvertTime' function) */

/**
 *	\class CEvent
 *
 *	\brief 	The CEvent class save the current time and position informations when the object is created.
 *			It also allow to acesse to this data with some accesors.
 *	
 *	\author Jonathan Aillet
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */
class CEvent 
{
	private :
	/* Attributes */
	/**************/
	// Date attributes
	time_t m_tDate;								/**< \brief Date (time_t). */
	struct tm m_oTime;							/**< \brief Date (struct tm *). */
	GeoDatationData m_oCurrentGeoDatationData;	/**< \brief Date (GeoDatationData). */
	
	// Link to the synchronization driver
	int m_iSynchronizationFd;			/**< \brief File descriptor of the synchronization entry. */
	
	// Timing attributes.
	unsigned int m_uiMicroSecond;		/**< \brief µSecond. */
	unsigned int m_uiSecond;			/**< \brief Second. */
	unsigned int m_uiMinute;			/**< \brief Minute. */
	unsigned int m_uiHour;				/**< \brief Hour (UTC). */
	unsigned int m_uiDay;				/**< \brief Day. */
	unsigned int m_uiMonth;				/**< \brief Month. */
	unsigned int m_uiYear;				/**< \brief Year. */
	
	// Location attributes.
	double m_dLatitude;					/**< \brief Latitude */
	double m_dLongitude;				/**< \brief Longitude */
	char m_cLatitudeRef;				/**< \brief Latitude reference */
	char m_cLongitudeRef;				/**< \brief Longitude reference */
	
	// Number of satellite(s) attribute.
	int m_dNbSat;						/**< \brief Number of satellite(s) */
	
	// Quality attribute.
	int m_iQuality;						/**< \brief Quality */
	
	// Quartz error attribute
	int m_iQuartzError;					/**< \brief Timer Quartz error */
	
	// Synchronization driver run mode
	int m_iDriverMode;					/**< \brief Synchronization driver run mode */
	
	/* Methods  */
	/************/
	// Convert the time and set the time attributes using the time_t variable : m_tDate
	void ConvertTime (bool bTimeZone);
	
	// Get the system time using the time.h library.
	int GetSystemTime();

	public :
	/* Methods  */
	/************/
	
	// Constructor of the CEvent class (with the possibilitie to set the µseconds).
	// TODO: évaluer + tard si intéressant d'avoir un constructeur avec en entrée des uS
	CEvent();	
	// Constructor of the CEvent class using a GeoDatationData structure.
	CEvent(GeoDatationData * GDData);
	//Destructor of the CEvent class.		
	~CEvent();
		
	// Send back the timing informations of the event.
	unsigned int GetYear();				// Send back the Year of the event.
	unsigned int GetMonth();			// Send back the Month of the event.
	unsigned int GetDay();				// Send back the Day of the event.
	unsigned int GetHour();				// Send back the Hour of the event (UTC).
	unsigned int GetMinute();			// Send back the Minute of the event.
	unsigned int GetSecond();			// Send back the Second of the event.
	unsigned int GetMicroSecond();		// Send back the µSecond of the event.
	unsigned int GetTimeInSeconds();	// Send back the time in Second of the event.
	
	// Send back the latitude of the event in several formats.
	double GetLatitude();				// Send back the raw latitude data of the event sent by the GPS.
	double GetPreciseLatitude();		// Send back the latitude data of the event using the format : DD.HHXXXX.
	unsigned int GetLatitudeDeg();		// Send back the degrees of the latitude of the event using the format : DD.
	unsigned int GetLatitudeMin();		// Send back the minutes of the latitude of the event using the format : MM.
	unsigned int GetLatitudeSec(); 		// Send back the seconds of the latitude of the event using the format : SS.
	unsigned int GetLatitudeHSec();		// Send back the hundredths of a seconds of the latitude of the event using the format : HH.
		
	// Send back the latitude reference of the event : N or S (North or South).
	char GetLatitudeRef();

	// Send back the longitude of the event in several formats.
	double GetLongitude();				// Send back the raw longitude data of the event sent by the GPS.
	double GetPreciseLongitude();		// Send back the longitude data of the event using the format : DD.HHXXXX.
	unsigned int GetLongitudeDeg();		// Send back the degrees of the longitude of the event using the format : DD.
	unsigned int GetLongitudeMin();		// Send back the minutes of the longitude of the event using the format : MM.
	unsigned int GetLongitudeSec(); 	// Send back the seconds of the longitude of the event using the format : SS.
	unsigned int GetLongitudeHSec();	// Send back the hundredths of a seconds of the longitude of the event using the format : HH.
	
	// Send back the longitude reference of the event : W or E (West or East).
	char GetLongitudeRef();
	
	// Send back the number of satellites used to obtain the other informations about the event.
	unsigned int GetNbSat();
	
	// Send back the quality of the GPS signal at the moment of the event.
	unsigned int GetQuality();
	
	// Send back the timer quartz error of the synchronization driver.
	int GetQuartzError ();
	
	// Send back the running mode of the synchronization driver.
	int GetSynchronizationDriverMode ();
};

#endif //CEVENT_H
