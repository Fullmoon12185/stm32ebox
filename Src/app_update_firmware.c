/*
 * app_update_firmware.c
 *
 *  Created on: Aug 6, 2021
 *      Author: Nguyet
 */


#include "app_uart.h"
#include "app_sim3g.h"

#include "app_uart.h"
#include "app_sim3g.h"
uint8_t	Flash_PageSize_Buffer_1[FLASH_PAGE_SIZE+1];
uint8_t	Flash_PageSize_Buffer_2[FLASH_PAGE_SIZE+1];


typedef enum{
	SETUP_FOR_UPDATE_FIRMWARE = 0,
	CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE,
	DETECT_SPECIAL_CHARACTER_FOR_UPDATE_FIRMWARE,
	PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE,
	RETURN_ACK_FOR_UPDATE_FIRMWARE,
	MAX_NUMBER_STATE_OF_PROCESSING_FOR_UPDATE_FIRMWARE
}PROCESS_UPDATE_FIRMWARE;

PROCESS_UPDATE_FIRMWARE processUpdateFirmwareState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;


void FSM_For_Update_Firmware(void){
	static uint8_t preReadCharacter,readCharacter;
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
//		if(Uart1_Received_Buffer_Available()){
//
//			preReadCharacter = readCharacter;
//			readCharacter = Uart1_Read_Received_Buffer();
//			if(readCharacter == '>'){
//				processDataState = PREPARE_FOR_SENDING_ACK;
//			}
//			else if(isEndOfCommand(preReadCharacter, readCharacter) == 1){
//				processDataState = CHECK_DATA_AVAILABLE_STATE_FOR_UPDATE_FIRMWARE;
//			}
//			else if((preReadCharacter == SUBSCRIBE_RECEIVE_MESSAGE_TYPE)
//					&& (readCharacter == LEN_FOR_UPDATE_POWER_CONSUPMPTION)){ //For update total power consumption
//				processDataState = PROCESSING_UPDATE_TOTAL_POWER_CONSUMPTION;
//				Clear_Sim3gDataProcessingBuffer();
//			}
//			else if((preReadCharacter == SUBSCRIBE_RECEIVE_MESSAGE_TYPE)
//					&& (readCharacter == LEN_FOR_UPDATE_FIRMWARE)){
////				processDataState = PROCESSING_UPDATE_FIRMWARE;
//				Clear_Sim3gDataProcessingBuffer();
//			}
//			else {
////				if(sim3gDataProcessingBufferIndex < RXBUFFERSIZE){
////					Sim3gDataProcessingBuffer[sim3gDataProcessingBuferIndex++] = readCharacter;
////				} else {
////					sim3gDataProcessingBufferIndex = 0;
////				}
//			}
//		}
		break;
	case PROCESSING_RECEIVED_DATA_FOR_UPDATE_FIRMWARE:
		break;
	case RETURN_ACK_FOR_UPDATE_FIRMWARE:
		break;
	default:
		break;
	}

}
