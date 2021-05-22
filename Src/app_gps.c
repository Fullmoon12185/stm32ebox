/*
 * app_gps.c
 *
 *  Created on: May 18, 2021
 *      Author: Nguyet
 */

#include "main.h"
#include "app_uart.h"
#include "app_gps.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum{
	CHECK_DATA_AVAILABLE_STATE = 0,
	DETECT_SPECIAL_CHARACTER,
	PREPARE_PROCESSING_GPS_DATA,
	MAX_NUMBER_STATE_OF_PROCESSING_DATA_FROM_GPS
}PROCESS_DATA_RECEIVED_FROM_GPS;

PROCESS_DATA_RECEIVED_FROM_GPS processGpsDataState = CHECK_DATA_AVAILABLE_STATE;


uint16_t GPSDataProcessingBufferIndex = 0;

GPS_t GPS;

void	GPS_Process(void);

//##################################################################################################################
double convertDegMinToDecDeg (float degMin)
{
  double min = 0.0;
  double decDeg = 0.0;

  //get the minutes, fmod() requires double
  min = fmod((double)degMin, 100.0);

  //rebuild coordinates in decimal degrees
  degMin = (int) ( degMin / 100 );
  decDeg = degMin + ( min / 60 );

  return decDeg;
}
//##################################################################################################################
void	GPS_Init(void)
{
	GPS.rxIndex=0;
}

void FSM_GPS_Process(void){
	static uint8_t readGpsCharacter = 0;
	static uint8_t preReadGpsCharacter = 0;
	switch(processGpsDataState){
		case CHECK_DATA_AVAILABLE_STATE:
			if(Uart1_Received_Buffer_Available()){
				memset(GPS.rxBuffer,0,sizeof(GPS.rxBuffer));
				processGpsDataState = DETECT_SPECIAL_CHARACTER;
			}
			break;
		case DETECT_SPECIAL_CHARACTER:
			if(Uart1_Received_Buffer_Available()){
				readGpsCharacter = Uart1_Read_Received_Buffer();
				if(preReadGpsCharacter == '\r' && readGpsCharacter == '\n'){
					processGpsDataState = PREPARE_PROCESSING_GPS_DATA;
				}
				else {
					GPS.rxBuffer[GPS.rxIndex] = readGpsCharacter;
					GPS.rxIndex = GPS.rxIndex + 1;
				}
			}

			break;
		case PREPARE_PROCESSING_GPS_DATA:
			GPS_Process();
			processGpsDataState = CHECK_DATA_AVAILABLE_STATE;
			break;
		default:
			break;
}

//##################################################################################################################
void	GPS_Process(void)
{
	char	*str;
	#if (_GPS_DEBUG==1)
	printf("%s",GPS.rxBuffer);
	#endif
	str=strstr((char*)GPS.rxBuffer,"$GPGGA,");
	if(str!=NULL)
	{
		memset(&GPS.GPGGA,0,sizeof(GPS.GPGGA));
		sscanf(str,"$GPGGA,%2hhd%2hhd%2hhd.%3hd,%f,%c,%f,%c,%hhd,%hhd,%f,%f,%c,%hd,%s,*%2s\r\n",&GPS.GPGGA.UTC_Hour,&GPS.GPGGA.UTC_Min,&GPS.GPGGA.UTC_Sec,&GPS.GPGGA.UTC_MicroSec,&GPS.GPGGA.Latitude,&GPS.GPGGA.NS_Indicator,&GPS.GPGGA.Longitude,&GPS.GPGGA.EW_Indicator,&GPS.GPGGA.PositionFixIndicator,&GPS.GPGGA.SatellitesUsed,&GPS.GPGGA.HDOP,&GPS.GPGGA.MSL_Altitude,&GPS.GPGGA.MSL_Units,&GPS.GPGGA.AgeofDiffCorr,GPS.GPGGA.DiffRefStationID,GPS.GPGGA.CheckSum);
		if(GPS.GPGGA.NS_Indicator==0)
			GPS.GPGGA.NS_Indicator='-';
		if(GPS.GPGGA.EW_Indicator==0)
			GPS.GPGGA.EW_Indicator='-';
		if(GPS.GPGGA.Geoid_Units==0)
			GPS.GPGGA.Geoid_Units='-';
		if(GPS.GPGGA.MSL_Units==0)
			GPS.GPGGA.MSL_Units='-';
		GPS.GPGGA.LatitudeDecimal=convertDegMinToDecDeg(GPS.GPGGA.Latitude);
		GPS.GPGGA.LongitudeDecimal=convertDegMinToDecDeg(GPS.GPGGA.Longitude);
	}
	memset(GPS.rxBuffer,0,sizeof(GPS.rxBuffer));
	GPS.rxIndex=0;
}


