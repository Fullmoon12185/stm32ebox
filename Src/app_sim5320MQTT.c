/*
 * app_sim5320MQTT.c
 *
 *  Created on: Dec 17, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_string.h"
#include "app_sim3g.h"

#include "app_scheduler.h"
#include "app_sim5320MQTT.h"

#define		DEBUG_MQTT(X)	X


#define 	MQTT_COMMAND_TIME_OUT		5000

#define		MQTT_MESSAGE_BUFFER_LENGTH	128
#define		CLOUD_MQTT		1

#define		NUMBER_OF_SUBSCRIBE_TOPIC	2
#define		NUMBER_OF_PUBLISH_TOPIC		2

extern const uint8_t OK[];
extern const uint8_t CONNECT_OK[];
extern const uint8_t ERROR_1[];
extern const uint8_t GREATER_THAN_SYMBOL[];
extern const uint8_t NETWORK_OPENED[];

const uint8_t Send_ok[] = "Send ok\r";
const uint8_t RECV_FROM[] = "RECV FROM\r";
const uint8_t MQTTOPEN_COMMAND[] 	= "AT+CIPOPEN=0,\"TCP\",\"tailor.cloudmqtt.com\",13255\r";

const uint8_t MQTTCLOSE_COMMAND[]   = "AT+CIPCLOSE\r\n";
const uint8_t CLIENT_ID[] 			= "ABCDEF";
const uint8_t USERNAME[] 			= "wayjxdzy";
const uint8_t PASSWORD[] 			= "2Awwc3zlS20r";
const uint8_t SUBSCRIBE_TOPIC_1[] 	= "sensor/s";
const uint8_t SUBSCRIBE_TOPIC_2[] 	= "sensor/123";

const uint8_t PUBLISH_TOPIC_1[] 		= "sensor/s1";
const uint8_t PUBLISH_TOPIC_2[] 		= "sensor/s2";

uint8_t publish_message[20] = "123456789";

uint8_t mqttMessageLength;
uint8_t mqttMessage[MQTT_MESSAGE_BUFFER_LENGTH];
uint8_t commandBuffer[20];
uint8_t commandBufferIndex = 0;

uint8_t mqttMessageIndex = 0;
uint8_t subscribeTopicIndex = 0;
uint8_t publishTopicIndex = 0;
uint8_t mqtt_Timeout_Task_Index = SCH_MAX_TASKS;


static uint8_t mqtt_TimeoutFlag = 0;
///////////////////////////////////////////////////////////////////////////
void Clear_Mqtt_Message_Buffer(void);

void SM_Mqtt_Open(void);
void SM_Mqtt_Wait_For_Response_From_Open_State(void);
void SM_Mqtt_Connect(void);
void SM_Mqtt_Wait_For_Response_From_Connect_State(void);
void SM_Mqtt_Subscribe(void);
void SM_Mqtt_Wait_For_Response_From_Subscribe_State(void);
void SM_Mqtt_Publish(void);
void SM_Mqtt_Wait_For_Response_From_Publish_State(void);
void SM_Mqtt_Ping_Request(void);
void SM_Wait_For_Mqtt_Ping_Resquest_Response(void);
void SM_Mqtt_Disconnect(void);
void SM_Wait_For_Response_From_Disconnect_State(void);
void SM_Wait_For_New_Command(void);

void SM_Send_Data(uint8_t mesageLength);

//void Setup_Mqtt_Connect_Message(void);
//void Setup_Mqtt_Subscribe_Message(const uint8_t * topic);
//void Setup_Mqtt_Publish_Message(const uint8_t * topic, uint8_t * message, uint8_t lenOfMessage);
void Setup_Mqtt_Ping_Request_Message(void);


MQTT_Machine_Type MQTT_State_Machine [] = {
		{MQTT_OPEN_STATE, 										SM_Mqtt_Open										},
		{MQTT_WAIT_FOR_RESPONSE_FROM_OPEN_STATE, 				SM_Mqtt_Wait_For_Response_From_Open_State			},
		{MQTT_CONNECT_STATE, 									SM_Mqtt_Connect										},
		{MQTT_WAIT_FOR_RESPONSE_FROM_CONNECT_STATE, 			SM_Mqtt_Wait_For_Response_From_Connect_State		},
		{MQTT_SUBSCRIBE_STATE,									SM_Mqtt_Subscribe									},
		{MQTT_WAIT_FOR_RESPONSE_FROM_SUBSCRIBE_STATE,			SM_Mqtt_Wait_For_Response_From_Subscribe_State		},
		{MQTT_PUBLISH_STATE,									SM_Mqtt_Publish										},
		{MQTT_WAIT_FOR_RESPONSE_FROM_PUBLISH_STATE,				SM_Mqtt_Wait_For_Response_From_Publish_State		},
		{MQTT_PING_REQUEST_STATE,								SM_Mqtt_Ping_Request								},
		{MQTT_WAIT_FOR_RESPONSE_FROM_REQUEST_STATE,				SM_Wait_For_Mqtt_Ping_Resquest_Response				},
		{MQTT_DISCONNECT_STATE, 								SM_Mqtt_Disconnect									},
		{MQTT_WAIT_FOR_RESPONSE_FROM_DISCONNECT_STATE,			SM_Wait_For_Response_From_Disconnect_State			},
		{MQTT_WAIT_FOR_NEW_COMMAND,								SM_Wait_For_New_Command								}

};

MQTT_STATE mqttState = MQTT_OPEN_STATE;
uint8_t MQTT_Run(void){
	if(mqttState < MAX_MQTT_NUMBER_STATES){
		(*MQTT_State_Machine[mqttState].func)();
		return 0;
	} else {
		return 1;
	}
}


void Mqtt_Clear_Timeout_Flag(void){
	mqtt_TimeoutFlag = 0;
}
void Mqtt_Command_Timeout(void){
	mqtt_TimeoutFlag = 1;
}
uint8_t is_Mqtt_Command_Timeout(void){
	return mqtt_TimeoutFlag;
}

void Set_Mqtt_State(MQTT_STATE newState){
	mqttState = newState;
}
MQTT_STATE Get_Mqtt_State(void){
	return mqttState;
}

void Clear_Mqtt_Message_Buffer(void){
	uint8_t idx;
	for(idx = 0; idx < MQTT_MESSAGE_BUFFER_LENGTH; idx ++){
		mqttMessage[idx] = 0;
 	}
	mqttMessageIndex = 0;
}
void SM_Send_Data(uint8_t mesageLength){

	commandBufferIndex = 0;
	uint8_t tempMessageLength = mesageLength;
	commandBuffer[commandBufferIndex++] = 'A';
	commandBuffer[commandBufferIndex++] = 'T';
	commandBuffer[commandBufferIndex++] = '+';
	commandBuffer[commandBufferIndex++] = 'C';
	commandBuffer[commandBufferIndex++] = 'I';
	commandBuffer[commandBufferIndex++] = 'P';
	commandBuffer[commandBufferIndex++] = 'S';
	commandBuffer[commandBufferIndex++] = 'E';
	commandBuffer[commandBufferIndex++] = 'N';
	commandBuffer[commandBufferIndex++] = 'D';
	commandBuffer[commandBufferIndex++] = '=';
	commandBuffer[commandBufferIndex++] = '0';
	commandBuffer[commandBufferIndex++] = ',';

	if(mesageLength >= 100){
		commandBuffer[commandBufferIndex++] = tempMessageLength/100 + 0x30;
		tempMessageLength = tempMessageLength % 100;
		commandBuffer[commandBufferIndex++] = tempMessageLength/10 + 0x30;
		tempMessageLength = tempMessageLength % 10;
		commandBuffer[commandBufferIndex++] = tempMessageLength + 0x30;

	} else if(mesageLength >= 10){
		commandBuffer[commandBufferIndex++] = tempMessageLength/10 + 0x30;
		tempMessageLength = tempMessageLength % 10;
		commandBuffer[commandBufferIndex++] = tempMessageLength + 0x30;
	} else {
		commandBuffer[commandBufferIndex++] = tempMessageLength + 0x30;
	}
	commandBuffer[commandBufferIndex++] = CR;
//	commandBuffer[commandBufferIndex++] = 0;

//	ATcommandSending((uint8_t *)commandBuffer);
	MQTTCommandSending((uint8_t *)commandBuffer, commandBufferIndex);
	UART3_SendToHost((uint8_t *)commandBuffer);
}

void SM_Mqtt_Open(void){
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)MQTTOPEN_COMMAND);
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_OPEN_STATE;
}

void SM_Mqtt_Wait_For_Response_From_Open_State(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			mqttState = MQTT_CONNECT_STATE;
		} else if(isReceivedData((uint8_t *)ERROR_1)){
			mqttState = MAX_MQTT_NUMBER_STATES;
		}
	}
	if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}

void SM_Mqtt_Connect(void){
	SCH_Delete_Task(mqtt_Timeout_Task_Index);
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	Setup_Mqtt_Connect_Message();
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_CONNECT_STATE;
}

void SM_Mqtt_Wait_For_Response_From_Connect_State(void){
	if(isGreaterThanSymbol()){
		MQTTCommandSending((uint8_t *)mqttMessage, mqttMessageLength);
	}
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)RECV_FROM)){
			mqttState = MQTT_SUBSCRIBE_STATE;
		} else if(isReceivedData((uint8_t *)ERROR_1)){
			mqttState = MAX_MQTT_NUMBER_STATES;
		}
	}
	if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}




void SM_Mqtt_Subscribe (void) {
	SCH_Delete_Task(mqtt_Timeout_Task_Index);
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	if(subscribeTopicIndex < NUMBER_OF_SUBSCRIBE_TOPIC){
		if(subscribeTopicIndex == 0){
			Setup_Mqtt_Subscribe_Message(SUBSCRIBE_TOPIC_1);
		} else if(subscribeTopicIndex == 1){
			Setup_Mqtt_Subscribe_Message(SUBSCRIBE_TOPIC_2);
		}
	}
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_SUBSCRIBE_STATE;
}


void SM_Mqtt_Wait_For_Response_From_Subscribe_State(void){
	if(isGreaterThanSymbol()){
		MQTTCommandSending((uint8_t *)mqttMessage, mqttMessageLength);
	}
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)RECV_FROM)){
			subscribeTopicIndex ++;
			if(subscribeTopicIndex < NUMBER_OF_SUBSCRIBE_TOPIC){
				mqttState = MQTT_SUBSCRIBE_STATE;
			} else {
				mqttState = MQTT_WAIT_FOR_NEW_COMMAND;
			}
		} else if(isReceivedData((uint8_t *)ERROR_1)){
			mqttState = MAX_MQTT_NUMBER_STATES;
		}
	}
	if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}


void SM_Wait_For_New_Command(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)RECV_FROM)){
			mqttState = MQTT_PUBLISH_STATE;
		} else if(isReceivedData((uint8_t *)ERROR_1)){
			mqttState = MAX_MQTT_NUMBER_STATES;
		}
	}
}


void SM_Mqtt_Publish(void){
	SCH_Delete_Task(mqtt_Timeout_Task_Index);
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_PUBLISH_STATE;
}

void SM_Mqtt_Wait_For_Response_From_Publish_State(void){
	if(isGreaterThanSymbol()){
		MQTTCommandSending((uint8_t *)mqttMessage, mqttMessageLength);
	}
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)Send_ok)){
			mqttState = MQTT_WAIT_FOR_NEW_COMMAND;

		} else if(isReceivedData((uint8_t *)ERROR_1)){
			mqttState = MAX_MQTT_NUMBER_STATES;
		}
	}
	if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}
void SM_Mqtt_Disconnect(void){
	SCH_Delete_Task(mqtt_Timeout_Task_Index);
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)MQTTCLOSE_COMMAND);
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_DISCONNECT_STATE;
}
void SM_Wait_For_Response_From_Disconnect_State(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			mqttState = MQTT_OPEN_STATE;
		} else if(isReceivedData((uint8_t *)ERROR_1)){
			mqttState = MAX_MQTT_NUMBER_STATES;
		}
	}
	if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}

void SM_Mqtt_Ping_Request(void) {
	SCH_Delete_Task(mqtt_Timeout_Task_Index);
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_REQUEST_STATE;
}
void SM_Wait_For_Mqtt_Ping_Resquest_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			mqttState = MQTT_WAIT_FOR_NEW_COMMAND;
		} else if(isReceivedData((uint8_t *)ERROR_1)){
			mqttState = MAX_MQTT_NUMBER_STATES;
		}
	}
	if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}
///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Connect_Message(void){

	uint8_t i;
	uint8_t clientIdLength = GetStringLength((uint8_t*)CLIENT_ID);
	uint8_t usernameLength = GetStringLength((uint8_t*)USERNAME);
	uint8_t passwordLength = GetStringLength((uint8_t*)PASSWORD);

	Clear_Mqtt_Message_Buffer();

	// Header
	mqttMessage[mqttMessageIndex++] = MQTT_MSG_CONNECT;
	mqttMessage[mqttMessageIndex++] = 18 + clientIdLength + usernameLength + passwordLength;    // Remaining length of the message (bytes 2-13 + clientId)

	// Protocol name
	mqttMessage[mqttMessageIndex++] = 0;                      // Protocol Name Length MSB
	mqttMessage[mqttMessageIndex++] = 6;                      // Protocol Name Length LSB
	mqttMessage[mqttMessageIndex++] = 'M';
	mqttMessage[mqttMessageIndex++] = 'Q';
	mqttMessage[mqttMessageIndex++] = 'I';
	mqttMessage[mqttMessageIndex++] = 's';
	mqttMessage[mqttMessageIndex++] = 'd';
	mqttMessage[mqttMessageIndex++] = 'p';

	// Protocol level
	mqttMessage[mqttMessageIndex++] = 3;                      // MQTT Protocol version = 4 (mqtt 3.1.1)

	// Connection flags   2 for clean session
 //   mqttMessage[9] = 2;
	mqttMessage[mqttMessageIndex++] = 0b11000010;  //enable username, password, clean session

	// Keep-alive (maximum duration)
	mqttMessage[mqttMessageIndex++] = 0x00;                     // Keep-alive Time Length MSB
	mqttMessage[mqttMessageIndex++] = 0x3c;                     // Keep-alive Time Length LSB

	// Client ID
	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
	mqttMessage[mqttMessageIndex++] = clientIdLength;        // Client ID length LSB
	for (i = 0; i < clientIdLength; i ++){
		mqttMessage[mqttMessageIndex++] = CLIENT_ID[i];
	}
//	memcpy(&mqttMessage[16], CLIENT_ID, clientIdLength);

	// Username
	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
	mqttMessage[mqttMessageIndex++] = usernameLength;        // Client ID length LSB
	for (i = 0; i < usernameLength; i ++){
		mqttMessage[mqttMessageIndex++] = USERNAME[i];
	}
//	memcpy(&mqttMessage[18 + clientIdLength], USERNAME, usernameLength);

	// Password
	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
	mqttMessage[mqttMessageIndex++] = passwordLength;        // Client ID length LSB
	for (i = 0; i < passwordLength; i ++){
		mqttMessage[mqttMessageIndex++] = PASSWORD[i];
	}
//	memcpy(&mqttMessage[20 + clientIdLength + usernameLength], PASSWORD, passwordLength);
	mqttMessageLength = mqttMessageIndex;
	SM_Send_Data(mqttMessageLength);

}
///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Connect_Message_311(void){
	uint8_t clientIdLength = sizeof(CLIENT_ID);
	uint8_t usernameLength = sizeof(USERNAME);
	uint8_t passwordLength = sizeof(PASSWORD);

	Clear_Mqtt_Message_Buffer();
	mqttMessageLength = 19 + clientIdLength + usernameLength + passwordLength;

	// Header
	mqttMessage[mqttMessageIndex++] = MQTT_MSG_CONNECT;
	mqttMessage[mqttMessageIndex++] = 17 + clientIdLength + usernameLength + passwordLength;    // Remaining length of the message (bytes 2-13 + clientId)

	// Protocol name
	mqttMessage[mqttMessageIndex++] = 0;                      // Protocol Name Length MSB
	mqttMessage[mqttMessageIndex++] = 4;                      // Protocol Name Length LSB
	mqttMessage[mqttMessageIndex++] = 'M';
	mqttMessage[mqttMessageIndex++] = 'Q';
	mqttMessage[mqttMessageIndex++] = 'T';
	mqttMessage[mqttMessageIndex++] = 'T';

	// Protocol level
	mqttMessage[mqttMessageIndex++] = 4;                      // MQTT Protocol version = 4 (mqtt 3.1.1)

	// Connection flags   2 for clean session
 //   mqttMessage[9] = 2;
	mqttMessage[mqttMessageIndex++] = 0b11000010;  //enable username, password, clean session

	// Keep-alive (maximum duration)
	mqttMessage[mqttMessageIndex++] = 100;                     // Keep-alive Time Length MSB
	mqttMessage[mqttMessageIndex++] = 100;                     // Keep-alive Time Length LSB

	// Client ID
	mqttMessage[12] = 0;                     // Client ID length MSB
	mqttMessage[13] = clientIdLength;        // Client ID length LSB
	memcpy(&mqttMessage[14], CLIENT_ID, clientIdLength);

	// Username
	mqttMessage[14 + clientIdLength] = 0;                     // Client ID length MSB
	mqttMessage[15 + clientIdLength] = usernameLength;        // Client ID length LSB
	memcpy(&mqttMessage[16 + clientIdLength], USERNAME, usernameLength);

	// Password
	mqttMessage[16 + clientIdLength + usernameLength] = 0;                     // Client ID length MSB
	mqttMessage[17 + clientIdLength + usernameLength] = passwordLength;        // Client ID length LSB
	memcpy(&mqttMessage[18 + clientIdLength + usernameLength], PASSWORD, passwordLength);
	SM_Send_Data(mqttMessageLength);
}
//////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Subscribe_Message(const uint8_t * topic){
	uint8_t i = 0;
	uint8_t lenTopic = GetStringLength((uint8_t*)topic);

	Clear_Mqtt_Message_Buffer();
	mqttMessageIndex = 0;
	// Write fixed header
	mqttMessage[mqttMessageIndex++] = MQTT_SUBSCRIBE;
	mqttMessage[mqttMessageIndex++] = 5 + lenTopic;

	//Variable Header
	mqttMessage[mqttMessageIndex++] = 0; // MSB
	mqttMessage[mqttMessageIndex++] = 1; // LSB message ID
	// TODO: make message id dynamic

	// Write topic
	mqttMessage[mqttMessageIndex++] = 0;         // lenTopic MSB
	mqttMessage[mqttMessageIndex++] = lenTopic;  // lenTopic LSB

	for (i = 0; i < lenTopic; i++){
		mqttMessage[mqttMessageIndex++] = topic[i];
	}
//	memcpy(&mqttMessage[mqttMessageIndex], SUBSCRIBE_TOPIC, lenTopic);
//	mqttMessageIndex += lenTopic;

	// Write QoS
	mqttMessage[mqttMessageIndex++] = 0;
	mqttMessageLength = mqttMessageIndex;
	SM_Send_Data(mqttMessageLength);
}

///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Publish_Message(const uint8_t * topic, uint8_t * message, uint8_t lenOfMessage){
	uint8_t i;
	uint8_t lenOfTopic = GetStringLength((uint8_t*)topic);

//	mqttMessageLength = 4 + lenTopic + lenMsg;
	Clear_Mqtt_Message_Buffer();
	// Write fixed header
	mqttMessage[mqttMessageIndex++] = MQTT_MSG_PUBLISH;
	mqttMessage[mqttMessageIndex++] = 2 + lenOfTopic + lenOfMessage;

	// Write topic
	mqttMessage[mqttMessageIndex++] = 0;         // lenTopic MSB
	mqttMessage[mqttMessageIndex++] = lenOfTopic;  // lenTopic LSB
	for (i = 0; i < lenOfTopic; i++){
		mqttMessage[mqttMessageIndex++] = topic[i];
	}
//	memcpy(&mqttMessage[mqttMessageIndex], PUBLISH_TOPIC, lenTopic);
//	mqttMessageIndex += lenTopic;

	// Write msg
	for (i = 0; i < lenOfMessage; i++){
		mqttMessage[mqttMessageIndex++] = message[i];
	}
//	memcpy(&mqttMessage[mqttMessageIndex], publish_message, lenMsg);
//	mqttMessageIndex += lenMsg;
	mqttMessageLength = mqttMessageIndex;
	SM_Send_Data(mqttMessageLength);
}


///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Ping_Request_Message(void){
	// Write fixed header
	Clear_Mqtt_Message_Buffer();
	mqttMessage[mqttMessageIndex++] = MQTT_PINGREQ;
	mqttMessage[mqttMessageIndex++] = 0;
	mqttMessageLength = mqttMessageIndex;
	SM_Send_Data(mqttMessageLength);
}

