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


extern uint32_t arrayOfADCValues[NUMBER_OF_ADC_CHANNELS*NUMBER_OF_SAMPLES_PER_AVERAGE];
extern uint32_t arrayOfAverageADCValues[NUMBER_OF_ADC_CHANNELS][NUMBER_OF_SAMPLES_PER_SECOND];

uint32_t temp;
void test1(void){
	HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
}

void test2(void){
	HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
}
int main(void)
{
	System_Initialization();
	SCH_Add_Task(test1, 0, 100);
	SCH_Add_Task(test2, 0, 1000);
	while (1){
		/* Insert delay 100 ms */

		SCH_Dispatch_Tasks();
//		temp = GetADCValue(0);
//		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
//		HAL_Delay(1000);
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
		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
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
