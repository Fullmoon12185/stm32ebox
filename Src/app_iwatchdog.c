/*
 * app_iwatchdog.c
 *
 *  Created on: Mar 23, 2020
 *      Author: VAIO
 */

#include "main.h"


IWDG_HandleTypeDef hiwdg;

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
