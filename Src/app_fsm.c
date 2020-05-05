/*
 * app_fsm.c
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_sim3g.h"
#include "app_sim5320MQTT.h"
#include "app_scheduler.h"
#include "app_uart.h"
#include "app_test.h"
#include "app_relay.h"
#include "app_gpio.h"
#include "app_power.h"

#define		TIME_FOR_PING_REQUEST		6000 //5s
#define		TIME_FOR_PUBLISH_MESSAGE	300 //5s


#define 	TIME_FOR_BUZZER				1


extern uint8_t PUBLISH_TOPIC_STATUS[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_POWER[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_VOLTAGE[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_CURRENT[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_POWERFACTOR[MAX_TOPIC_LENGTH];


extern uint8_t publish_message[MQTT_MESSAGE_BUFFER_LENGTH];
extern uint8_t publishTopicIndex;
extern uint8_t publish_message_length;

extern int32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_ADC_CHANNELS];

extern FlagStatus array_Of_Outlet_Status[NUMBER_OF_ADC_CHANNELS];

uint8_t ping_Request_TimeoutFlag = 0;
uint32_t ping_Request_TimeoutIndex = NO_TASK_ID;

uint8_t publish_message_TimeoutFlag = 0;
uint32_t publish_message_TimeoutIndex = NO_TASK_ID;


typedef enum {
	SIM3G_OPEN_CONNECTION = 0,
	SIM3G_WAIT_FOR_A_MOMENT,
	SIM3G_SETUP_SUBSCRIBE_TOPICS,
	SIM3G_SETUP_PUBLISH_TOPICS,
	MAX_SERVER_COMMUNICATION_FSM_NUMBER_STATES
} SERVER_COMMUNICATION_FSM_STATE;

SERVER_COMMUNICATION_FSM_STATE serverCommunicationFsmState = SIM3G_OPEN_CONNECTION;




uint8_t is_Ping_Request_Timeout(void);
void Set_Ping_Request_Timeout_Flag(void);
void Clear_Ping_Request_Timeout_Flag(void);

void Set_Publish_Message_Timeout_Flag(void);
void Clear_Publish_Message_Timeout_Flag(void);
uint8_t is_Publish_Message_Timeout(void);

void Update_Publish_Status_Message(void);
void Update_Publish_Power_Message(uint8_t outletID, int32_t displayData);

void Update_Publish_Power_Message_All_Outlets(void);
void Update_Publish_Power_Factor_Message_All_Outlets(void);
void Update_Publish_Voltage_Message_All_Outlets(void);
void Update_Publish_Ampere_Message_All_Outlets(void);

void Clear_Ping_Request_Timeout_Flag(void){
	ping_Request_TimeoutFlag = 0;
}
void Set_Ping_Request_Timeout_Flag(void){
	ping_Request_TimeoutFlag = 1;
}
uint8_t is_Ping_Request_Timeout(void){
	return ping_Request_TimeoutFlag;
}

void Clear_Publish_Message_Timeout_Flag(void){
	publish_message_TimeoutFlag = 0;
}
void Set_Publish_Message_Timeout_Flag(void){
	publish_message_TimeoutFlag = 1;
}
uint8_t is_Publish_Message_Timeout(void){
	return publish_message_TimeoutFlag;
}


void Update_Publish_Power_Message(uint8_t outletID, int32_t displayData){

	uint8_t publishMessageIndex = 0;
	int32_t tempValue;
	tempValue = displayData;
	if(outletID > 9) return;
	for(publishMessageIndex = 0; publishMessageIndex < 20; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
//	publish_message[publishMessageIndex++] = BOX_ID + 0x30;
	publish_message[publishMessageIndex++] = outletID + 0x30;
	if(displayData >= 10000){
		publish_message[publishMessageIndex++] = tempValue/10000 + 0x30;
		tempValue = tempValue % 10000;
		publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
		tempValue = tempValue % 1000;
		publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
		tempValue = tempValue % 100;
		publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
		tempValue = tempValue % 10;
		publish_message[publishMessageIndex++] = tempValue + 0x30;

	} else if (displayData >= 1000){
		publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
		tempValue = tempValue % 1000;
		publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
		tempValue = tempValue % 100;
		publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
		tempValue = tempValue % 10;
		publish_message[publishMessageIndex++] = tempValue + 0x30;

	} else if (displayData >= 100){
		publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
		tempValue = tempValue % 100;
		publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
		tempValue = tempValue % 10;
		publish_message[publishMessageIndex++] = tempValue + 0x30;

	} else if(displayData >= 10){
		publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
		tempValue = tempValue % 10;
		publish_message[publishMessageIndex++] = tempValue + 0x30;
	} else {
		publish_message[publishMessageIndex++] = tempValue + 0x30;
	}
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Power_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint32_t tempValue;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){

		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Power_Consumption(outletID);
		} else {
			publish_message[publishMessageIndex++] = outletID/10 + 0x30;
			publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Main_Power_Consumption();
		}

		if(tempValue >= 1000000){
			publish_message[publishMessageIndex++] = tempValue/1000000 + 0x30;
			tempValue = tempValue % 1000000;
			publish_message[publishMessageIndex++] = tempValue/100000 + 0x30;
			tempValue = tempValue % 100000;
			publish_message[publishMessageIndex++] = tempValue/10000 + 0x30;
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if(tempValue >= 100000){
			publish_message[publishMessageIndex++] = tempValue/100000 + 0x30;
			tempValue = tempValue % 100000;
			publish_message[publishMessageIndex++] = tempValue/10000 + 0x30;
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if(tempValue >= 10000){
			publish_message[publishMessageIndex++] = tempValue/10000 + 0x30;
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if (tempValue >= 1000){
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;


		} else if (tempValue >= 100){
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if(tempValue >= 10){
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;
		} else {
			publish_message[publishMessageIndex++] = tempValue + 0x30;
		}
		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
	publish_message_length = publishMessageIndex;
}


void Update_Publish_Power_Factor_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint8_t tempValue;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){
		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Power_Factor(outletID);
		} else {
			publish_message[publishMessageIndex++] = outletID/10 + 0x30;
			publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Main_Power_Factor();
		}

		if (tempValue >= 100){
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if(tempValue >= 10){
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;
		} else {
			publish_message[publishMessageIndex++] = tempValue + 0x30;
		}
		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Voltage_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint8_t tempValue;
	for(publishMessageIndex = 0; publishMessageIndex < 20; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
//	publish_message[publishMessageIndex++] = BOX_ID + 0x30;
	for(uint8_t outletID = 0; outletID < 1; outletID++){
		publish_message[publishMessageIndex++] = outletID + 0x30;
		publish_message[publishMessageIndex++] = '-';
		tempValue = Get_Voltage(outletID);
		if (tempValue >= 100){
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if(tempValue >= 10){
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;
		} else {
			publish_message[publishMessageIndex++] = tempValue + 0x30;
		}
	}
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Current_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint32_t tempValue;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
//	publish_message[publishMessageIndex++] = BOX_ID + 0x30;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){

		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Current(outletID);
		} else {
			publish_message[publishMessageIndex++] = outletID/10 + 0x30;
			publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Main_Current();
		}
		if(tempValue >= 1000000){
			publish_message[publishMessageIndex++] = tempValue/1000000 + 0x30;
			tempValue = tempValue % 1000000;
			publish_message[publishMessageIndex++] = tempValue/100000 + 0x30;
			tempValue = tempValue % 100000;
			publish_message[publishMessageIndex++] = tempValue/10000 + 0x30;
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if(tempValue >= 100000){
			publish_message[publishMessageIndex++] = tempValue/100000 + 0x30;
			tempValue = tempValue % 100000;
			publish_message[publishMessageIndex++] = tempValue/10000 + 0x30;
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if(tempValue >= 10000){
			publish_message[publishMessageIndex++] = tempValue/10000 + 0x30;
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if (tempValue >= 1000){
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;


		} else if (tempValue >= 100){
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

		} else if(tempValue >= 10){
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;
		} else {
			publish_message[publishMessageIndex++] = tempValue + 0x30;
		}
		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
	publish_message_length = publishMessageIndex;
}


void Update_Publish_Status_Message(void){
	uint8_t publishMessageIndex = 0;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID ++){
		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
			if(Get_Relay_Status(outletID)){
				if((uint8_t)Get_Node_Status(outletID) <= 9){
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID) + 0x30;
				} else {
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID)/10 + 0x30;
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID)%10 + 0x30;
				}
			} else {
				publish_message[publishMessageIndex++] = 0x30;
			}
		} else {
			publish_message[publishMessageIndex++] = outletID/10 + 0x30;
			publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			publish_message[publishMessageIndex++] = '-';
			publish_message[publishMessageIndex++] = (uint8_t)Get_Main_Status() + 0x30;
		}
		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
	publish_message_length = publishMessageIndex;
}

void Led_Status_Display(void){
	static uint8_t ledState = 0;
	static uint8_t ledDislayTaskIndex = SCH_MAX_TASKS;
	switch(ledState){
	case 0:
		SCH_Delete_Task(ledDislayTaskIndex);
		ledDislayTaskIndex = SCH_Add_Task(test2, 0, 100);
		ledState = 1;
		break;
	case 1:
		if(Get_Mqtt_State() == MQTT_WAIT_FOR_NEW_COMMAND){
			SCH_Delete_Task(ledDislayTaskIndex);
			ledDislayTaskIndex = SCH_Add_Task(test2, 0, 10);
			ledState = 2;
		}
		break;
	case 2:
		if(serverCommunicationFsmState == SIM3G_OPEN_CONNECTION){
			ledState = 0;
		}
		break;
	}
}



void Server_Communication(void){
//	static uint8_t publishChannelIndex = 0;
//	static uint16_t whTest = 0;
//	Led_Status_Display();
	switch(serverCommunicationFsmState){
	case SIM3G_OPEN_CONNECTION:
		if(Sim3g_Run()){
			Set_Mqtt_State(MQTT_OPEN_STATE);
			SCH_Delete_Task(ping_Request_TimeoutIndex);
			Clear_Ping_Request_Timeout_Flag();
			ping_Request_TimeoutIndex = SCH_Add_Task(Set_Ping_Request_Timeout_Flag, 200, 0);
			serverCommunicationFsmState = SIM3G_WAIT_FOR_A_MOMENT;
		}
		break;
	case SIM3G_WAIT_FOR_A_MOMENT:
		if(is_Ping_Request_Timeout()){
			Clear_Ping_Request_Timeout_Flag();
			ping_Request_TimeoutIndex = SCH_Add_Task(Set_Ping_Request_Timeout_Flag, TIME_FOR_PING_REQUEST, 0);

			Clear_Publish_Message_Timeout_Flag();
			publish_message_TimeoutIndex = SCH_Add_Task(Set_Publish_Message_Timeout_Flag,TIME_FOR_PUBLISH_MESSAGE, 0);

			serverCommunicationFsmState = SIM3G_SETUP_SUBSCRIBE_TOPICS;
		}
		break;
	case SIM3G_SETUP_SUBSCRIBE_TOPICS:
		if(Uart1_Received_Buffer_Available() == 0){
			if(MQTT_Run()){
				Set_Sim3G_State(RESET_SIM3G);
				serverCommunicationFsmState = SIM3G_OPEN_CONNECTION;
			} else {
				if(Get_Mqtt_State() == MQTT_WAIT_FOR_NEW_COMMAND){
					serverCommunicationFsmState = SIM3G_SETUP_PUBLISH_TOPICS;
				}
			}
		}
		break;
	case SIM3G_SETUP_PUBLISH_TOPICS:
		if(Get_Mqtt_State() != MQTT_WAIT_FOR_NEW_COMMAND){
			serverCommunicationFsmState = SIM3G_SETUP_SUBSCRIBE_TOPICS;
		} else {
			if(is_Set_Relay_Timeout()){
//				if(Get_Is_Receive_Data_From_Server() == SET){
//					Set_Is_Receive_Data_From_Server(RESET);
//					SCH_Delete_Task(ping_Request_TimeoutIndex);
//					Clear_Ping_Request_Timeout_Flag();
//					ping_Request_TimeoutIndex = SCH_Add_Task(Set_Ping_Request_Timeout_Flag, TIME_FOR_PING_REQUEST, 0);
//				}
//
//				if(is_Ping_Request_Timeout()){
//					Set_Mqtt_State(MQTT_PING_REQUEST_STATE);
//					SCH_Delete_Task(ping_Request_TimeoutIndex);
//					Clear_Ping_Request_Timeout_Flag();
//					ping_Request_TimeoutIndex = SCH_Add_Task(Set_Ping_Request_Timeout_Flag, TIME_FOR_PING_REQUEST, 0);
//				}
//				else
				if(Get_Is_Update_Relay_Status() == SET || Get_Is_Node_Status_Changed() == SET){
//					Update_Publish_Status_Message();
//					Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_STATUS, publish_message, publish_message_length);
//					Set_Mqtt_State(MQTT_PUBLISH_STATE);
					publishTopicIndex = 0;
//					SCH_Delete_Task(publish_message_TimeoutIndex);
//					Clear_Publish_Message_Timeout_Flag();
//					publish_message_TimeoutIndex = SCH_Add_Task(Set_Publish_Message_Timeout_Flag, TIME_FOR_PUBLISH_MESSAGE, 0);
//					Turn_On_Buzzer();
//					SCH_Add_Task(Turn_Off_Buzzer, TIME_FOR_BUZZER, 0);

				} else if (is_Publish_Message_Timeout()){
					if(publishTopicIndex == 0){
						publishTopicIndex = 1;
						Update_Publish_Status_Message();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_STATUS, publish_message, publish_message_length);
					} else if(publishTopicIndex == 1){
						publishTopicIndex = 2;
						Update_Publish_Power_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_POWER, publish_message, publish_message_length);
					} else if(publishTopicIndex == 2){
						publishTopicIndex = 3;
						Update_Publish_Power_Factor_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_POWERFACTOR, publish_message, publish_message_length);
					} else if(publishTopicIndex == 3){
						publishTopicIndex = 4;
						Update_Publish_Voltage_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_VOLTAGE, publish_message, publish_message_length);
					} else if(publishTopicIndex == 4){
						publishTopicIndex = 0;
						Update_Publish_Current_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_CURRENT, publish_message, publish_message_length);
					}
					Set_Mqtt_State(MQTT_PUBLISH_STATE);
					Clear_Publish_Message_Timeout_Flag();
					publish_message_TimeoutIndex = SCH_Add_Task(Set_Publish_Message_Timeout_Flag, TIME_FOR_PUBLISH_MESSAGE, 0);

					Turn_On_Buzzer();
					SCH_Add_Task(Turn_Off_Buzzer, TIME_FOR_BUZZER, 0);
				}
			}
		}
		break;
	default:
		break;
	}
}




