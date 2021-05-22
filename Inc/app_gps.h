/*
 * app_gps.h
 *
 *  Created on: May 18, 2021
 *      Author: Nguyet
 */

#ifndef APP_GPS_H_
#define APP_GPS_H_

#include "main.h"
#include <stdint.h>

//##################################################################################################################

typedef struct
{
	uint8_t			UTC_Hour;
	uint8_t			UTC_Min;
	uint8_t			UTC_Sec;
	uint16_t		UTC_MicroSec;

	float				Latitude;
	double			LatitudeDecimal;
	char				NS_Indicator;
	float				Longitude;
	double			LongitudeDecimal;
	char				EW_Indicator;

	uint8_t			PositionFixIndicator;
	uint8_t			SatellitesUsed;
	float				HDOP;
	float				MSL_Altitude;
	char				MSL_Units;
	float				Geoid_Separation;
	char				Geoid_Units;

	uint16_t		AgeofDiffCorr;
	char				DiffRefStationID[4];
	char				CheckSum[2];

}GPGGA_t;

typedef struct
{
	uint8_t		rxBuffer[512];
	uint16_t	rxIndex;
	uint8_t		rxTmp;
	uint32_t	LastTime;

	GPGGA_t		GPGGA;

}GPS_t;

extern GPS_t GPS;
//##################################################################################################################
void	GPS_Init(void);
void FSM_GPS_Process(void);
//##################################################################################################################


#endif /* APP_GPS_H_ */
