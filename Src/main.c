/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <app_init.h>
#include "main.h"
#include "app_fsm.h"
#include "app_uart.h"
#include "app_adc.h"
#include "app_scheduler.h"
#include "app_relay.h"
#include "app_led_display.h"
#include "app_sim3g.h"
#include "app_test.h"
#include "app_pcf8574.h"
#include "app_gpio.h"
#include "app_iwatchdog.h"
#include "app_eeprom.h"
#include "app_spi.h"
#include "app_25LC512.h"


//#define UNSIGNED_CHAR_FIRST_EEPROM_DATA_ADDRESS 0x0001
//#define UNSIGNED_CHAR_SECOND_EEPROM_DATA_ADDRESS 0x0100
//#define UNSIGNED_INT_FIRST_EEPROM_DATA_ADDRESS 0x00C9
//#define UNSIGNED_INT_SECOND_EEPROM_DATA_ADDRESS 0x10C9
//#define UNSIGNED_SHORT_FIRST_EEPROM_DATA_ADDRESS 0x0400
//#define UNSIGNED_SHORT_SECOND_EEPROM_DATA_ADDRESS 0x0500
//
//// For character
//unsigned char glb_ucDataFromFirstEEProm[200];
//unsigned char glb_ucDataToFirstEEProm[200];
//unsigned char glb_ucDataFromSecondEEProm[200];
//unsigned char glb_ucDataToSecondEEProm[200];
//
//// For integer
//unsigned int glb_uiDataFromFirstEEProm[10];
//unsigned int glb_uiDataToFirstEEProm[10]={0x112345,0x212345,0x312345,0x412345,0x512345,0x612345,0x712345,0x812345,0x912345};
//unsigned int glb_uiDataFromSecondEEProm[10];
//unsigned int glb_uiDataToSecondEEProm[10]={0x123459,0x123458,0x123457,0x123456,0x123455,0x123454,0x123453,0x123452,0x123451};
//
//unsigned short glb_usDataFromFirstEEProm[10];
//unsigned short glb_usDataToFirstEEProm[10]={0x1123,0x2123,0x3123,0x4123,0x5123,0x6123,0x7123,0x8123,0x9123};
//unsigned short glb_usDataFromSecondEEProm[10];
//unsigned short glb_usDataToSecondEEProm[10]={0x3459,0x3458,0x3457,0x3456,0x3455,0x3454,0x3453,0x3452,0x3451};
uint8_t i,s;
uint32_t e, l;
uint8_t strtmp1[] = "                                    ";
uint8_t tmp ;
int main(void)
{
	System_Initialization();
 	MX_GPIO_Init();

 	eeprom_read_outlet  (&i, &s, &e, &l);
 	 s ++;
 	 e++;
 	 l++;
 	eeprom_write_outlet (i, s, e, l);
 	sprintf((char*) strtmp1, "i:%d\t s:%d\t e:%d\t l:%d\t \r\n", (int) i, (int)s, (int)e, (int)l);
 		UART3_SendToHost((uint8_t *)strtmp1);

	while (1){
	}
	return 0;
}


//
//#define UNSIGNED_INT_FIRST_EEPROM_DATA_ADDRESS 0x00C9
//
//typedef enum {
//	POWER_CONSUMPTION_CALCULATION = 0,
//	POST_DATA_TO_SERVER,
//	MAX_MAIN_FSM_NUMBER_STATES
//} MAIN_FSM_STATE;
//
//MAIN_FSM_STATE mainState = POWER_CONSUMPTION_CALCULATION;
//
//void Main_FSM(void);
//
//uint8_t i,s;
//uint32_t e, l;
//uint8_t strtmp1[] = "                                    ";
//int main(void)
//{
////	unsigned char glb_ucDataToFirstEEProm[200];
////	unsigned char glb_ucDataFromFirstEEProm[200];
////	unsigned char glb_ucDataToFirstEEProm[200];
//	System_Initialization();
////	Set_Sim3G_State(POWER_ON_SIM3G);
//	UART3_SendToHost((uint8_t*)"Start program \r\n");
//
//	UART3_SendToHost((uint8_t*)"SPI init \r\n");
//	 	ResetChipSelect();
////	Turn_On_Buzzer();
////	HAL_Delay(100);
////	Turn_Off_Buzzer();
////	HAL_Delay(100);
////	Turn_On_Buzzer();
////	HAL_Delay(100);
////	Turn_Off_Buzzer();
////	SCH_Add_Task(test2, 3, 100);
////	SCH_Add_Task(PCF_read, 7, 100);
////	SCH_Add_Task(LED_Display_FSM, 0, 20);
////	Set_Relay(0);
////	Set_Relay(3);
////	Set_Relay(8);
////	UART3_SendToHost((uint8_t*)"eeprom init \r\n");
////	HAL_Delay(100);
//
//
//	eeprom_initilize();
//	HAL_Delay(500);
//	UART3_SendToHost((uint8_t*)"eeprom read last value \r\n");
//	eeprom_read_outlet(&i, &s, &e, &l);
//	//i;
//	s++;
//	e++;
//	l++;
//	UART3_SendToHost((uint8_t*)"eeprom write value \r\n");
//	eeprom_write_outlet(i, s,  e, l );
//	sprintf((char*) strtmp1, "i:%d\t s:%d\t e:%d\t l:%d\t \r\n", (int) i, (int)s, (int)e, (int)l);
//	UART3_SendToHost((uint8_t *)strtmp1);
//
//	while (1){
////		Watchdog_Refresh();
////		SCH_Dispatch_Tasks();
////		Main_FSM();
//	}
//	return 0;
//}
//
//
//
//void Main_FSM(void){
//
////	Zero_Point_Detection();
//	PowerConsumption_FSM();
//	FSM_Process_Data_Received_From_Sim3g();
////	Server_Communication();
//	switch(mainState){
//	case POWER_CONSUMPTION_CALCULATION:
//
//		if(Is_Done_Getting_ADC() == SET){
//			mainState = POST_DATA_TO_SERVER;
//		}
//		break;
//	case POST_DATA_TO_SERVER:
//		Server_Communication();
//		Power_Loop();
//		if(Is_Done_Getting_ADC() == RESET){
//			mainState = POWER_CONSUMPTION_CALCULATION;
//		}
//		break;
//	default:
//		break;
//	}
//}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	while(1){
		HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
		HAL_Delay(50);
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
