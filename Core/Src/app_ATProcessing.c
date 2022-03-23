/*
 * app_ATProcessing.c
 *
 *  Created on: Sep 10, 2021
 *      Author: thodo
 */

#include <Utility/app_string.h>
#include "app_ATProcessing.h"

uint8_t temp;

//AT result
AT_Result at_result = AT_NONE;						/*!< AT Result when UART RX receive from huart1 SIMCOM7600*/
static uint8_t data_respone[UART_RX_BUFFERSIZE];					/*!< buffer contains temporatory data when handle respond from UART SIMCOM7600 */
static uint8_t data_respone_length;						/*!< data_length for access from external function*/
static uint8_t data_respone_index = 0;						/*!< index of data_respone when handle respond from UART SIMCOM7600 */

//Expected Result
const uint8_t PB_DONE[] = "PB DONE";				/*!< Simcom7600 will respond "PB DONE" when it start up*/
//const uint8_t OK[] = "OK";							/*!< Simcom7600 will respond "OK" when ATcommand valid*/
const char* OK = "OK";
const uint8_t ERROR_1[] = "ERROR";					/*!< Simcom7600 will respond "ERROR"  when ATcommand invalid*/
//const uint8_t INPUT[] = ">";						/*!< Simcom7600 will respond ">" when SIMCOM7600 require input*/
//const uint8_t HTTP_RESPONE_CHECK[] = "integerValue";/*!< Simcom7600 will respond "integerValue" when HTTP respond from Simcom7600 has "intergerValue" field*/
//const uint8_t HTTP_NOT_FOUND[] = "NOT_FOUND";		/*!< Simcom7600 will respond "NOT_FOUND" when HTTP request invalid*/
const uint8_t IMEI_CHECK[] = "+SIMEI: ";			/*!< Simcom7600 will respond "+SIMEI: " when ATcommand request IMEI number*/
const uint8_t RECEIVE_RESPONSE[] = "+HTTPACTION:";
const uint8_t FIRMWARE_VERSION[] = "new_version";
const uint8_t FIRMWARE_CHECKSUM[] = "checksum";
const uint8_t FIRMWARE_DATA[] = "\"data\":";



/**
  * @brief Wait_For_Respone Initialization Function
  * Every external call this function, It will check Available of RX Buffer.
  * If Available It will Read 1 byte and add to temp buffer.
  * Then Temp buffer will be compared to pattern being passed in isReceiveData function.
  * @param None
  * @retval None
  */
void AT_Processing(){
	if(at_result != AT_NONE){
		return;
	}
	else if(UART_SIM7600_Received_Buffer_Available()){
		data_respone[data_respone_index]=UART_SIM7600_Read_Received_Buffer();
//		UART_DEBUG_Transmit_Size(data_respone + data_respone_index, 1);
//		LOG("1");
		data_respone_index++;

		if(isReceiveData((char*)data_respone, (uint16_t)data_respone_index, OK)){
			LOG("\r\nOK\r\n");
			data_respone_length = data_respone_index;
			data_respone_index = 0;
			at_result = AT_OK;
		}
		else if(isReceiveData((char*)data_respone, data_respone_index, (char*)ERROR_1/*ERROR_1*/)){
			LOG("\r\nERROR\r\n");
			data_respone_length = data_respone_index;
			data_respone_index = 0;
			at_result = AT_ERROR;
		}
		else if(isReceiveData((char*)data_respone, data_respone_index, (char*)PB_DONE /*PB_DONE*/)){
			LOG("\r\nPB DONE\r\n");
			data_respone_length = data_respone_index;
			data_respone_index = 0;
			at_result = AT_PB_DONE;
		}
		else if(isReceiveData((char*)data_respone, data_respone_index, (char*)IMEI_CHECK)){
			LOG("\r\nRECEIVE IMEI\r\n");
			data_respone_length = data_respone_index;
			data_respone_index = 0;
			at_result = AT_IMEI;
		}
		else if(isReceiveData((char*)data_respone, data_respone_index, (char*)RECEIVE_RESPONSE)){
			LOG("\r\nRECEIVE HTTP RESPONSE\r\n");
			data_respone_length = data_respone_index;
			data_respone_index = 0;
			at_result = AT_HTTP_RESPONSE;
		}
		else if(isReceiveData((char*)data_respone, data_respone_index, (char*)FIRMWARE_VERSION)){
			LOG("\r\nRECEIVE VERSION RESPONSE\r\n");
			data_respone_length = data_respone_index;
			data_respone_index = 0;
			at_result = AT_FIRMWARE_VERSION;
		}
		else if(isReceiveData((char*)data_respone, data_respone_index, (char*)FIRMWARE_DATA)){
			LOG("\r\nRECEIVE FIRMWARE DATA\r\n");
			data_respone_length = data_respone_index;
			data_respone_index = 0;
			at_result = AT_FIRMWARE_DATA;
		}
		else if(isReceiveData((char*)data_respone, data_respone_index, (char*)FIRMWARE_CHECKSUM)){
			LOG("\r\nRECEIVE FIRMWARE CHECKSUM\r\n");
			data_respone_length = data_respone_index;
			data_respone_index = 0;
			at_result = AT_FIRMWARE_CHECKSUM;
		}
	}
//	UART_DEBUG_Transmit_Size(aUART_RxBuffer, RXBUFFERSIZE);
	return;
}


/**
  * @brief Get_AT_Result Initialization Function
  * @param None
  * @retval at_result
  */
AT_Result Get_AT_Result(){
	return at_result;
}


/**
  * @brief Clear_AT_Result Initialization Function
  * @param None
  * @retval None
  */
void Clear_AT_Result(){
	at_result = AT_NONE;
}
