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
uint8_t	Flash_PageSize_Buffer_1[FLASH_PAGE_SIZE+1];
uint8_t	Flash_PageSize_Buffer_2[FLASH_PAGE_SIZE+1];
uint16_t indexForUpdateFirmwareBuffer = 0;
uint16_t receivedUpdateFirmwareDataLength = 0;

typedef enum{
	SETUP_FOR_UPDATE_FIRMWARE = 0,
	CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE,
	DETECT_SPECIAL_CHARACTER_FOR_UPDATE_FIRMWARE,
	PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_1,
	PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_2,
	START_WRITE_RECEIVED_DATA_TO_FLASH,
	RETURN_ACK_FOR_UPDATE_FIRMWARE,
	MAX_NUMBER_STATE_OF_PROCESSING_FOR_UPDATE_FIRMWARE
}PROCESS_UPDATE_FIRMWARE;

PROCESS_UPDATE_FIRMWARE processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;


void FSM_For_Update_Firmware(void){
	static uint8_t readCharacter_1, readCharacter_2, readCharacter_3, readCharacter_4 ;
	switch(processUpdateFirmwareState){
	case SETUP_FOR_UPDATE_FIRMWARE:


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
					&& readCharacter_4 == GetStringLength((uint8_t*)SUBSCRIBE_TOPIC_2))
			{
				indexForUpdateFirmwareBuffer = 0;
				receivedUpdateFirmwareDataLength = readCharacter_3;
				processDataState = PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_1;
			}
			else if(readCharacter_1 == SUBSCRIBE_RECEIVE_MESSAGE_TYPE
					&& readCharacter_4 == GetStringLength((uint8_t*)SUBSCRIBE_TOPIC_2))
			{
				indexForUpdateFirmwareBuffer = 0;
				receivedUpdateFirmwareDataLength = readCharacter_2 + (readCharacter_3 - 1)*128;
				processDataState = PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_1;
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
					Flash_PageSize_Buffer_1[indexForUpdateFirmwareBuffer] = 0;
					if(indexForUpdateFirmwareBuffer < receivedUpdateFirmwareDataLength - 4){
						processUpdateFirmwareState = START_WRITE_RECEIVED_DATA_TO_FLASH;
					}
				}
			}
		}
		break;
	case PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE_2:
		break;
	case START_WRITE_RECEIVED_DATA_TO_FLASH:
		processUpdateFirmwareState = RETURN_ACK_FOR_UPDATE_FIRMWARE;
		break;
	case RETURN_ACK_FOR_UPDATE_FIRMWARE:

		processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
		break;
	default:
		break;
	}

}
