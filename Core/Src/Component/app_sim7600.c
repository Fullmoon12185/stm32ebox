/**
 *@file app_sim7600.c
 *@author thodo
 */

#include <Component/app_sim7600.h>
#include <Peripheral/app_uart.h>
#include "app_ATProcessing.h"


/*
 * app_sim3g.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_scheduler.h"

#define TIMER_TO_POWER_ON_SIM7600				200			/*!< Time for Simcom7600 POWER_ON is LOW 200 Ticks equal 2000 miliseconds*/
#define TIMER_TO_POWER_ON_SIM7600_TIMEOUT		200			/*!< Time for Simcom7600 wait after POWER_ON is LOW 200 Ticks equal 2000 miliseconds*/

#define TIMER_TO_RESET_SIM7600					200 //50			/*!< Time for Simcom7600 RESET is LOW 200 Ticks equal 2000 miliseconds*/
#define TIMER_TO_RESET_SIM7600_TIMEOUT			400			/*!< Time for after Simcom7600 RESET is LOW 200 Ticks equal 2000 miliseconds*/

#define TIMEOUT_STARTUP_RESET					20000		/*!< Time for Simcom7600 wait Start up 20000 Ticks equal 200000 miliseconds*/

#define TIMEOUT_SETTING_COMMAND					20000		/*!< Time for Simcom7600 wait Start up 20000 Ticks equal 200000 miliseconds*/

#define TIMEOUT_GETTING_DEPOSIT					2000		/*!< Time for Simcom7600 wait Start up 20000 Ticks equal 200000 miliseconds*/
#define TIMEOUT_GETTING_PHONENUMBER					2000		/*!< Time for Simcom7600 wait Start up 20000 Ticks equal 200000 miliseconds*/

#define NO_SETTING_COMMAND						9

uint8_t imei_buffer[IMEI_LENGTH+1];							/*!< Buffer contain imei data*/
uint8_t imei_count = 0;

uint8_t deposit_buffer[DEPOSIT_LENGTH];							/*!< Buffer contain imei data*/
uint8_t phone_number_buffer[DEPOSIT_LENGTH];							/*!< Buffer contain imei data*/


FlagStatus sim7600_timeout_flag = RESET;					/*!< Simcom7600_timeout_flag for time control */
FlagStatus deposit_request_flag = RESET;


FlagStatus is_Sim7600_TimeOutFlag(void);
void Set_Sim7600_Timeout_Flag(void);
void Clear_Sim7600_Timeout_Flag(void);

void Sim7600_State_Display(void);

uint8_t Sim7600_Run(void);

//Private Func
void Power_Signal_Low(void);
void Power_Signal_High(void);
void Reset_Signal_Low(void);
void Reset_Signal_High(void);
FlagStatus Get_Imei();


//State Machine Func

void Sim7600_Power_On(void);
void Sim7600_Wait_For_Power_On_Respone(void);

void Sim7600_Reset(void);
void Sim7600_Wait_For_Reset_Respone(void);

void Sim7600_Start_Up(void);
void Sim7600_Wait_For_Start_Up_Respone(void);

void Sim7600_Setting(void);
void Sim7600_Wait_For_Setting_Respone(void);

void Sim7600_Get_Imei(void);
void Sim7600_Wait_For_Get_Imei_Respone(void);

void Sim7600_Get_Phone_Number(void);
void Sim7600_Wait_For_Get_Phone_Number_Response(void);

void Sim7600_Close_Session(void);
void Sim7600_Wait_For_Close_Session(void);


Sim7600_State prev_sim7600_state;
Sim7600_State sim7600_state = SIM7600_RESET;


uint8_t *setting_command[NO_SETTING_COMMAND]= {
		(uint8_t*)"AT\r\n",
		(uint8_t*)"ATE1\r\n",
		(uint8_t*)"AT+CGDCONT=1,\"IP\",\"v-internet\"\r\n",
		(uint8_t*)"AT+CGSOCKCONT=1,\"IP\",\"cmet\"\r\n",
		(uint8_t*)"AT+CSOCKSETPN=1\r\n",
		(uint8_t*)"AT+CIPMODE=0\r\n",
		(uint8_t*)"AT+NETOPEN\r\n",
		(uint8_t*)"AT+CUSD=1\r\n",
		(uint8_t*)"AT+CSCS=\"GSM\"\r\n",
//		(uint8_t*)"AT+IPADDR\r\n"
};

uint8_t setting_index = 0;

Sim7600_Machine_Type Sim7600_State_Machine[] = {
		{SIM7600_POWER_ON							, 	Sim7600_Power_On						},
		{SIM7600_WAIT_FOR_POWER_ON_RESPONE			, 	Sim7600_Wait_For_Power_On_Respone		},

		{SIM7600_RESET								, 	Sim7600_Reset							},
		{SIM7600_WAIT_FOR_RESET_RESPONE				, 	Sim7600_Wait_For_Reset_Respone			},

		{SIM7600_START_UP							, 	Sim7600_Start_Up						},
		{SIM7600_WAIT_FOR_START_UP_RESPONE			, 	Sim7600_Wait_For_Start_Up_Respone		},

		{SIM7600_SETTING							, 	Sim7600_Setting							},
		{SIM7600_WAIT_FOR_SETTING_RESPONE			, 	Sim7600_Wait_For_Setting_Respone		},

		{SIM7600_GET_IMEI							, 	Sim7600_Get_Imei						},
		{SIM7600_WAIT_FOR_GET_IMEI_RESPONE			, 	Sim7600_Wait_For_Get_Imei_Respone		}
};

/**
 * is_Sim7600_TimeOutFlag()
 * @param None.
 * @retval Flagstatus: SET or RESET
 * @brief This function return sim7600_timeout_flag.
 */
FlagStatus is_Sim7600_TimeOutFlag(){
	return sim7600_timeout_flag;
}


/**
 * Set_Sim7600_Timeout_Flag()
 * @param None.
 * @retval None
 * @brief This function set sim7600_timeout_flag to SET.
 */
void Set_Sim7600_Timeout_Flag(void){
	sim7600_timeout_flag = SET;
}


/**
 * Clear_Sim7600_Timeout_Flag()
 * @param None.
 * @retval None
 * @brief This function set sim7600_timeout_flag to RESET.
 */
void Clear_Sim7600_Timeout_Flag(void){
	sim7600_timeout_flag = RESET;
}


/**
 * Sim7600_State_Display()
 * @brief This is function for display state of State Machine. It only show State when having a state changation.
 */
void Sim7600_State_Display(void){
	if(sim7600_state!=prev_sim7600_state){
		switch(sim7600_state){
		case SIM7600_POWER_ON:
			LOG("\r\nSIM7600 POWER ON\r\n");
			break;
		case SIM7600_WAIT_FOR_POWER_ON_RESPONE:
			LOG((uint8_t*)"\r\nSIM7600 WAIT FOR POWER ON RESPONE\r\n");
			break;
		case SIM7600_RESET:
			LOG((uint8_t*)"\r\nSIM7600 RESET\r\n");
			break;
		case SIM7600_WAIT_FOR_RESET_RESPONE:
			LOG((uint8_t*)"\r\nSIM7600 WAIT FOR RESET RESPONE\r\n");
			break;
		case SIM7600_START_UP:
			LOG((uint8_t*)"\r\nSIM7600 START UP\r\n");
			break;
		case SIM7600_WAIT_FOR_START_UP_RESPONE:
			LOG((uint8_t*)"\r\nSIM7600 WAIT FOR START UP RESPONE\r\n");
			break;
		case SIM7600_SETTING:
			LOG((uint8_t*)"\r\nSIM7600 SETTING\r\n");
			break;
		case SIM7600_WAIT_FOR_SETTING_RESPONE:
			LOG((uint8_t*)"\r\nSIM7600 WAIT FOR SETTING RESPONE\r\n");
			break;
		case SIM7600_GET_IMEI:
			LOG((uint8_t*)"\r\nSIM7600 GET IMEI\r\n");
			break;
		case SIM7600_WAIT_FOR_GET_IMEI_RESPONE:
			LOG((uint8_t*)"\r\nSIM7600 WAIT FOR GET IMEI RESPONE\r\n");
			break;
		case SIM7600_MAX_STATE:
			LOG((uint8_t*)"\r\nSIM7600 MAX STATE\r\n");
			break;
		default:
			break;
		}
	}
}

/**
 * Sim7600_Run()
 * @param None
 * @retval 0 if state < SIM7600_MAX_STATE else return 1
 * @brief This is function can be called from external file. It run follow state machine method. Not have param.
 */
uint8_t Sim7600_Run(void){
	Sim7600_State_Display();
	prev_sim7600_state = sim7600_state;
	if(sim7600_state < SIM7600_MAX_STATE){
		(*Sim7600_State_Machine[sim7600_state].func)();
		return 0;
	} else {
		return 1;
	}
}


/**
 * Power_Signal_Low()
 * @param None
 * @retval None
 * @brief This is function set Power Signal to RESET
 */
void Power_Signal_Low(void){
//	HAL_GPIO_WritePin(SIM7600_4G_PWRON_PORT, SIM7600_4G_PWRON, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SIM7600_4G_PWRON_PORT, SIM7600_4G_PWRON, GPIO_PIN_SET);
}

/**
 * Power_Signal_High()
 * @param None
 * @retval None
 * @brief This is function set Power Signal to SET
 */
void Power_Signal_High(void){
//	HAL_GPIO_WritePin(SIM7600_4G_PWRON_PORT, SIM7600_4G_PWRON, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SIM7600_4G_PWRON_PORT, SIM7600_4G_PWRON, GPIO_PIN_RESET);
}

/**
 * Reset_Signal_Low()
 * @param None
 * @retval None
 * @brief This is function set Reset Signal to RESET
 */
void Reset_Signal_Low(void){
//	HAL_GPIO_WritePin(SIM7600_4G_PERST_PORT, SIM7600_4G_PERST, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SIM7600_4G_PERST_PORT, SIM7600_4G_PERST, GPIO_PIN_SET);
}


/**
 * Reset_Signal_High()
 * @param None
 * @retval None
 * @brief This is function set Reset Signal to SET
 */
void Reset_Signal_High(void){
//	HAL_GPIO_WritePin(SIM7600_4G_PERST_PORT, SIM7600_4G_PERST, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SIM7600_4G_PERST_PORT, SIM7600_4G_PERST, GPIO_PIN_RESET);

}


/**
 * @param None
 * @retval None
 * @brief This is function for initiating Simcom7600 : GPIO,...
 */
void Sim7600_Init(void){
//	Sim7600_GPIO_Init();
}


/**
 * Sim7600_Power_On()
 * @param None
 * @retval None
 * @brief This is function for control POWER_ON signal in Simcom7600 module
 */
void Sim7600_Power_On(void){
	SCH_Add_Task(Power_Signal_Low, 0, 0);
	SCH_Add_Task(Power_Signal_High, TIMER_TO_POWER_ON_SIM7600, 0);
	Clear_Sim7600_Timeout_Flag();
	SCH_Add_Task(Set_Sim7600_Timeout_Flag, TIMER_TO_POWER_ON_SIM7600_TIMEOUT, 0);
	sim7600_state = SIM7600_WAIT_FOR_POWER_ON_RESPONE;
}

/**
 * Sim7600_Wait_For_Power_On_Respone()
 * @param None
 * @retval None
 * @brief This is function for waiting POWER_ON State respone, after timeout TIMER_TO_POWER_ON_SIM7600_TIMEOUT
 */
void Sim7600_Wait_For_Power_On_Respone(void){
	if(is_Sim7600_TimeOutFlag()){
		sim7600_state = SIM7600_START_UP;
	}
}

/**
 * Sim7600_Reset()
 * @param None
 * @retval None
 * @brief This is function for control RESET signal in Simcom7600 module
 * @brief Keep RESET signal LOW during TIMER_TO_RESET_SIM7600
 */
void Sim7600_Reset(void){
	SCH_Add_Task(Reset_Signal_High, 0, 0);
	SCH_Add_Task(Reset_Signal_Low, TIMER_TO_RESET_SIM7600, 0);
	Clear_Sim7600_Timeout_Flag();
	SCH_Add_Task(Set_Sim7600_Timeout_Flag, TIMER_TO_RESET_SIM7600_TIMEOUT, 0);
	sim7600_state = SIM7600_WAIT_FOR_RESET_RESPONE;
}


/**
 * Sim7600_Wait_For_Reset_Respone()
 * @param None
 * @retval None
 * @brief This is function for waiting simcom7600_timeout_flag is SET
 */
void Sim7600_Wait_For_Reset_Respone(void){
	if(is_Sim7600_TimeOutFlag()){
		sim7600_state = SIM7600_POWER_ON;
	}
}

/**
 * Sim7600_Start_Up()
 * @param None
 * @retval None
 * @brief This is function for start up Simcom7600 module. Timeout is TIMEOUT_STARTUP_RESET.
 */
void Sim7600_Start_Up(void){
	sim7600_state = SIM7600_WAIT_FOR_START_UP_RESPONE;
	Clear_Sim7600_Timeout_Flag();
	SCH_Add_Task(Set_Sim7600_Timeout_Flag, TIMEOUT_STARTUP_RESET, 0);
}



/**
 * Sim7600_Wait_For_Start_Up_Respone()
 * @param None
 * @retval None
 * @brief This is function for waiting start up respond.
 * @brief If Timeout so Reset Simcom7600 else AT_Result = AT_PB_DONE switch to SIM7600_GET_IMEI
 */
void Sim7600_Wait_For_Start_Up_Respone(void){
	// Time out if too long time cannot receive PB_DONE
	if(is_Sim7600_TimeOutFlag()){
		sim7600_state = SIM7600_RESET;
	}
//	Wait_For_Respone(AT_PB_DONE);
	switch (Get_AT_Result()){
		case AT_PB_DONE:
			Clear_AT_Result();
			Set_Sim7600_Timeout_Flag();
			sim7600_state = SIM7600_SETTING;//SIM7600_GET_IMEI;
			break;
		default:
			Clear_AT_Result();
			break;
	}
}


/**
 * Sim7600_Get_Imei()
 * @param None
 * @retval None
 * @brief This is function for Get IMEI from Simcomo7600 to set LockerID.
 */
void Sim7600_Setting(void){
	// Clear_Reiceive_Buffer();
	UART_SIM7600_Transmit(setting_command[setting_index]);
	sim7600_state = SIM7600_WAIT_FOR_SETTING_RESPONE;
}


/**
 * Sim7600_Wait_For_Get_Imei_Respone()
 * @param None
 * @retval None
 * @brief This is function for waiting respone from GET IMEI State.
 */
void Sim7600_Wait_For_Setting_Respone(void){
	if(is_Sim7600_TimeOutFlag()){
		switch (Get_AT_Result()){
			case AT_OK:
				Clear_AT_Result();
				if(setting_index >= (NO_SETTING_COMMAND - 1) ){
					setting_index = 0;
					sim7600_state = SIM7600_GET_IMEI;
				}
				else{
					setting_index ++;
					sim7600_state = SIM7600_SETTING;
				}
				if(setting_index == 6){
					Clear_Sim7600_Timeout_Flag();
					SCH_Add_Task(Set_Sim7600_Timeout_Flag, 500, 0);
				}
				break;
			case AT_ERROR:
				Clear_AT_Result();
				sim7600_state = SIM7600_MAX_STATE;
				break;
			default:
				break;
		}
	}
}

/**
 * Sim7600_Get_Imei()
 * @param None
 * @retval None
 * @brief This is function for Get IMEI from Simcomo7600 to set LockerID.
 */
void Sim7600_Get_Imei(void){
	// Clear_Reiceive_Buffer();
	UART_SIM7600_Transmit((uint8_t*)"AT+SIMEI?\r\n");
	sim7600_state = SIM7600_WAIT_FOR_GET_IMEI_RESPONE;
}


/**
 * Sim7600_Wait_For_Get_Imei_Respone()
 * @param None
 * @retval None
 * @brief This is function for waiting respone from GET IMEI State.
 */
void Sim7600_Wait_For_Get_Imei_Respone(void){
//	Wait_For_Respone(AT_IMEI);
	switch (Get_AT_Result()){
		case AT_IMEI:
			if(Get_Imei()){
				Clear_AT_Result();
				sim7600_state = SIM7600_MAX_STATE;
				UART_DEBUG_Transmit_Size(imei_buffer, IMEI_LENGTH);
			}
			break;
		case AT_ERROR:
			Clear_AT_Result();
			sim7600_state = SIM7600_MAX_STATE;
		default:
			break;
	}
}



/**
 * SIM7600_Set_State()
 * @param sim7600_state
 * @retval None
 * @brief This function set mqtt_state.
 */
void SIM7600_Set_State(Sim7600_State newState){
	sim7600_state = newState;
}



/**
 * MQTT_Get_State()
 * @param None
 * @retval sim7600_state
 * @brief This function Set mqtt_state.
 */
Sim7600_State Sim7600_Get_State(void){
	return sim7600_state;
}

/**
 * Sim7600_Get_IMEI()
 * @param None
 * @retval uint8_t*: pointer to IMEI buffer
 * @brief This function Set mqtt_state.
 */
uint8_t *Sim7600_Get_IMEI(){
//	return "862649046029253";//imei_buffer;
	return (uint8_t*)"locker-dev-1";
//	return imei_buffer;
}





FlagStatus Get_Imei(){
	if(imei_count == IMEI_LENGTH){
		imei_count = 0;
		return SET;
	}
	if(UART_SIM7600_Received_Buffer_Available()){
		imei_buffer[imei_count++] = UART_SIM7600_Read_Received_Buffer();
	}
	return RESET;
}

void Reset_SimConfiguration_State(){
	SIM7600_Set_State(SIM7600_RESET);
}




