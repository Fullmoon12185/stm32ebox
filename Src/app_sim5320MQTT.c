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

extern const uint8_t OK[];
extern const uint8_t CONNECT_OK[];
extern const uint8_t ERROR_1[];
extern const uint8_t GREATER_THAN_SYMBOL[];
extern const uint8_t NETWORK_OPENED[];

const uint8_t MQTTOPEN_COMMAND[] 	= "AT+CIPOPEN=0,\"TCP\",\"tailor.cloudmqtt.com\", 13255";
const uint8_t MQTTCLOSE_COMMAND[]   = "AT+CIPCLOSE\r\n";
const uint8_t CLIENT_ID[] 			= "sim5320MQTT";
const uint8_t USERNAME[] 			= "wayjxdzy";
const uint8_t PASSWORD[] 			= "2Awwc3zlS20r";
const uint8_t SUBSCRIBE_TOPIC[] 	= "subcribe/eboxReceive";
const uint8_t PUBLISH_TOPIC[] 		= "publish/eboxSend";
uint8_t publish_message[20];
uint8_t mqttMessageLength;
uint8_t mqttMessage[MQTT_MESSAGE_BUFFER_LENGTH];
uint8_t mqttMessageIndex = 0;


static uint8_t mqtt_TimeoutFlag = 0;
///////////////////////////////////////////////////////////////////////////
void Clear_Mqtt_Message_Buffer(void);
void SM_Mqtt_Open(void);
void SM_Mqtt_Connect(void);
void Setup_Mqtt_Connect_Message(void);
void SM_Mqtt_Subscribe(void);
void Setup_Mqtt_Subscribe_Message(void);
void SM_Mqtt_Publish(void);
void Setup_Mqtt_Publish_Message(void);
void SM_Mqtt_Ping_Request(void);
void Setup_Mqtt_Ping_Request_Message(void);


MQTT_Machine_Type MQTT_State_Machine [] = {
		{MQTT_OPEN_STATE, 				SM_Mqtt_Open		},
		{MQTT_CONNECT_STATE, 			SM_Mqtt_Connect		},
		{MQTT_SUBSCRIBE_STATE,			SM_Mqtt_Subscribe	},
		{MQTT_PUBLISH_STATE,			SM_Mqtt_Publish		},
		{MQTT_PING_REQUEST_STATE,		SM_Mqtt_Ping_Request}

};

MQTT_STATE mqttState = MQTT_OPEN_STATE;
void MQTT_Run(void){
	if(mqttState < MAX_MQTT_NUMBER_STATES){
		(*MQTT_State_Machine[mqttState].func)();
	} else {
		//throw an excepton.
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

void Clear_Mqtt_Message_Buffer(void){
	uint8_t idx;
	for(idx = 0; idx < MQTT_MESSAGE_BUFFER_LENGTH; idx ++){
		mqttMessage[idx] = 0;
 	}
	mqttMessageIndex = 0;
}
void SM_Send_Data(){
	uint8_t commandBuffer[20];
	uint8_t commandBufferIndex = 0;
	uint8_t tempMessageLength = mqttMessageLength;
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

	if(mqttMessageLength >= 100){
		commandBuffer[commandBufferIndex++] = tempMessageLength/100;
		tempMessageLength = tempMessageLength % 100;
		commandBuffer[commandBufferIndex++] = tempMessageLength/10;
		tempMessageLength = tempMessageLength % 10;
		commandBuffer[commandBufferIndex++] = tempMessageLength;

	} else if(mqttMessageLength >= 10){
		commandBuffer[commandBufferIndex++] = tempMessageLength/10;
		tempMessageLength = tempMessageLength % 10;
		commandBuffer[commandBufferIndex++] = tempMessageLength;
	} else {
		commandBuffer[commandBufferIndex++] = tempMessageLength;
	}

	commandBuffer[commandBufferIndex++] = CR;
	commandBuffer[commandBufferIndex++] = 0;
	ATcommandSending((uint8_t *)commandBuffer);
	//	Mqtt_Clear_Timeout_Flag();
	//	SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	//	ATcommandSending((uint8_t *)AT_CIPSEND);
	//	mqttState = WAIT_FOR_SEND_DATA_RESPONSE;
}

void SM_Wait_For_Send_Data_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)GREATER_THAN_SYMBOL)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Mqtt_Command_Timeout()){

	}
}
void SM_Mqtt_Open(void){
	Mqtt_Clear_Timeout_Flag();
	SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)MQTTOPEN_COMMAND);
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_OPEN_STATE;
}
void SM_Mqtt_Wait_For_Response_From_Open_State(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			mqttState = MQTT_CONNECT_STATE;
		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Mqtt_Command_Timeout()){

	}
}

void SM_Mqtt_Connect(void){
	Setup_Mqtt_Connect_Message();
	Mqtt_Clear_Timeout_Flag();
	SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)mqttMessage);
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_CONNECT_STATE;
}
void SM_Mqtt_Wait_For_Response_From_Connect_State(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			mqttState = MQTT_SUBSCRIBE_STATE;
		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Mqtt_Command_Timeout()){

	}
}


void SM_Mqtt_Subscribe (void) {
	Setup_Mqtt_Subscribe_Message();
	Mqtt_Clear_Timeout_Flag();
	SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)mqttMessage);
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_SUBSCRIBE_STATE;
}
void SM_Mqtt_Wait_For_Response_From_Subscribe_State(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){
			mqttState = MQTT_PUBLISH_STATE;
		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Mqtt_Command_Timeout()){

	}
}

void SM_Mqtt_Publish(void){
	Setup_Mqtt_Publish_Message();
}
void SM_MQTT_Disconnect(void){
	Mqtt_Clear_Timeout_Flag();
	SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	ATcommandSending((uint8_t *)MQTTCLOSE_COMMAND);
	mqttState = MQTT_WAIT_FOR_DISCONNECT_RESPONSE;
}
void SM_Wait_For_Disconnect_Response(void){
	if(isSim3gReceiveReady()){
		if(isReceivedData((uint8_t *)OK)){

		} else if(isReceivedData((uint8_t *)ERROR_1)){

		}
	}
	if(is_Mqtt_Command_Timeout()){

	}
}

void SM_Mqtt_Ping_Request(void) {


}
void SM_Wait_For_Mqtt_Ping_Resquest_Response(void){

}
///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Connect_Message(void){
	uint8_t clientIdLength = sizeof(CLIENT_ID);
	uint8_t usernameLength = sizeof(USERNAME);
	uint8_t passwordLength = sizeof(PASSWORD);

	Clear_Mqtt_Message_Buffer();
	mqttMessageLength = 21 + clientIdLength + usernameLength + passwordLength;

	// Header
	mqttMessage[0] = MQTT_MSG_CONNECT;
	mqttMessage[1] = 19 + clientIdLength + usernameLength + passwordLength;    // Remaining length of the message (bytes 2-13 + clientId)

	// Protocol name
	mqttMessage[2] = 0;                      // Protocol Name Length MSB
	mqttMessage[3] = 4;                      // Protocol Name Length LSB
	mqttMessage[4] = 'M';
	mqttMessage[5] = 'Q';
	mqttMessage[6] = 'I';
	mqttMessage[7] = 's';
	mqttMessage[8] = 'd';
	mqttMessage[9] = 'p';

	// Protocol level
	mqttMessage[10] = 3;                      // MQTT Protocol version = 4 (mqtt 3.1.1)

	// Connection flags   2 for clean session
 //   mqttMessage[9] = 2;
	mqttMessage[11] = 0b11000010;  //enable username, password, clean session

	// Keep-alive (maximum duration)
	mqttMessage[12] = 0x00;                     // Keep-alive Time Length MSB
	mqttMessage[13] = 0x3c;                     // Keep-alive Time Length LSB

	// Client ID
	mqttMessage[14] = 0;                     // Client ID length MSB
	mqttMessage[15] = clientIdLength;        // Client ID length LSB
	memcpy(&mqttMessage[16], CLIENT_ID, clientIdLength);

	// Username
	mqttMessage[16 + clientIdLength] = 0;                     // Client ID length MSB
	mqttMessage[17 + clientIdLength] = usernameLength;        // Client ID length LSB
	memcpy(&mqttMessage[18 + clientIdLength], USERNAME, usernameLength);

	// Password
	mqttMessage[18 + clientIdLength + usernameLength] = 0;                     // Client ID length MSB
	mqttMessage[19 + clientIdLength + usernameLength] = passwordLength;        // Client ID length LSB
	memcpy(&mqttMessage[20 + clientIdLength + usernameLength], PASSWORD, passwordLength);

}
///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Connect_Message_311(void){
	uint8_t clientIdLength = sizeof(CLIENT_ID);
	uint8_t usernameLength = sizeof(USERNAME);
	uint8_t passwordLength = sizeof(PASSWORD);

	Clear_Mqtt_Message_Buffer();
	mqttMessageLength = 19 + clientIdLength + usernameLength + passwordLength;

	// Header
	mqttMessage[0] = MQTT_MSG_CONNECT;
	mqttMessage[1] = 17 + clientIdLength + usernameLength + passwordLength;    // Remaining length of the message (bytes 2-13 + clientId)

	// Protocol name
	mqttMessage[2] = 0;                      // Protocol Name Length MSB
	mqttMessage[3] = 4;                      // Protocol Name Length LSB
	mqttMessage[4] = 'M';
	mqttMessage[5] = 'Q';
	mqttMessage[6] = 'T';
	mqttMessage[7] = 'T';

	// Protocol level
	mqttMessage[8] = 4;                      // MQTT Protocol version = 4 (mqtt 3.1.1)

	// Connection flags   2 for clean session
 //   mqttMessage[9] = 2;
	mqttMessage[9] = 0b11000010;  //enable username, password, clean session

	// Keep-alive (maximum duration)
	mqttMessage[10] = 100;                     // Keep-alive Time Length MSB
	mqttMessage[11] = 100;                     // Keep-alive Time Length LSB

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
}
//////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Subscribe_Message(void){
	uint8_t lenTopic = sizeof(SUBSCRIBE_TOPIC);

	Clear_Mqtt_Message_Buffer();
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

	memcpy(&mqttMessage[mqttMessageIndex], SUBSCRIBE_TOPIC, lenTopic);
	mqttMessageIndex += lenTopic;

	// Write QoS
	mqttMessage[mqttMessageIndex++] = 1;
}

///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Publish_Message(void){
	uint8_t lenTopic = sizeof(PUBLISH_TOPIC);
	uint8_t lenMsg   = GetStringLength(publish_message);


	Clear_Mqtt_Message_Buffer();
	// Write fixed header
	mqttMessage[mqttMessageIndex++] = MQTT_MSG_PUBLISH;
	mqttMessage[mqttMessageIndex++] = 2 + lenTopic + lenMsg;

	// Write topic
	mqttMessage[mqttMessageIndex++] = 0;         // lenTopic MSB
	mqttMessage[mqttMessageIndex++] = lenTopic;  // lenTopic LSB
	memcpy(&mqttMessage[mqttMessageIndex], PUBLISH_TOPIC, lenTopic);
	mqttMessageIndex += lenTopic;

	// Write msg
	memcpy(&mqttMessage[mqttMessageIndex], publish_message, lenMsg);
	mqttMessageIndex += lenMsg;
}


///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Ping_Request_Message(void){
	// Write fixed header
	Clear_Mqtt_Message_Buffer();
	mqttMessage[mqttMessageIndex++] = MQTT_PINGREQ;
	mqttMessage[mqttMessageIndex++] = 0;
}

