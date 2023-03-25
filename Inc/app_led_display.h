/*
 * app_led_display.h
 *
 *  Created on: Dec 31, 2019
 *      Author: VAIO
 */

#ifndef APP_LED_DISPLAY_H_
#define APP_LED_DISPLAY_H_

#include "app_power.h"

#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)

typedef enum  {
	NONE = 0,
#if(BOX_PLACE == BOX_GENERAL || BOX_PLACE == BOX_WITH_6_OUTLETS)
	RED = 2,
	GREEN = 1,
#elif(BOX_PLACE == BOX_AT_SHOW_ROOM)
	RED = 2,
	GREEN = 1,
#elif(BOX_PLACE == BOX_AT_XI)
	RED = 1,
	GREEN = 2,
#endif
	YELLOW = 3,
	BLINK_RED_SLOW = 4,
	BLINK_RED_FAST = 5,
	BLINK_GREEN_SLOW = 6,
	BLINK_GREEN_FAST = 7,
	BLINK_YELLOW_SLOW = 8,
	BLINK_YELLOW_FAST = 9
}LED_COLOR;

#else

typedef enum  {
	NONE = 0,
#if(BOX_PLACE == BOX_GENERAL || BOX_PLACE == BOX_WITH_6_OUTLETS)
	RED = 1,
	GREEN = 2,
#elif(BOX_PLACE == BOX_AT_SHOW_ROOM)
	RED = 2,
	GREEN = 1,
#elif(BOX_PLACE == BOX_AT_XI)
	RED = 1,
	GREEN = 2,
#endif
	YELLOW = 3,
	BLINK_RED_SLOW = 4,
	BLINK_RED_FAST = 5,
	BLINK_GREEN_SLOW = 6,
	BLINK_GREEN_FAST = 7,
	BLINK_YELLOW_SLOW = 8,
	BLINK_YELLOW_FAST = 9
}LED_COLOR;

#endif
//Normal OFF
//Chargin Red
//Charge Full = Green 1s  then  red 1s off = {6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4}
//Unplug = Blink Red fast 3 times then off after 5s  = {4,4,4}
//Nofuse = blink 2 fast 1 slow						= {5,5,4}
//No relay = blink 1 fast 2 slow					=  {5,4,4}
//No power
//Over current 										=  {5,5,5}
//Over money 										= {4,5,5}
//Over time 										= {4,4,5}

void Led_Display_Init(void);
void Led_Display_Color(uint8_t ledpos, LED_COLOR color);
void Led_Update_Status_Buffer(uint8_t position, NodeStatus status);

void Led_Display_Set_All(LED_COLOR color);
void Led_Display_Clear_All(void);

void Led_Display(void);

void LED_Display_FSM(void);
void Test_Led_Display(void);

#endif /* APP_LED_DISPLAY_H_ */
