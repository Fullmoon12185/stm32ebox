/**
 * @file 	: app_http.h
 *  Created on: Aug 6, 2021
 * @author: thodo
 * @brief Header file for app_http.c file. This file implement state machine for HTTP Operation: Get Board Information, Get Number of Cabinet.
 */

#ifndef INC_SERVER_COMMUNICATION_APP_HTTP_H_
#define INC_SERVER_COMMUNICATION_APP_HTTP_H_
#include "main.h"

/*
*@brief Describe state of HTTP Operation
*/
typedef enum{
	HTTP_INIT = 0, 				/*!< This is state for init http service*/
	HTTP_WAIT_FOR_INIT,			/*!< This is state for waiting init service respone and switch to specific state*/

	HTTP_PARA,					/*!< This is state for pass parameter URL,User-Agent,... to HTTP Request */
	HTTP_WAIT_FOR_PARA,			/*!< This is state for wait for HTTP_PARA respone, It can OK or ERROR*/

	HTTP_ACTION,				/*!< This is state for passing ACTION argument to simcom7600 ,example: 0 = GET , 1 = POST ,...*/
	HTTP_WAIT_FOR_ACTION,		/*!< This is state for waiting HTTP_ACTION respone, it can OK or ERROR */

	HTTP_READ,					/*!< This is state for READ body from HTTP respone */
	HTTP_WAIT_FOR_READ,			/*!< This is state for waiting HTTP_READ respone, It can be OK or ERROR*/

	HTTP_TERM,					/*!< This is state for terminating HTTP service*/
	HTTP_WAIT_FOR_TERM,			/*!< This is state for waiting respone from HTTP_TERM, It can be OK or ERROR*/

	HTTP_DONE,					/*!< This is state for notice that HTTP Operation is done.*/
	HTTP_MAX_STATE				/*!< This is state for notice that HTTP Service is Error, that need to reset Simcom7600 and rerun service*/
}HTTP_State;


typedef enum{
	DONE,
	PROCESSING,
	ERR_CHECKSUM,
	ERR_CURRENT_FIRMWARE_ADDRESS_WRONG
}Firmware_Data_State;



typedef struct{
	HTTP_State http_state;		/*!< This is HTTP_State of State Machine. It help to manage state easier*/
	void(*func)(void);			/*!< This is function pointer when State Machine get onto http_state above so It will run*/
}HTTP_Machine_TypeDef;

uint8_t HTTP_Run();
HTTP_State HTTP_Get_State();
void HTTP_Set_State(HTTP_State http_state);
FlagStatus HTTP_is_Done();



void HTTP_Init();
void HTTP_Wait_For_Init();
void HTTP_Para();
void HTTP_Wait_For_Para();
void HTTP_Action();
void HTTP_Wait_For_Action();
void HTTP_Read();
void HTTP_Wait_For_Read();
void HTTP_Term();
void HTTP_Wait_For_Term();
FlagStatus is_Http_TimeOutFlag(void);
void Set_Http_Timeout_Flag(void);
void Clear_Http_Timeout_Flag(void);
uint16_t Get_IntegerValue_From_HTTP_Respone();
FlagStatus Get_CabinetInfo_From_HTTP_Respone(uint16_t cabinetID);
void Reset_Result();
FlagStatus HTTP_Get_Content_Length();
void Clear_Http_Command();
void Set_Http_Command(char * atcommand);

uint32_t HTTP_Return_Content_Length();

void Reset_HttpConfiguration_State();
#endif /* INC_SERVER_COMMUNICATION_APP_HTTP_H_ */
