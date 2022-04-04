/**
 * @file app_mqtt.h
 * @author thodo
 * @brief Header file for app_mqtt.c. This file has some function for get information from internet using MQTT Protocol.
 */

#ifndef INC_SERVER_COMMUNICATION_APP_MQTT_H_
#define INC_SERVER_COMMUNICATION_APP_MQTT_H_


#include <Peripheral/app_uart.h>
#include "stdio.h"

#define MAX_PAYLOAD_LENGTH 30
#define kClientidSize 	2
#define kTopicSize 		2

/*
 * @struct MQTTClient_TypeDef
 * @brief This is MQTTClient configuratation
 */
typedef struct {
	  uint8_t 	client_index; 		/*!<	client_index = 0 or 1	*/
	  char  	clientid[50];			/*!<	clientid is string, It can be "Locker"	*/
	  char		*server_address;	/*!<	server_address is mqtt_server address of MQTT Broker	*/
	  char 		*user;				/*!<	user is string for authentication MQTTClient, It can be "logUser"	*/
	  char 		*pass;				/*!<	pass is string for authentication MQTTClient, It can be "logPwd"	*/
	  uint8_t 	clean_session;		/*!<	clean_session is flag for whether keep session when MQTT Client be disconnected*/
	  uint8_t 	keepAlive;			/*!<	keepAlive is time for every MQTTClient resend message to notice that is still on air	*/
}MQTTClient_TypeDef;

/*
 * @struct MESSAGE_TypeDef
 * @brief This is all message attribute
 */
typedef struct {
	  char topic[50];					/*!<	topic is string name for exchange message between 2 or more MQTTClient, It can be "commnad"	*/
	  char payload[200];				/*!<	payload is string message be exchanged between 2 or more MQTTClient, It can be "status=1"	*/
	  uint8_t qos;					/*!<	qos is quality of serveice
	   	   	   	   	   	   	   	   	   	   	0: only 1 time
	   	   	   	   	   	   	   	   	   	   	1: exactly 1 time
	   	   	   	   	   	   	   	   	   	   	2: least 1 time   */
	  uint8_t pub_timeout ;			/*!<	pub_timeout is timeout for publish operation : range 60~255	*/
	  uint8_t retain ;				/*!<	retain is flag for keep message in MQTT Broker
	   	   	   	   	   	   	   	   	   	   	0: Remove message after send to Subcriber
	   	   	   	   	   	   	   	   	   	   	1: Keep messege in MQTT Broker   */
	  uint8_t dup;					/*!<	dup is flag for dupplicate message in MQTT Broker, It relate to qos in MQTTClient configurations
	   	   	   	   	   	   	   	   	   	   	0: disable
	   	   	   	   	   	   	   	   	   	   	1: enable*/
}MESSAGE_TypeDef;




typedef enum{
	MQTT_INIT = 0,
	MQTT_START,
	MQTT_WAIT_FOR_START,

	MQTT_ACCQUIRE_CLIENT,
	MQTT_WAIT_FOR_ACCQUIRE_CLIENT,

	MQTT_CONNECT,
	MQTT_WAIT_FOR_CONNECT,

	MQTT_TOPIC,
	MQTT_WAIT_FOR_TOPIC,
	MQTT_INPUT_TOPIC,
	MQTT_WAIT_FOR_INPUT_TOPIC,

	MQTT_PAYLOAD,
	MQTT_WAIT_FOR_PAYLOAD,
	MQTT_INPUT_PAYLOAD,
	MQTT_WAIT_FOR_INPUT_PAYLOAD,

	MQTT_PUBLISH,
	MQTT_WAIT_FOR_PUBLISH,

	MQTT_WAIT_NEXT_COMMAND,
	MQTT_MAX_STATE
}MQTT_State;

/*
 * @struct MQTT_machine_Typedef
 * @brief State and Coressponding Function
 */
typedef struct{
	MQTT_State mqtt_state;			/*!<	MQTT state	*/
	void(*func)(void);				/*!<	func of mqtt_state 	*/
}MQTT_Machine_TypeDef;




uint8_t MQTT_Run();

MQTT_State MQTT_Get_State();

void MQTT_Set_State( MQTT_State mqtt_state);

void MQTT_Set_Message(char *topic,char *payload);

uint8_t* MQTT_Get_Message_Subcribe(uint8_t *mess_length);

FlagStatus MQTT_OpenCabinet_Checking();




FlagStatus is_Mqtt_TimeOutFlag(void);
void Set_Mqtt_Timeout_Flag(void);
void Clear_Mqtt_Timeout_Flag(void);

FlagStatus is_Reset_Mqtt_Service_Flag();
void Set_Reset_Mqtt_Service_Flag(void);
void Clear_Reset_Mqtt_Service_Flag(void);
void MQTT_Display_State(void);

void MQTT_Init();
void MQTT_Start();
void MQTT_Wait_For_Start();
void MQTT_Accquire_Client();
void MQTT_Wait_For_Accquire_Client();
void MQTT_Connect();
void MQTT_Wait_For_Connect();
void MQTT_Topic();
void MQTT_Wait_For_Topic();
void MQTT_Input_Topic();
void MQTT_Wait_For_Input_Topic();
void MQTT_Payload();
void MQTT_Wait_For_Payload();
void MQTT_Input_Payload();
void MQTT_Wait_For_Input_Payload();
void MQTT_Publish();
void MQTT_Wait_For_Publish();
void MQTT_Wait_Next_Command();

FlagStatus Set_Data_To_Buffer(uint8_t * buffer);
void Clear_Mqtt_Command();
FlagStatus MQTT_Available();


void Set_Mqtt_Command(char * mqtt_command);

void Reset_MqttConfiguration_State();

void Set_Got_Message();

FlagStatus is_Has_Message_Available();

FlagStatus is_MQTT_Available();

void MQTT_Add_Message(char *topic,char *payload);


void MQTT_Dispatch_Message();

MESSAGE_TypeDef MQTT_Get_Newest_Message();
#endif /* INC_SERVER_COMMUNICATION_APP_MQTT_H_ */
