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

#define		BOX_ID	1
#define		TIME_FOR_PING_REQUEST		6000 //5s
#define		TIME_FOR_PUBLISH_MESSAGE	1000 //5s

extern const uint8_t PUBLISH_TOPIC_STATUS[];
extern const uint8_t PUBLISH_TOPIC_POWER[];

extern uint8_t publish_message[MQTT_MESSAGE_BUFFER_LENGTH];
extern uint8_t publishTopicIndex;
extern uint8_t publish_message_length;

extern int32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_ADC_CHANNELS];

extern FlagStatus array_Of_Outlet_Status[NUMBER_OF_ADC_CHANNELS];

uint8_t ping_Request_TimeoutFlag = 0;
uint8_t ping_Request_TimeoutIndex = SCH_MAX_TASKS;

uint8_t publish_message_TimeoutFlag = 0;
uint8_t publish_message_TimeoutIndex = SCH_MAX_TASKS;


typedef enum {
	SIM3G_OPEN_CONNECTION = 0,
	SIM3G_WAIT_FOR_A_MOMENT,
	SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS,
	MAX_MAIN_FSM_NUMBER_STATES
} MAIN_FSM_STATE;

MAIN_FSM_STATE mainFsmState = SIM3G_OPEN_CONNECTION;




uint8_t is_Ping_Request_Timeout(void);
void Ping_Request(void);
void Ping_Request_Clear_Timeout_Flag(void);

void Publish_Message_Timeout(void);
void Publish_Message_Clear_Timeout_Flag(void);
uint8_t is_Publish_Message_Timeout(void);

void Update_Publish_Status_Message(void);
void Update_Publish_Power_Message(uint8_t outletID, int32_t displayData);

void Ping_Request_Clear_Timeout_Flag(void){
	ping_Request_TimeoutFlag = 0;
}
void Ping_Request_Timeout(void){
	ping_Request_TimeoutFlag = 1;
}
uint8_t is_Ping_Request_Timeout(void){
	return ping_Request_TimeoutFlag;
}

void Publish_Message_Clear_Timeout_Flag(void){
	publish_message_TimeoutFlag = 0;
}
void Publish_Message_Timeout(void){
	publish_message_TimeoutFlag = 1;
}
uint8_t is_Publish_Message_Timeout(void){
	return publish_message_TimeoutFlag;
}

void Update_Power_Consumption(void){
	for(uint8_t outletID = 0; outletID < NUMBER_OF_RELAYS; outletID++){
		if(Get_Relay_Status(outletID)){
			array_Of_Power_Consumption_In_WattHour[outletID] = array_Of_Power_Consumption_In_WattHour[outletID] + (outletID + 1) * 10;
		} else {
			array_Of_Power_Consumption_In_WattHour[outletID] = 0;
		}
	}
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
	int32_t tempValue;
	for(publishMessageIndex = 0; publishMessageIndex < 20; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
//	publish_message[publishMessageIndex++] = BOX_ID + 0x30;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_RELAYS; outletID++){
		publish_message[publishMessageIndex++] = outletID + 0x30;
		publish_message[publishMessageIndex++] = '-';
//		tempValue = array_Of_Power_Consumption_In_WattHour[outletID];
//		if(outletID == 0) tempValue =
		tempValue = Main.nodes[outletID].energy/1000;///3600;
//		if(displayData >= 10000){
//			publish_message[publishMessageIndex++] = tempValue/10000 + 0x30;
//			tempValue = tempValue % 10000;
//			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
//			tempValue = tempValue % 1000;
//			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
//			tempValue = tempValue % 100;
//			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
//			tempValue = tempValue % 10;
//			publish_message[publishMessageIndex++] = tempValue + 0x30;
//
//		} else if (tempValue >= 1000){
			tempValue = tempValue % 10000;
			publish_message[publishMessageIndex++] = tempValue/1000 + 0x30;
			tempValue = tempValue % 1000;
			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
			tempValue = tempValue % 100;
			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
			tempValue = tempValue % 10;
			publish_message[publishMessageIndex++] = tempValue + 0x30;

			if(outletID < NUMBER_OF_RELAYS - 1)
				publish_message[publishMessageIndex++] = ',';
//		} else if (tempValue >= 100){
//			publish_message[publishMessageIndex++] = tempValue/100 + 0x30;
//			tempValue = tempValue % 100;
//			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
//			tempValue = tempValue % 10;
//			publish_message[publishMessageIndex++] = tempValue + 0x30;
//
//		} else if(tempValue >= 10){
//			publish_message[publishMessageIndex++] = tempValue/10 + 0x30;
//			tempValue = tempValue % 10;
//			publish_message[publishMessageIndex++] = tempValue + 0x30;
//		} else {
//			publish_message[publishMessageIndex++] = tempValue + 0x30;
//		}
	}
	publish_message_length = publishMessageIndex;
}


void Update_Publish_Status_Message(void){
	uint8_t publishMessageIndex = 0;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
//	publish_message[publishMessageIndex++] = BOX_ID + 0x30;

	for(uint8_t outletID = 0; outletID <= NUMBER_OF_RELAYS; outletID ++){
		if (outletID < NUMBER_OF_RELAYS) publish_message[publishMessageIndex++] = outletID + 0x30;
		else {
			publish_message[publishMessageIndex++] = 1 + 0x30;
			publish_message[publishMessageIndex++] = 0 + 0x30;
		}
		publish_message[publishMessageIndex++] = '-';
//		if(Get_Relay_Status(outletID)){
//			if(array_Of_Power_Consumption_In_WattHour[outletID] < 1000){
//				chargingStatus = 1;
//			} else {
//				chargingStatus = 2;
//			}
//			publish_message[publishMessageIndex++] = chargingStatus + 0x30;
//		} else {
//			publish_message[publishMessageIndex++] = 0x30;
//		}
		if(outletID == 0 ) publish_message[publishMessageIndex++] = 0x30 + Main.status;
		else publish_message[publishMessageIndex++] = 0x30 + Main.nodes[outletID-1].nodeStatus;
		if(outletID < NUMBER_OF_RELAYS )
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
		if(mainFsmState == SIM3G_OPEN_CONNECTION){
			ledState = 0;
		}
		break;
	}
}

void main_fsm(void){
//	static uint8_t publishChannelIndex = 0;
//	static uint16_t whTest = 0;
	Led_Status_Display();
	switch(mainFsmState){
	case SIM3G_OPEN_CONNECTION:
		if(Sim3g_Run()){
			Set_Mqtt_State(MQTT_OPEN_STATE);
			SCH_Delete_Task(ping_Request_TimeoutIndex);
			Ping_Request_Clear_Timeout_Flag();
			ping_Request_TimeoutIndex = SCH_Add_Task(Ping_Request_Timeout, 200, 0);
			mainFsmState = SIM3G_WAIT_FOR_A_MOMENT;
		}
		break;
	case SIM3G_WAIT_FOR_A_MOMENT:
		if(is_Ping_Request_Timeout()){
			Ping_Request_Clear_Timeout_Flag();
			ping_Request_TimeoutIndex = SCH_Add_Task(Ping_Request_Timeout, TIME_FOR_PING_REQUEST, 0);

			Publish_Message_Clear_Timeout_Flag();
			publish_message_TimeoutIndex = SCH_Add_Task(Publish_Message_Timeout,TIME_FOR_PUBLISH_MESSAGE, 0);

			mainFsmState = SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS;
		}
		break;
	case SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS:
//		HAL_Delay(1000);
		if(MQTT_Run()){
			Set_Sim3G_State(POWER_OFF_SIM3G);
			mainFsmState = SIM3G_OPEN_CONNECTION;
		} else {
			if(Get_Mqtt_State() == MQTT_WAIT_FOR_NEW_COMMAND){
				if(Get_Is_Receive_Data_From_Server() == SET){
					Set_Is_Receive_Data_From_Server(RESET);
					SCH_Delete_Task(ping_Request_TimeoutIndex);
					Ping_Request_Clear_Timeout_Flag();
					ping_Request_TimeoutIndex = SCH_Add_Task(Ping_Request_Timeout, TIME_FOR_PING_REQUEST, 0);
				} else if(is_Ping_Request_Timeout()){
					Set_Mqtt_State(MQTT_PING_REQUEST_STATE);
					Ping_Request_Clear_Timeout_Flag();
					ping_Request_TimeoutIndex = SCH_Add_Task(Ping_Request_Timeout, TIME_FOR_PING_REQUEST, 0);
				} else if(Get_Is_Update_Relay_Status() == SET){
					Update_Publish_Status_Message();
					Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_STATUS, publish_message, publish_message_length);
					Set_Mqtt_State(MQTT_PUBLISH_STATE);
					SCH_Delete_Task(publish_message_TimeoutIndex);
					Publish_Message_Clear_Timeout_Flag();
					publish_message_TimeoutIndex = SCH_Add_Task(Publish_Message_Timeout, TIME_FOR_PUBLISH_MESSAGE, 0);
				} else if (is_Publish_Message_Timeout()){
					if(publishTopicIndex == 0){
						publishTopicIndex = 1;
						Update_Power_Consumption();
						Update_Publish_Power_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_POWER, publish_message, publish_message_length);
						Set_Mqtt_State(MQTT_PUBLISH_STATE);
						Publish_Message_Clear_Timeout_Flag();
						publish_message_TimeoutIndex = SCH_Add_Task(Publish_Message_Timeout, TIME_FOR_PUBLISH_MESSAGE, 0);
					} else {
						publishTopicIndex = 0;
						Update_Publish_Status_Message();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_STATUS, publish_message, publish_message_length);
						Set_Mqtt_State(MQTT_PUBLISH_STATE);
						Publish_Message_Clear_Timeout_Flag();
						publish_message_TimeoutIndex = SCH_Add_Task(Publish_Message_Timeout, TIME_FOR_PUBLISH_MESSAGE, 0);
					}
				}

			}
		}
		break;
	default:
		break;
	}
}
