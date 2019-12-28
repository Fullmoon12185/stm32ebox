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

#define DEBUG_SIM3G(x)    				x

#define DATA_TO_SEND_LENGTH				20
#define TIMER_TO_POWER_ON_SIM3G			50
#define TIMER_TO_POWER_OFF_SIM3G		500
#define COMMAND_TIME_OUT				2000

const uint8_t OK[] = "OK";
const uint8_t CONNECT_OK[] = "CONNECT OK";
const uint8_t ERROR_1[] = "ERROR";
const uint8_t GREATER_THAN_SYMBOL[] = ">";
const uint8_t NETWORK_OPENED[] = "Network opened";


const uint8_t AT[] = "AT\r\n";
//Define PDP context
const uint8_t AT_CGSOCKCONT[] = "AT+CGSOCKCONT,1,\"100.100.100.100\",\"cmet\"\r\n";
//Set authentication parameters
const uint8_t AT_CGSOCKAUTH[] = "AT+CGSOCKAUTH=1,1,\"teST\",\"1234\"\r\n";
//Set the number of active PDP context
const uint8_t AT_CGSOCKSETPN[] = "AT+CGSOCKSETPN=1\r\n";
//PDP context activation
const uint8_t AT_NETOPEN[] = "AT+NETOPEN=,,1\r\n";
//PDP context deactivatioin
const uint8_t AT_NETCLOSE[] = "AT+NETCLOSE\r\n";
//Option of receiving data's length
const uint8_t AT_CIPHEAD[] = "AT+CIPHEAD=1\r\n";
//Option of receiving data's length
const uint8_t AT_CIPSRIP[] = "AT+CIPSRIP=1\r\n";
//when we use tcp/ip we need to adjust number of retransmission according to network status or
//you want to know your data is received by remote end
const uint8_t AT_CIPCCFG [] = "AT+CIPCCFG=3,500,1,1\r\n";
//Establish a connection
const uint8_t AT_CIPOPEN[] = "AT+CIPOEN=0,\"TCP\",100.100.100.100\",100\r\n";
//Send data
const uint8_t LENGTH_POSITION = 13;
uint8_t AT_CIPSEND[16] = "AT+CIPSEND=0,4\r\n";

uint8_t DataToSend[DATA_TO_SEND_LENGTH];




static uint8_t timeoutFlag = 0;


static void setUpReceiveResponse(void);
static void ATcommandSending(uint8_t * buffer);
void Sim3g_Enable(void);
void Sim3g_WakeUp(void);
void Power_Signal_Low(void);
void Power_Signal_High(void);
void Sim3g_Clear_Timeout_Flag(void);
void Sim3g_Command_Timeout(void);
uint8_t is_Sim3g_Command_Timeout(void);




void SM_Sim3G_Status(void);
void SM_Wait_For_Sim3g_Status_Response(void);
void SM_Power_On_Sim3g(void);
void SM_Wait_For_Sim3g_Power_On(void);
void SM_Power_Off_Sim3g(void);
void SM_Wait_For_Sim3g_Power_Off(void);
void SM_Define_PDP_Context(void);
void SM_Wait_For_Define_PDP_Context_Response(void);
void SM_Set_Authenticaton_Parameter(void);
void SM_Set_PDP_Context_Number(void);
void SM_PDP_Context_Activation(void);
void SM_Wait_For_PDP_Context_Activation_Response(void);
void SM_PDP_Context_Deactivation(void);
void SM_Receive_Data_Length_Option(void);
void SM_Receive_Data_Address_Option(void);
void SM_Adjust_Number_Of_Retransmission(void);
void SM_Wait_For_Adjust_Number_Of_Retransmission_Response(void);
void SM_Establish_Connection(void);
void SM_Wait_For_Establish_Connection_Response(void);
void SM_Send_Data(void);
void SM_Wait_For_Send_Data_Response(void);
void SM_Disconnection_Connection(void);
void SM_Wait_For_Disconnection_Connection_Response(void);



Sim3g_Machine_Type Sim3G_State_Machine [] = {
		{SIM3G_STATUS, 										SM_Sim3G_Status										},
		{WAIT_FOR_SIM3G_STATUS_RESPONSE, 					SM_Wait_For_Sim3g_Status_Response					},
		{POWER_ON_SIM3G, 									SM_Power_On_Sim3g									},
		{WAIT_FOR_SIM3G_POWER_ON, 							SM_Wait_For_Sim3g_Power_On							},
		{POWER_OFF_SIM3G, 									SM_Power_Off_Sim3g									},
		{WAIT_FOR_SIM3G_POWER_OFF, 							SM_Wait_For_Sim3g_Power_Off							},
		{DEFINE_PDP_CONTEXT, 								SM_Define_PDP_Context								},
		{WAIT_FOR_DEFINE_PDP_CONTEXT_RESPONSE, 				SM_Wait_For_Define_PDP_Context_Response				},
		{SET_AUTHENTICATION_PARAMETER, 						SM_Set_Authenticaton_Parameter						},
		{SET_PDP_CONTEXT_NUMBER, 							SM_Set_PDP_Context_Number							},
		{PDP_CONTEXT_ACTIVATION, 							SM_PDP_Context_Activation							},
		{WAIT_FOR_PDP_CONTEXT_ACTIVATION_RESPONSE, 			SM_Wait_For_PDP_Context_Activation_Response			},
		{PDP_CONTEXT_DEACTIVATION, 							SM_PDP_Context_Deactivation							},
		{RECEIVE_DATA_LENGTH_OPTION, 						SM_Receive_Data_Length_Option						},
		{RECEIVE_DATA_ADDRESS_OPTION, 						SM_Receive_Data_Address_Option						},
		{ADJUST_NUMBER_OF_RETRANSMISSION, 					SM_Adjust_Number_Of_Retransmission},
		{WAIT_FOR_ADJUST_NUMBER_OF_RETRANSMISSION_RESPONSE, SM_Wait_For_Adjust_Number_Of_Retransmission_Response},
		{ESTABLISH_CONNECTION, 								SM_Establish_Connection								},
		{WAIT_FOR_ESTABLISH_CONNECTION_RESPONSE, 			SM_Wait_For_Establish_Connection_Response			},
		{SEND_DATA, 										SM_Send_Data										},
		{WAIT_FOR_SEND_DATA_RESPONSE, 						SM_Wait_For_Send_Data_Response						},
		{DISCONNECT_CONNECTION, 							SM_Disconnection_Connection							},
		{WAIT_FOR_DISCONNECT_CONNECTION_RESPONSE, 			SM_Wait_For_Disconnection_Connection_Response		},
};



SIM3G_STATE sim3gState = SIM3G_STATUS;

void Sim3g_Run(void){
	if(sim3gState < MAX_NUMBER_STATES){
		(*Sim3G_State_Machine[sim3gState].func)();
	} else {
		//throw an excepton.
	}
}




void Sim3g_Init(void){

}

void Sim3g_Enable(void){

}

void Sim3g_WakeUp(void){

}
void Power_Signal_Low(void){

}
void Power_Signal_High(void){

}

void Sim3g_Clear_Timeout_Flag(void){
	timeoutFlag = 0;
}
void Sim3g_Command_Timeout(void){
	timeoutFlag = 1;
}
uint8_t is_Sim3g_Command_Timeout(void){
	return timeoutFlag;
}


static void setUpReceiveResponse(void){
	UART1_Receive();
}
static void ATcommandSending(uint8_t * buffer){
	if(isUART1TransmissionReady() == SET){
		UART1_Transmit(buffer);
	}
	setUpReceiveResponse();
}

void SM_Sim3G_Status(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT);
	sim3gState = WAIT_FOR_SIM3G_STATUS_RESPONSE;
}

void SM_Wait_For_Sim3g_Status_Response(void){
	if(isUART1ReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}

void SM_Power_On_Sim3g(void){
	SCH_Add_Task(Power_Signal_Low, 0, 0);
	SCH_Add_Task(Power_Signal_High, TIMER_TO_POWER_ON_SIM3G, 0);
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, TIMER_TO_POWER_ON_SIM3G,0);
	sim3gState = WAIT_FOR_SIM3G_POWER_ON;
}

void SM_Wait_For_Sim3g_Power_On(void){
	if(is_Sim3g_Command_Timeout()){
		sim3gState = SIM3G_STATUS;
	}
}
void SM_Power_Off_Sim3g(void){
	SCH_Add_Task(Power_Signal_Low, 0, 0);
	SCH_Add_Task(Power_Signal_High, TIMER_TO_POWER_ON_SIM3G, 0);
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, TIMER_TO_POWER_ON_SIM3G,0);
	sim3gState = WAIT_FOR_SIM3G_POWER_OFF;
}
void SM_Wait_For_Sim3g_Power_Off(void){
	if(is_Sim3g_Command_Timeout()){
		sim3gState = POWER_ON_SIM3G;
	}
}
void SM_Define_PDP_Context(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CGSOCKCONT);
	sim3gState = WAIT_FOR_DEFINE_PDP_CONTEXT_RESPONSE;
	
}
void SM_Wait_For_Define_PDP_Context_Response(void){
	if(isUART1ReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
void SM_Set_Authenticaton_Parameter(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CGSOCKAUTH);
	sim3gState = WAIT_FOR_DEFINE_PDP_CONTEXT_RESPONSE;
}
void SM_Set_PDP_Context_Number(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CGSOCKSETPN);
	sim3gState = WAIT_FOR_DEFINE_PDP_CONTEXT_RESPONSE;
}

void SM_PDP_Context_Activation(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_NETOPEN);
	sim3gState = WAIT_FOR_PDP_CONTEXT_ACTIVATION_RESPONSE;
	
}
void SM_Wait_For_PDP_Context_Activation_Response(void){
	if(isUART1ReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
void SM_PDP_Context_Deactivation(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_NETCLOSE);
	sim3gState = WAIT_FOR_PDP_CONTEXT_ACTIVATION_RESPONSE;
}
void SM_Receive_Data_Length_Option(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CIPHEAD);
	sim3gState = WAIT_FOR_PDP_CONTEXT_ACTIVATION_RESPONSE;
}
void SM_Receive_Data_Address_Option(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CIPSRIP);
	sim3gState = WAIT_FOR_PDP_CONTEXT_ACTIVATION_RESPONSE;
}
void SM_Adjust_Number_Of_Retransmission(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CIPCCFG);
	sim3gState = WAIT_FOR_PDP_CONTEXT_ACTIVATION_RESPONSE;
}
void SM_Wait_For_Adjust_Number_Of_Retransmission_Response(void){
	if(isUART1ReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}


void SM_Establish_Connection(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CIPOPEN);
	sim3gState = WAIT_FOR_ESTABLISH_CONNECTION_RESPONSE;
}
void SM_Wait_For_Establish_Connection_Response(void){
	if(isUART1ReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
void SM_Send_Data(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CIPOPEN);
	sim3gState = WAIT_FOR_SEND_DATA_RESPONSE;
}
void SM_Wait_For_Send_Data_Response(void){
	if(isUART1ReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}
void SM_Disconnection_Connection(void){
	Sim3g_Clear_Timeout_Flag();
	SCH_Add_Task(Sim3g_Command_Timeout, COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)AT_CIPOPEN);
	sim3gState = WAIT_FOR_DISCONNECT_CONNECTION_RESPONSE;
}
void SM_Wait_For_Disconnection_Connection_Response(void){
	if(isUART1ReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Sim3g_Command_Timeout()){

	}
}





