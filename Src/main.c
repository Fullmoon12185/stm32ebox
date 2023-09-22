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
#include "app_i2c_lcd.h"
#include "app_init.h"
#include "app_send_sms.h"

#define		NORMAL_RUN	0
#define		TEST_RUN	1

static uint8_t log[100];

typedef enum {
	POWER_CONSUMPTION_CALCULATION = 0,
	POST_DATA_TO_SERVER,
	MAX_MAIN_FSM_NUMBER_STATES
} MAIN_FSM_STATE;

MAIN_FSM_STATE mainState = POWER_CONSUMPTION_CALCULATION;

static uint8_t runtestState = 0;
void LCD_Show_Box_ID(void);
void Test_Timer(void);

void Main_FSM(void);

int main(void)
{
	System_Initialization();
	UART3_SendToHost((uint8_t*)"Start program \r\n");
	sprintf(log,"Ebox Version: %d\r\n", (uint8_t)VERSION_EBOX);
	UART3_SendToHost((uint8_t*)log);

	PCF_Init();
#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	Lcd_Initialization();
	Show_Box_ID(Get_Box_ID());
#elif(VERSION_EBOX == 2 || VERSION_EBOX == 3 || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A || VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
	Lcd_Initialization();
	Show_Box_ID(Get_Box_ID());
#endif




	Setup_Eeprom();
	SCH_Add_Task(PCF_read, 13, 23);
	SCH_Add_Task(Start_Sending_Sms_Message, 6000*2, 6000*60*24*7);
//	Set_All();
	HAL_Delay(2000);
	if(Get_Box_ID() != 0){
		SCH_Add_Task(LED_Display_FSM, 19, 23);
		SCH_Add_Task(Watchdog_Counting, 7, 101);
		runtestState = NORMAL_RUN;
	} else {
		runtestState = TEST_RUN;
#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A || VERSION_EBOX == 2 || VERSION_EBOX == 3 || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A || VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
		SCH_Add_Task(LCD_Show_Box_ID, 7, 73);
		SCH_Add_Task(Test_Led_Display, 13, 57);
#endif

	}
	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, SET);
#if(WATCHDOG_ENABLE == 1)
	if(runtestState == NORMAL_RUN){
	    MX_IWDG_Init();
	}
#endif
	while (1){
		SCH_Dispatch_Tasks();
		if(runtestState == NORMAL_RUN){
			Main_FSM();
		}
	}
	return 0;
}


void LCD_Show_Box_ID(void){
	Show_Box_ID(Get_Box_ID());
}

void Test_Timer(void){
	static uint32_t counter = 0;
	counter++;
	char strtmp [] = "                                ";
	sprintf((char*) strtmp, "i: %d \r\n", (int) counter);
	UART3_SendToHost((uint8_t *)strtmp);

}

void Main_FSM(void){

#if(WATCHDOG_ENABLE == 1)
	if(Is_Watchdog_Reset() == 0
//			&& !isConnectionLost()
			&& !Is_Watchdog_Reset_Due_To_Not_Sending_Mqtt_Message()){
		Watchdog_Refresh();
	}
#endif
	PowerConsumption_FSM();
	FSM_Process_Data_Received_From_Sim3g();

	switch(mainState){
	case POWER_CONSUMPTION_CALCULATION:
		if(Is_Done_Getting_ADC() == SET){
			mainState = POST_DATA_TO_SERVER;
#if(WATCHDOG_ENABLE == 1)
			Reset_Watchdog_Counting();
#endif
		}

		break;
	case POST_DATA_TO_SERVER:
		Server_Communication();
		Process_System_Power();
		if(Is_Done_Getting_ADC() == RESET){
//#if(VERSION_EBOX == 2 || VERSION_EBOX == 3 || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A || VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
			Show_KWH(Get_Main_Power_Consumption());
//#endif
			Process_Main_Current_Over_Max_Current();
			mainState = POWER_CONSUMPTION_CALCULATION;
		}
		break;
	default:
		break;
	}
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
//	while(1){
//		HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
//		HAL_Delay(50);
//	}
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
