/*
 * app_power.h
 *
 *  Created on: Mar 11, 2020
 *      Author: Nguyet
 */

#ifndef APP_POWER_H_
#define APP_POWER_H_

typedef enum {
	POWER_FINISH_STATE = 0,
	POWER_BEGIN_STATE,
	POWER_CHECK_STATUS_STATE,
	POWER_CHECK_COMMAND_STATE,
	POWER_ERROR_HANDLER_STATE,
	POWER_WARNING_HANDLER_STATE,
	POWER_MAX_NUMBER_OF_STATES
}POWER_FSM_STATE;



#define No_CHANNEL 10

typedef enum {
	SYSTEM = 0,
	NODE1 = 1,
	NODE2 = 2,
	NODE3 = 3,
	NODE4 = 4,
	NODE5 = 5,
	NODE6 = 6,
	NODE7 = 7,
	NODE8 = 8,
	NODE9 = 9,
	NODE10 = 10,
}NodeName;

typedef enum  {
	NODE_NORMAL 		= 	0,
	CHARGING			=	1,
	CHARGEFULL			=	2,
	UNPLUG				=	3,
	NO_POWER			=	4,
	NO_FUSE				=	5,
	NO_RELAY			=	6,
	NODE_OVER_CURRENT	=	7,
	NODE_OVER_MONEY		=	8,
	NODE_OVER_TIME		=	9,
	NODE_READY
}NodeStatus;

//cap dien ma chua cam thi time out
// dang sac het thoi gian thi timeout
// dang sac ma rut thi cung boa UNPLUG
// kiem tra fuse vaf relay

typedef enum  {
	SYSTEM_NORMAL			=	0,
	NODE_OVER_TOTAL			=	1,
	NODE_UNDER_TOTAL		=	2,
	SYSTEM_OVER_CURRENT		=	3,
	SYSTEM_OVER_TEMP		=	4,
	SYSTEM_SHAKING			=	5,
	SYSTEM_NO_INTERNET		=	6
}SystemStatus;



typedef struct Commands {
	uint8_t target;
	uint8_t command;
	uint8_t data;
} Command;

void Node_Update(uint8_t outletID, uint32_t current, uint8_t voltage, uint8_t power_factor, uint8_t time_period);
uint32_t Get_Power_Consumption(uint8_t outletID);
void Set_Limit_Energy(uint8_t outletID, uint32_t limit_energy);
void Power_Setup(void);
void Power_Loop(void);

#endif /* APP_POWER_H_ */
