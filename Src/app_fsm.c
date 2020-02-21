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

#define		BOX_ID	1
#define		TIME_FOR_PING_REQUEST	6000 //5s

extern const uint8_t PUBLISH_TOPIC_1[];
extern const uint8_t PUBLISH_TOPIC_2[];

extern uint8_t publish_message[20];
extern uint8_t publishTopicIndex;
extern uint8_t publish_message_length;

extern int32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_ADC_CHANNELS];


uint8_t main_Fsm_TimeoutFlag = 0;
uint8_t main_Fsm_TimeoutIndex = SCH_MAX_TASKS;

typedef enum {
	SIM3G_OPEN_CONNECTION = 0,
	SIM3G_WAIT_FOR_A_MOMENT,
	SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS,
	MAX_MAIN_FSM_NUMBER_STATES
} MAIN_FSM_STATE;

MAIN_FSM_STATE mainFsmState = SIM3G_OPEN_CONNECTION;




uint8_t is_Main_FSM_Timeout(void);
void Main_FSM_Timeout(void);
void Main_FSM_Clear_Timeout_Flag(void);



void Main_FSM_Clear_Timeout_Flag(void){
	main_Fsm_TimeoutFlag = 0;
}
void Main_FSM_Timeout(void){
	main_Fsm_TimeoutFlag = 1;
}
uint8_t is_Main_FSM_Timeout(void){
	return main_Fsm_TimeoutFlag;
}

void UpdatePublishMessage(uint8_t outletID, int32_t displayData){

	uint8_t publishMessageIndex = 0;
	int32_t tempValue;
	tempValue = displayData;
	if(outletID > 9) return;
	for(publishMessageIndex = 0; publishMessageIndex < 20; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
	publish_message[publishMessageIndex++] = BOX_ID + 0x30;
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



void main_fsm(void){

	switch(mainFsmState){
	case SIM3G_OPEN_CONNECTION:
		if(Sim3g_Run()){
			Set_Mqtt_State(MQTT_OPEN_STATE);
			SCH_Delete_Task(main_Fsm_TimeoutIndex);
			Main_FSM_Clear_Timeout_Flag();
			main_Fsm_TimeoutIndex = SCH_Add_Task(Main_FSM_Timeout, 200, 0);
			mainFsmState = SIM3G_WAIT_FOR_A_MOMENT;
		}
		break;
	case SIM3G_WAIT_FOR_A_MOMENT:
		if(is_Main_FSM_Timeout()){
			Main_FSM_Clear_Timeout_Flag();
			main_Fsm_TimeoutIndex = SCH_Add_Task(Main_FSM_Timeout, TIME_FOR_PING_REQUEST, 0);
			mainFsmState = SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS;
		}
		break;
	case SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS:
		if(MQTT_Run()){
			Set_Sim3G_State(POWER_OFF_SIM3G);
			mainFsmState = SIM3G_OPEN_CONNECTION;
		} else {
			if(Get_Mqtt_State() == MQTT_WAIT_FOR_NEW_COMMAND){
//				HAL_Delay(5000);
//				if(publishTopicIndex == 0){
//					publishTopicIndex = 1;
//					UpdatePublishMessage(0, 1234);
//					Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_1, publish_message, publish_message_length);
//				} else {
//					publishTopicIndex = 0;
//					UpdatePublishMessage(1, 4567);
//					Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_2, publish_message, publish_message_length);
//				}
//
//				Set_Mqtt_State(MQTT_PUBLISH_STATE);
//				Main_FSM_Clear_Timeout_Flag();
//				SCH_Add_Task(Main_FSM_Timeout, TIME_FOR_PUBLISH_MESSAGE, 0);
				if(is_Main_FSM_Timeout()){
					Set_Mqtt_State(MQTT_PING_REQUEST_STATE);
					Main_FSM_Clear_Timeout_Flag();
					main_Fsm_TimeoutIndex = SCH_Add_Task(Main_FSM_Timeout, TIME_FOR_PING_REQUEST, 0);
				}
			}


		}

		break;
	default:
		break;
	}
}
