/*
 * app_send_sms.c
 *
 *  Created on: Nov 20, 2022
 *      Author: Nguyet
 */
#include "main.h"
#include "app_string.h"
#include "app_sim3g.h"
#include "app_send_sms.h"
#include "app_sim5320MQTT.h"
#include "app_scheduler.h"


#define		DEBUG_SEND_SMS(x)		x


#define		CTRL_Z	26

uint8_t smsBuffer[40];
uint8_t smsBufferIndex = 0;
//const uint8_t smsCommand[] = "AT+CMGS=\"+84915075588\"\r";
const uint8_t turnOnPDUMode[] = "AT+CMGF=0\r";
const uint8_t turnOffPDUMode[] = "AT+CMGF=1\r";
const uint8_t smsCommand[] = "AT+CMGS=\"+84902860954\"\r";

const uint8_t atCUSDCommand[] ="AT+CUSD=1,\"*101#\",15\r";
const uint8_t smsMessage[] = "Hi Stefan, I'm still alive - ";

uint8_t sendSmsFlag = 0;
uint8_t timeoutSendSmsFlag = 0;


extern FlagStatus isReadyToSendDataToServer;
extern FlagStatus isOKFlag;
extern FlagStatus isErrorFlag;

typedef enum {
	TURN_OFF_PDU = 0,
	WAIT_FOR_TURN_OFF_PDU_MODE,
	SENDING_COMMAND,
	WAIT_FOR_READY_THEN_SEND_CONTENT,
	TURN_ON_PDU,
	WAIT_FOR_TURN_ON_PDU_MODE,
	DONE_SENDING_SMS_MESSAGE,
	OTHERS
} SMS_STATE;

static SMS_STATE smsState = TURN_ON_PDU;
static SMS_STATE preSmsState = OTHERS;

void Send_An_Sms_Message(void);
void Send_Sms_Content(void);

void Set_Send_Sms_Flag(void){
	sendSmsFlag = 1;
}
void Clear_Send_Sms_Flag(void){
	sendSmsFlag = 0;
}
uint8_t Is_Set_Send_Sms_Flag(void){
	return sendSmsFlag == 1;
}
void Set_Timeout_Send_Sms_Flag(void){
	timeoutSendSmsFlag = 1;
}
void Clear_Timeout_Send_Sms_Flag(void){
	timeoutSendSmsFlag = 0;
}
uint8_t Is_Set_Timeout_Send_Sms_Flag(void){
	return timeoutSendSmsFlag == 1;
}



void Start_Sending_Sms_Message(void){
	Set_Send_Sms_Flag();
	Set_Timeout_Send_Sms_Flag();
	smsState = TURN_OFF_PDU;
}
uint8_t Is_Done_Sending_Sms_Message(void){
	return smsState == DONE_SENDING_SMS_MESSAGE;
}
void Send_Sms_Content(void){

	uint16_t boxID = Get_Box_ID();
	for (uint8_t i = 0; i < GetStringLength((uint8_t*)smsMessage); i ++){
		smsBuffer[smsBufferIndex++] = smsMessage[i];
	}
	smsBuffer[smsBufferIndex++] = boxID / 1000 + 0x30;
	boxID = boxID % 1000;
	smsBuffer[smsBufferIndex++] = boxID / 100 + 0x30;
	boxID = boxID % 100;
	smsBuffer[smsBufferIndex++] = boxID / 10 + 0x30;
	boxID = boxID % 10;
	smsBuffer[smsBufferIndex++] = boxID + 0x30;
	smsBuffer[smsBufferIndex++] = CTRL_Z;
	smsBuffer[smsBufferIndex++] = 0;

	ATcommandSending((uint8_t*)smsBuffer);
}

void Send_An_Sms_Message(void){
	isReadyToSendDataToServer = RESET;
	ATcommandSending((uint8_t*)smsCommand);
}

void Turn_Off_PDU_Mode(void){
	isOKFlag = RESET;
	ATcommandSending((uint8_t*)turnOffPDUMode);
}
void Turn_On_PDU_Mode(void){
	isOKFlag = RESET;
	ATcommandSending((uint8_t*)turnOnPDUMode);
}

void Send_SMS_State_Display(void){
	if(preSmsState != smsState){
		preSmsState = smsState;
		switch(smsState){
		case TURN_OFF_PDU:
			DEBUG_SEND_SMS(UART3_SendToHost((uint8_t*)"TURN_OFF_PDU\r\n"););
		break;
		case SENDING_COMMAND:
			DEBUG_SEND_SMS(UART3_SendToHost((uint8_t*)"SENDING_COMMAND\r\n"););
			break;
		case WAIT_FOR_READY_THEN_SEND_CONTENT:
			DEBUG_SEND_SMS(UART3_SendToHost((uint8_t*)"WAIT_FOR_READY_THEN_SEND_CONTENT\r\n"););
			break;
		case TURN_ON_PDU:
			DEBUG_SEND_SMS(UART3_SendToHost((uint8_t*)"TURN_ON_PDU\r\n"););
		break;
		case DONE_SENDING_SMS_MESSAGE:
			DEBUG_SEND_SMS(UART3_SendToHost((uint8_t*)"DONE_SENDING_SMS_MESSAGE\r\n"););
			break;
		default:
			break;
		}
	}
}

void FSM_For_Sending_SMS_Message(void){

	if(Is_Set_Send_Sms_Flag() && Is_Ready_To_Send_MQTT_Data()){
		Send_SMS_State_Display();
		switch(smsState){
		case TURN_OFF_PDU:
			if(Is_Set_Timeout_Send_Sms_Flag()){
				isOKFlag = RESET;
				isErrorFlag = RESET;
				Turn_Off_PDU_Mode();
				smsState = WAIT_FOR_TURN_OFF_PDU_MODE;
			}
			break;
		case WAIT_FOR_TURN_OFF_PDU_MODE:
			if(isOKFlag){
				smsState = SENDING_COMMAND;
			}
			if(isErrorFlag){
				Clear_Timeout_Send_Sms_Flag();
				SCH_Add_Task(Set_Timeout_Send_Sms_Flag, 300, 0);
				smsState = TURN_OFF_PDU;
			}
			break;
		case SENDING_COMMAND:
				Send_An_Sms_Message();
				smsState = WAIT_FOR_READY_THEN_SEND_CONTENT;
			break;
		case WAIT_FOR_READY_THEN_SEND_CONTENT:
			if(isReadyToSendDataToServer){
				Send_Sms_Content();
				Clear_Timeout_Send_Sms_Flag();
				SCH_Add_Task(Set_Timeout_Send_Sms_Flag, 300, 0);
				smsState = TURN_ON_PDU;
			}
			break;
		case TURN_ON_PDU:
			if(Is_Set_Timeout_Send_Sms_Flag()){
				Turn_On_PDU_Mode();
				isOKFlag = RESET;
				isErrorFlag = RESET;
				smsState = WAIT_FOR_TURN_ON_PDU_MODE;
			}
			break;
		case WAIT_FOR_TURN_ON_PDU_MODE:
			if(isOKFlag){
				Clear_Send_Sms_Flag();
				smsState = DONE_SENDING_SMS_MESSAGE;
			}
			if(isErrorFlag){
				Clear_Timeout_Send_Sms_Flag();
				SCH_Add_Task(Set_Timeout_Send_Sms_Flag, 300, 0);
				smsState = TURN_ON_PDU;
			}
			break;
		case DONE_SENDING_SMS_MESSAGE:

			break;
		default:
			smsState = TURN_OFF_PDU;
			break;
		}
	}

}


