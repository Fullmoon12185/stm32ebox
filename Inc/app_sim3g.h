/*
 * app_sim3g.h
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */


#ifndef APP_SIM3G_H_
#define APP_SIM3G_H_



typedef enum {
	POWER_ON_SIM3G = 0,
	WAIT_FOR_SIM3G_POWER_ON,
	POWER_OFF_SIM3G,
	WAIT_FOR_SIM3G_POWER_OFF,

	RESET_SIM3G,
	WAIT_FOR_SIM3G_RESET,
	SIM3G_START_UP,
	WAIT_FOR_SIM3G_STARTUP_RESPONSE,

	SIM3G_SETTING,
	WAIT_FOR_SIM3G_SETTING_RESPONSE,

	MAX_SIM3G_NUMBER_STATES
}SIM3G_STATE;


typedef struct {
	SIM3G_STATE state;
	void (*func)(void);
}Sim3g_Machine_Type;


typedef struct {
	uint8_t * ATCommand;
	uint8_t * expectedReturn;
}AT_COMMAND_ARRAY;



void TestSendATcommand(void);



void Sim3g_Init(void);
uint8_t Sim3g_Run(void);
uint8_t isSim3g_Run_Finished(void);
void Set_Sim3G_State(SIM3G_STATE newState);
SIM3G_STATE Get_Sim3G_State(void);



void Sim3g_Disable(void);
void Sim3g_Enable(void);
void Sim3g_WakeUp(void);
void Power_Signal_Low(void);
void Power_Signal_High(void);
void Reset_Signal_Low(void);
void Reset_Signal_High(void);
void Sim3g_Clear_Timeout_Flag(void);
void Sim3g_Command_Timeout(void);
uint8_t is_Sim3g_Command_Timeout(void);



FlagStatus isReceivedDataFromServer(uint8_t message_type, uint8_t len_of_message);
void Processing_Received_Data(uint8_t * sub_topic, uint8_t boxID);
FlagStatus isReceivedData(const uint8_t * str);

void FSM_Process_Data_Received_From_Sim3g(void);
FlagStatus isGreaterThanSymbol(void);
FlagStatus isRecvFrom(void);
FlagStatus isIPClose(void);
FlagStatus isSendOK(void);


void Set_Is_Receive_Data_From_Server(FlagStatus status);
FlagStatus Get_Is_Receive_Data_From_Server(void);

#endif /* APP_SIM3G_H_ */


