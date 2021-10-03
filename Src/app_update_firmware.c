/*
 * app_update_firmware.c
 *
 *  Created on: Aug 6, 2021
 *      Author: Nguyet
 */


#include "main.h"

#include "app_flash_internal.h"
#include "app_uart.h"
#include "app_sim3g.h"

#include "app_string.h"

#define	DEBUG_UPDATE_FIMWARE(X) 			X

#define 	LENGTH_OF_IGNORED_DATA	10
#define		TIME_OUT_FOR_UPDATE_FIRMWARE	10000


uint8_t	Flash_PageSize_Buffer_1[FLASH_PAGE_SIZE+1];
uint8_t	Flash_PageSize_Buffer_2[FLASH_PAGE_SIZE+1];
uint16_t indexForUpdateFirmwareBuffer = 0;
uint16_t indexForReceivedPackage = 0;

uint16_t receivedUpdateFirmwareDataLength = 0;
uint16_t prePackageIndex = 0;
uint16_t packageIndex = 0;


uint8_t updateFirmwareTimeoutFlag  = 0;
uint32_t updateFirmwareTimeoutIndex = NO_TASK_ID;
extern uint8_t SUBSCRIBE_UPDATE_FIRMWARE[MAX_TOPIC_LENGTH];


typedef enum{
	SETUP_FOR_UPDATE_FIRMWARE = 0,
	READY_TO_RECEIVE_FIRMWARE,
	CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE,
	DETECT_SPECIAL_CHARACTER_FOR_UPDATE_FIRMWARE,
	CHECK_TOPIC_NAME,
	CHECK_PACKAGE_INDEX,
	PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_1,
	PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_2,
	START_WRITE_RECEIVED_DATA_TO_FLASH,
	ASKING_RESEND_PREVIOUS_PACKAGE,
	RETURN_ACK_FOR_UPDATE_FIRMWARE,
	UPDATE_FIRMWARE_FINISHED,
	MAX_NUMBER_STATE_OF_PROCESSING_FOR_UPDATE_FIRMWARE
}PROCESS_UPDATE_FIRMWARE;

PROCESS_UPDATE_FIRMWARE processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;

void FSM_For_Update_Firmware(void);
void Send_ACK(const uint8_t * pubmes);

void Clear_Update_Firmware_Timeout_Flag(void);
void Set_Update_Firmware_Timeout_Flag(void);
uint8_t Is_Update_Firmware_Timeout_Flag(void);
void Update_Firmware_State_Display(void);

void Clear_Update_Firmware_Timeout_Flag(void){
	updateFirmwareTimeoutFlag = 0;
}
void Set_Update_Firmware_Timeout_Flag(void){
	updateFirmwareTimeoutFlag = 1;
}
uint8_t Is_Update_Firmware_Timeout_Flag(void){
	return (updateFirmwareTimeoutFlag == 1);
}

void Reset_Update_Firmware_State(void){
	processUpdateFirmwareState = READY_TO_RECEIVE_FIRMWARE;
}
void Update_Firmware_State_Display(void){
	static PROCESS_UPDATE_FIRMWARE pre_processUpdateFirmwareState;
	if(pre_processUpdateFirmwareState != processUpdateFirmwareState){
		pre_processUpdateFirmwareState = processUpdateFirmwareState;
		switch(processUpdateFirmwareState){
		case SETUP_FOR_UPDATE_FIRMWARE:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"SETUP_FOR_UPDATE_FIRMWARE\r\n"););
			break;
		case READY_TO_RECEIVE_FIRMWARE:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"READY_TO_RECEIVE_FIRMWARE\r\n"););
			break;
		case CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE\r\n"););
			break;
		case DETECT_SPECIAL_CHARACTER_FOR_UPDATE_FIRMWARE:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"DETECT_SPECIAL_CHARACTER_FOR_UPDATE_FIRMWARE\r\n"););
			break;
		case CHECK_TOPIC_NAME:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"CHECK_TOPIC_NAME\r\n"););
			break;
		case CHECK_PACKAGE_INDEX:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"CHECK_PACKAGE_INDEX\r\n"););
			break;
		case PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_1:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_1\r\n"););
			break;
		case PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_2:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_2\r\n"););
			break;
		case START_WRITE_RECEIVED_DATA_TO_FLASH:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"START_WRITE_RECEIVED_DATA_TO_FLASH\r\n"););
			break;
		case ASKING_RESEND_PREVIOUS_PACKAGE:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"ASKING_RESEND_PREVIOUS_PACKAGE\r\n"););
			break;
		case RETURN_ACK_FOR_UPDATE_FIRMWARE:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"RETURN_ACK_FOR_UPDATE_FIRMWARE\r\n"););
			break;
		case UPDATE_FIRMWARE_FINISHED:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"UPDATE_FIRMWARE_FINISHED\r\n"););
			break;
		case MAX_NUMBER_STATE_OF_PROCESSING_FOR_UPDATE_FIRMWARE:
			DEBUG_UPDATE_FIMWARE(UART3_SendToHost((uint8_t*)"MAX_NUMBER_STATE_OF_PROCESSING_FOR_UPDATE_FIRMWARE\r\n"););
			break;

		default:
			break;
		}
	}

}
void Processing_Update_Firmware(void){
	Clear_Update_Firmware_Timeout_Flag();
	updateFirmwareTimeoutIndex = SCH_Add_Task(Set_Update_Firmware_Timeout_Flag, TIME_OUT_FOR_UPDATE_FIRMWARE, 0);
	Reset_Update_Firmware_State();
	while(processUpdateFirmwareState != UPDATE_FIRMWARE_FINISHED){
		Update_Firmware_State_Display();
		FSM_For_Update_Firmware();
		if(Is_Update_Firmware_Timeout_Flag()){
			break;
		}
	}
}

void Send_ACK(const uint8_t * pubmes){
	if(GetStringLength(pubmes) > 0)
		Setup_Mqtt_Publish_Message(SUBCRIBE_UPDATE_FIRMWARE, pubmes, GetStringLength(pubmes));
}



void FSM_For_Update_Firmware(void){
	static uint8_t readCharacter_1, readCharacter_2, readCharacter_3, readCharacter_4 ;
	switch(processUpdateFirmwareState){
	case READY_TO_RECEIVE_FIRMWARE:
		Send_ACK((uint8_t*)"ready");
		processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
		break;
	case CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE:
		if(Uart1_Received_Buffer_Available()){
			processUpdateFirmwareState = DETECT_SPECIAL_CHARACTER_FOR_UPDATE_FIRMWARE;
		}
		break;
	case DETECT_SPECIAL_CHARACTER_FOR_UPDATE_FIRMWARE:
		if(Uart1_Received_Buffer_Available()){

			readCharacter_1 = readCharacter_2;
			readCharacter_2 = readCharacter_3;
			readCharacter_3 = readCharacter_4;
			readCharacter_4 = Uart1_Read_Received_Buffer();
			if(isEndOfCommand(readCharacter_3, readCharacter_4) == 1){
				processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
			}
			else if(readCharacter_2 == SUBSCRIBE_RECEIVE_MESSAGE_TYPE
					&& readCharacter_4 == GetStringLength((uint8_t*)SUBSCRIBE_UPDATE_FIRMWARE))
			{
				indexForReceivedPackage = 0;
				receivedUpdateFirmwareDataLength = readCharacter_3;
				processUpdateFirmwareState = CHECK_TOPIC_NAME;
			}
			else if(readCharacter_1 == SUBSCRIBE_RECEIVE_MESSAGE_TYPE
					&& readCharacter_4 == GetStringLength((uint8_t*)SUBSCRIBE_UPDATE_FIRMWARE))
			{
				indexForReceivedPackage = 0;
				receivedUpdateFirmwareDataLength = readCharacter_2 + (readCharacter_3 - 1)*128;
				processUpdateFirmwareState = CHECK_TOPIC_NAME;
			}
		}
		break;
	case CHECK_TOPIC_NAME:
		if(Uart1_Received_Buffer_Available()){
			readCharacter_1 = readCharacter_2;
			readCharacter_2 = readCharacter_3;
			readCharacter_3 = readCharacter_4;
			readCharacter_4 = Uart1_Read_Received_Buffer();
			if(isEndOfCommand(readCharacter_3, readCharacter_4) == 1){
				processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
			} else if(readCharacter_1 == 'U' &&
					readCharacter_2 == 'E' &&
					readCharacter_3 == 'b' &&
					readCharacter_4 == 'o') {
				processUpdateFirmwareState = CHECK_PACKAGE_INDEX;
			}
		}
		break;
	case CHECK_PACKAGE_INDEX:
		if(Uart1_Received_Buffer_Available()){
			readCharacter_1 = readCharacter_2;
			readCharacter_2 = readCharacter_3;
			readCharacter_3 = readCharacter_4;
			readCharacter_4 = Uart1_Read_Received_Buffer();
			if(isEndOfCommand(readCharacter_3, readCharacter_4) == 1){
				processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
			} else if(readCharacter_4 == ':') {
				packageIndex = (readCharacter_1 - 0x30)*100 + (readCharacter_2 - 0x30)*10 + (readCharacter_3 - 0x30);
				if(packageIndex == prePackageIndex + 1){
					prePackageIndex = packageIndex;
					processUpdateFirmwareState = PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_1;
				} else {
					processUpdateFirmwareState = ASKING_RESEND_PREVIOUS_PACKAGE;
				}
			}
		}
		break;
	case PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_1:
		if(Uart1_Received_Buffer_Available()){
			readCharacter_3 = readCharacter_4;
			readCharacter_4 = Uart1_Read_Received_Buffer();
			if(isEndOfCommand(readCharacter_3, readCharacter_4) == 1){
				processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
			}
			else
			{
				if(indexForUpdateFirmwareBuffer < FLASH_PAGE_SIZE){
					Flash_PageSize_Buffer_1[indexForUpdateFirmwareBuffer] = readCharacter_4;
					indexForUpdateFirmwareBuffer++;
					indexForReceivedPackage++;
					Flash_PageSize_Buffer_1[indexForUpdateFirmwareBuffer] = 0;
					if(indexForUpdateFirmwareBuffer == FLASH_PAGE_SIZE - 1){
						processUpdateFirmwareState = START_WRITE_RECEIVED_DATA_TO_FLASH;
					} else if(indexForReceivedPackage >= receivedUpdateFirmwareDataLength - LENGTH_OF_IGNORED_DATA){
						processUpdateFirmwareState = RETURN_ACK_FOR_UPDATE_FIRMWARE;
					}
				}
			}
		}
		break;
	case PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_2:

		break;
	case START_WRITE_RECEIVED_DATA_TO_FLASH:
		//start writing to flash

		//reset index
		indexForUpdateFirmwareBuffer = 0;
		processUpdateFirmwareState = RETURN_ACK_FOR_UPDATE_FIRMWARE;
		break;
	case ASKING_RESEND_PREVIOUS_PACKAGE:
		Send_ACK((uint8_t*)"resend");
		processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
		break;
	case RETURN_ACK_FOR_UPDATE_FIRMWARE:
		//send ack back
		Send_ACK((uint8_t*)"ack");
		if(indexForReceivedPackage < 128){
			processUpdateFirmwareState = UPDATE_FIRMWARE_FINISHED;
		} else {
			processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
		}
		break;
	case UPDATE_FIRMWARE_FINISHED:

		break;
	default:
		break;
	}

}
