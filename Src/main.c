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


//extern uint32_t array_Of_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//
//
//extern uint32_t array_Of_Max_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//extern uint32_t array_Of_Min_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//
//extern uint32_t array_Of_Vpp_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//extern uint32_t array_Of_Vrms_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//
//extern uint32_t array_Of_Irms_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//extern uint32_t array_Of_Power_Consumption[NUMBER_OF_ADC_CHANNELS];
//extern uint32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_ADC_CHANNELS];


int main(void)
{
//	Sys_Init();
	System_Initialization();
//	SCH_Add_Task(test1, 0, 100);
	SCH_Add_Task(test2, 0, 100);
//	SCH_Add_Task(test3, 0, 50);

//	SCH_Add_Task(test4, 0, 200);
//	SCH_Add_Task(test5, 0, 200);


//	UART3_SendToHost((uint8_t*)"Power_Signal_Low \r\n");
//	HAL_Delay(1000);
//	Power_Signal_Low();
//	HAL_Delay(100);
//	Power_Signal_High();
//	UART3_SendToHost((uint8_t*)"Power_Signal_High \r\n");
	Set_Relay(0);
	Set_Relay(1);

	Set_Sim3G_State(POWER_ON_SIM3G);
	UART3_SendToHost((uint8_t*)"Start program \r\n");
//

	while (1){
		/* Insert delay 100 ms */

//		Sim3g_Run();
//		Set_Relay(8);
//		HAL_Delay(1000);
//		Reset_Relay(8);
//
//		HAL_Delay(1000);
//		test7();
//		test5();
		main_fsm();
		FSM_Process_Data_Received_From_Sim3g();
//		PowerConsumption_FSM();
//		Sim3g_Receive_Setup();

//		Set_Relay(7);
//		HAL_Delay(1000);
//		Reset_Relay(7);
//
//		HAL_Delay(1000);
		SCH_Dispatch_Tasks();
//		Led_Display();
	}
	return 0;
}




/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	while(1){
//		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
		HAL_Delay(100);
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
