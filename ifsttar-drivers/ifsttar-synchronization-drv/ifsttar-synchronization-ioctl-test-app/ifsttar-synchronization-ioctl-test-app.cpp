#include "../../../SDK/common.h"

using namespace std;

/**
 *	\fn		int main()
 *	\brief	Demonstration program of Synchronization driver
 */
int main()
{
	int iFd = -1;
	int iRet = 0;
	unsigned int uiInfo = 0;
	
	struct tm oTime;
	
	GpsData * ToSendGpsData;
	
	GeoDatationData * ReceivedGpsData;
	
	ToSendGpsData = (GpsData *) malloc (sizeof (GpsData));
	ReceivedGpsData = (GeoDatationData *) malloc (sizeof (GeoDatationData));
	
	iFd = open("/dev/synchronization", O_RDWR);
	if (iFd < 0)
		return -1;
	
	oTime.tm_year = 1989-1900;
	oTime.tm_mon = 12-1;
	oTime.tm_mday = 20;
	oTime.tm_hour = 13;
	oTime.tm_min = 28;
	oTime.tm_sec = 42;
	
	ToSendGpsData->oTimeTTimeInSeconds = mktime (&oTime);
		
	ToSendGpsData->dLatitude = 4709.37471;
	ToSendGpsData->dLongitude = 00138.29279;
	ToSendGpsData->cLatitudeRef = 'N';
	ToSendGpsData->cLongitudeRef = 'W';
	ToSendGpsData->iQuality = 1;
	ToSendGpsData->uiNbSat = 5;
	
	sleep(1);
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_OFFICIAL_GD_DATA, ReceivedGpsData);
	if(iRet < 0)
		printf("Erreur ioctl get gps data, mode %d\n", iRet);
	else {
		oTime = *localtime(&ReceivedGpsData->oData.oTimeTTimeInSeconds);
		printf("Date du driver de synchro : %02d/%02d/%04d, %02d:%02d:%02d:%06d\n", oTime.tm_mday, oTime.tm_mon+1, oTime.tm_year+1900, oTime.tm_hour, oTime.tm_min, oTime.tm_sec, ReceivedGpsData->uiMicroSecond);
	}
	
	iRet = ioctl (iFd, SYNC_IOCTL_SET_TEMPORARY_GPS_DATA, ToSendGpsData);
	if(iRet < 0)
		printf("Erreur ioctl set gps data\n");
	else
		printf("New data in synchro driver\n");
	
	sleep(2);
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_OFFICIAL_GD_DATA, ReceivedGpsData);
	if(iRet < 0)
		printf("Erreur ioctl get gps data, mode %d\n", iRet);
	else {
		oTime = *localtime(&ReceivedGpsData->oData.oTimeTTimeInSeconds);
		printf("Date du driver de synchro : %02d/%02d/%04d, %02d:%02d:%02d:%06d\n", oTime.tm_mday, oTime.tm_mon+1, oTime.tm_year+1900, oTime.tm_hour, oTime.tm_min, oTime.tm_sec, ReceivedGpsData->uiMicroSecond);
	}
	
	sleep(2);
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_DRIVER_MODE, &uiInfo);
	if (iRet < 0)
		printf("Erreur ioctl get driver mode\n");
	else
		printf("Driver Mode : %d\n", uiInfo-110);
		
	iRet = ioctl (iFd, SYNC_IOCTL_GET_DRIVER_STATE, &uiInfo);
	if (iRet < 0)
		printf("Erreur ioctl get driver state\n");
	else
		printf("Driver state : %d\n", uiInfo);
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_GPS_STATUS, &uiInfo);
	if (iRet < 0)
		printf("Erreur ioctl get gps status\n");
	else {
		if (uiInfo == 0) {
			printf("Gps Off\n");
			
			iRet = ioctl (iFd, SYNC_IOCTL_GET_TIME_SINCE_GPS_OFF, &uiInfo);
			if(iRet < 0)
				printf("Erreur ioctl get time from Gps Off\n");
			else
				printf("Gps Off since %d seconds\n", uiInfo);
		}
		else if (uiInfo == 1)
			printf("Gps On\n");
		else
			printf("Error, wrong gps status\n");
	}
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_GPS_OFF_TIME, &uiInfo);
	if(iRet < 0)
		printf("Erreur ioctl get Gps Off time\n");
	else
		printf("The Gps module should be shutdown during %d minutes\n", uiInfo);
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_TEMPERATURE, &uiInfo);
	if(iRet < 0)
		printf("Erreur ioctl get temperature\n");
	else
		printf("Temperature : %d\n", uiInfo);
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_CAPTURE_TIMER_CLOCK, &uiInfo);
	if(iRet < 0)
		printf("Erreur ioctl get capture timer clock\n");
	else
		printf("Capture timer clock : %d Hz\n", uiInfo);
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_RELAY_TIMER_CLOCK, &uiInfo);
	if(iRet < 0)
		printf("Erreur ioctl get relay timer clock\n");
	else
		printf("Relay timer clock : %d Hz\n", uiInfo);
	
	iRet = ioctl (iFd, SYNC_IOCTL_SET_GPS_OFF_TIME, 3);
	if(iRet < 0)
		printf("Erreur ioctl set Gps Off time\n");
	else {
		iRet = ioctl (iFd, SYNC_IOCTL_GET_GPS_OFF_TIME, &uiInfo);
		if(iRet < 0)
			printf("Erreur ioctl get Gps Off time\n");
		else
			printf("The Gps module Off time has been changed, now, it should be shutdown during %d minutes\n", uiInfo);
	}
	
	iRet = ioctl (iFd, SYNC_IOCTL_SET_DRIVER_MODE, MODE3);
	if(iRet < 0)
		printf("Erreur ioctl set driver mode\n");
	else {
		iRet = ioctl (iFd, SYNC_IOCTL_GET_DRIVER_MODE, &uiInfo);
		if(iRet < 0)
			printf("Erreur ioctl get driver mode\n");
		else
			printf("Driver Mode : %d\n", uiInfo-110);
	}
	
	sleep(3);
	
	iRet = ioctl (iFd, SYNC_IOCTL_GET_GPS_STATUS, &uiInfo);
	if(iRet < 0)
		printf("Erreur ioctl get gps status\n");
	else {
		if (uiInfo == 0)
			printf("Gps Off\n");
		else if (uiInfo == 1)
			printf("Gps On\n");
		else
			printf("Error, wrong gps status\n");
	}
	
	free(ToSendGpsData);
	free(ReceivedGpsData);
    
    return 0;
}
