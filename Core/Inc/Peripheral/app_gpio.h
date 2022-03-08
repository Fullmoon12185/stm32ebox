/**
 * @file app_gpio.h
 * @author thodo
 * @brief Header file for app_gpio.c file. This is a file for GPIO controller: Led, Buzzer, Dipswitch
 */
#ifndef APP_GPIO_H_
#define APP_GPIO_H_

#include "main.h"


void GPIO_Init(void);
void SPI_CS_Init(void);

//void LED_Init(void);
void Switch_Init(void);
void Buzzer_Init(void);

void Turn_On_Buzzer(void);
void Turn_Off_Buzzer(void);
uint8_t Get_BoardID();

void Turn_On_Led2(void);
void Turn_Off_Led2(void);
void Led2_Init_Mode_Done(void);
void Led2_Init_Mode_Error(void);
void Led2_Normal_Working(void);
void Led2_Error_Working(void);
void Led2_Open_Cabinet(void);
void Led2_Close_Cabinet(void);
void Led2_Publish_Data(void);
void Led2_MainB_Send_To_SubB(void);
void Led2_MainB_Recv_From_SubB(void);
void Led2_SubB_Send_To_MainB(void);
void Led2_SubB_Recv_From_MainB(void);


#endif /* APP_GPIO_H_ */


