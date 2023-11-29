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
#define		SMS_BUFFER_LENGTH		30

#define		CTRL_Z	26

/*====================================================================*/
/*Firmware version management*/
/*Version 1.0.0:
Date: 22/02/2023
Content:
   - Add version and update fota
.0.2
- still on outlet when full charge or unplug
*/


#if(BOX_PLACE == BOX_WITH_6_OUTLETS)
	const uint8_t firmwareVersion[] =  "6OLs-";
#else

#if(VERSION_EBOX == VERSION_3_WITH_ALL_CT_5A)
	const uint8_t firmwareVersion[] =  "3.0.2-";
#elif(VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A)
	const uint8_t firmwareVersion[] =  "4.0.2-";
#elif(VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
	const uint8_t firmwareVersion[] =  "5.0.2-";
#elif(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	const uint8_t firmwareVersion[] =  "6.0.2-";

#endif

#endif

//const uint8_t smsCommand[] = "AT+CMGS=\"+84915075588\"\r";
const uint8_t turnOnPDUMode[] = "AT+CMGF=0\r";
const uint8_t turnOffPDUMode[] = "AT+CMGF=1\r";
const uint8_t smsCommand[] = "AT+CMGS=\"+84346702559\"\r";

const uint8_t atCUSDCommand[] ="AT+CUSD=1,\"*101#\",15\r";
const uint8_t smsNormalMessage[] =       "ALIVE - ";
const uint8_t smsFirstMessage[] =        "BOOT- ";
const uint8_t smsLostConnectionMessage[] = "Lost connection -";

uint8_t sendSmsFlag = 0;
uint8_t timeoutSendSmsFlag = 0;

uint8_t smsBufferIndex = 0;
uint8_t smsBuffer[SMS_BUFFER_LENGTH];

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



SMS_MESSAGE_TYPE smsMessageType = SMS_FIRST_MESSAGE_AFTER_REBOOT;


void Send_An_Sms_Message_Command(void);
void Send_Sms_Normal_Message(void);
void Send_Sms_First_Message(void);
void Send_Sms_Lost_Connection_Message(void);


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

void Send_Sms_Message_Init(void){
	smsState = TURN_ON_PDU;
	preSmsState = OTHERS;
	smsMessageType = SMS_FIRST_MESSAGE_AFTER_REBOOT;
}


void Set_Sms_Message_Type(SMS_MESSAGE_TYPE msgType){
	smsMessageType = msgType;
}

void Start_Sending_Sms_Message(void){
	Set_Send_Sms_Flag();
	Set_Timeout_Send_Sms_Flag();
	smsState = TURN_OFF_PDU;
}
void Start_Sending_Lost_Connection_Message(void){
	Set_Sms_Message_Type(SMS_LOST_CONNECTION);
	Start_Sending_Sms_Message();
}
uint8_t Is_Done_Sending_Sms_Message(void){
	return smsState == DONE_SENDING_SMS_MESSAGE;
}
void Send_Sms_Normal_Message(void){
	uint16_t boxID = Get_Box_ID();
	smsBufferIndex = 0;
	for (uint8_t i = 0; i < GetStringLength((uint8_t*)smsNormalMessage); i ++){
		smsBuffer[smsBufferIndex++] = smsNormalMessage[i];
	}
	if(smsBufferIndex > SMS_BUFFER_LENGTH - 6) return;

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
void Send_Sms_First_Message(void){

	uint16_t boxID = Get_Box_ID();
	smsBufferIndex = 0;

	for (uint8_t i = 0; i < GetStringLength((uint8_t*)smsFirstMessage); i ++){
		smsBuffer[smsBufferIndex++] = smsFirstMessage[i];
	}
	for (uint8_t i = 0; i < GetStringLength((uint8_t*)firmwareVersion); i ++){
		smsBuffer[smsBufferIndex++] = firmwareVersion[i];
	}
	if(smsBufferIndex > SMS_BUFFER_LENGTH - 6) return;

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

void Send_Sms_Lost_Connection_Message(void){

	uint16_t boxID = Get_Box_ID();
	smsBufferIndex = 0;
	for (uint8_t i = 0; i < GetStringLength((uint8_t*)smsLostConnectionMessage); i ++){
		smsBuffer[smsBufferIndex++] = smsLostConnectionMessage[i];
	}
	if(smsBufferIndex > SMS_BUFFER_LENGTH - 6) return;

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



void Send_An_Sms_Message_Command(void){
	isReadyToSendDataToServer = RESET;
	ATcommandSending((uint8_t*)smsCommand);
}
void Send_An_Sms_Message(void){
	if(smsMessageType == SMS_FIRST_MESSAGE_AFTER_REBOOT){
		Send_Sms_First_Message();
	} else if(smsMessageType == SMS_NORMAL_MESSAGE){
		Send_Sms_Normal_Message();
	} else if(smsMessageType == SMS_LOST_CONNECTION){
		Send_Sms_Lost_Connection_Message();
	}
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
				Send_An_Sms_Message_Command();
				smsState = WAIT_FOR_READY_THEN_SEND_CONTENT;
			break;
		case WAIT_FOR_READY_THEN_SEND_CONTENT:
			if(isReadyToSendDataToServer){
				Send_An_Sms_Message();
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
				Set_Sms_Message_Type(SMS_NORMAL_MESSAGE);
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


