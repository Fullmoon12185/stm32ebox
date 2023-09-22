/*
 * app_led_display.c
 *
 *  Created on: Dec 31, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_led_display.h"
#include "app_uart.h"

#define		NUMBER_OF_LEDS					20
#define		TIME_FOR_FAST_BLINK_ON			1
#define		TIME_FOR_FAST_BLINK_OFF			2

#define		TIME_FOR_SLOW_BLINK_ON			5
#define		TIME_FOR_SLOW_BLINK_OFF			10

uint32_t ledStatus;

#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
void Network_Led_Init(void);
void Network_Connecting(void);
void Network_Connected(void);
void Network_Led_Display_Color(LED_COLOR color);
void Network_LED_Display_FSM(void);

LED_COLOR networkStatus = NONE;
uint8_t networkLedDisplayCounter = 0;

LED_COLOR ledStatusBuffer[NUMBER_OF_RELAYS+2] = {
		NONE,
		RED,
		GREEN,
		YELLOW,
		BLINK_RED_SLOW,
		BLINK_RED_FAST,
		BLINK_GREEN_FAST,
		BLINK_GREEN_SLOW,
		BLINK_YELLOW_SLOW,
		BLINK_YELLOW_FAST
	};

uint8_t ledDisplayCounterBuffer[NUMBER_OF_RELAYS+2] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t ledStateBuffer[NUMBER_OF_RELAYS+2] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#else



LED_COLOR ledStatusBuffer[NUMBER_OF_RELAYS] = {
		NONE,
		RED,
		GREEN,
		YELLOW,
		BLINK_RED_SLOW,
		BLINK_RED_FAST,
		BLINK_GREEN_FAST,
		BLINK_GREEN_SLOW,
		BLINK_YELLOW_SLOW,
		BLINK_YELLOW_FAST
	};

uint8_t ledDisplayCounterBuffer[NUMBER_OF_RELAYS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t ledStateBuffer[NUMBER_OF_RELAYS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

#endif
static void Latch_Enable(int8_t count);
static void Latch_Disable(int8_t count);
static void Output_Enable(void);
//static void Output_Disable(void);
static void Clock_On(int8_t count);
static void Clock_Off(int8_t count);



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

#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	Network_Led_Init();
#endif

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


void Led_Update_Status_Buffer(uint8_t position, NodeStatus status){
	if(position < NUMBER_OF_RELAYS){
		if(status == NODE_NORMAL) {
			ledStatusBuffer[position] = (LED_COLOR)GREEN;
		} else if(status == NODE_READY) {
			ledStatusBuffer[position] = (LED_COLOR)BLINK_GREEN_SLOW;
		} else if(status == CHARGING) {
			ledStatusBuffer[position] = (LED_COLOR)BLINK_RED_SLOW;
		} else if(status == CHARGEFULL) {
			ledStatusBuffer[position] = (LED_COLOR)BLINK_GREEN_FAST;
		} else if(status == UNPLUG) {
			ledStatusBuffer[position] = (LED_COLOR)BLINK_RED_FAST;
		} else if(status == NO_POWER) {
			ledStatusBuffer[position] = (LED_COLOR)BLINK_RED_FAST;
		} else if(status == NO_FUSE) {
			ledStatusBuffer[position] = (LED_COLOR)BLINK_RED_FAST;
		} else if(status == TOTAL_OVER_CURRRENT) {
			ledStatusBuffer[position] = (LED_COLOR)BLINK_RED_FAST;
		} else if(status == NO_RELAY) {
			ledStatusBuffer[position] = (LED_COLOR)BLINK_RED_SLOW;
		} else if(status == NODE_OVER_CURRENT) {
			ledStatusBuffer[position] = (LED_COLOR)RED;
		} else if(status == NODE_OVER_MONEY) {
			ledStatusBuffer[position] = (LED_COLOR)RED;
		} else if(status == NODE_OVER_TIME) {
			ledStatusBuffer[position] = (LED_COLOR)RED;
		} else if(status == RELAY_BROKEN) {
			ledStatusBuffer[position] = (LED_COLOR)RED;
		}

	}
}

void Led_Display_Color(uint8_t position, LED_COLOR color) {
	if(position < NUMBER_OF_RELAYS){
		uint32_t colorMask = color & 0x00000003;
//for general
#if(BOX_PLACE == BOX_GENERAL)
		ledStatus &= ~(0x00000003 << ((NUMBER_OF_RELAYS - position - 1) * 2));
		ledStatus |= colorMask << ((NUMBER_OF_RELAYS - position - 1) * 2);
#elif(BOX_PLACE == BOX_WITH_6_OUTLETS)
		ledStatus &= ~(0x00000003 << ((NUMBER_OF_RELAYS - position - 1) * 2));
		if(position <= 5)
			ledStatus |= colorMask << ((NUMBER_OF_RELAYS - position - 1) * 2);
#elif(BOX_PLACE == BOX_AT_XI)
		//change the led display in reverse order 0ld
		ledStatus &= ~(0x00000003 << (position * 2));
		ledStatus |= colorMask << (position * 2);

#elif(BOX_PLACE == BOX_AT_SHOW_ROOM)
//for showroom
//change the led display in reverse order 0ld
		ledStatus &= ~(0x00000003 << (position * 2));
		ledStatus |= colorMask << (position * 2);
#endif
	}
}

#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
void Network_Led_Init(void){
	networkLedDisplayCounter = 0;
	networkStatus = BLINK_RED_SLOW;
}

void Network_Off(void){
	networkStatus = BLINK_RED_SLOW;
}

void Network_Connecting(void){
	networkStatus = BLINK_GREEN_FAST;
}

void Network_Connected(void){
	networkStatus = BLINK_YELLOW_SLOW;
}

void Network_Led_Display_Color(LED_COLOR color) {
	uint32_t colorMask = color & 0x00000003;
	uint8_t networkLedPosition = 2;
	ledStatus &= ~(0x00000003 << ((NUMBER_OF_RELAYS + networkLedPosition - 1) * 2));
	ledStatus |= colorMask << ((NUMBER_OF_RELAYS + networkLedPosition - 1) * 2);
}

void Network_LED_Display_FSM(void) {
	if(networkStatus == NONE || networkStatus == GREEN || networkStatus == RED || networkStatus == YELLOW){
		Network_Led_Display_Color(networkStatus);
	} else if(networkStatus == BLINK_YELLOW_SLOW || networkStatus == BLINK_GREEN_SLOW || networkStatus == BLINK_RED_SLOW ) {
		if(networkLedDisplayCounter < TIME_FOR_SLOW_BLINK_ON){
			if(networkStatus == BLINK_YELLOW_SLOW){
				Network_Led_Display_Color(YELLOW);
			} else if(networkStatus == BLINK_GREEN_SLOW){
				Network_Led_Display_Color(GREEN);
			}else if(networkStatus == BLINK_RED_SLOW){
				Network_Led_Display_Color(RED);
			}
		} else if(networkLedDisplayCounter < TIME_FOR_SLOW_BLINK_OFF) {
			Network_Led_Display_Color(NONE);
		}
		networkLedDisplayCounter ++;
		if(networkLedDisplayCounter >= TIME_FOR_SLOW_BLINK_OFF){
			networkLedDisplayCounter = 0;
		}
	} else if(networkStatus == BLINK_YELLOW_FAST || networkStatus == BLINK_GREEN_FAST || networkStatus == BLINK_RED_FAST ) {
		if(networkLedDisplayCounter < TIME_FOR_FAST_BLINK_ON){
			if(networkStatus == BLINK_YELLOW_FAST){
				Network_Led_Display_Color(YELLOW);
			} else if(networkStatus == BLINK_GREEN_FAST){
				Network_Led_Display_Color(GREEN);
			}else if(networkStatus == BLINK_RED_FAST){
				Network_Led_Display_Color(RED);
			}
		} else if(networkLedDisplayCounter < TIME_FOR_FAST_BLINK_OFF) {
			Network_Led_Display_Color(NONE);
		}
		networkLedDisplayCounter ++;
		if(networkLedDisplayCounter >= TIME_FOR_FAST_BLINK_OFF){
			networkLedDisplayCounter = 0;
		}
	}
} 
#endif


void Led_Display_Clear_All(void) {
	for(uint8_t i = 0; i < NUMBER_OF_RELAYS; i++){
		Led_Update_Status_Buffer(i, NONE);
	}
}

void Led_Display_Set_All(LED_COLOR color) {
	for(uint8_t i = 0; i < NUMBER_OF_RELAYS; i++){
		Led_Update_Status_Buffer(i, color);
	}
}

void Test_Led_Display(void){
	static uint8_t ledIndex = 0;

	ledStatus = 0x01 << ledIndex;
	ledIndex = (ledIndex + 1)%20;

	Led_Display();
}
void LED_Display_FSM(void) { // call each 200ms
	LED_COLOR tempLedStatusBuffer;
	for (uint8_t relayIndex = 0; relayIndex < NUMBER_OF_RELAYS; relayIndex++) {
		tempLedStatusBuffer = ledStatusBuffer[relayIndex];
		if (tempLedStatusBuffer == NONE || tempLedStatusBuffer == RED
				|| tempLedStatusBuffer == GREEN || tempLedStatusBuffer == YELLOW) {

			Led_Display_Color(relayIndex, tempLedStatusBuffer);

		} else if (tempLedStatusBuffer == BLINK_RED_FAST) {

			if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_FAST_BLINK_ON) {
				Led_Display_Color(relayIndex, RED);
			} else if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_FAST_BLINK_OFF) {
				Led_Display_Color(relayIndex, NONE);
			}
			ledDisplayCounterBuffer[relayIndex]++;
			if (ledDisplayCounterBuffer[relayIndex] >= TIME_FOR_FAST_BLINK_OFF) {
				ledDisplayCounterBuffer[relayIndex] = 0;
			}

		} else if (tempLedStatusBuffer == BLINK_RED_SLOW) {
			if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_SLOW_BLINK_ON) {
				Led_Display_Color(relayIndex, RED);
			} else if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_SLOW_BLINK_OFF){
				Led_Display_Color(relayIndex, NONE);
			}
			ledDisplayCounterBuffer[relayIndex]++;
			if (ledDisplayCounterBuffer[relayIndex] >= TIME_FOR_SLOW_BLINK_OFF) {
				ledDisplayCounterBuffer[relayIndex] = 0;
			}
		} else if (tempLedStatusBuffer == BLINK_GREEN_FAST) {
			if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_FAST_BLINK_ON){
				Led_Display_Color(relayIndex, GREEN);
			} else if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_FAST_BLINK_OFF) {
				Led_Display_Color(relayIndex, NONE);
			}
			ledDisplayCounterBuffer[relayIndex]++;
			if (ledDisplayCounterBuffer[relayIndex] >= TIME_FOR_FAST_BLINK_OFF) {
				ledDisplayCounterBuffer[relayIndex] = 0;
			}
		} else if (tempLedStatusBuffer == BLINK_GREEN_SLOW) {
			if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_SLOW_BLINK_ON){
				Led_Display_Color(relayIndex, GREEN);
			} else if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_SLOW_BLINK_OFF){
				Led_Display_Color(relayIndex, NONE);
			}
			ledDisplayCounterBuffer[relayIndex]++;
			if (ledDisplayCounterBuffer[relayIndex] >= TIME_FOR_SLOW_BLINK_OFF){
				ledDisplayCounterBuffer[relayIndex] = 0;
			}
		} else if (tempLedStatusBuffer == BLINK_YELLOW_FAST) {
			if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_FAST_BLINK_ON){
				Led_Display_Color(relayIndex,YELLOW);
			} else if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_FAST_BLINK_OFF) {
				Led_Display_Color(relayIndex, NONE);
			}
			ledDisplayCounterBuffer[relayIndex]++;
			if (ledDisplayCounterBuffer[relayIndex] >= TIME_FOR_FAST_BLINK_OFF) {
				ledDisplayCounterBuffer[relayIndex] = 0;
			}
		} else if (tempLedStatusBuffer == BLINK_YELLOW_SLOW) {
			if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_SLOW_BLINK_ON){
				Led_Display_Color(relayIndex, YELLOW);
			} else if (ledDisplayCounterBuffer[relayIndex] < TIME_FOR_SLOW_BLINK_OFF){
				Led_Display_Color(relayIndex, NONE);
			}
			ledDisplayCounterBuffer[relayIndex]++;
			if (ledDisplayCounterBuffer[relayIndex] >= TIME_FOR_SLOW_BLINK_OFF){
				ledDisplayCounterBuffer[relayIndex] = 0;
			}
		}
	}
	#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
		Network_LED_Display_FSM();
	#endif

	Led_Display();
}

