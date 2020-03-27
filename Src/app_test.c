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
#include "app_scheduler.h"
#include "app_gpio.h"


extern const uint8_t SUBSCRIBE_TOPIC_1[];
extern const uint8_t SUBSCRIBE_TOPIC_2[];

extern const uint8_t PUBLISH_TOPIC_STATUS[];
extern const uint8_t PUBLISH_TOPIC_POWER[];
uint8_t strTest[] = "                                   ";
void test1(void){


	static uint8_t bool2 = 0;
	static uint8_t index2 = 0;

	sprintf((char*) strTest, "test1 %d\r\n", (int) index2);
	UART3_SendToHost((uint8_t *)strTest);
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
	static int test3Counter = 0;
	sprintf((char*) strTest, "seconds = %d\r\n", (int) test3Counter++);
	UART3_SendToHost((uint8_t *)strTest);
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

	UART3_SendToHost((uint8_t *)"test5\r\n");
}
void test8(void){
	UART3_SendToHost((uint8_t *)"test8\r\n");
}
void test6(void){

	SCH_Add_Task(test1, 0, 1000);
	HAL_Delay(10);
	SCH_Add_Task(test3, 2, 100);
	HAL_Delay(10);
	SCH_Add_Task(test2, 3, 20);
	HAL_Delay(10);
	SCH_Add_Task(test5, 3, 200);

	HAL_Delay(10);
	SCH_Add_Task(test8, 7, 21);
}

void test7(void){
	TestSendATcommand();
}
