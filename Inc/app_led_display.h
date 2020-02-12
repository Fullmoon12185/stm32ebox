/*
 * app_led_display.h
 *
 *  Created on: Dec 31, 2019
 *      Author: VAIO
 */

#ifndef APP_LED_DISPLAY_H_
#define APP_LED_DISPLAY_H_

void Led_Display_Init(void);
void Led_Display_Update_Buffer(uint8_t ledpos, FlagStatus status);
void Led_Display(void);
#endif /* APP_LED_DISPLAY_H_ */
