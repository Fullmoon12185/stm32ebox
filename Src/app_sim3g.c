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

#define DEBUG_SIM3G(x)    						x

#define DATA_TO_SEND_LENGTH						20

#define TIMER_TO_POWER_ON_SIM3G					50
#define TIMER_TO_POWER_OFF_SIM3G				500
#define TIMER_TO_POWER_ON_SIM3G_TIMEOUT			5000
#define TIMER_TO_POWER_OFF_SIM3G_TIMEOUT		5000


#define TIMER_TO_RESET_SIM3G					500
#define TIMER_TO_RESET_SIM3G_TIMEOUT			5000
#define COMMAND_TIME_OUT						2000

const uint8_t OK[] = "OK";
const uint8_t CONNECT_OK[] = "CONNECT OK";
const uint8_t ERROR_1[] = "ERROR";
const uint8_t GREATER_THAN_SYMBOL[] = ">";
const uint8_t NETWORK_OPENED[] = "Network opened";



const uint8_t AT[] = "AT\r\n";
const uint8_t AT_CGDCONT[] = "AT+CGDCONT=1,\"IP\",\"v-internet\"\r\n";
//Define PDP context
const uint8_t AT_CGSOCKCONT[] = "AT+CGSOCKCONT,1,\"IP\",\"cmet\"\r\n";
//Set authentication parameters
const uint8_t AT_CGSOCKAUTH[] = "AT+CGSOCKAUTH=1,1,\"teST\",\"1234\"\r\n";
//Set the number of active PDP context
const uint8_t AT_CGSOCKSETPN[] = "AT+CGSOCKSETPN=1\r\n";
const uint8_t AT_CIPMODE[] = "AT+CIPMODE=0\r\n";
//PDP context activation
const uint8_t AT_NETOPEN[] = "AT+NETOPEN\r\n";
//Get SIM ip address
const uint8_t AT_IPADDR [] = "AT+IPADDR\r\n";
//PDP context deactivatioin
const uint8_t AT_NETCLOSE[] = "AT+NETCLOSE\r\n";
//Option of receiving data's length
const uint8_t AT_CIPHEAD[] = "AT+CIPHEAD=1\r\n";
//Option of receiving data's length
const uint8_t AT_CIPSRIP[] = "AT+CIPSRIP=1\r\n";
//when we use tcp/ip we need to adjust number of retransmission according to network status or
//you want to know your data is received by remote end
const uint8_t AT_CIPCCFG [] = "AT+CIPCCFG=3,500,1,1\r\n";




static uint8_t sim3g_TimeoutFlag = 0;



void Sim3g_GPIO_Init(void);
void Sim3g_State_Display(void);

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




void SM_Power_On_Sim3g(void);
void SM_Wait_For_Sim3g_Power_On(void);
void SM_Reset_Sim3g(void);
void SM_Wait_For_Sim3g_Reset(void);
void SM_Power_Off_Sim3g(void);
void SM_Wait_For_Sim3g_Power_Off(void);

void SM_Sim3G_Status(void);
void SM_Wait_For_Sim3g_Status_Response(void);
void SM_APN_Setting(void);
void SM_Wait_For_APN_Setting_Response(void);

void SM_Define_TCPIP_Context(void);
void SM_Wait_For_Define_TCPIP_Context_Response(void);
//void SM_Set_Authenticaton_Parameter(void);
void SM_Set_PDP_Context_Number(void);
void SM_Wait_For_Set_PDP_Context_Number_Response(void);

void SM_Command_Mode(void);
void SM_Wait_For_Command_Mode_Response(void);
void SM_Netopen(void);
void SM_Wait_For_Netopen_Response(void);
void SM_Ipconfig(void);
void SM_Wait_For_Ipconfig_Response(void);
void SM_Netclose(void);
void SM_Wait_For_Netclose_Response(void);




Sim3g_Machine_Type Sim3G_State_Machine [] = {
		{SIM3G_STATUS, 										SM_Sim3G_Status										},
		{WAIT_FOR_SIM3G_STATUS_RESPONSE, 					SM_Wait_For_Sim3g_Status_Response					},
		{POWER_ON_SIM3G, 									SM_Power_On_Sim3g									},
		{WAIT_FOR_SIM3G_POWER_ON, 							SM_Wait_For_Sim3g_Power_On							},
		{RESET_SIM3G, 										SM_Reset_Sim3g										},
		{WAIT_FOR_SIM3G_RESET, 								SM_Wait_For_Sim3g_Reset		  						},
		{POWER_OFF_SIM3G, 									SM_Power_Off_Sim3g									},
		{WAIT_FOR_SIM3G_POWER_OFF, 							SM_Wait_For_Sim3g_Power_Off							},
		{APN_SETTING,		 								SM_APN_Setting										},
		{WAIT_FOR_APN_SETTING_RESPONSE, 					SM_Wait_For_APN_Setting_Response	    			},
		{DEFINE_TCPIP_CONTEXT, 								SM_Define_TCPIP_Context								},
		{WAIT_FOR_DEFINE_TCPIP_CONTEXT_RESPONSE, 			SM_Wait_For_Define_TCPIP_Context_Response			},
		{SET_PDP_CONTEXT_NUMBER, 							SM_Set_PDP_Context_Number							},
		{WAIT_FOR_SET_PDP_CONTEXT_NUMBER_RESPONSE, 			SM_Wait_For_Set_PDP_Context_Number_Response			},
		{COMMAND_MODE, 										SM_Command_Mode										},
		{WAIT_FOR_COMMAND_MODE_RESPONSE, 					SM_Wait_For_Command_Mode_Response						},

		{NETOPEN, 											SM_Netopen											},
		{WAIT_FOR_NETOPEN_RESPONSE, 						SM_Wait_For_Netopen_Response							},
		{IPCONFIG,											SM_Ipconfig											},
		{WAIT_FOR_IPCONFIG_RESPONSE,						SM_Wait_For_Ipconfig_Response						},
		{NETCLOSE, 											SM_Netclose											},
		{WAIT_FOR_NETCLOSE_RESPONSE, 						SM_Wait_For_Netclose_Response 						},
//		{ADJUST_NUMBER_OF_RETRANSMISSION, 					SM_Adjust_Number_Of_Retransmission					},
//		{WAIT_FOR_ADJUST_NUMBER_OF_RETRANSMISSION_RESPONSE, SM_Wait_For_Adjust_Number_Of_Retransmission_Response},


//		{RECEIVE_DATA_LENGTH_OPTION, 						SM_Receive_Data_Length_Option						},
//		{RECEIVE_DATA_ADDRESS_OPTION, 						SM_Receive_Data_Address_Option						},
//		{SET_AUTHENTICATION_PARAMETER, 						SM_Set_Authenticaton_Parameter						},
};



SIM3G_STATE sim3gState = SIM3G_STATUS;
SIM3G_STATE pre_sim3gState = SIM3G_STATUS;


void Sim3g_Run(void){
	if(sim3gState < WAIT_FOR_IPCONFIG_RESPONSE){
		(*Sim3G_State_Machine[sim3gState].func)();
		Sim3g_State_Display();
	}
}
void Sim3g_State_Display(void){
	if(pre_sim3gState != sim3gState){
		pre_sim3gState = sim3gState;
		switch(sim3gState){
		case SIM3G_STATUS:
			DEBUG_SIM3G(printf("SIM3G_STATUS"));
			break;
		case WAIT_FOR_SIM3G_STATUS_RESPONSE:
			DEBUG_SIM3G(printf("WAIT_FOR_SIM3G_STATUS_RESPONSE"));
			break;

		case POWER_ON_SIM3G:
			DEBUG_SIM3G(printf("POWER_ON_SIM3G"));
			break;
		case WAIT_FOR_SIM3G_POWER_ON:
			DEBUG_SIM3G(printf("WAIT_FOR_SIM3G_POWER_ON"));
			break;
		case POWER_OFF_SIM3G:
			DEBUG_SIM3G(printf("POWER_OFF_SIM3G"));
			break;

		case WAIT_FOR_SIM3G_POWER_OFF:
			DEBUG_SIM3G(printf("WAIT_FOR_SIM3G_POWER_OFF"));
			break;

		case RESET_SIM3G:
			DEBUG_SIM3G(printf("RESET_SIM3G"));
			break;
		case WAIT_FOR_SIM3G_RESET:
			DEBUG_SIM3G(printf("WAIT_FOR_SIM3G_RESET"));
			break;

		case APN_SETTING:
			DEBUG_SIM3G(printf("APN_SETTING"));
			break;
		case WAIT_FOR_APN_SETTING_RESPONSE:
			DEBUG_SIM3G(printf("WAIT_FOR_APN_SETTING_RESPONSE"));
			break;

		case DEFINE_TCPIP_CONTEXT:
			DEBUG_SIM3G(printf("DEFINE_TCPIP_CONTEXT"));
			break;
		case WAIT_FOR_DEFINE_TCPIP_CONTEXT_RESPONSE:
			DEBUG_SIM3G(printf("WAIT_FOR_DEFINE_TCPIP_CONTEXT_RESPONSE"));
			break;
		case SET_PDP_CONTEXT_NUMBER:
			DEBUG_SIM3G(printf("SET_PDP_CONTEXT_NUMBER"));
			break;
		case WAIT_FOR_SET_PDP_CONTEXT_NUMBER_RESPONSE:
			DEBUG_SIM3G(printf("WAIT_FOR_SET_PDP_CONTEXT_NUMBER_RESPONSE"));
			break;
		case COMMAND_MODE:
			DEBUG_SIM3G(printf("COMMAND_MODE"));
			break;
		case WAIT_FOR_COMMAND_MODE_RESPONSE:
			DEBUG_SIM3G(printf("WAIT_FOR_COMMAND_MODE_RESPONSE"));
			break;
		case NETOPEN:
			DEBUG_SIM3G(printf("NETOPEN"));
			break;
		case WAIT_FOR_NETOPEN_RESPONSE:
			DEBUG_SIM3G(printf("WAIT_FOR_NETOPEN_RESPONSE"));
			break;
		case IPCONFIG:
			DEBUG_SIM3G(printf("IPCONFIG"));
			break;
		case WAIT_FOR_IPCONFIG_RESPONSE:
			DEBUG_SIM3G(printf("WAIT_FOR_IPCONFIG_RESPONSE"));
			break;
		case NETCLOSE:
			DEBUG_SIM3G(printf("NETCLOSE"));
			break;
		case WAIT_FOR_NETCLOSE_RESPONSE:
			DEBUG_SIM3G(printf("WAIT_FOR_NETCLOSE_RESPONSE"));
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
	Sim3g_GPIO_Init();
	Sim3g_Enable();
	Set_Sim3G_State(SIM3G_STATUS);
	Sim3g_Run();
}
void Sim3g_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = SIM5320_3G_WAKEUP;
	HAL_GPIO_Init(SIM5320_3G_WAKEUP_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SIM5320_3G_PWRON;
	HAL_GPIO_Init(SIM5320_3G_PWRON_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SIM5320_3G_PERST;
	HAL_GPIO_Init(SIM5320_3G_PERST_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SIM5320_3G_REG_EN;
	HAL_GPIO_Init(SIM5320_3G_REG_EN_PORT, &GPIO_InitStruct);


}

void Sim3g_Disable(void){
	HAL_GPIO_WritePin(PA8_3G_REG_EN_PORT, PA8_3G_REG_EN, GPIO_PIN_RESET);
}
void Sim3g_Enable(void){
	HAL_GPIO_WritePin(PA8_3G_REG_EN_PORT, PA8_3G_REG_EN, GPIO_PIN_SET);

}

void Sim3g_WakeUp(void){
	HAL_GPIO_WritePin(PC7_3G_WAKEUP_PORT, PC7_3G_WAKEUP, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(PC7_3G_WAKEUP_PORT, PC7_3G_WAKEUP, GPIO_PIN_RESET);
}
void Power_Signal_Low(void){
	HAL_GPIO_WritePin(PC8_3G_PWRON_PORT, PC8_3G_PWRON, GPIO_PIN_RESET);
}
void Power_Signal_High(void){
	HAL_GPIO_WritePin(PC8_3G_PWRON_PORT, PC8_3G_PWRON, GPIO_PIN_SET);
}
void Reset_Signal_Low(void){
	HAL_GPIO_WritePin(PC9_3G_PERST_PORT, PC9_3G_PERST, GPIO_PIN_RESET);
}
void Reset_Signal_High(void){
	HAL_GPIO_WritePin(PC9_3G_PERST_PORT, PC9_3G_PERST, GPIO_PIN_SET);
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

void SM_Power_On_Sim3g(void){
	SCH_Add_Task(Power_Signal_Low, 0, 0);
	SCH_Add_Task(Power_Signal_High, TIMER_TO_POWER_ON_SIM3G, 0);
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, TIMER_TO_POWER_ON_SIM3G_TIMEOUT,0);
	sim3gState = WAIT_FOR_SIM3G_POWER_ON;
}

void SM_Wait_For_Sim3g_Power_On(void){
	if(is_Sim3g_Command_Timeout()){
		sim3gState = SIM3G_STATUS;
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
		sim3gState = SIM3G_STATUS;
	}
}
void SM_Power_Off_Sim3g(void){
	SCH_Add_Task(Power_Signal_Low, 0, 0);
	SCH_Add_Task(Power_Signal_High, TIMER_TO_POWER_OFF_SIM3G, 0);
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, TIMER_TO_POWER_OFF_SIM3G_TIMEOUT,0);
	sim3gState = WAIT_FOR_SIM3G_POWER_OFF;
}
void SM_Wait_For_Sim3g_Power_Off(void){
	if(is_Sim3g_Command_Timeout()){
		sim3gState = POWER_ON_SIM3G;
	}
}


void SM_Sim3G_Status(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT);
	sim3gState = WAIT_FOR_SIM3G_STATUS_RESPONSE;
}

void SM_Wait_For_Sim3g_Status_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			sim3gState = APN_SETTING;
		} else if(isReceivedData((uint8_t *)ERROR_1)){
			sim3gState = POWER_OFF_SIM3G;
		}
	}
	if(is_Sim3g_Command_Timeout()){
		sim3gState = POWER_ON_SIM3G;
	}
}


void SM_APN_Setting(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CGDCONT);
	sim3gState = WAIT_FOR_APN_SETTING_RESPONSE;

}
void SM_Wait_For_APN_Setting_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			sim3gState = DEFINE_TCPIP_CONTEXT;
		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
void SM_Define_TCPIP_Context(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CGSOCKCONT);
	sim3gState = WAIT_FOR_DEFINE_TCPIP_CONTEXT_RESPONSE;
	
}
void SM_Wait_For_Define_TCPIP_Context_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			sim3gState = SET_PDP_CONTEXT_NUMBER;
		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}

void SM_Set_PDP_Context_Number(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CGSOCKSETPN);
	sim3gState = WAIT_FOR_SET_PDP_CONTEXT_NUMBER_RESPONSE;
}
void SM_Wait_For_Set_PDP_Context_Number_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			sim3gState = COMMAND_MODE;
		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
void SM_Command_Mode(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CIPMODE);
	sim3gState = WAIT_FOR_COMMAND_MODE_RESPONSE;
}
void SM_Wait_For_Command_Mode_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			sim3gState = NETOPEN;
		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
void SM_Netopen(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_NETOPEN);
	sim3gState = WAIT_FOR_NETOPEN_RESPONSE;
	
}
void SM_Wait_For_Netopen_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
void SM_Ipconfig(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_IPADDR);
	sim3gState = WAIT_FOR_IPCONFIG_RESPONSE;

}
void SM_Wait_For_Ipconfig_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
//void SM_Set_Authenticaton_Parameter(void){
//	Sim3g_Clear_Timeout_Flag();
//	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
//	ATcommandSending((uint8_t *)AT_CGSOCKAUTH);
//	sim3gState = WAIT_FOR_SET_AUTHENTICATION_PARAMETER_RESPONSE;
//}
void SM_Netclose(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_NETCLOSE);
	sim3gState = WAIT_FOR_NETCLOSE_RESPONSE;
}
void SM_Wait_For_Netclose_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
//void SM_Receive_Data_Length_Option(void){
//	Sim3g_Clear_Timeout_Flag();
//	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
//	ATcommandSending((uint8_t *)AT_CIPHEAD);
//	sim3gState = WAIT_FOR_RECEIVE_DATA_LENGTH_RESPONSE;
//}
//void SM_Receive_Data_Address_Option(void){
//	Sim3g_Clear_Timeout_Flag();
//	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
//	ATcommandSending((uint8_t *)AT_CIPSRIP);
//	sim3gState = WAIT_FOR_RECEIVE_DATA_ADDRESS_OPTION_RESPONSE;
//}
//void SM_Adjust_Number_Of_Retransmission(void){
//	Sim3g_Clear_Timeout_Flag();
//	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
//	ATcommandSending((uint8_t *)AT_CIPCCFG);
//	sim3gState = WAIT_FOR_ADJUST_NUMBER_OF_RETRANSMISSION_RESPONSE;
//}
//void SM_Wait_For_Adjust_Number_Of_Retransmission_Response(void){
//	if(isUART1ReceiveReady()){
//		if(isReceivedData((uint8_t *)OK)){
//
//		} else if(isReceivedData((uint8_t *)ERROR_1)){
//
//		}
//	}
//	if(is_Sim3g_Command_Timeout()){
//
//	}
//}








