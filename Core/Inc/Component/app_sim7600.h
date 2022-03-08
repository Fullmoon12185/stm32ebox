/**
 *@file app_sim7600.h
 *@author thodo
 *@brief Header file for app_sim7600.c. This file contains some function to control Simcom7600 like Reset, poweron,Startup, Sim7600_Get_IMEI(),...
 */

#ifndef INC_COMPONENT_APP_SIM7600_H_
#define INC_COMPONENT_APP_SIM7600_H_
#include "main.h"

typedef enum {
	SIM7600_POWER_ON,
	SIM7600_WAIT_FOR_POWER_ON_RESPONE,
	SIM7600_RESET,
	SIM7600_WAIT_FOR_RESET_RESPONE,
	SIM7600_START_UP,
	SIM7600_WAIT_FOR_START_UP_RESPONE,
	SIM7600_SETTING,
	SIM7600_WAIT_FOR_SETTING_RESPONE,
	SIM7600_GET_IMEI,
	SIM7600_WAIT_FOR_GET_IMEI_RESPONE,
	SIM7600_MAX_STATE
}Sim7600_State;

/*
 * @struct Sim7600_Machine_Type
 */
typedef struct {
	Sim7600_State state;	/*!<	state of Simcom7600	*/
	void (*func)(void);		/*!<	function of state	*/
}Sim7600_Machine_Type;



void Sim7600_Init(void);
uint8_t Sim7600_Run(void);
void SIM7600_Set_State(Sim7600_State newState);
Sim7600_State SIM7600_Get_State(void);

uint8_t *Sim7600_Get_IMEI();
uint8_t* Get_Deposit_From_UART();
uint8_t* Get_Phone_Number_From_UART();

void Reset_SimConfiguration_State();

void Sim7600_Request_Deposit();
uint8_t * Simcom7600_Get_Phone_Number();


void Reset_SimConfiguration_State();

void Sim7600_Request_Deposit();


void Simcom7600_Request_Signal_Quality();

uint8_t Simcom7600_Get_Signal_Quality();

#endif /* INC_COMPONENT_APP_SIM7600_H_ */
