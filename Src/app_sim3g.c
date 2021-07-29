/*
 * app_sim3g.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_uart.h"
#include "app_sim3g.h"
#include "app_scheduler.h"
#include "app_relay.h"
#include "app_string.h"
#include "app_power.h"
#include "app_pcf8574.h"

#define DEBUG_SIM3G(X)    						X

#define DATA_TO_SEND_LENGTH						20
#define TIME_OUT_FOR_STOP_CHARGING	            120 //1 unit corresponding 30s


#define TIMER_TO_POWER_ON_SIM3G					(300/INTERRUPT_TIMER_PERIOD)
#define TIMER_TO_POWER_OFF_SIM3G				(3000/INTERRUPT_TIMER_PERIOD)
#define TIMER_TO_POWER_ON_SIM3G_TIMEOUT			(2000/INTERRUPT_TIMER_PERIOD)
#define TIMER_TO_POWER_OFF_SIM3G_TIMEOUT		(10000/INTERRUPT_TIMER_PERIOD)


#define TIMER_TO_RESET_SIM3G					(300/INTERRUPT_TIMER_PERIOD)
#define TIMER_TO_RESET_SIM3G_TIMEOUT			(2000/INTERRUPT_TIMER_PERIOD)
#define COMMAND_TIME_OUT						(20000/INTERRUPT_TIMER_PERIOD)
#define SETTING_TIME_OUT						(2000/INTERRUPT_TIMER_PERIOD)

#define	WAIT_FOR_NETWORK_ESTABLISMENT_TIME_OUT	(30000/INTERRUPT_TIMER_PERIOD) //10s

#define MAX_RETRY_NUMBER						3

#define		COMMAND_ONLY						'1'
#define		START_UPDATE_TOTAL_POWER_CONSUMPTION	'*'
#define		END_UPDATE_TOTAL_POWER_CONSUMPTION		'#'

#define		COMMAND_WITH_MAX_ENERGY				'3'

extern uint8_t SUBSCRIBE_TOPIC_1[MAX_TOPIC_LENGTH]; //CEbox_xxxx
extern uint8_t SUBSCRIBE_TOPIC_2[MAX_TOPIC_LENGTH];
extern uint8_t SUBSCRIBE_TOPIC_3[MAX_TOPIC_LENGTH]; //REbox_xxxx


//const uint8_t AT[] = "AT\r";
//const uint8_t AT_CGDCONT[] = "AT+CGDCONT=1,\"IP\",\"v-internet\"\r";
////Define PDP context
//const uint8_t AT_CGSOCKCONT[] = "AT+CGSOCKCONT=1,\"IP\",\"cmet\"\r";
////Set authentication parameters
//const uint8_t AT_CGSOCKAUTH[] = "AT+CGSOCKAUTH=1,1,\"teST\",\"1234\"\r";
////Set the number of active PDP context
//const uint8_t AT_CGSOCKSETPN[] = "AT+CSOCKSETPN=1\r";
//const uint8_t AT_CIPMODE[] = "AT+CIPMODE=0\r";
////PDP context activation
//const uint8_t AT_NETOPEN[] = "AT+NETOPEN=,,1\r";
////Get SIM ip address
//const uint8_t AT_IPADDR [] = "AT+IPADDR\r";
////PDP context deactivatioin
//const uint8_t AT_NETCLOSE[] = "AT+NETCLOSE\r";
////Option of receiving data's length
//const uint8_t AT_CIPHEAD[] = "AT+CIPHEAD=1\r";
////Option of receiving data's length
//const uint8_t AT_CIPSRIP[] = "AT+CIPSRIP=1\r";
////when we use tcp/ip we need to adjust number of retransmission according to network status or
////you want to know your data is received by remote end
//const uint8_t AT_CIPCCFG [] = "AT+CIPCCFG=3,500,1,1\r";


__IO ITStatus isGreaterThanSymbolReceived = RESET;

const uint8_t SMS_DONE[] = "SMS DONE";
const uint8_t PB_DONE[] = "PB DONE\r";
const uint8_t STIN25[] = "+STIN: 25\r";

const uint8_t OK[] = "OK\r";
const uint8_t CONNECT_OK[] = "CONNECT OK\r";
const uint8_t ERROR_1[] = "ERROR\r";
const uint8_t GREATER_THAN_SYMBOL[] = ">";
const uint8_t NETWORK_OPENED[] = "Network opened";
const uint8_t Send_ok[] = "Send ok\r";
const uint8_t RECV_FROM[] = "RECV FROM";
const uint8_t IP_CLOSE[] = "+IPCLOSE";
const uint8_t ISCIPSEND[]= "+CIPSEND";
const uint8_t SIM_NOT_INSERT[] = "+CME ERROR: SIM not inserted\r";

FlagStatus isPBDoneFlag = RESET;
FlagStatus isStin25 = RESET;
FlagStatus isOKFlag = RESET;
FlagStatus isErrorFlag = RESET;
FlagStatus isIPCloseFlag = RESET;
FlagStatus isRecvFromFlag = RESET;
FlagStatus isReadyToSendDataToServer = RESET;
FlagStatus isSendOKFlag = RESET;
FlagStatus isReceiveDataFromServer = RESET;
FlagStatus isCipsend = RESET;


static uint8_t sim3g_TimeoutFlag = 0;
static uint8_t sim3g_Setting_TimeoutFlag = 0;
static uint32_t sim3g_Timeout_Task_Index = NO_TASK_ID;
static uint32_t sim3g_Setting_Timeout_Task_Index = NO_TASK_ID;
static uint8_t sim3g_Retry_Counter = 0;


uint8_t Sim3gDataProcessingBuffer[RXBUFFERSIZE];
uint8_t sim3gDataProcessingBufferIndex = 0;

uint16_t counterForResetChargingAfterDisconnection = 0;

typedef enum{
	CHECK_DATA_AVAILABLE_STATE = 0,
	DETECT_SPECIAL_CHARACTER,
	PREPARE_FOR_SENDING_DATA,
	PREPARE_PROCESSING_RECEIVED_DATA,
	PROCESSING_RECEIVED_DATA,
	PROCESSING_RETAINED_DATA,
	PROCESSING_UPDATE_TOTAL_POWER_CONSUMPTION,
	MAX_NUMBER_STATE_OF_PROCESSING_DATA_FROM_SIM3G
}PROCESS_DATA_RECEIVED_FROM_SIM3G;

PROCESS_DATA_RECEIVED_FROM_SIM3G processDataState = CHECK_DATA_AVAILABLE_STATE;


void Clear_All_Flags(void);
FlagStatus isReceivedData(const uint8_t * str);
void Processing_Received_Data(uint8_t * sub_topic, uint16_t boxID);
FlagStatus isReceivedDataFromServer(uint8_t message_type, uint8_t len_of_message);


void Sim3g_GPIO_Init(void);
void Sim3g_State_Display(void);

//void Sim3g_Disable(void);
//void Sim3g_Enable(void);
void Sim3g_WakeUp(void);
void Power_Signal_Low(void);
void Power_Signal_High(void);
void Reset_Signal_Low(void);
void Reset_Signal_High(void);
void Sim3g_Clear_Timeout_Flag(void);
void Sim3g_Command_Timeout(void);
uint8_t is_Sim3g_Command_Timeout(void);
void Sim3g_Setting_Clear_Timeout_Flag(void);
void Sim3g_Setting_Timeout(void);
uint8_t is_Sim3g_Setting_Timeout(void);

void Setting_Up_Timeout(void);


void SM_Power_On_Sim3g(void);
void SM_Wait_For_Sim3g_Power_On(void);
void SM_Reset_Sim3g(void);
void SM_Wait_For_Sim3g_Reset(void);
void SM_Power_Off_Sim3g(void);
void SM_Wait_For_Sim3g_Power_Off(void);

void SM_Sim3g_Startup(void);
void SM_Wait_For_Sim3g_Startup_Response(void);

void SM_Wait_For_Sim3g_Network_Establishment(void);
void SM_Sim3g_Setting(void);
void SM_Wait_For_Sim3g_Setting_Response(void);

void Clear_Sim3gDataProcessingBuffer(void);
uint8_t isEndOfCommand(uint8_t pre, uint8_t cur);

Sim3g_Machine_Type Sim3G_State_Machine [] = {
		{POWER_ON_SIM3G, 									SM_Power_On_Sim3g									},
		{WAIT_FOR_SIM3G_POWER_ON, 							SM_Wait_For_Sim3g_Power_On							},
		{POWER_OFF_SIM3G, 									SM_Power_Off_Sim3g									},
		{WAIT_FOR_SIM3G_POWER_OFF, 							SM_Wait_For_Sim3g_Power_Off							},
		{RESET_SIM3G, 										SM_Reset_Sim3g										},
		{WAIT_FOR_SIM3G_RESET, 								SM_Wait_For_Sim3g_Reset		  						},
		{SIM3G_START_UP, 									SM_Sim3g_Startup									},
		{WAIT_FOR_SIM3G_STARTUP_RESPONSE, 					SM_Wait_For_Sim3g_Startup_Response					},
		{WAIT_FOR_NETWORK_ESTABLISHMENT, 					SM_Wait_For_Sim3g_Network_Establishment				},
		{SIM3G_SETTING, 									SM_Sim3g_Setting									},
		{WAIT_FOR_SIM3G_SETTING_RESPONSE, 					SM_Wait_For_Sim3g_Setting_Response					},


};

#define		NUMBER_OF_COMMANDS_FOR_SETUP_SIM3G	8

const AT_COMMAND_ARRAY atCommandArrayForCheckingSim3g[] = {
		{(uint8_t*)"AT\r",  									(uint8_t*)"OK\r"		},
		{(uint8_t*)"ATE1\r",  									(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+CSQ\r\n",							 	(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+CREG?\r\n",  							(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+CPSI?\r\n",  							(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+CGREG?\r",  								(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+IPADDR\r",  								(uint8_t*)"OK\r"		},
};
uint8_t atCommandArrayIndexForCheckingSim3g = 0;

const AT_COMMAND_ARRAY atCommandArrayForSetupSim3g[] = {
		{(uint8_t*)"AT\r",  									(uint8_t*)"OK\r"		},
		{(uint8_t*)"ATE1\r",  									(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+CGDCONT=1,\"IP\",\"v-internet\"\r", 		(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+CGSOCKCONT=1,\"IP\",\"cmet\"\r",  		(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+CSOCKSETPN=1\r",  						(uint8_t*)"OK\r"		},
		{(uint8_t*)"AT+CIPMODE=0\r",  							(uint8_t*)"OK\r"		},
#if(VERSION_EBOX == 1)
		{(uint8_t*)"AT+NETOPEN=,,1\r",  						(uint8_t*)"OK\r"		},
#elif(VERSION_EBOX == 15)
		{(uint8_t*)"AT+NETOPEN=,,1\r",  						(uint8_t*)"OK\r"		},
#elif (VERSION_EBOX == 2)
		{(uint8_t*)"AT+NETOPEN\r",  						(uint8_t*)"OK\r"		},
#elif (VERSION_EBOX == 3)
		{(uint8_t*)"AT+NETOPEN\r",  						(uint8_t*)"OK\r"		},
#endif
		{(uint8_t*)"AT+IPADDR\r",  								(uint8_t*)"OK\r"		},
};
uint8_t atCommandArrayIndex = 0;

SIM3G_STATE sim3gState = POWER_ON_SIM3G;
SIM3G_STATE pre_sim3gState = MAX_SIM3G_NUMBER_STATES;

void TestSendATcommand(void){
	uint8_t i;
	for (i = 0; i < NUMBER_OF_COMMANDS_FOR_SETUP_SIM3G; i++){
		UART3_SendToHost(atCommandArrayForSetupSim3g[i].ATCommand);
		UART3_SendToHost(atCommandArrayForSetupSim3g[i].expectedReturn);
	}
}


uint8_t Sim3g_Run(void){
	DEBUG_SIM3G(Sim3g_State_Display(););
	if(sim3gState < MAX_SIM3G_NUMBER_STATES){
		(*Sim3G_State_Machine[sim3gState].func)();
		return 0;
	} else {
		return 1;
	}
}
uint8_t isSim3g_Run_Finished(void){
	return sim3gState == MAX_SIM3G_NUMBER_STATES;
}
void Sim3g_State_Display(void){
	if(pre_sim3gState != sim3gState){
		pre_sim3gState = sim3gState;
		switch(sim3gState){
		case SIM3G_START_UP:
			DEBUG_SIM3G(UART3_SendToHost((uint8_t*)"CHECK_SIM3G_START_UP"););

			break;
		case WAIT_FOR_SIM3G_STARTUP_RESPONSE:
			DEBUG_SIM3G(UART3_SendToHost((uint8_t*)"WAIT_FOR_SIM3G_STARTUP_RESPONSE"););
			break;
		case POWER_ON_SIM3G:
			DEBUG_SIM3G(UART3_SendToHost((uint8_t*)"POWER_ON_SIM3G\r"););
			break;
		case WAIT_FOR_SIM3G_POWER_ON:
			DEBUG_SIM3G(UART3_SendToHost((uint8_t*)"WAIT_FOR_SIM3G_POWER_ON\r"););

			break;
		case POWER_OFF_SIM3G:
			DEBUG_SIM3G(UART3_SendToHost((uint8_t*)"POWER_OFF_SIM3G\r"););
			break;

		case WAIT_FOR_SIM3G_POWER_OFF:
			DEBUG_SIM3G(UART3_SendToHost((uint8_t*)"WAIT_FOR_SIM3G_POWER_OFF\r"););
			break;

		case RESET_SIM3G:
			DEBUG_SIM3G(UART3_SendToHost((uint8_t*)"RESET_SIM3G\r"););
			break;
		case WAIT_FOR_SIM3G_RESET:
			DEBUG_SIM3G(UART3_SendToHost((uint8_t*)"WAIT_FOR_SIM3G_RESET\r"););
			break;
		default:
			break;
		}
	}

}
void Set_Sim3G_State(SIM3G_STATE newState){
	sim3gState = newState;
}
SIM3G_STATE Get_Sim3G_State(void){
	return sim3gState;
}


void Sim3g_Init(void){

	sim3g_TimeoutFlag = 0;
	sim3g_Setting_TimeoutFlag = 0;
	sim3g_Timeout_Task_Index = SCH_MAX_TASKS;
	sim3g_Retry_Counter = 0;
	sim3gState = SIM3G_START_UP;
	pre_sim3gState = MAX_SIM3G_NUMBER_STATES;
	counterForResetChargingAfterDisconnection = 0;
	Sim3g_GPIO_Init();
#if(VERSION_EBOX == 1)
	Sim3g_Enable();
#elif(VERSION_EBOX == 15)
	Sim3g_Enable();
#endif
	Power_Signal_High();
	Reset_Signal_High();
}

void Sim3g_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = SIM5320_3G_PWRON;
	HAL_GPIO_Init(SIM5320_3G_PWRON_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SIM5320_3G_PERST;
	HAL_GPIO_Init(SIM5320_3G_PERST_PORT, &GPIO_InitStruct);

#if(VERSION_EBOX == 1)
	GPIO_InitStruct.Pin = SIM5320_3G_WAKEUP;
	HAL_GPIO_Init(SIM5320_3G_WAKEUP_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SIM5320_3G_REG_EN;
	HAL_GPIO_Init(SIM5320_3G_REG_EN_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(PC7_3G_WAKEUP_PORT, PC7_3G_WAKEUP, GPIO_PIN_SET);
#elif(VERSION_EBOX == 15)
	GPIO_InitStruct.Pin = SIM5320_3G_WAKEUP;
	HAL_GPIO_Init(SIM5320_3G_WAKEUP_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SIM5320_3G_REG_EN;
	HAL_GPIO_Init(SIM5320_3G_REG_EN_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(PC7_3G_WAKEUP_PORT, PC7_3G_WAKEUP, GPIO_PIN_SET);

#endif
}

//#ifdef	SIM3G_ENABLE_PORT
void Sim3g_Disable(void){
#if(VERSION_EBOX == 1)
	HAL_GPIO_WritePin(PA8_3G_REG_EN_PORT, PA8_3G_REG_EN, GPIO_PIN_RESET);
#elif(VERSION_EBOX == 15)
	HAL_GPIO_WritePin(PA8_3G_REG_EN_PORT, PA8_3G_REG_EN, GPIO_PIN_RESET);
#endif
}
void Sim3g_Enable(void){
#if(VERSION_EBOX == 1)
	HAL_GPIO_WritePin(PA8_3G_REG_EN_PORT, PA8_3G_REG_EN, GPIO_PIN_SET);
#elif(VERSION_EBOX == 15)
	HAL_GPIO_WritePin(PA8_3G_REG_EN_PORT, PA8_3G_REG_EN, GPIO_PIN_SET);
#endif
}
//#endif
void Check_Sim_3G_Available(void){
	static CHECK_SIM_3G_STATE checkSim3G_State = SEND_AT_COMMAND;
	switch(checkSim3G_State){
	case SEND_AT_COMMAND:
		SCH_Delete_Task(sim3g_Timeout_Task_Index);
		Sim3g_Clear_Timeout_Flag();
		sim3g_Timeout_Task_Index = SCH_Add_Task(Sim3g_Command_Timeout, 200,0);
		isOKFlag = RESET;
		isErrorFlag = RESET;
		isIPCloseFlag = RESET;
		ATcommandSending((uint8_t *)"AT\r\n");
		checkSim3G_State = 1;
		break;
	case WAIT_FOR_RESPONSE_FROM_AT_COMMAND:
		if(isOKFlag){
			checkSim3G_State = 2;
		}
		if(is_Sim3g_Command_Timeout()){

		}
		break;
	case 2:
		break;
	}
}


#if(VERSION_EBOX == 1)
void Sim3g_WakeUp(void){
	HAL_GPIO_WritePin(PC7_3G_WAKEUP_PORT, PC7_3G_WAKEUP, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(PC7_3G_WAKEUP_PORT, PC7_3G_WAKEUP, GPIO_PIN_RESET);
}
#elif(VERSION_EBOX == 15)
void Sim3g_WakeUp(void){
	HAL_GPIO_WritePin(PC7_3G_WAKEUP_PORT, PC7_3G_WAKEUP, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(PC7_3G_WAKEUP_PORT, PC7_3G_WAKEUP, GPIO_PIN_RESET);
}
#endif

void Power_Signal_Low(void){
	HAL_GPIO_WritePin(PC8_3G_PWRON_PORT, PC8_3G_PWRON, GPIO_PIN_SET);
}
void Power_Signal_High(void){
	HAL_GPIO_WritePin(PC8_3G_PWRON_PORT, PC8_3G_PWRON, GPIO_PIN_RESET);
}
void Reset_Signal_Low(void){
#if (VERSION_EBOX == 1)
	HAL_GPIO_WritePin(PC9_3G_PERST_PORT, PC9_3G_PERST, GPIO_PIN_RESET);
#elif (VERSION_EBOX == 15)
	HAL_GPIO_WritePin(PC9_3G_PERST_PORT, PC9_3G_PERST, GPIO_PIN_RESET);
#elif (VERSION_EBOX == 2 || VERSION_EBOX == 3)
	HAL_GPIO_WritePin(PC9_3G_PERST_PORT, PC9_3G_PERST, GPIO_PIN_SET);
#endif
}
void Reset_Signal_High(void){
#if(VERSION_EBOX == 1) //sim5320
	HAL_GPIO_WritePin(PC9_3G_PERST_PORT, PC9_3G_PERST, GPIO_PIN_SET);
#elif(VERSION_EBOX == 15) //sim5320
	HAL_GPIO_WritePin(PC9_3G_PERST_PORT, PC9_3G_PERST, GPIO_PIN_SET);	
#elif (VERSION_EBOX == 2 || VERSION_EBOX == 3) //sim 7600
	HAL_GPIO_WritePin(PC9_3G_PERST_PORT, PC9_3G_PERST, GPIO_PIN_RESET);
#endif
}

void Sim3g_Clear_Timeout_Flag(void){
	sim3g_TimeoutFlag = 0;
}
void Sim3g_Command_Timeout(void){
	sim3g_TimeoutFlag = 1;
}
uint8_t is_Sim3g_Command_Timeout(void){
	return sim3g_TimeoutFlag;
}

void Sim3g_Setting_Clear_Timeout_Flag(void){
	sim3g_Setting_TimeoutFlag = 0;
}
void Sim3g_Setting_Timeout(void){
	sim3g_Setting_TimeoutFlag = 1;
}
uint8_t is_Sim3g_Setting_Timeout(void){
	return sim3g_Setting_TimeoutFlag;
}

void Clear_All_Uart_Receive_Flags(void){
	isOKFlag = RESET;
	isStin25 = RESET;
	isPBDoneFlag = RESET;
	isErrorFlag = RESET;
	isIPCloseFlag = RESET;
	isRecvFromFlag = RESET;
	isReadyToSendDataToServer = RESET;
	isSendOKFlag = RESET;
	isReceiveDataFromServer = RESET;
}

void SM_Power_On_Sim3g(void){
	SCH_Add_Task(Power_Signal_Low, 0, 0);
	SCH_Add_Task(Power_Signal_High, TIMER_TO_POWER_ON_SIM3G, 0);
	Clear_All_Uart_Receive_Flags();
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, TIMER_TO_POWER_ON_SIM3G_TIMEOUT,0);
	sim3gState = WAIT_FOR_SIM3G_POWER_ON;
}

void SM_Wait_For_Sim3g_Power_On(void){
	if(is_Sim3g_Command_Timeout()){
		sim3gState = SIM3G_START_UP;
	}
}

void SM_Reset_Sim3g(void){
	SCH_Add_Task(Reset_Signal_Low, 0, 0);
	SCH_Add_Task(Reset_Signal_High, TIMER_TO_RESET_SIM3G, 0);
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, TIMER_TO_RESET_SIM3G_TIMEOUT, 0);
	sim3gState = WAIT_FOR_SIM3G_RESET;
}

void SM_Wait_For_Sim3g_Reset(void){
	if(is_Sim3g_Command_Timeout()){
		sim3gState = SIM3G_START_UP;
	}
}
void SM_Power_Off_Sim3g(void){
	SCH_Add_Task(Power_Signal_Low, 0, 0);
	SCH_Add_Task(Power_Signal_High, TIMER_TO_POWER_OFF_SIM3G, 0);
	Sim3g_Disable();
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, TIMER_TO_POWER_OFF_SIM3G,0);
	sim3gState = WAIT_FOR_SIM3G_POWER_OFF;
}
void SM_Wait_For_Sim3g_Power_Off(void){
	if(is_Sim3g_Command_Timeout()){
		Sim3g_Enable();
		sim3gState = POWER_ON_SIM3G;
	}
}


void Setting_Up_Timeout(void){
	sim3g_Retry_Counter ++;
	SCH_Delete_Task(sim3g_Timeout_Task_Index);
	Sim3g_Clear_Timeout_Flag();
	sim3g_Timeout_Task_Index = SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
}

void SM_Sim3g_Startup(void){

	SCH_Delete_Task(sim3g_Timeout_Task_Index);
	Sim3g_Clear_Timeout_Flag();
	sim3g_Timeout_Task_Index = SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	sim3gState = WAIT_FOR_SIM3G_STARTUP_RESPONSE;
}

void SM_Wait_For_Sim3g_Startup_Response(void){
	if(isPBDoneFlag == SET){
		isPBDoneFlag = RESET;
		isStin25 = RESET;

		SCH_Delete_Task(sim3g_Timeout_Task_Index);
		Sim3g_Clear_Timeout_Flag();
		sim3g_Timeout_Task_Index = SCH_Add_Task(Sim3g_Command_Timeout, WAIT_FOR_NETWORK_ESTABLISMENT_TIME_OUT,0);
		sim3gState = WAIT_FOR_NETWORK_ESTABLISHMENT;
	} else if(isErrorFlag){
		isErrorFlag = RESET;
		sim3gState = RESET_SIM3G;
	}
	if(is_Sim3g_Command_Timeout() || isIPCloseFlag){
		isIPCloseFlag = RESET;
		sim3gState = RESET_SIM3G;
	}

}

void SM_Wait_For_Sim3g_Network_Establishment(void){
	if(is_Sim3g_Command_Timeout()){
		atCommandArrayIndex = 0;
		sim3g_Retry_Counter = 0;
		sim3gState = SIM3G_SETTING;
		SCH_Delete_Task(sim3g_Timeout_Task_Index);
		Sim3g_Clear_Timeout_Flag();
		sim3g_Timeout_Task_Index = SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
		Sim3g_Setting_Clear_Timeout_Flag();
		sim3g_Setting_Timeout_Task_Index = SCH_Add_Task(Sim3g_Setting_Timeout, SETTING_TIME_OUT,0);
	}

}

void SM_Sim3g_Setting(void){
	if(is_Sim3g_Setting_Timeout()){
		Setting_Up_Timeout();
		isOKFlag = RESET;
		isErrorFlag = RESET;
		isIPCloseFlag = RESET;
		ATcommandSending((uint8_t *)atCommandArrayForSetupSim3g[atCommandArrayIndex].ATCommand);
		sim3gState = WAIT_FOR_SIM3G_SETTING_RESPONSE;
	}
}
void SM_Wait_For_Sim3g_Setting_Response(void){
	if(isOKFlag){
		isOKFlag = RESET;
		sim3gState = SIM3G_SETTING;
		Sim3g_Setting_Clear_Timeout_Flag();
		sim3g_Setting_Timeout_Task_Index = SCH_Add_Task(Sim3g_Setting_Timeout, SETTING_TIME_OUT,0);
		atCommandArrayIndex++;
		if(atCommandArrayIndex == NUMBER_OF_COMMANDS_FOR_SETUP_SIM3G){
			sim3gState = MAX_SIM3G_NUMBER_STATES;
		}
	} else if(isErrorFlag){
		isErrorFlag = RESET;
		if(sim3g_Retry_Counter > MAX_RETRY_NUMBER){
			sim3g_Retry_Counter = 0;
			sim3gState = RESET_SIM3G;
		} else {
			sim3gState = SIM3G_SETTING;
		}
	}
	if(is_Sim3g_Command_Timeout() || isIPCloseFlag){
		isIPCloseFlag = RESET;
		sim3g_Retry_Counter = 0;
		sim3gState = RESET_SIM3G;
	}
}


FlagStatus isReceivedData(const uint8_t * str){
	uint8_t i = 0;
	uint8_t str_len = GetStringLength((uint8_t *)str);
	while(i < str_len){
		if(Sim3gDataProcessingBuffer[i] != str[i]){
			return RESET;
		}
		i++;
	}
	return SET;
}
FlagStatus isReceivedDataFromServer(uint8_t message_type, uint8_t len_of_message){
	if(Sim3gDataProcessingBuffer[0]  == message_type &&
			Sim3gDataProcessingBuffer[1] == len_of_message)
		return SET;
	return RESET;
}

void Processing_Received_Data(uint8_t * sub_topic, uint16_t boxID){
	uint8_t lentopic = GetStringLength(sub_topic);
	uint8_t relayIndex;
	uint8_t relayStatus;
	uint16_t tempBoxID;

	tempBoxID = (Sim3gDataProcessingBuffer[8+0] - 0x30)*1000;
	tempBoxID += (Sim3gDataProcessingBuffer[8+1] - 0x30)*100;
	tempBoxID += (Sim3gDataProcessingBuffer[8+2] - 0x30)*10;
	tempBoxID += (Sim3gDataProcessingBuffer[8+3] - 0x30);

	if(boxID == tempBoxID){
		if(Sim3gDataProcessingBuffer[2 + lentopic] == COMMAND_ONLY){
			relayIndex = Sim3gDataProcessingBuffer[2 + lentopic + 1] - 0x30;
			relayStatus = Sim3gDataProcessingBuffer[2 + lentopic + 2] - 0x30;
			if(relayStatus == SET){
				Set_Relay(relayIndex);
				Set_Limit_Energy(relayIndex, 0xffffffff);
			} else {
				Reset_Relay(relayIndex);
				Set_Limit_Energy(relayIndex, 0);
			}
		}
	}
}

void Processing_Update_Total_Power_Consumption(uint8_t * sub_topic, uint16_t boxID){
	uint8_t lentopic = GetStringLength(sub_topic);
	uint16_t tempBoxID;
	uint64_t updateTotalPowerConsumption = 0;
	tempBoxID = (Sim3gDataProcessingBuffer[8+0] - 0x30)*1000;
	tempBoxID += (Sim3gDataProcessingBuffer[8+1] - 0x30)*100;
	tempBoxID += (Sim3gDataProcessingBuffer[8+2] - 0x30)*10;
	tempBoxID += (Sim3gDataProcessingBuffer[8+3] - 0x30);

	if(boxID == tempBoxID){
		if((Sim3gDataProcessingBuffer[2 + lentopic] == START_UPDATE_TOTAL_POWER_CONSUMPTION)
				&& (Sim3gDataProcessingBuffer[2 + lentopic + 11] == END_UPDATE_TOTAL_POWER_CONSUMPTION)){
			for(uint8_t idx = 1; idx <= 10; idx++){
				updateTotalPowerConsumption = updateTotalPowerConsumption*10 + (Sim3gDataProcessingBuffer[2 + lentopic + idx] - 0x30);
			}
			Set_Main_Power_Consumption(updateTotalPowerConsumption);
		}
	}
}

void Processing_Received_Data_From_Retained_Message(uint8_t * sub_topic, uint16_t boxID){
	uint8_t lentopic = GetStringLength(sub_topic);
	uint8_t relayIndex;
	uint8_t relayStatus;
	uint16_t tempBoxID;

	tempBoxID = (Sim3gDataProcessingBuffer[8+0] - 0x30)*1000;
	tempBoxID += (Sim3gDataProcessingBuffer[8+1] - 0x30)*100;
	tempBoxID += (Sim3gDataProcessingBuffer[8+2] - 0x30)*10;
	tempBoxID += (Sim3gDataProcessingBuffer[8+3] - 0x30);

	if(boxID == tempBoxID){
		for(uint8_t i = 0; i < NUMBER_OF_RELAYS; i++){
			relayIndex = Sim3gDataProcessingBuffer[2 + lentopic + i*4 + 0] - 0x30;
			relayStatus = Sim3gDataProcessingBuffer[2 + lentopic + i*4 + 2] - 0x30;
			if(relayStatus == SET){
				Set_Relay(relayIndex);
				Set_Limit_Energy(relayIndex, 0xffffffff);
			} else {
				Reset_Relay(relayIndex);
				Set_Limit_Energy(relayIndex, 0);
			}
		}
	}
}

uint8_t isEndOfCommand(uint8_t pre, uint8_t cur){
	if(pre == '\r' && cur == '\n')
		return 1;
	else if(pre == '\n' && cur == '\n')
		return 1;
	else
		return 0;

}

void FSM_Process_Data_Received_From_Sim3g(void){
	static uint8_t readCharacter = 0;
	static uint8_t preReadCharacter = 0;

	switch(processDataState){
	case CHECK_DATA_AVAILABLE_STATE:
		if(Uart1_Received_Buffer_Available()){
			Clear_Sim3gDataProcessingBuffer();
			processDataState = DETECT_SPECIAL_CHARACTER;
			isReceiveDataFromServer = SET;
		}
		break;
	case DETECT_SPECIAL_CHARACTER:
		if(Uart1_Received_Buffer_Available()){

			preReadCharacter = readCharacter;
			readCharacter = Uart1_Read_Received_Buffer();
			if(readCharacter == '>'){
				processDataState = PREPARE_FOR_SENDING_DATA;
			}else if(isEndOfCommand(preReadCharacter, readCharacter) == 1){
				processDataState = PREPARE_PROCESSING_RECEIVED_DATA;
			} else if((preReadCharacter == SUBSCRIBE_RECEIVE_MESSAGE_TYPE)
					&& (readCharacter == LEN_SUBSCRIBE_RECEIVE_MESSAGE_TYPE1 || readCharacter == LEN_SUBSCRIBE_RECEIVE_MESSAGE_TYPE2)){
				processDataState = PROCESSING_RECEIVED_DATA;
				Clear_Sim3gDataProcessingBuffer();
			}
			else if((preReadCharacter == SUBSCRIBE_RECEIVE_RETAINED_MESSAGE_TYPE)
					&& (readCharacter == LEN_SUBSCRIBE_RECEIVE_RETAINED_MESSAGE_TYPE)){
				processDataState = PROCESSING_RETAINED_DATA;
				Clear_Sim3gDataProcessingBuffer();
			} else if((preReadCharacter == SUBSCRIBE_RECEIVE_MESSAGE_TYPE)
					&& (readCharacter == LEN_FOR_UPDATE_POWER_CONSUPMPTION)){ //For update total power consumption
				processDataState = PROCESSING_UPDATE_TOTAL_POWER_CONSUMPTION;
				Clear_Sim3gDataProcessingBuffer();
			}
			else {
				Sim3gDataProcessingBuffer[sim3gDataProcessingBufferIndex++] = readCharacter;
			}
		}

		break;
	case PREPARE_FOR_SENDING_DATA:
		isReadyToSendDataToServer = SET;
		processDataState = CHECK_DATA_AVAILABLE_STATE;
		break;
	case PREPARE_PROCESSING_RECEIVED_DATA:
//		UART3_SendToHost((uint8_t*)"a");
//		UART3_SendToHost((uint8_t*)Sim3gDataProcessingBuffer);
		if(isReceivedData((uint8_t *)PB_DONE)){
			isPBDoneFlag = SET;
		} else if(isReceivedData((uint8_t *)STIN25)){
			isStin25 = SET;
		} else if(isReceivedData((uint8_t *)OK)){
			isOKFlag = SET;
		} else if(isReceivedData((uint8_t *)ERROR_1)){
			isErrorFlag = SET;
//			UART3_SendToHost((uint8_t*)"aNg");
			if(counterForResetChargingAfterDisconnection < TIME_OUT_FOR_STOP_CHARGING){
				counterForResetChargingAfterDisconnection ++;
			} else {
				counterForResetChargingAfterDisconnection  = TIME_OUT_FOR_STOP_CHARGING;
			}
		}else if(isReceivedData((uint8_t *)SIM_NOT_INSERT)){
//			isErrorFlag = SET;
			UART3_SendToHost((uint8_t*)"SIM_NOT_INSERT");
			if(counterForResetChargingAfterDisconnection < TIME_OUT_FOR_STOP_CHARGING){
				counterForResetChargingAfterDisconnection ++;
			} else {
				counterForResetChargingAfterDisconnection  = TIME_OUT_FOR_STOP_CHARGING;
			}

		}else if(isReceivedData((uint8_t *)IP_CLOSE)){
			isIPCloseFlag = SET;
		} else if(isReceivedData((uint8_t *)RECV_FROM)){
			isSendOKFlag = RESET;
			isRecvFromFlag = SET;
		} else if(isReceivedData((uint8_t *)Send_ok)){
			isSendOKFlag = SET;
		} else if(isReceivedData((uint8_t *)ISCIPSEND)){
			isCipsend = SET;
		}
		processDataState = CHECK_DATA_AVAILABLE_STATE;
		break;
	case PROCESSING_RECEIVED_DATA:
		if(Uart1_Received_Buffer_Available()){
			preReadCharacter = readCharacter;
			readCharacter = Uart1_Read_Received_Buffer();
			if(isEndOfCommand(preReadCharacter, readCharacter) == 1){
				Processing_Received_Data((uint8_t*)SUBSCRIBE_TOPIC_1, Get_Box_ID());
				processDataState = CHECK_DATA_AVAILABLE_STATE;
			} else if(preReadCharacter == SUBSCRIBE_RECEIVE_MESSAGE_TYPE
					&& (readCharacter == LEN_SUBSCRIBE_RECEIVE_MESSAGE_TYPE1 || readCharacter == LEN_SUBSCRIBE_RECEIVE_MESSAGE_TYPE2)){
				processDataState = PROCESSING_RECEIVED_DATA;
				Processing_Received_Data((uint8_t*)SUBSCRIBE_TOPIC_1,  Get_Box_ID());
				Clear_Sim3gDataProcessingBuffer();
			} else {
				Sim3gDataProcessingBuffer[sim3gDataProcessingBufferIndex++] = readCharacter;
				if(sim3gDataProcessingBufferIndex >= DATA_RECEIVE_LENGTH){
					Processing_Received_Data((uint8_t*)SUBSCRIBE_TOPIC_1, Get_Box_ID());
					processDataState = CHECK_DATA_AVAILABLE_STATE;
				}
			}
		}
		break;
	case PROCESSING_RETAINED_DATA:
			if(Uart1_Received_Buffer_Available()){
				preReadCharacter = readCharacter;
				readCharacter = Uart1_Read_Received_Buffer();
				if(isEndOfCommand(preReadCharacter, readCharacter)){
					processDataState = CHECK_DATA_AVAILABLE_STATE;
				}
				else {
					Sim3gDataProcessingBuffer[sim3gDataProcessingBufferIndex++] = readCharacter;
					if(sim3gDataProcessingBufferIndex >= DATA_RETAINED_MESSAGE_LENGTH){
						Processing_Received_Data_From_Retained_Message((uint8_t*)SUBSCRIBE_TOPIC_3, Get_Box_ID());
						processDataState = CHECK_DATA_AVAILABLE_STATE;
					}
				}
			}
			break;
	case PROCESSING_UPDATE_TOTAL_POWER_CONSUMPTION:
		if(Uart1_Received_Buffer_Available()){
			preReadCharacter = readCharacter;
			readCharacter = Uart1_Read_Received_Buffer();
			if(isEndOfCommand(preReadCharacter, readCharacter)){
				processDataState = CHECK_DATA_AVAILABLE_STATE;
			}
			else {
				Sim3gDataProcessingBuffer[sim3gDataProcessingBufferIndex++] = readCharacter;
				if(readCharacter == END_UPDATE_TOTAL_POWER_CONSUMPTION){
					Processing_Update_Total_Power_Consumption((uint8_t*)SUBSCRIBE_TOPIC_1, Get_Box_ID());
					processDataState = CHECK_DATA_AVAILABLE_STATE;
				}
			}
		}
		break;
	default:
		break;
	}
}

void Clear_Sim3gDataProcessingBuffer(void){
	uint8_t i;
	for(i = 0; i < RXBUFFERSIZE; i++){
		Sim3gDataProcessingBuffer[i] = 0;
	}
	sim3gDataProcessingBufferIndex = 0;
}
void UART3_SendReceivedBuffer(void){
	UART3_SendToHost((uint8_t *)Sim3gDataProcessingBuffer);
}

uint8_t isConnestionLost(void){
	return (counterForResetChargingAfterDisconnection == TIME_OUT_FOR_STOP_CHARGING);
}
void ClearCounter(void){
	counterForResetChargingAfterDisconnection = 0;
}
FlagStatus isRecvFrom(void){
	return isRecvFromFlag;
}

FlagStatus isIPClose(void){
	return isIPCloseFlag;
}
FlagStatus isSendOK(void){
	return isIPCloseFlag;
}

void Set_Is_Receive_Data_From_Server(FlagStatus status){
	isReceiveDataFromServer = status;
}
FlagStatus Get_Is_Receive_Data_From_Server(void){
	return isReceiveDataFromServer;
}


