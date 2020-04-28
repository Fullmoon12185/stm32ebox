/*
 * app_iwatchdog.c
 *
 *  Created on: Mar 23, 2020
 *      Author: VAIO
 */

#include "main.h"


IWDG_HandleTypeDef hiwdg;
static uint32_t counter_for_watchdog = 0;
/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

void Watchdog_Refresh(void){
	HAL_IWDG_Refresh(&hiwdg);
}

uint8_t Is_Watchdog_Reset(void){
	if(counter_for_watchdog > 3){
		return 1;
	}
	return 0;
}
void Watchdog_Counting(void){
	counter_for_watchdog++;
}

void Reset_Watchdog_Counting(void){
	counter_for_watchdog = 0;
}
