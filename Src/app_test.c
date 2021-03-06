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
#include "app_pcf8574.h"


extern const uint8_t SUBSCRIBE_TOPIC_1[];
extern const uint8_t SUBSCRIBE_TOPIC_2[];

extern const uint8_t PUBLISH_TOPIC_STATUS[];
extern const uint8_t PUBLISH_TOPIC_POWER[];

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

	Test_Led_Display();


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
}

void test7(void){
	TestSendATcommand();
}

