/*
 * app_led_display.c
 *
 *  Created on: Dec 31, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_led_display.h"

#define		NUMBER_OF_LEDS	20

uint32_t ledStatus;
LED_COLOR ledStatusBuffer[NUMBER_OF_RELAYS] = { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE };
uint8_t ledStateBuffer[NUMBER_OF_RELAYS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


static void Latch_Enable(int8_t count);
static void Latch_Disable(int8_t count);
static void Output_Enable(void);
//static void Output_Disable(void);
static void Clock_On(int8_t count);
static void Clock_Off(int8_t count);

void Led_Display_Set_All(void);
void Led_Display_Clear_All(void);


void Led_Display_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = LED_SDI;
	HAL_GPIO_Init(LED_SDI_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LED_SCK;
	HAL_GPIO_Init(LED_SCK_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LED_LE;
	HAL_GPIO_Init(LED_LE_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LED_OE;
	HAL_GPIO_Init(LED_OE_PORT, &GPIO_InitStruct);

	Output_Enable();
	Latch_Disable(1);
	Clock_Off(1);
	Led_Display_Clear_All();

}


static void Latch_Enable(int8_t count){
	if(count <= 0) return;
	while(count-- != 0){
		HAL_GPIO_WritePin(LED_LE_PORT, LED_LE, GPIO_PIN_SET);
	}
}
static void Latch_Disable(int8_t count){
	if(count <= 0) return;
	while(count-- != 0){
		HAL_GPIO_WritePin(LED_LE_PORT, LED_LE, GPIO_PIN_RESET);
	}
}
static void Output_Enable(void){
	HAL_GPIO_WritePin(LED_OE_PORT, LED_OE, GPIO_PIN_RESET);
}
//static void Output_Disable(void){
//	HAL_GPIO_WritePin(LED_OE_PORT, LED_OE, GPIO_PIN_SET);
//}
static void Clock_On(int8_t count){
	if(count <= 0) return;
	while(count-- != 0){
		HAL_GPIO_WritePin(LED_SCK_PORT, LED_SCK, GPIO_PIN_SET);
	}

}
static void Clock_Off(int8_t count){
	if(count <= 0) return;
	while(count-- != 0){
		HAL_GPIO_WritePin(LED_SCK_PORT, LED_SCK, GPIO_PIN_RESET);
	}

}
static void Data_Out(GPIO_PinState state){
	HAL_GPIO_WritePin(LED_SDI_PORT, LED_SDI, state);
}




void Led_Display(void) {
	uint8_t i;
	uint32_t ledValue = ledStatus << 4;
	Latch_Disable(1);
	for (i = 0; i < 24; i++) {
		Clock_Off(1);
		Data_Out(ledValue & 0x00000001);
		Clock_On(10);
		ledValue = ledValue >> 1;
	}

	Latch_Enable(1);
}

void Led_Display_Color(uint8_t position, LED_COLOR color) {
	uint32_t colorMask = color & 0x00000003;
	ledStatus &= ~(0x00000003 << (position * 2));
	ledStatus |= colorMask << (position * 2);
}


void Led_Display_Clear_All(void) {
	ledStatus = 0;
	Led_Display();
}

void Led_Display_Set_All(void) {
	ledStatus = 0xffffffff;
	Led_Display();
}

void LED_Display_FSM(void) { // call each 200ms
	LED_COLOR tmp;
	for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++) {
		tmp = ledStatusBuffer[i];
		if (tmp == NONE || tmp == RED || tmp == GREEN || tmp == YELLOW) {
			Led_Display_Color(i, tmp);
		} else if (tmp == BLINK_RED_FAST) {
			if (ledStatusBuffer[i] < 1) {
				Led_Display_Color(i, RED);
			} else if (ledStatusBuffer[i] < 2) {
				Led_Display_Color(i, NONE);
			}
			ledStatusBuffer[i]++;
			if (ledStatusBuffer[i] > 2) {
				ledStatusBuffer[i] = 0;
			}
		} else if (tmp == BLINK_RED_SLOW) {
			if (ledStatusBuffer[i] < 2) {
				Led_Display_Color(i, RED);
			} else if (ledStatusBuffer[i] < 3){
				Led_Display_Color(i, NONE);
			}
			ledStatusBuffer[i]++;
			if (ledStatusBuffer[i] > 3) {
				ledStatusBuffer[i] = 0;
			}
		} else if (tmp == BLINK_GREEN_FAST) {
			if (ledStatusBuffer[i] < 1){
				Led_Display_Color(i, GREEN);
			} else if (ledStatusBuffer[i] < 2) {
				Led_Display_Color(i, NONE);
			}
			ledStatusBuffer[i]++;
			if (ledStatusBuffer[i] > 2) {
				ledStatusBuffer[i] = 0;
			}
		} else if (tmp == BLINK_GREEN_SLOW) {
			if (ledStatusBuffer[i] < 2){
				Led_Display_Color(i, GREEN);
			} else if (ledStatusBuffer[i] < 3){
				Led_Display_Color(i, NONE);
			}
			ledStatusBuffer[i]++;
			if (ledStatusBuffer[i] > 3){
				ledStatusBuffer[i] = 0;
			}
		}
	}
	Led_Display();
}

