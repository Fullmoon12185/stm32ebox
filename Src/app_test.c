/*
 * app_test.c
 *
 *  Created on: Feb 12, 2020
 *      Author: VAIO
 */
#include "main.h"

#include "app_relay.h"
#include "app_led_display.h"
#include "app_sim3g.h"
#include "app_uart.h"
#include "app_sim5320MQTT.h"


extern const uint8_t SUBSCRIBE_TOPIC_1[];
extern const uint8_t SUBSCRIBE_TOPIC_2[];

extern const uint8_t PUBLISH_TOPIC_1[];
extern const uint8_t PUBLISH_TOPIC_2[];

void test1(void){


	static uint8_t bool2 = 0;
	static uint8_t index2 = 0;
	if(bool2 == 0){
		Set_Relay(index2);
	} else {
		Reset_Relay(index2);
	}
	if(index2 == 9) {
		bool2 = (bool2 + 1)%2;
		index2 = 0;
	} else {
		index2 ++;
	}


}
void test3(void){

	static uint8_t bool1 = 0;
	static uint8_t index1 = 0;

	if(bool1 == 0){
		Led_Display_Update_Buffer(index1*2, 1);
		Led_Display_Update_Buffer(index1*2 + 1, 1);
	} else {
		Led_Display_Update_Buffer(index1*2, 0);
		Led_Display_Update_Buffer(index1*2 + 1, 0);
	}
	if(index1 == 9) {
		bool1 = (bool1 + 1)%2;
		index1 = 0;
	} else {
		index1 ++;
	}


}
void test2(void){
	HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
}

void test4(void){
	static uint8_t poweron = 0;
	if(poweron == 0){
		poweron = 1;
		Sim3g_Disable();
	} else {
		poweron = 0;
		Sim3g_Enable();
	}
}

void test5(void){
//	testSendcommand();
	Sim3g_Run();
}

void test6(void){
	Setup_Mqtt_Connect_Message();//////////////////////////////////////////////////////////////////////////
	Setup_Mqtt_Subscribe_Message((uint8_t*)SUBSCRIBE_TOPIC_1);
	Setup_Mqtt_Publish_Message((uint8_t *)PUBLISH_TOPIC_1, (uint8_t *)"123456789", 9);
}
