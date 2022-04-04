/**
 *@file app_mqtt.c
 *@author thodo
 */

#include <app_ATProcessing.h>
#include <ServerCommunication/app_mqtt.h>

#include "Component/app_sim7600.h"
#include "Component/app_i2c_lcd.h"
#include <string.h>
#include "app_scheduler.h"
#include "Utility/app_string.h"
#include "main.h"

#define SERVER_TYPE 0								/*!<Server Type:
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 0: TCP
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 1: SSL/TLS*/
#define CONNECT_TIMEOUT  	300						/*!<Timeout for MQTT Connect.
														This is time need to setup connection to Server*/
#define SUBCRIBE_TIMEOUT  	300						/*!<Timeout for Subcribe.
 	 	 	 	 	 	 	 	 	 	 	 	 	 	This is time need to client subcribe topic at MQTTBroker*/
#define MQTT_MESSAGE_BUFFER_SIZE	20
#define ATCOMMAND_TIMEOUT	300

uint8_t LockerID[IMEI_LENGTH];						/*!<This buffer contains LockerID of board*/

MQTT_State prev_mqtt_state = MQTT_INIT;			/*!< Previous state of MQTT State Machine.
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 Be used to compare mqtt_state and show state of State Machine*/
MQTT_State mqtt_state = MQTT_INIT;					/*!< Current state of MQTT State Machine.
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 Be used to choose function operation*/

uint8_t atcommand[250];								/*!< Buffer contains at command*/


uint8_t SUB_TOPIC_1[50];							/*!< Buffer contain Subcribe Topic.
														It is not be initiated. User must set it in run time*/

uint16_t data_index = 0;
FlagStatus ret_code;
FlagStatus flag_ignore = RESET;
FlagStatus start_record_subcribe_message = RESET;
FlagStatus stop_record_subcribe_message = RESET;
FlagStatus  reset_mqtt_service = RESET;
FlagStatus subcribe_openCabinet_topic = RESET;
FlagStatus subcribe_updateFirmware_topic = RESET;

FlagStatus default_command = SET;
uint8_t mqtt_num_ignore = 0;
uint8_t log[50];

uint8_t SUB_TOPIC_FOR_GETTING_CERTIFICATE[50];

uint8_t PUB_TOPIC_FOR_GETTING_CERTIFICATE[] = CERTIFICATE_TOPIC;

uint8_t breakData[] = "\r\n+CMQTTRXPAYLOAD: ";

char SUBCRIBE_MESSAGE[50];
char TOPIC_OPENCABINET[50];
char TOPIC_UPDATE_FIRMWARE[50];

uint32_t task_MQTT_Timeout_ID;

/*
 * Last Will Topic - Last Will Message
 */
const uint8_t WILL_TOPIC[] = CONNECTION_TOPIC;
uint8_t WILL_MESSAGE[50] ;					/*!< 0 is Running , 1 is Disconnected*/
char clientID[50];


const uint8_t PUB_TOPIC[]="status";					/*!< Topic for publish message*/

const uint8_t PAYLOAD[]="okok";						/*!< Initiated payload*/

uint8_t mess_receive_subcribe[MAX_PAYLOAD_LENGTH];	/*!< Buffer for receive subcribe message*/
uint8_t mess_length = 0;

FlagStatus mqtt_timeout_flag = SET;					/*!< Time out flag for check whether MQTT State Machine got into timeout or not */


MESSAGE_TypeDef publish_message;					/*!< Variable for publish message.
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 When User wanna publish message to MQTT Broker.
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 They need to set attribute of message to it.*/

MESSAGE_TypeDef message_buffer[MQTT_MESSAGE_BUFFER_SIZE];
uint8_t message_head = 0;
uint8_t message_tail = 0;

/*
 * Initiated mqtt_client configuration
 */
MQTTClient_TypeDef mqtt_client = {
		.client_index = 0,
		.clientid = "",
		.server_address = MQTT_SERVER,
		.user = USER_NAME,
		.pass =PASS_WORD,
		.keepAlive = 60,
		.clean_session = 0
};







MQTT_Machine_TypeDef mqtt_state_machine[]={
		{MQTT_INIT								, 		MQTT_Init							},
		{MQTT_START								, 		MQTT_Start							},
		{MQTT_WAIT_FOR_START					, 		MQTT_Wait_For_Start					},

		{MQTT_ACCQUIRE_CLIENT					, 		MQTT_Accquire_Client				},
		{MQTT_WAIT_FOR_ACCQUIRE_CLIENT			, 		MQTT_Wait_For_Accquire_Client		},

		{MQTT_CONNECT							, 		MQTT_Connect						},
		{MQTT_WAIT_FOR_CONNECT					, 		MQTT_Wait_For_Connect				},

		{MQTT_TOPIC								, 		MQTT_Topic							},
		{MQTT_WAIT_FOR_TOPIC					, 		MQTT_Wait_For_Topic					},
		{MQTT_INPUT_TOPIC						, 		MQTT_Input_Topic					},
		{MQTT_WAIT_FOR_INPUT_TOPIC				, 		MQTT_Wait_For_Input_Topic			},

		{MQTT_PAYLOAD							, 		MQTT_Payload						},
		{MQTT_WAIT_FOR_PAYLOAD					, 		MQTT_Wait_For_Payload				},
		{MQTT_INPUT_PAYLOAD						, 		MQTT_Input_Payload					},
		{MQTT_WAIT_FOR_INPUT_PAYLOAD			, 		MQTT_Wait_For_Input_Payload			},

		{MQTT_PUBLISH							, 		MQTT_Publish						},
		{MQTT_WAIT_FOR_PUBLISH					, 		MQTT_Wait_For_Publish				},

		{MQTT_WAIT_NEXT_COMMAND					, 		MQTT_Wait_Next_Command				}
};


/**
 * MQTT_Display_State()
 * @brief This is function for display state of State Machine. It only show State when having a state changation.
 */
void MQTT_Display_State(void){
	if(prev_mqtt_state!=mqtt_state){
		prev_mqtt_state = mqtt_state;
		switch (mqtt_state) {
			case MQTT_INIT:
				LOG("\r\nMQTT INIT\r\n");
				break;
			case MQTT_START:
				LOG("\r\nMQTT START\r\n");
				break;
			case MQTT_WAIT_FOR_START:
				LOG("\r\nMQTT WAIT FOR START\r\n");
				break;
			case MQTT_ACCQUIRE_CLIENT:
				LOG("\r\nMQTT ACCQUIRE CLIENT\r\n");
				break;
			case MQTT_WAIT_FOR_ACCQUIRE_CLIENT:
				LOG("\r\nMQTT WAIT FOR ACCQUIRE CLIENT\r\n");
				break;
			case MQTT_CONNECT:
				LOG("\r\nMQTT CONNECT\r\n");
				break;
			case MQTT_WAIT_FOR_CONNECT:
				LOG("\r\nMQTT WAIT FOR CONNECT\r\n");
				break;
			case MQTT_TOPIC:
				LOG("\r\nMQTT TOPIC\r\n");
				break;
			case MQTT_WAIT_FOR_TOPIC:
				LOG("\r\nMQTT WAIT FOR TOPIC\r\n");
			case MQTT_INPUT_TOPIC:
				LOG("\r\nMQTT INPUT TOPIC\r\n");
				break;
			case MQTT_WAIT_FOR_INPUT_TOPIC:
				LOG("\r\nMQTT WAIT FOR INPUT TOPIC\r\n");
				break;
			case MQTT_PAYLOAD:
				LOG("\r\nMQTT PAYLOAD\r\n");
				break;
			case MQTT_WAIT_FOR_PAYLOAD:
				LOG("\r\nMQTT WAIT FOR PAYLOAD\r\n");
				break;
			case MQTT_INPUT_PAYLOAD:
				LOG("\r\nMQTT INPUT PAYLOAD\r\n");
				break;
			case MQTT_WAIT_FOR_INPUT_PAYLOAD:
				LOG("\r\nMQTT WAIT FOR INPUT PAYLOAD\r\n");
				break;

			case MQTT_PUBLISH:
				LOG("\r\nMQTT PUBLISH\r\n");
				break;
			case MQTT_WAIT_FOR_PUBLISH:
				LOG("\r\nMQTT WAIT FOR PUBLISH\r\n");
				break;
			case MQTT_WAIT_NEXT_COMMAND:
				LOG("\r\nMQTT WAIT NEW COMMAND\r\n");
				break;
			default:
				break;
		}
	}
}

/**
 * MQTT_Run()
 * @param None
 * @retval 0 if state < SIM7600_MAX_STATE else return 1
 * @brief This is function can be called from external file. It run follow state machine method. Not have param.
 */
uint8_t MQTT_Run(void){
	MQTT_Display_State();
	if(mqtt_state == MQTT_WAIT_NEXT_COMMAND){
		return 1;
	}
	else if(mqtt_state == MQTT_MAX_STATE){
		//Get error, need to break out LockerConfiguration and Update to Server
		return 2;
	}
	else{
		(*mqtt_state_machine[mqtt_state].func)();
		return 0;
	}
}

void MQTT_Declar(){
	sprintf((char*)TOPIC_OPENCABINET, "*******");
	sprintf((char*)TOPIC_UPDATE_FIRMWARE, "********");

}

/**
 * MQTT_Init()
 * @param None
 * @retval None
 * @brief This function for initiating some variables and buffer in run time
 */
void MQTT_Init(){
	mqtt_state = MQTT_START;
}


/**
 * MQTT_Start()
 * @param None
 * @retval None
 * @brief This function for starting MQTT Service
 */
void MQTT_Start(){
	Clear_Reiceive_Buffer();
	//Clear_Reiceive_Buffer();
	sprintf(mqtt_client.clientid,"\"%d\"",Get_Box_ID());
	sprintf((char*)atcommand,"AT+CMQTTSTART\r\n");
	UART_SIM7600_Transmit(atcommand);
	Clear_Mqtt_Command();
	mqtt_state = MQTT_WAIT_FOR_START;
}

/**
 * MQTT_Wait_For_Start()
 * @param None
 * @retval None
 * @brief This function for waiting to start MQTT service respond.
 * @brief If AT_Result = AT_OK so switch to MQTT_ACCQUIRE_CLIENT
 * @brief else It is AT_ERROR so switch to MQTT_MAX_STATE for reset Simcom7600 after.
 */
void MQTT_Wait_For_Start(){
//	Wait_For_Respone(AT_OK);
	switch (Get_AT_Result()){
		case AT_OK:
			Clear_AT_Result();
			mqtt_state=MQTT_ACCQUIRE_CLIENT;
			break;
		case AT_ERROR:
			Clear_AT_Result();
			Lcd_Clear_Display();
			Lcd_Show_String("MQTT Config: Fail", 0, 0);
			Lcd_Show_String("START", 1, 0);
			mqtt_state = MQTT_MAX_STATE;
			break;
		default:
			Clear_AT_Result();
			break;
	}
}

/**
 * MQTT_Accquire_Client()
 * @param None
 * @retval None
 * @brief This function for waiting to accquire a MQTT Client
 */
void MQTT_Accquire_Client(){
	// SSL Server
	sprintf((char*)atcommand,"AT+CMQTTACCQ=%d,%s,%d\r\n",mqtt_client.client_index,mqtt_client.clientid,SERVER_TYPE);
	//Clear_Reiceive_Buffer();
	UART_SIM7600_Transmit(atcommand);
	Clear_Mqtt_Command();
	mqtt_state = MQTT_WAIT_FOR_ACCQUIRE_CLIENT;
}


/**
 * MQTT_Wait_For_Accquire_Client()
 * @param None
 * @retval None
 * @brief This function for waiting to accquire MQTT client respond.
 * @brief If AT_Result = AT_OK so switch to MQTT_SSL_CFG
 * @brief else It is AT_ERROR so switch to MQTT_MAX_STATE for reset Simcom7600 after.
 */
void MQTT_Wait_For_Accquire_Client(){
//	Wait_For_Respone(AT_OK);
	switch (Get_AT_Result()) {
		case AT_OK:
			Clear_AT_Result();
			mqtt_state=MQTT_CONNECT;
			break;
		case AT_ERROR:
			Clear_AT_Result();
			Lcd_Clear_Display();
			Lcd_Show_String("MQTT Config: Fail", 0, 0);
			Lcd_Show_String("ACCQUIRE_CLIENT", 1, 0);
			mqtt_state = MQTT_MAX_STATE;
			break;
		default:
			Clear_AT_Result();
			break;
	}
}


/**
 * MQTT_Connect()
 * @param None
 * @retval None
 * @brief This function for connecting MQTT Client to MQTT Broker.
 */
void MQTT_Connect(){
	sprintf((char*)atcommand,"AT+CMQTTCONNECT=%d,%s,%d,%d,%s,%s\r\n",mqtt_client.client_index,mqtt_client.server_address,
			mqtt_client.keepAlive,mqtt_client.clean_session,mqtt_client.user,mqtt_client.pass);
	//Clear_Reiceive_Buffer();
	UART_SIM7600_Transmit(atcommand);
	Clear_Mqtt_Command();
	Clear_Mqtt_Timeout_Flag();
	task_MQTT_Timeout_ID= SCH_Add_Task(Set_Mqtt_Timeout_Flag, CONNECT_TIMEOUT, 0);
	mqtt_state = MQTT_WAIT_FOR_CONNECT;
}


/**
 * MQTT_Wait_For_Connect()
 * @param None
 * @retval None
 * @brief This function for waiting to connect MQTT Client to MQTT Broker respond.
 * @brief If AT_Result = AT_OK so switch to MQTT_SUBCRIBE_TOPIC.
 * @brief else It is AT_ERROR so switch to MQTT_MAX_STATE for reset Simcom7600 after.
 */
void MQTT_Wait_For_Connect(){
	if(is_Mqtt_TimeOutFlag()){
//		Wait_For_Respone(AT_OK);
		switch (Get_AT_Result()) {
			case AT_OK:
				Clear_AT_Result();
				mqtt_state=MQTT_WAIT_NEXT_COMMAND;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_ERROR:
				Clear_AT_Result();
				Lcd_Clear_Display();
				Lcd_Show_String("MQTT Config: Fail", 0, 0);
				Lcd_Show_String("CONNECT", 1, 0);
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			default:
				Clear_AT_Result();
				break;
		}
	}
}


/**
 * MQTT_Topic()
 * @param None
 * @retval None
 * @brief This function for send ATcommand contains client_index and topic length for publish.
 */
void MQTT_Topic(){
	Clear_AT_Result();
	sprintf((char*)atcommand,"AT+CMQTTTOPIC=%d,%d\r\n",mqtt_client.client_index,strlen(publish_message.topic));
	// Clear_Reiceive_Buffer();
	Clear_Mqtt_Timeout_Flag();
	task_MQTT_Timeout_ID = SCH_Add_Task(Set_Mqtt_Timeout_Flag, ATCOMMAND_TIMEOUT, 0);
	UART_SIM7600_Transmit(atcommand);
	Clear_Mqtt_Command();
	mqtt_state = MQTT_WAIT_FOR_TOPIC;
}

/**
 * MQTT_Wait_For_Topic()
 * @param None
 * @retval None
 * @brief This function for waiting atcommand Topic respond.
 * @brief If AT_Result = AT_OK so switch to MQTT_INPUT_TOPIC to input a Topic content.
 * @brief else It is AT_ERROR so switch to MQTT_MAX_STATE for reset Simcom7600 after.
 */
void MQTT_Wait_For_Topic(){
//	Wait_For_Respone(AT_INPUT);
	if(is_Mqtt_TimeOutFlag()){
		LOG("TIME OUT\r\n");
		Clear_AT_Result();
		mqtt_state = MQTT_MAX_STATE;
	}
	else{
		switch (Get_AT_Result()) {
			case AT_INPUT:
				Clear_AT_Result();
				mqtt_state=MQTT_INPUT_TOPIC;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_ERROR:
				Clear_AT_Result();
				Lcd_Clear_Display();
				Lcd_Show_String("MQTT Config: Fail", 0, 0);
				Lcd_Show_String("TOPIC", 1, 0);
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_SUBCRIBE_MESSAGE:
				break;
			// When Simcom reset in Publish Message
			case AT_PB_DONE:
				Clear_AT_Result();
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			default:
				Clear_AT_Result();
				break;
		}
	}
}

/**
 * MQTT_Input_Topic()
 * @param None
 * @retval None
 * @brief This function for input topic content.
 */
void MQTT_Input_Topic(){
//	Clear_Reiceive_Buffer();
	Clear_AT_Result();
	sprintf((char*)atcommand, "%s", publish_message.topic);
	Clear_Mqtt_Timeout_Flag();
	task_MQTT_Timeout_ID = SCH_Add_Task(Set_Mqtt_Timeout_Flag, ATCOMMAND_TIMEOUT, 0);
	UART_SIM7600_Transmit(atcommand);
	Clear_Mqtt_Command();
	mqtt_state = MQTT_WAIT_FOR_INPUT_TOPIC;
}


/**
 * MQTT_Wait_For_Input_Topic()
 * @param None
 * @retval None
 * @brief This function for waiting to input topic content.
 * @brief If AT_Result = AT_OK so switch to MQTT_PAYLOAD to input a payload content.
 * @brief else It is AT_ERROR so switch to MQTT_MAX_STATE for reset Simcom7600 after.
 */
void MQTT_Wait_For_Input_Topic(){
//	Wait_For_Respone(AT_OK);
	if(is_Mqtt_TimeOutFlag()){
		LOG("TIME OUT\r\n");
		Clear_AT_Result();
		mqtt_state = MQTT_MAX_STATE;
	}
	else{
		switch (Get_AT_Result()) {
			case AT_OK:
				Clear_AT_Result();
				mqtt_state=MQTT_PAYLOAD;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_ERROR:
				Clear_AT_Result();
				Lcd_Clear_Display();
				Lcd_Show_String("MQTT Config: Fail", 0, 0);
				Lcd_Show_String("INPUT_TOPIC", 1, 0);
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_SUBCRIBE_MESSAGE:
				break;
			// When Simcom reset in Publish Message
			case AT_PB_DONE:
				Clear_AT_Result();
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			default:
				Clear_AT_Result();
				break;
		}
	}
}


/**
 * MQTT_Payload()
 * @param None
 * @retval None
 * @brief This function for send ATcommand contains client_index and payload length for publish.
 */
void MQTT_Payload(){
//	Clear_AT_Result();
	sprintf((char*)atcommand,"AT+CMQTTPAYLOAD=%d,%d\r\n",mqtt_client.client_index,strlen(publish_message.payload));
//	Clear_Reiceive_Buffer();
	Clear_Mqtt_Timeout_Flag();
	task_MQTT_Timeout_ID = SCH_Add_Task(Set_Mqtt_Timeout_Flag, ATCOMMAND_TIMEOUT, 0);
	UART_SIM7600_Transmit(atcommand);
	Clear_Mqtt_Command();
	mqtt_state = MQTT_WAIT_FOR_PAYLOAD;
}

/**
 * MQTT_Wait_For_Payload()
 * @param None
 * @retval None
 * @brief This function for waiting atcommand payload respond.
 * @brief If AT_Result = AT_OK so switch to MQTT_INPUT_PAYLOAD to input a Topic content.
 * @brief else It is AT_ERROR so switch to MQTT_MAX_STATE for reset Simcom7600 after.
 */
void MQTT_Wait_For_Payload(){
//	Wait_For_Respone(AT_INPUT);
	if(is_Mqtt_TimeOutFlag()){
		LOG("TIME OUT\r\n");
		Clear_AT_Result();
		mqtt_state = MQTT_MAX_STATE;
	}
	else{
		switch (Get_AT_Result()) {
			case AT_INPUT:
				Clear_AT_Result();
				mqtt_state = MQTT_INPUT_PAYLOAD;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_ERROR:
				Clear_AT_Result();
				Lcd_Clear_Display();
				Lcd_Show_String("MQTT Config: Fail", 0, 0);
				Lcd_Show_String("PAYLOAD", 1, 0);
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_SUBCRIBE_MESSAGE:
				break;
			// When Simcom reset in Publish Message
			case AT_PB_DONE:
				Clear_AT_Result();
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			default:
				Clear_AT_Result();
				break;
		}
	}
}

/**
 * MQTT_Input_Payload()
 * @param None
 * @retval None
 * @brief This function for input payload content.
 */
void MQTT_Input_Payload(){
//	Clear_AT_Result();
//	Clear_Reiceive_Buffer();
	sprintf((char*)atcommand,"%s",publish_message.payload);
	Clear_Mqtt_Timeout_Flag();
	task_MQTT_Timeout_ID = SCH_Add_Task(Set_Mqtt_Timeout_Flag, ATCOMMAND_TIMEOUT, 0);
	UART_SIM7600_Transmit(atcommand);
	Clear_Mqtt_Command();
	mqtt_state = MQTT_WAIT_FOR_INPUT_PAYLOAD;
}


/**
 * MQTT_Wait_For_Input_Payload()
 * @param None
 * @retval None
 * @brief This function for waiting to input payload content.
 * @brief If AT_Result = AT_OK so switch to MQTT_PUBLISH to publish a message.
 * @brief else It is AT_ERROR so switch to MQTT_MAX_STATE for reset Simcom7600 after.
 */
void MQTT_Wait_For_Input_Payload(){
//	Wait_For_Respone(AT_OK);
	if(is_Mqtt_TimeOutFlag()){
		LOG("TIME OUT\r\n");
		Clear_AT_Result();
		mqtt_state = MQTT_MAX_STATE;
	}
	else{
		switch (Get_AT_Result()) {
			case AT_OK:
				Clear_AT_Result();
				mqtt_state = MQTT_PUBLISH;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_ERROR:
				Lcd_Clear_Display();
				Lcd_Show_String("MQTT Config: Fail", 0, 0);
				Lcd_Show_String("IN_PAYLOAD", 1, 0);
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				Clear_AT_Result();
				break;
			case AT_SUBCRIBE_MESSAGE:
				break;
			// When Simcom reset in Publish Message
			case AT_PB_DONE:
				Clear_AT_Result();
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			default:
				Clear_AT_Result();
				break;
		}
	}
}

/**
 * MQTT_Publish()
 * @param None
 * @retval None
 * @brief This function for publish message.
 */
void MQTT_Publish(){
//	Clear_AT_Result();
	sprintf((char*)atcommand,"AT+CMQTTPUB=%d,%d,%d,%d,%d\r\n",mqtt_client.client_index,publish_message.qos,
			publish_message.pub_timeout, publish_message.retain, publish_message.dup);
//	Clear_Reiceive_Buffer();
	Clear_Mqtt_Timeout_Flag();
	task_MQTT_Timeout_ID = SCH_Add_Task(Set_Mqtt_Timeout_Flag, ATCOMMAND_TIMEOUT, 0);
	UART_SIM7600_Transmit(atcommand);
	Clear_Mqtt_Command();
	mqtt_state = MQTT_WAIT_FOR_PUBLISH;
}


/**
 * MQTT_Wait_For_Publish()
 * @param None
 * @retval None
 * @brief This function for waiting to publish message.
 * @brief If AT_Result = AT_OK so switch to MQTT_WAIT_NEXT_COMMAND to publish an other message.
 * @brief else It is AT_ERROR so switch to MQTT_WAIT_NEXT_COMMAND for retry.
 */
void MQTT_Wait_For_Publish(){
//	Wait_For_Respone(AT_OK);
	if(is_Mqtt_TimeOutFlag()){
		LOG("TIME OUT\r\n");
		Clear_AT_Result();
		mqtt_state = MQTT_MAX_STATE;
	}
	else{
		switch (Get_AT_Result()){
			case AT_OK:
				Clear_AT_Result();
				mqtt_state=MQTT_WAIT_NEXT_COMMAND;
				sprintf(log,"%d\r\n",HAL_GetTick());
				LOG(log);
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_ERROR:
				Clear_AT_Result();
				Lcd_Clear_Display();
				Lcd_Show_String("MQTT Config: Fail", 0, 0);
				Lcd_Show_String("PUBLISH", 1, 0);
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			case AT_SUBCRIBE_MESSAGE:
				break;
			// When Simcom reset in Publish Message
			case AT_PB_DONE:
				Clear_AT_Result();
				mqtt_state = MQTT_MAX_STATE;
				SCH_Delete_Task(task_MQTT_Timeout_ID);
				break;
			default:
				Clear_AT_Result();
				break;
		}
	}
}



/**
 * MQTT_Wait_Next_Command()
 * @param None
 * @retval None
 * @brief This function do not anything. It wait to external function set other mqtt_state then get out of here.
 */
void MQTT_Wait_Next_Command(){
	return;
}

/**
 * MQTT_Get_State()
 * @param None
 * @retval mqtt_state
 * @brief This function get mqtt_state.
 */
MQTT_State MQTT_Get_State(){
	return mqtt_state;
}

/**
 * MQTT_Set_State()
 * @param _mqtt_state
 * @retval None
 * @brief This function allow external function can set mqtt_state.
 */
void MQTT_Set_State(MQTT_State _mqtt_state){
	mqtt_state = _mqtt_state;
}


/**
 * MQTT_Set_Message()
 * @param topic: topic for publish message
 * @param payload: payload for publish message
 * @retval None
 * @brief This function allow external function can set publish message via topic and payload.
 */
void MQTT_Set_Message(char *topic,char *payload){
	memcpy(publish_message.topic,topic,strlen(topic));
	memcpy(publish_message.payload,payload,strlen(payload));
	publish_message.qos =1;
	publish_message.retain =0;
	publish_message.dup =0;
	publish_message.pub_timeout = 120;
	return;
}


/**
 * MQTT_Get_Message_Subcribe()
 * @param payload_length: pass length value to pointer.
 * @retval uint8_t*: pointer to message_receive_buffer
 * @brief This function return buffer subcribe message.
 */
uint8_t* MQTT_Get_Message_Subcribe(uint8_t * payload_length){
	uint8_t	temp;
	// Ignore ",$(PAYLOAD_LENGTH)\r\n"
	if(UART_SIM7600_Received_Buffer_Available()){
		temp = UART_SIM7600_Read_Received_Buffer();
		if(start_record_subcribe_message){
			if(temp == '\r'){
				stop_record_subcribe_message = SET;
				start_record_subcribe_message = RESET;
			}
			else{
				mess_receive_subcribe[mess_length++]=temp;
			}
		}
		else if(stop_record_subcribe_message){
			(*payload_length) = mess_length;
			mess_length = 0;
			stop_record_subcribe_message = RESET;
			return mess_receive_subcribe;
		}
		else if(temp =='\n'){
			start_record_subcribe_message = SET;
		}
	}
	return NULL;




//	for(;;){
//		if(UART_SIM7600_Received_Buffer_Available()){
//			if(UART_SIM7600_Read_Received_Buffer()=='\n'){
//				break;
//			}
//		}
//
//	}
//	//Get Payload Content
//	for(;;) {
//		if(UART_SIM7600_Received_Buffer_Available()){
//			temp=UART_SIM7600_Read_Received_Buffer();
//			if(temp!='\r'){
//				mess_receive_subcribe[length++]=temp;
//			}
//			else{
//				break;
//			}
//		}
//	}
//	*payload_length = length;
//	return mess_receive_subcribe;
}


/**
 * is_Mqtt_TimeOutFlag()
 * @param None.
 * @retval Flagstatus: SET or RESET
 * @brief This function return mqtt_timeout_flag.
 */
FlagStatus is_Mqtt_TimeOutFlag(){
	return mqtt_timeout_flag;
}


/**
 * Set_Mqtt_Timeout_Flag()
 * @param None.
 * @retval None
 * @brief This function set mqtt_timeout_flag to SET.
 */
void Set_Mqtt_Timeout_Flag(void){
	mqtt_timeout_flag = SET;
}

/**
 * Clear_Mqtt_Timeout_Flag()
 * @param None.
 * @retval None
 * @brief This function set mqtt_timeout_flag to RESET.
 */
void Clear_Mqtt_Timeout_Flag(void){
	mqtt_timeout_flag = RESET;
}


/**
 * is_Mqtt_TimeOutFlag()
 * @param None.
 * @retval Flagstatus: SET or RESET
 * @brief This function return mqtt_timeout_flag.
 */
FlagStatus is_Reset_Mqtt_Service_Flag(){
	return reset_mqtt_service;
}


/**
 * Set_Mqtt_Timeout_Flag()
 * @param None.
 * @retval None
 * @brief This function set mqtt_timeout_flag to SET.
 */
void Set_Reset_Mqtt_Service_Flag(void){
	reset_mqtt_service = SET;
}

/**
 * Clear_Mqtt_Timeout_Flag()
 * @param None.
 * @retval None
 * @brief This function set mqtt_timeout_flag to RESET.
 */
void Clear_Reset_Mqtt_Service_Flag(void){
	reset_mqtt_service = RESET;
}

FlagStatus Set_Data_To_Buffer(uint8_t * buffer){
	//	"integerValue$(pointer here)": "0"
	//	-> need to ignore 3 character
	if(UART_SIM7600_Received_Buffer_Available()){
		uint8_t val_of_character = UART_SIM7600_Read_Received_Buffer();
//		sprintf(log,"%d",data_index);
//		UART_DEBUG_Transmit_Size(&val_of_character,1);
		if(flag_ignore){
			mqtt_num_ignore++;
			if(mqtt_num_ignore > 7){
				mqtt_num_ignore = 0;
				flag_ignore = RESET;
			}
			return RESET;
		}
		if(data_index++ <3){
			return RESET;
		}
	//	UART_DEBUG_Transmit_Size(val_of_character, 1);

		if(val_of_character!='"'){
			buffer[data_index-4] = val_of_character;
			if(isReceiveData((char*)buffer, (data_index-3), (char*)breakData)){
				flag_ignore = SET;
				data_index = data_index - strlen((char*)breakData);
			}
			if(isReceiveData((char*)buffer, (data_index-3), "\n")){
				if(!isReceiveData((char*)buffer, data_index-3, "\r\n")){
					buffer[data_index-4] = '\r';
					buffer[data_index-3] = '\n';
					data_index++;
				}
			}
			return RESET;
		}
		else{
			sprintf(log	,"\r\n%d\r\n",data_index);
			LOG(log);
			data_index = 0;
			return SET;
		}
	}
	else{
		return RESET;
	}

}

void Set_Got_Message(){
	Clear_AT_Result();
}


void Set_Mqtt_Command(char * mqtt_command){
	sprintf((char*)atcommand, "%s", mqtt_command);
	default_command = RESET;
}

void Clear_Mqtt_Command(){
	default_command = SET;
}

FlagStatus MQTT_Available(){
	return mqtt_state == MQTT_WAIT_NEXT_COMMAND;
}


void MQTT_Add_Message(char *topic,char *payload){
	MESSAGE_TypeDef temp_message;
	strcpy(temp_message.topic,topic);
	strcpy(temp_message.payload,payload);
	temp_message.qos =1;
	temp_message.retain =0;
	temp_message.dup =0;
	temp_message.pub_timeout = 120;
	message_buffer[message_head] = temp_message;
	message_head = (message_head+1)%MQTT_MESSAGE_BUFFER_SIZE;
	return;
}

void MQTT_Dispatch_Message(){
	publish_message = message_buffer[message_tail];
	message_tail = (message_tail+1)%MQTT_MESSAGE_BUFFER_SIZE;
	mqtt_state = MQTT_TOPIC;
}

MESSAGE_TypeDef MQTT_Get_Newest_Message(){
	MESSAGE_TypeDef temp_message;
	temp_message = message_buffer[message_tail];
	return temp_message;
}

FlagStatus is_Has_Message_Available(){
	return message_head != message_tail;
}

FlagStatus Clear_All_MQTT_Message(){
	message_head = 0;
	message_tail = 0;
}


FlagStatus is_MQTT_Available(){
	return mqtt_state == MQTT_WAIT_NEXT_COMMAND;
}
//
//void Setup_Mqtt_Connect_Message(uint8_t * clientid,uint8_t * username,uint8_t * password ,uint8_t *willtopic,uint8_t *willmessage,uint8_t keepalive,uint8_t cleansession){
//	uint8_t i;
//	uint8_t clientIdLength = strlen((uint8_t*)clientid);
//	uint8_t usernameLength = strlen((uint8_t*)username);
//	uint8_t passwordLength = strlen((uint8_t*)password);
//	uint8_t willTopicLength = strlen((uint8_t*)willtopic);
//	uint8_t willMessageLength = strlen((uint8_t*)willmessage);
//
//	Clear_Mqtt_Message_Buffer();
//
//	// Header
//	mqttMessage[mqttMessageIndex++] = 0x10;
//
//	mqttMessage[mqttMessageIndex++] = 10 + clientIdLength + usernameLength + passwordLength;    // Remaining length of the message (bytes 2-13 + clientId)
//	// Protocol name
//	mqttMessage[mqttMessageIndex++] = 0;                      // Protocol Name Length MSB
//	mqttMessage[mqttMessageIndex++] = 4;                      // Protocol Name Length LSB
//	mqttMessage[mqttMessageIndex++] = 'M';
//	mqttMessage[mqttMessageIndex++] = 'Q';
//	mqttMessage[mqttMessageIndex++] = 'T';
//	mqttMessage[mqttMessageIndex++] = 'T';
//
//	// Protocol Level
//	mqttMessage[mqttMessageIndex++] =  4;					  // MQTT Protocol version = 4 (mqtt 3.1.1)
//
//	// Connection flags   2 for clean session
// //   mqttMessage[9] = 2;
//	mqttMessage[mqttMessageIndex++] = 0b11000010;  //enable username, password, clean session
//
//	// Keep-alive (maximum duration)
////	mqttMessage[mqttMessageIndex++] = 0x00;                     // Keep-alive Time Length MSB
////	mqttMessage[mqttMessageIndex++] = 0x3c;                     // Keep-alive Time Length LSB
//
//	mqttMessage[mqttMessageIndex++] = 0x00;                     // Keep-alive Time Length MSB
//		mqttMessage[mqttMessageIndex++] = 120;                    // Keep-alive Time Length LSB
//
//	// Client ID
//	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
//	mqttMessage[mqttMessageIndex++] = clientIdLength;        // Client ID length LSB
//	for (i = 0; i < clientIdLength; i ++){
//		mqttMessage[mqttMessageIndex++] = clientid[i];
//	}
//
//	// Username
//	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
//	mqttMessage[mqttMessageIndex++] = usernameLength;        // Client ID length LSB
//	for (i = 0; i < usernameLength; i ++){
//		mqttMessage[mqttMessageIndex++] = username[i];
//	}
//
//	// Password
//	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
//	mqttMessage[mqttMessageIndex++] = passwordLength;        // Client ID length LSB
//	for (i = 0; i < passwordLength; i ++){
//		mqttMessage[mqttMessageIndex++] = password[i];
//	}
//	mqttMessageLength = mqttMessageIndex;
//	SM_Send_Data(mqttMessageLength);
//}
