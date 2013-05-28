//----------------------------------------------------------------------------------------------//
//                                                                                              //
//      FILE		:	ifsttar-synchronization-drv.h	 										//
//      AUTHOR		:	Jonathan AILLET <jonathan.aillet@gmail.com>								//
//      Description	:	Header file of the synchronization driver.								//
//						It defines the used structures, the IOCTLs, ...							//
//																								//
//----------------------------------------------------------------------------------------------//

#ifndef _IFSTTAR_SYNC_DRV_H_
#define _IFSTTAR_SYNC_DRV_H_

// ---- Include Files ------------------------------------------------------------------------- //

#include <linux/ioctl.h>

// Drivers errors header file
#include "../drivers_errors.h"

// ---- Definitions --------------------------------------------------------------------------- //

// Maximum pins number to mux to used a external pin of a timer
#define TIMER_MUX_PINS_NB				3

// Measurements number to have an average quartz error
#define QUARTZ_DRIFT_NUMBER				16

// Measurements number in the preemption time array
#define PREEMPTION_TIME_VALUE_NUMBER	32

// Gps Off time maximum number
#define GPS_MAXIMUM_OFF_TIME			60

// This part is a copy of linux/interrupt.h (used by 'omap_dm_timer_set_capture' function)
#define IRQF_TRIGGER_RISING				0x00000001											// Rising edge iD
#define IRQF_TRIGGER_FALLING			0x00000002											// Falling edge iD
#define IRQF_TRIGGER_EDGE				(IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)		// Both edge iD

// Driver running modes
#define	MODE1 							901			// Gps always On no quartz correction
#define	MODE2 							902 		// Gps always On with quartz correction
#define	MODE3 							903 		// Gps alternates On/Off periods including quartz drift

// Timers Id
#define CAPTURE_TIMER_ID				911			// Pps capture timer iD
#define RELAY_TIMER_ID					912			// Relay timer iD

// ---- Constants and Types ------------------------------------------------------------------- //

// Driver state machine definition
typedef enum
{
	STATE_Initialization,							// STATE 0 : Initialization
	STATE_Wait_for_Gps_Fixed,						// STATE 1 : The Gps is On, et we wait for Gps to be stable (quality > 0) for some seconds (default : 16)
	STATE_Gps_Fixed,								// STATE 2 : The Gps is On and stable
	STATE_Switch_Gps_to_Off,						// STATE 3 : Set the Gps module to Off, start the timer which will replace the Pps
	STATE_Gps_Off									// STATE 4 : The Gps is Off
} SYCHRONIZATION_DRIVER_STATES;

// Structure GpsData
typedef struct _GpsData {
	time_t oTimeTTimeInSeconds;		// Time in seconds since Epoch (the 01/01/1970)
	double dLatitude;				// Latiude
	double dLongitude;				// Longitude
	char cLatitudeRef;				// Latitude reference
	char cLongitudeRef;				// Longitude reference
	int iQuality;					// GPS Quality Indicator : 0 = No GPS, 1 = GPS, 2 = DGPS
	unsigned int uiNbSat;			// Used satellites number
} GpsData;

// Structure GeoDatationData
typedef struct _GeoDatationData {
	GpsData oData;					// Gps Data structure
	unsigned int uiMicroSecond;		// Microsecond
	int iQuartzError;				// Quartz error in µseconds
	int iDriverMode;				// Driver running mode
} GeoDatationData;

// ---- IOCTL Commands ------------------------------------------------------------------------ //

// Magic number used to generate the ioctl command iD
#define SYNC_MAGIC  'S'

// Command used to generate the ioctl command iD
#define SYNC_CMD_GET_OFFICIAL_GD_DATA		0x41
#define SYNC_CMD_GET_DRIVER_MODE			0x42
#define SYNC_CMD_GET_DRIVER_STATE			0x43
#define SYNC_CMD_GET_GPS_STATUS				0x44
#define SYNC_CMD_GET_TIME_SINCE_GPS_OFF		0x45
#define SYNC_CMD_GET_GPS_OFF_TIME			0x46
#define SYNC_CMD_GET_TEMPERATURE			0x47
#define SYNC_CMD_GET_CAPTURE_TIMER_CLOCK	0x48
#define SYNC_CMD_GET_RELAY_TIMER_CLOCK		0x49
#define SYNC_CMD_SET_TEMPORARY_GPS_DATA		0x51
#define SYNC_CMD_SET_DRIVER_MODE			0x52
#define SYNC_CMD_SET_GPS_OFF_TIME			0x56

// Generation of the ioctl command iD using 'linux/ioctl.h'
// Command used to get the current GeoDatationData structure
#define SYNC_IOCTL_GET_OFFICIAL_GD_DATA		_IOWR(SYNC_MAGIC, SYNC_CMD_GET_OFFICIAL_GD_DATA, GeoDatationData)	// arg is a GeoDatationData pointer
// Command used to get the current driver running mode
#define SYNC_IOCTL_GET_DRIVER_MODE			_IO(SYNC_MAGIC,	SYNC_CMD_GET_DRIVER_MODE)							// arg is a integer
// Command used to get the current driver state
#define SYNC_IOCTL_GET_DRIVER_STATE			_IO(SYNC_MAGIC,	SYNC_CMD_GET_DRIVER_STATE)							// arg is a integer
// Command used to get the current Gps status (On/Off)
#define SYNC_IOCTL_GET_GPS_STATUS			_IO(SYNC_MAGIC,	SYNC_CMD_GET_GPS_STATUS)							// arg is a integer
// Command used to get the current time since the Gps module is Off
#define SYNC_IOCTL_GET_TIME_SINCE_GPS_OFF	_IO(SYNC_MAGIC,	SYNC_CMD_GET_TIME_SINCE_GPS_OFF)					// arg is a integer
// Command used to get the current time during which the Gps should be turned off
#define SYNC_IOCTL_GET_GPS_OFF_TIME			_IO(SYNC_MAGIC,	SYNC_CMD_GET_GPS_OFF_TIME)							// arg is a integer
// Command used to get the current temperature
#define SYNC_IOCTL_GET_TEMPERATURE			_IO(SYNC_MAGIC,	SYNC_CMD_GET_TEMPERATURE)							// arg is a integer
// Command used to get the capture timer clock
#define SYNC_IOCTL_GET_CAPTURE_TIMER_CLOCK	_IO(SYNC_MAGIC,	SYNC_CMD_GET_CAPTURE_TIMER_CLOCK)					// arg is a integer
// Command used to get the relay timer clock
#define SYNC_IOCTL_GET_RELAY_TIMER_CLOCK	_IO(SYNC_MAGIC,	SYNC_CMD_GET_RELAY_TIMER_CLOCK)						// arg is a integer
// Command used to set temporary GpsData structure
#define SYNC_IOCTL_SET_TEMPORARY_GPS_DATA	_IOW(SYNC_MAGIC, SYNC_CMD_SET_TEMPORARY_GPS_DATA, GpsData)			// arg is a GpsData pointer
// Command used set the current driver running mode
#define SYNC_IOCTL_SET_DRIVER_MODE			_IO(SYNC_MAGIC,	SYNC_CMD_SET_DRIVER_MODE)							// arg is a integer
// Command used to set the current time during which the Gps should be turned off
#define SYNC_IOCTL_SET_GPS_OFF_TIME			_IO(SYNC_MAGIC,	SYNC_CMD_SET_GPS_OFF_TIME)							// arg is a integer

// ---- Variable Externs ---------------------------------------------------------------------- //

// ---- Function Prototypes ------------------------------------------------------------------- //


#endif /* IFSTTAR_SYNC_DRV_H */
