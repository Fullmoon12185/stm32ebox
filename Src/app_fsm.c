/*
 * app_fsm.c
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_sim3g.h"
#include "app_sim5320MQTT.h"

extern const uint8_t PUBLISH_TOPIC_1[];
extern const uint8_t PUBLISH_TOPIC_2[];

extern uint8_t publish_message[20];
extern uint8_t publishTopicIndex;


typedef enum {
	SIM3G_OPEN_CONNECTION = 0,
	SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS,
	MAX_MAIN_FSM_NUMBER_STATES
} MAIN_FSM_STATE;

MAIN_FSM_STATE mainFsmState = SIM3G_OPEN_CONNECTION;
void main_fsm(void){

	switch(mainFsmState){
	case SIM3G_OPEN_CONNECTION:
		if(Sim3g_Run()){
			mainFsmState = SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS;
			HAL_Delay(2000);
		}
		break;
	case SIM3G_SETUP_PUBLISH_SUBSCRIBE_TOPICS:
		if(MQTT_Run()){
			mainFsmState = SIM3G_OPEN_CONNECTION;
		} else {
			if(Get_Mqtt_State() == MQTT_WAIT_FOR_NEW_COMMAND){
				HAL_Delay(3000);
				if(publishTopicIndex == 0){
					publishTopicIndex = 1;
					Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_1, publish_message, 10);
				} else {
					publishTopicIndex = 0;
					Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_2, publish_message, 10);
				}

				Set_Mqtt_State(MQTT_PUBLISH_STATE);
			}
		}

		break;
	default:
		break;
	}
}
