/*
 * app_i2c_lcd.h
 *
 *  Created on: Jun 8, 2020
 *      Author: Nguyet
 */

#ifndef INC_APP_I2C_LCD_H_
#define INC_APP_I2C_LCD_H_

#include "main.h"

void LCD_Testing(void);
void Lcd_Initialization (void);   // initialize lcd

void Lcd_Send_Cmd (char cmd);  // send command to the lcd

void Lcd_Send_Data (char data);  // send data to the lcd

void Lcd_Send_String (char *str);  // send string to the lcd

void Lcd_Clear_Display (void);	//clear display lcd

void Lcd_Goto_XY (int row, int col); //set proper location on screen

void Lcd_Show_String(char *str, int row, int col);
void Lcd_Show_Box_ID(uint16_t bID);


uint8_t PCF_Scan();

#endif /* INC_APP_I2C_LCD_H_ */
