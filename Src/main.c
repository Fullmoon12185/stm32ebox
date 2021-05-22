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
#include "app_gps.h"

#define		NORMAL_RUN	0
#define		TEST_RUN	1

typedef enum {
	POWER_CONSUMPTION_CALCULATION = 0,
	POST_DATA_TO_SERVER,
	MAX_MAIN_FSM_NUMBER_STATES
} MAIN_FSM_STATE;

MAIN_FSM_STATE mainState = POWER_CONSUMPTION_CALCULATION;

static uint8_t runtestState = 0;
void LCD_Show_Box_ID(void);

void Main_FSM(void);

int main(void)
{

	System_Initialization();
	printf("start program!\r\n");

//	UART3_SendToHost((uint8_t*)"Start program \r\n");

//	PCF_Init();
//#if(VERSION_EBOX == 2)
//	Lcd_Initialization();
//	Show_Box_ID(Get_Box_ID());
//#endif


	SCH_Add_Task(LED_Toggle, 11, 100);



#if(WATCHDOG_ENABLE == 1)
	    MX_IWDG_Init();
#endif
	while (1){
		SCH_Dispatch_Tasks();
		FSM_GPS_Process();
	}
	return 0;
}


void LCD_Show_Box_ID(void){
	Show_Box_ID(Get_Box_ID());
}

void Main_FSM(void){

#if(WATCHDOG_ENABLE == 1)
	if(Is_Watchdog_Reset() == 0 && !isConnestionLost()){
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
#if(VERSION_EBOX == 2)
			Show_KWH(Get_Main_Power_Consumption());
#endif
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
