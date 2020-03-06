/*
 * app_led_display.c
 *
 *  Created on: Dec 31, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_led_display.h"

#define		NUMBER_OF_LEDS	20

//uint8_t ledPositions[NUMBER_OF_LEDS] = { 25, 24, 27, 26, 29, 28, 31, 30, 23, 22, 9, 8, 11, 10, 13, 12, 15, 14, 7, 6
//
//};
//uint32_t ledStatusBuffer = 0;
uint32_t ledStatus;
static void Latch_Enable(int8_t count);
static void Latch_Disable(int8_t count);
static void Output_Enable(void);
//static void Output_Disable(void);
static void Clock_On(int8_t count);
static void Clock_Off(int8_t count);

void Clear_Led_Display_Buffer(void);

void Led_Display_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
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

static void Latch_Enable(int8_t count) {
	if (count <= 0) return;
	while (count-- != 0) {
		HAL_GPIO_WritePin(LED_LE_PORT, LED_LE, GPIO_PIN_RESET);
	}
}
static void Latch_Disable(int8_t count) {
	if (count <= 0) return;
	while (count-- != 0) {
		HAL_GPIO_WritePin(LED_LE_PORT, LED_LE, GPIO_PIN_SET);
	}
}
static void Output_Enable(void) {
	HAL_GPIO_WritePin(LED_OE_PORT, LED_OE, GPIO_PIN_SET);
}
//static void Output_Disable(void){
//	HAL_GPIO_WritePin(LED_OE_PORT, LED_OE, GPIO_PIN_SET);
//}
static void Clock_On(int8_t count) {
	if (count <= 0) return;
	while (count-- != 0) {
		HAL_GPIO_WritePin(LED_SCK_PORT, LED_SCK, GPIO_PIN_RESET);
	}

}
static void Clock_Off(int8_t count) {
	if (count <= 0) return;
	while (count-- != 0) {
		HAL_GPIO_WritePin(LED_SCK_PORT, LED_SCK, GPIO_PIN_SET);
	}

}
static void Data_Out(GPIO_PinState state) {
	HAL_GPIO_WritePin(LED_SDI_PORT, LED_SDI, state);
}


void Led_Display_Show(void) {
	uint8_t i;
	uint32_t ledValue = ledStatus << 4;
//		HAL_GPIO_WritePin(LED7_LE_GPIO_Port, LED7_LE_Pin, 1);
	Latch_Disable(1);
	for (i = 0; i < 24; i++) {
//			HAL_GPIO_WritePin(LED7_CLK_GPIO_Port, LED7_CLK_Pin, 1);

//			if ((ledValue & 0x00000001) > 0) {
//				HAL_GPIO_WritePin(LED7_SDI_GPIO_Port, LED7_SDI_Pin, 0);
//			} else {
//				HAL_GPIO_WritePin(LED7_SDI_GPIO_Port, LED7_SDI_Pin, 1);
//			}
		Clock_Off(1);
		Data_Out(ledValue & 0x00000001);
//			HAL_GPIO_WritePin(LED7_CLK_GPIO_Port, LED7_CLK_Pin, 0);
		Clock_On(10);

//		HAL_Delay(1);
		ledValue = ledValue >> 1;
	}
//		HAL_GPIO_WritePin(LED7_LE_GPIO_Port, LED7_LE_Pin, 0);

	Latch_Enable(1);
}

//void Led_Display_Update_Buffer(uint8_t ledpos, FlagStatus status){
//	if(ledpos >= NUMBER_OF_LEDS) return;
//	if(status == RESET){
//		ledStatusBuffer = ledStatusBuffer & (~ (1 << ledPositions[ledpos]));
//	} else {
//		ledStatusBuffer = ledStatusBuffer | (1 << ledPositions[ledpos]);
//	}
//}
void Led_Display_Color(uint8_t position, enum LedColor color) {
	uint32_t colorMask = color & 0x00000003;
//	if(position >=0  && position < No_CHANNEL) position -=1;
//	if(position >= 0 && position <No_CHANNEL) {
//		position = No_CHANNEL - position - 1;
		ledStatus &= ~(0x00000003 << (position * 2));
		ledStatus |= colorMask << (position * 2);
//	}
//	Led_Display();
}

//void Clear_Led_Display_Buffer(void){
//	uint8_t i = 0;
//	for(i = 0; i < NUMBER_OF_LEDS; i++){
//		Led_Display_Update_Buffer(i, RESET);
//	}
//}

void Led_Display_Clear_All(void) {
	ledStatus = 0;
	Led_Display_Show();
}

void Led_Display_Set_All(void) {
	ledStatus = 0xffffffff;
	Led_Display_Show();
}
//uint8_t ledCounter = 0;
//void Led_Display(void){
//	uint8_t i;
//
//	for (i = 0; i < 32; i ++){
//		Data_Out((ledStatusBuffer >> i) & 0x01);
//		Clock_Off(1);
//		Clock_On(1);
//	}
//	Latch_Enable(1);
//	Latch_Disable(1);
//
//}



