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

#define		CLOUD_MQTT  	1
#define		MOSQUITTO		0


#define		DEBUG_MQTT(X)	X
#define 	MQTT_COMMAND_TIME_OUT		(50000/INTERRUPT_TIMER_PERIOD)

#define 	MQTT_SUBSCRIBE_TIME_OUT		(3000/INTERRUPT_TIMER_PERIOD)



extern const uint8_t OK[];
extern const uint8_t CONNECT_OK[];
extern const uint8_t ERROR_1[];
extern const uint8_t GREATER_THAN_SYMBOL[];
extern const uint8_t NETWORK_OPENED[];
extern FlagStatus isOKFlag;
extern FlagStatus isErrorFlag;
extern FlagStatus isIPCloseFlag;
extern FlagStatus isRecvFromFlag;
extern FlagStatus isSendOKFlag;

extern FlagStatus isReadyToSendDataToServer;
extern FlagStatus isReceiveDataFromServer;
extern FlagStatus isCipsend;


const uint8_t MQTTCLOSE_COMMAND[]   = "AT+CIPCLOSE\r\n";
const uint8_t LAST_WILL_MESSAGE[]   = "0-0,1-0,2-0,3-0,4-0,5-0,6-0,7-0,8-0,9-0,10-5";
uint8_t CLIENT_ID[MAX_TOPIC_LENGTH];

//nguyen cloudmqtt

//const uint8_t MQTTOPEN_COMMAND[] 	= "AT+CIPOPEN=0,\"TCP\",\"tailor.cloudmqtt.com\",13255\r";
//const uint8_t USERNAME[] 			= "wayjxdzy";
//const uint8_t PASSWORD[] 			= "2Awwc3zlS20r";

//Vinh cloudmqtt
#if(CLOUD_MQTT == 1)
//const uint8_t MQTTOPEN_COMMAND[] 	= "AT+CIPOPEN=0,\"TCP\",\"tailor.cloudmqtt.com\",13204\r";
//const uint8_t USERNAME[] 			= "lyooeovx";
//const uint8_t PASSWORD[] 			= "7Pq6P2PWLqME";

const uint8_t MQTTOPEN_COMMAND[] 	= "AT+CIPOPEN=0,\"TCP\",\"mqtt-dev.demo-application.net\",8083\r";
const uint8_t USERNAME[] 			= "eboost-k2";
const uint8_t PASSWORD[] 			= "ZbHzPb5W";

#elif(MOSQUITTO == 1)
const uint8_t MQTTOPEN_COMMAND[] 	= "AT+CIPOPEN=0,\"TCP\",\"broker.hivemq.com\",1883\r";
const uint8_t USERNAME[] 			= "";
const uint8_t PASSWORD[] 			= "";

#endif

//const uint8_t SUBSCRIBE_TOPIC_1[] 	= "eBox/switch";
//const uint8_t SUBSCRIBE_TOPIC_2[] 	= "eBox/saves1";

uint8_t SUBSCRIBE_TOPIC_1[MAX_TOPIC_LENGTH];
uint8_t SUBSCRIBE_TOPIC_2[MAX_TOPIC_LENGTH];

//const uint8_t PUBLISH_TOPIC_STATUS[] 		= "eBox/status";
//const uint8_t PUBLISH_TOPIC_POWER[] 		= "eBox/power";


uint8_t PUBLISH_TOPIC_STATUS[MAX_TOPIC_LENGTH];
uint8_t PUBLISH_TOPIC_POWER[MAX_TOPIC_LENGTH];
uint8_t PUBLISH_TOPIC_VOLTAGE[MAX_TOPIC_LENGTH];
uint8_t PUBLISH_TOPIC_CURRENT[MAX_TOPIC_LENGTH];
uint8_t PUBLISH_TOPIC_POWERFACTOR[MAX_TOPIC_LENGTH];


uint8_t publish_message[MQTT_MESSAGE_BUFFER_LENGTH];
uint8_t publish_message_length = 0;

uint8_t mqttMessageLength;
uint8_t mqttMessage[MQTT_MESSAGE_BUFFER_LENGTH];
uint8_t commandBuffer[20];
uint8_t commandBufferIndex = 0;

uint8_t mqttMessageIndex = 0;
uint8_t subscribeTopicIndex = 0;
uint8_t publishTopicIndex = 0;
uint32_t mqtt_Timeout_Task_Index = NO_TASK_ID;

uint32_t mqtt_Timeout_Subscribe_Task_Index = NO_TASK_ID;


static uint8_t mqtt_TimeoutFlag = 0;
static uint8_t mqtt_Subscribe_TimeoutFlag = 0;
///////////////////////////////////////////////////////////////////////////

void Mqtt_Clear_Timeout_Flag(void);
void Mqtt_Command_Timeout(void);
uint8_t is_Mqtt_Command_Timeout(void);
void Mqtt_Clear_Subscribe_Timeout_Flag(void);
void Mqtt_Command_Timeout(void);
uint8_t is_Mqtt_Subscribe_Timeout(void);


void MQTT_State_Display(void);
void Clear_Mqtt_Message_Buffer(void);

void SM_Mqtt_Open(void);
void SM_Mqtt_Wait_For_Response_From_Open_State(void);
void SM_Mqtt_Connect(void);
void SM_Mqtt_Receive_Greater_Than_Symbol_Connect_State(void);
void SM_Mqtt_Wait_For_Response_From_Connect_State(void);
void SM_Mqtt_Subscribe(void);
void SM_Mqtt_Receive_Greater_Than_Symbol_Subscribe_State(void);
void SM_Mqtt_Wait_For_Response_From_Subscribe_State(void);


void SM_Mqtt_Publish(void);
void SM_Mqtt_Receive_Greater_Than_Symbol_Publish_State(void);
void SM_Mqtt_Wait_For_Response_From_Publish_State(void);

void SM_Mqtt_Ping_Request(void);
void SM_Mqtt_Receive_Greater_Than_Symbol_Ping_Request_State(void);
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
		{MQTT_RECEIVE_GREATER_THAN_SYMBOL_CONNECT_STATE, 		SM_Mqtt_Receive_Greater_Than_Symbol_Connect_State   },
		{MQTT_WAIT_FOR_RESPONSE_FROM_CONNECT_STATE, 			SM_Mqtt_Wait_For_Response_From_Connect_State		},
		{MQTT_SUBSCRIBE_STATE,									SM_Mqtt_Subscribe									},
		{MQTT_RECEIVE_GREATER_THAN_SYMBOL_SUBSCRIBE_STATE, 		SM_Mqtt_Receive_Greater_Than_Symbol_Subscribe_State   },
		{MQTT_WAIT_FOR_RESPONSE_FROM_SUBSCRIBE_STATE,			SM_Mqtt_Wait_For_Response_From_Subscribe_State		},
		{MQTT_PUBLISH_STATE,									SM_Mqtt_Publish										},
		{MQTT_RECEIVE_GREATER_THAN_SYMBOL_PUBLISH_STATE, 		SM_Mqtt_Receive_Greater_Than_Symbol_Publish_State   },
		{MQTT_WAIT_FOR_RESPONSE_FROM_PUBLISH_STATE,				SM_Mqtt_Wait_For_Response_From_Publish_State		},
		{MQTT_PING_REQUEST_STATE,								SM_Mqtt_Ping_Request								},
		{MQTT_RECEIVE_GREATER_THAN_SYMBOL_PING_REQUEST_STATE, 	SM_Mqtt_Receive_Greater_Than_Symbol_Ping_Request_State   },
		{MQTT_WAIT_FOR_RESPONSE_FROM_REQUEST_STATE,				SM_Wait_For_Mqtt_Ping_Resquest_Response				},
		{MQTT_DISCONNECT_STATE, 								SM_Mqtt_Disconnect									},
		{MQTT_WAIT_FOR_RESPONSE_FROM_DISCONNECT_STATE,			SM_Wait_For_Response_From_Disconnect_State			},
		{MQTT_WAIT_FOR_NEW_COMMAND,								SM_Wait_For_New_Command								}

};

MQTT_STATE mqttState = MQTT_OPEN_STATE;
MQTT_STATE pre_mqttState = MAX_MQTT_NUMBER_STATES;

void Set_Up_Topic_Names(void){

	uint16_t boxID = Get_Box_ID();
	uint8_t indexTopicCharacter = 0;
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = 'C';
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = 'E';
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = 'b';
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = 'o';
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = 'x';
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = '_';

	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = boxID / 1000 + 0x30;
	boxID = boxID%1000;
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = boxID / 100 + 0x30;
	boxID = boxID%100;
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = boxID / 10 + 0x30;
	boxID = boxID%10;
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = boxID + 0x30;
	SUBSCRIBE_TOPIC_1[indexTopicCharacter++] = 0;

	indexTopicCharacter = 0;
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = 'Z';
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = 'E';
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = 'b';
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = 'o';
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = 'x';
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = '_';

	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[6];
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[7];
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[8];
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[9];
	SUBSCRIBE_TOPIC_2[indexTopicCharacter++] = 0;


	indexTopicCharacter = 0;
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = 'S';
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = 'E';
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = 'b';
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = 'o';
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = 'x';
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = '_';

	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[6];
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[7];
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[8];
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[9];
	PUBLISH_TOPIC_STATUS[indexTopicCharacter++] = 0;

	indexTopicCharacter = 0;
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = 'P';
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = 'E';
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = 'b';
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = 'o';
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = 'x';
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = '_';

	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[6];
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[7];
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[8];
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[9];
	PUBLISH_TOPIC_POWER[indexTopicCharacter++] = 0;


	indexTopicCharacter = 0;
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = 'V';
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = 'E';
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = 'b';
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = 'o';
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = 'x';
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = '_';

	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[6];
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[7];
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[8];
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[9];
	PUBLISH_TOPIC_VOLTAGE[indexTopicCharacter++] = 0;

	indexTopicCharacter = 0;
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = 'A';
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = 'E';
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = 'b';
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = 'o';
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = 'x';
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = '_';

	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[6];
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[7];
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[8];
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[9];
	PUBLISH_TOPIC_CURRENT[indexTopicCharacter++] = 0;



	indexTopicCharacter = 0;
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = 'P';
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = 'F';
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = 'E';
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = 'b';
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = 'o';
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = 'x';
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = '_';

	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[6];
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[7];
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[8];
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[9];
	PUBLISH_TOPIC_POWERFACTOR[indexTopicCharacter++] = 0;

	indexTopicCharacter = 0;
	CLIENT_ID[indexTopicCharacter++] = 'E';
	CLIENT_ID[indexTopicCharacter++] = 'b';
	CLIENT_ID[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[6];
	CLIENT_ID[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[7];
	CLIENT_ID[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[8];
	CLIENT_ID[indexTopicCharacter++] = SUBSCRIBE_TOPIC_1[9];
	CLIENT_ID[indexTopicCharacter++] = 0;
}



uint8_t MQTT_Run(void){
//	MQTT_State_Display();
	if(mqttState < MAX_MQTT_NUMBER_STATES){
		(*MQTT_State_Machine[mqttState].func)();
		return 0;
	} else {
		return 1;
	}
}

void MQTT_State_Display(void){
	if(pre_mqttState != mqttState){
		pre_mqttState = mqttState;
		switch(mqttState){
		case MQTT_OPEN_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_OPEN_STATE"););
			break;
		case MQTT_WAIT_FOR_RESPONSE_FROM_OPEN_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_WAIT_FOR_RESPONSE_FROM_OPEN_STATE"););
			break;
		case MQTT_CONNECT_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_CONNECT_STATE\r"););
			break;
		case MQTT_WAIT_FOR_RESPONSE_FROM_CONNECT_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_WAIT_FOR_RESPONSE_FROM_CONNECT_STATE\r"););

			break;
		case MQTT_SUBSCRIBE_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_SUBSCRIBE_STATE\r"););
			break;

		case MQTT_WAIT_FOR_RESPONSE_FROM_SUBSCRIBE_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_WAIT_FOR_RESPONSE_FROM_SUBSCRIBE_STATE\r"););
			break;

		case MQTT_PUBLISH_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_PUBLISH_STATE\r"););
			break;
		case MQTT_WAIT_FOR_RESPONSE_FROM_PUBLISH_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_WAIT_FOR_RESPONSE_FROM_PUBLISH_STATE\r"););
			break;
		case MQTT_PING_REQUEST_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_PING_REQUEST_STATE\r"););
			break;
		case MQTT_WAIT_FOR_RESPONSE_FROM_REQUEST_STATE:
			DEBUG_MQTT(UART3_SendToHost((uint8_t*)"MQTT_WAIT_FOR_RESPONSE_FROM_REQUEST_STATE\r"););
			break;
		default:
			break;
		}
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

void Mqtt_Clear_Subscribe_Timeout_Flag(void){
	mqtt_Subscribe_TimeoutFlag = 0;
}
void Mqtt_Subscribe_Timeout(void){
	mqtt_Subscribe_TimeoutFlag = 1;
}
uint8_t is_Mqtt_Subscribe_Timeout(void){
	return mqtt_Subscribe_TimeoutFlag;
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

	isReadyToSendDataToServer = RESET;
	MQTTCommandSending((uint8_t *)commandBuffer, commandBufferIndex);
}

void SM_Mqtt_Open(void){
	SCH_Delete_Task(mqtt_Timeout_Task_Index);
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	Clear_All_Uart_Receive_Flags();
	ATcommandSending((uint8_t *)MQTTOPEN_COMMAND);
	mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_OPEN_STATE;
}

void SM_Mqtt_Wait_For_Response_From_Open_State(void){
	if(isOKFlag){
		isOKFlag = RESET;
		mqttState = MQTT_CONNECT_STATE;
		SCH_Delete_Task(mqtt_Timeout_Task_Index);
		Mqtt_Clear_Timeout_Flag();
		mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, 100, 0);
	} else if(isErrorFlag){
		isErrorFlag = 0;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}

void SM_Mqtt_Connect(void){
	if(is_Mqtt_Command_Timeout()){
		SCH_Delete_Task(mqtt_Timeout_Task_Index);
		Mqtt_Clear_Timeout_Flag();
		mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
		isReadyToSendDataToServer = RESET;
		Setup_Mqtt_Connect_Message();
		mqttState = MQTT_RECEIVE_GREATER_THAN_SYMBOL_CONNECT_STATE;
	}
}
void SM_Mqtt_Receive_Greater_Than_Symbol_Connect_State(void){
	if(isReadyToSendDataToServer){
		isReadyToSendDataToServer = RESET;
		isRecvFromFlag = RESET;
		isSendOKFlag = RESET;
		isErrorFlag = RESET;
		MQTTCommandSending((uint8_t *)mqttMessage, mqttMessageLength);
		mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_CONNECT_STATE;
	} else if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}
void SM_Mqtt_Wait_For_Response_From_Connect_State(void){
	if(isRecvFromFlag){
		isRecvFromFlag = RESET;
		subscribeTopicIndex = 0;
		mqttState = MQTT_SUBSCRIBE_STATE;
	} else if(isErrorFlag){
		isErrorFlag = 0;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(is_Mqtt_Command_Timeout()){
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
	isErrorFlag = RESET;
	isIPCloseFlag = RESET;
	mqttState = MQTT_RECEIVE_GREATER_THAN_SYMBOL_SUBSCRIBE_STATE;
}

void SM_Mqtt_Receive_Greater_Than_Symbol_Subscribe_State(void){
	if(isReadyToSendDataToServer){
		isReadyToSendDataToServer = RESET;
		isRecvFromFlag = RESET;
		isSendOKFlag = RESET;
		isErrorFlag = RESET;
		isIPCloseFlag = RESET;
		MQTTCommandSending((uint8_t *)mqttMessage, mqttMessageLength);
		mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_SUBSCRIBE_STATE;

		SCH_Delete_Task(mqtt_Timeout_Subscribe_Task_Index);
		Mqtt_Clear_Subscribe_Timeout_Flag();
		mqtt_Timeout_Subscribe_Task_Index = SCH_Add_Task(Mqtt_Subscribe_Timeout, MQTT_SUBSCRIBE_TIME_OUT, 0);
	} else if(isErrorFlag){
		isErrorFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(isIPCloseFlag){
		isIPCloseFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}

void SM_Mqtt_Wait_For_Response_From_Subscribe_State(void){
	if(isRecvFromFlag){
		if(is_Mqtt_Subscribe_Timeout()){
			isRecvFromFlag = RESET;
			subscribeTopicIndex ++;
			if(subscribeTopicIndex < NUMBER_OF_SUBSCRIBE_TOPIC){
				mqttState = MQTT_SUBSCRIBE_STATE;
			} else {
				subscribeTopicIndex = 0;
				mqttState = MQTT_WAIT_FOR_NEW_COMMAND;
			}
		}
	} else if(isErrorFlag){
		isErrorFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(isIPCloseFlag){
		isIPCloseFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}


void SM_Wait_For_New_Command(void){
	if(isIPCloseFlag){
		isIPCloseFlag = RESET;
		mqttState = MQTT_OPEN_STATE;
	}
	else if(isErrorFlag){
		isErrorFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}


void SM_Mqtt_Publish(void){
	isErrorFlag = RESET;
	isIPCloseFlag = RESET;
	SCH_Delete_Task(mqtt_Timeout_Task_Index);
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	mqttState = MQTT_RECEIVE_GREATER_THAN_SYMBOL_PUBLISH_STATE;
}

void SM_Mqtt_Receive_Greater_Than_Symbol_Publish_State(void){
	if(isReadyToSendDataToServer){
		isReadyToSendDataToServer = RESET;
		isSendOKFlag = RESET;
		isErrorFlag = RESET;
		isIPCloseFlag = RESET;

		isCipsend = RESET;
		MQTTCommandSending((uint8_t *)mqttMessage, mqttMessageLength);
		mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_PUBLISH_STATE;
	} else if(isErrorFlag){
		isErrorFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(isIPCloseFlag){
		isIPCloseFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}
void SM_Mqtt_Wait_For_Response_From_Publish_State(void){
	if(isSendOKFlag || isCipsend){
		isSendOKFlag = RESET;
		isCipsend = RESET;
		mqttState = MQTT_WAIT_FOR_NEW_COMMAND;
	} else if(isErrorFlag){
		isErrorFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(isIPCloseFlag){
		isIPCloseFlag = RESET;
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
	if(isOKFlag){
		isOKFlag = RESET;
		mqttState = MQTT_OPEN_STATE;
	} else if(isErrorFlag){
		isErrorFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	}

	if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}

void SM_Mqtt_Ping_Request(void) {
	SCH_Delete_Task(mqtt_Timeout_Task_Index);
	Mqtt_Clear_Timeout_Flag();
	mqtt_Timeout_Task_Index = SCH_Add_Task(Mqtt_Command_Timeout, MQTT_COMMAND_TIME_OUT,0);
	Setup_Mqtt_Ping_Request_Message();
	mqttState = MQTT_RECEIVE_GREATER_THAN_SYMBOL_PING_REQUEST_STATE;
}

void SM_Mqtt_Receive_Greater_Than_Symbol_Ping_Request_State(void){
	if(isReadyToSendDataToServer){
		isReadyToSendDataToServer = RESET;
		isSendOKFlag = RESET;
		isErrorFlag = RESET;
		MQTTCommandSending((uint8_t *)mqttMessage, mqttMessageLength);
		mqttState = MQTT_WAIT_FOR_RESPONSE_FROM_REQUEST_STATE;
	} else if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}
void SM_Wait_For_Mqtt_Ping_Resquest_Response(void){
	if(isSendOKFlag){
		isSendOKFlag = RESET;
		mqttState = MQTT_WAIT_FOR_NEW_COMMAND;
	} else if(isErrorFlag){
		isErrorFlag = RESET;
		mqttState = MAX_MQTT_NUMBER_STATES;
	} else if(is_Mqtt_Command_Timeout()){
		mqttState = MAX_MQTT_NUMBER_STATES;
	}
}
///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Connect_Message(void){

#if(CLOUD_MQTT == 1)
	uint8_t i;
	const uint8_t clientIdLength = GetStringLength((uint8_t*)CLIENT_ID);
	const uint8_t usernameLength = GetStringLength((uint8_t*)USERNAME);
	const uint8_t passwordLength = GetStringLength((uint8_t*)PASSWORD);
	const uint8_t willPayloadLength = GetStringLength((uint8_t*)LAST_WILL_MESSAGE);
	const uint8_t willTopicLength = GetStringLength((uint8_t*)PUBLISH_TOPIC_STATUS);

	Clear_Mqtt_Message_Buffer();

	// Header
	mqttMessage[mqttMessageIndex++] = MQTT_MSG_CONNECT;

	mqttMessage[mqttMessageIndex++] = 22 + clientIdLength + willPayloadLength + willTopicLength + usernameLength + passwordLength;    // Remaining length of the message (bytes 2-13 + clientId)
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
	mqttMessage[mqttMessageIndex++] = 0b11101110;  //enable username, password, will retain, will-qos (2), will flag, clean session

	// Keep-alive (maximum duration)
//	mqttMessage[mqttMessageIndex++] = 0x00;                     // Keep-alive Time Length MSB
//	mqttMessage[mqttMessageIndex++] = 0x0a;                     // Keep-alive Time Length LSB

	mqttMessage[mqttMessageIndex++] = 0x00;                     // Keep-alive Time Length MSB
		mqttMessage[mqttMessageIndex++] = 0x0;                     // Keep-alive Time Length LSB

	// Client ID
	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
	mqttMessage[mqttMessageIndex++] = clientIdLength;        // Client ID length LSB
	for (i = 0; i < clientIdLength; i ++){
		mqttMessage[mqttMessageIndex++] = CLIENT_ID[i];
	}


	//will topic
	mqttMessage[mqttMessageIndex++] = 0;                     // Will topic length MSB
	mqttMessage[mqttMessageIndex++] = willTopicLength;        			// Will topic length LSB
	for (i = 0; i < willTopicLength; i ++){
		mqttMessage[mqttMessageIndex++] = PUBLISH_TOPIC_STATUS[i];
	}

	//end will topic

	//will message
	mqttMessage[mqttMessageIndex++] = 0;                     // Will message length MSB
	mqttMessage[mqttMessageIndex++] = willPayloadLength;        			// Will message length LSB
	for (i = 0; i < willPayloadLength; i ++){
		mqttMessage[mqttMessageIndex++] = LAST_WILL_MESSAGE[i];
	}

	//end will message


	// Username
	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
	mqttMessage[mqttMessageIndex++] = usernameLength;        // Client ID length LSB
	for (i = 0; i < usernameLength; i ++){
		mqttMessage[mqttMessageIndex++] = USERNAME[i];
	}

	// Password
	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
	mqttMessage[mqttMessageIndex++] = passwordLength;        // Client ID length LSB
	for (i = 0; i < passwordLength; i ++){
		mqttMessage[mqttMessageIndex++] = PASSWORD[i];
	}
	mqttMessageLength = mqttMessageIndex;
	SM_Send_Data(mqttMessageLength);


#elif(MOSQUITTO == 1)
	uint8_t i;
	uint8_t clientIdLength = GetStringLength((uint8_t*)CLIENT_ID);
	uint8_t usernameLength = 0;//GetStringLength((uint8_t*)USERNAME);
	uint8_t passwordLength = 0;//GetStringLength((uint8_t*)PASSWORD);

	Clear_Mqtt_Message_Buffer();

	// Header
	mqttMessage[mqttMessageIndex++] = MQTT_MSG_CONNECT;

	mqttMessage[mqttMessageIndex++] = 14 + clientIdLength;    // Remaining length of the message (bytes 2-13 + clientId)
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
		mqttMessage[mqttMessageIndex++] = 3;

	// Protocol level
	// MQTT Protocol version = 3 (mqtt 3.1)
	// MQTT Protocol version = 4 (mqtt 3.1.1)
	// MQTT Protocol version = 5 (mqtt 5.0)
//	mqttMessage[mqttMessageIndex++] = 5;

	// Connection flags   2 for clean session
 //   mqttMessage[9] = 2;
	mqttMessage[mqttMessageIndex++] = 0b00100110;  //enable username, password, clean session

	// Keep-alive (maximum duration)
	mqttMessage[mqttMessageIndex++] = 0x00;                     // Keep-alive Time Length MSB
	mqttMessage[mqttMessageIndex++] = 0x3c;                     // Keep-alive Time Length LSB

//	mqttMessage[mqttMessageIndex++] = 0x00;                     // Keep-alive Time Length MSB
//	mqttMessage[mqttMessageIndex++] = 0x0;                     // Keep-alive Time Length LSB

	// Client ID
	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
	mqttMessage[mqttMessageIndex++] = clientIdLength;        // Client ID length LSB
	for (i = 0; i < clientIdLength; i ++){
		mqttMessage[mqttMessageIndex++] = CLIENT_ID[i];
	}

	if(usernameLength > 0){
		// Username
		mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
		mqttMessage[mqttMessageIndex++] = usernameLength;        // Client ID length LSB
		for (i = 0; i < usernameLength; i ++){
			mqttMessage[mqttMessageIndex++] = USERNAME[i];
		}
	}

	if(passwordLength > 0){
		// Password
		mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
		mqttMessage[mqttMessageIndex++] = passwordLength;        // Client ID length LSB
		for (i = 0; i < passwordLength; i ++){
			mqttMessage[mqttMessageIndex++] = PASSWORD[i];
		}
	}
	mqttMessageLength = mqttMessageIndex;
	SM_Send_Data(mqttMessageLength);
#endif
}
/////////////////////////////////////////////////////////////////////////////
//void Setup_Mqtt_Connect_Message_311(void){
//	uint8_t i;
//	uint8_t clientIdLength = GetStringLength((uint8_t*)CLIENT_ID);
//	uint8_t usernameLength = GetStringLength((uint8_t*)USERNAME);
//	uint8_t passwordLength = GetStringLength((uint8_t*)PASSWORD);
//
//	Clear_Mqtt_Message_Buffer();
//
//	// Header
//	mqttMessage[mqttMessageIndex++] = MQTT_MSG_CONNECT;
//	mqttMessage[mqttMessageIndex++] = 16 + clientIdLength + usernameLength + passwordLength;    // Remaining length of the message (bytes 2-13 + clientId)
//
//	// Protocol name
//	mqttMessage[mqttMessageIndex++] = 0;                      // Protocol Name Length MSB
//	mqttMessage[mqttMessageIndex++] = 6;                      // Protocol Name Length LSB
//	mqttMessage[mqttMessageIndex++] = 'M';
//	mqttMessage[mqttMessageIndex++] = 'Q';
//	mqttMessage[mqttMessageIndex++] = 'T';
//	mqttMessage[mqttMessageIndex++] = 'T';
//
//	// Protocol level
//	mqttMessage[mqttMessageIndex++] = 3;                      // MQTT Protocol version = 4 (mqtt 3.1.1)
//
//	// Connection flags   2 for clean session
// //   mqttMessage[9] = 2;
//	mqttMessage[mqttMessageIndex++] = 0b11000010;  //enable username, password, clean session
//
//	// Keep-alive (maximum duration)
//	mqttMessage[mqttMessageIndex++] = 0x00;                     // Keep-alive Time Length MSB
//	mqttMessage[mqttMessageIndex++] = 0x3c;                     // Keep-alive Time Length LSB
//
//	// Client ID
//	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
//	mqttMessage[mqttMessageIndex++] = clientIdLength;        // Client ID length LSB
//	for (i = 0; i < clientIdLength; i ++){
//		mqttMessage[mqttMessageIndex++] = CLIENT_ID[i];
//	}
//	// Username
//	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
//	mqttMessage[mqttMessageIndex++] = usernameLength;        // Client ID length LSB
//	for (i = 0; i < usernameLength; i ++){
//		mqttMessage[mqttMessageIndex++] = USERNAME[i];
//	}
//	// Password
//	mqttMessage[mqttMessageIndex++] = 0;                     // Client ID length MSB
//	mqttMessage[mqttMessageIndex++] = passwordLength;        // Client ID length LSB
//	for (i = 0; i < passwordLength; i ++){
//		mqttMessage[mqttMessageIndex++] = PASSWORD[i];
//	}
//	mqttMessageLength = mqttMessageIndex;
//	SM_Send_Data(mqttMessageLength);
//}
//////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Subscribe_Message(uint8_t * topic){
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

	// Write QoS
	mqttMessage[mqttMessageIndex++] = 0;
	mqttMessageLength = mqttMessageIndex;
	SM_Send_Data(mqttMessageLength);
}

///////////////////////////////////////////////////////////////////////////
void Setup_Mqtt_Publish_Message(uint8_t * topic, uint8_t * message, uint8_t lenOfMessage){
	uint8_t i;
	uint8_t lenOfTopic = GetStringLength((uint8_t*)topic);

//	mqttMessageLength = 4 + lenTopic + lenMsg;
	Clear_Mqtt_Message_Buffer();
	// Write fixed header
	mqttMessage[mqttMessageIndex++] = MQTT_MSG_PUBLISH | RETAIN_MESSAGE;
	mqttMessage[mqttMessageIndex++] = 2 + lenOfTopic + lenOfMessage;

	// Write topic
	mqttMessage[mqttMessageIndex++] = 0;         // lenTopic MSB
	mqttMessage[mqttMessageIndex++] = lenOfTopic;  // lenTopic LSB
	for (i = 0; i < lenOfTopic; i++){
		mqttMessage[mqttMessageIndex++] = topic[i];
	}

	// Write msg
	for (i = 0; i < lenOfMessage; i++){
		mqttMessage[mqttMessageIndex++] = message[i];
	}
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


