/*
 * app_iwatchdog.c
 *
 *  Created on: Mar 23, 2020
 *      Author: VAIO
 */

#include "main.h"


#define DURATION_FOR_RESET_MODULE_SIM	120
#define DURATION_FOR_WATCH_DOG_RESET	3000

IWDG_HandleTypeDef hiwdg;
static uint8_t counterForWatchdogReset = 0;
static uint8_t counterForResetModuleSim = 0;
static uint16_t counterForWatchdogResetDueToNotSendingMqttMessage = 0;
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

	counterForWatchdogReset = 0;
	counterForResetModuleSim = 0;
	counterForWatchdogResetDueToNotSendingMqttMessage = 0;
  /* USER CODE END IWDG_Init 2 */

}

void Watchdog_Refresh(void){
	HAL_IWDG_Refresh(&hiwdg);
}

uint8_t Is_Watchdog_Reset(void){
	if(counterForWatchdogReset > 3){
		return 1;
	}
	return 0;
}
void Watchdog_Counting(void){
	counterForWatchdogReset++;
	counterForResetModuleSim++;
	counterForWatchdogResetDueToNotSendingMqttMessage++;
}

void Reset_Watchdog_Counting(void){
	counterForWatchdogReset = 0;
}

void Clear_Counter_For_Reset_Module_Sim(void){
	counterForResetModuleSim = 0;
}
uint8_t Is_Reset_Module_Sim(void){
	return (counterForResetModuleSim > DURATION_FOR_RESET_MODULE_SIM);
}

void Clear_For_Watchdog_Reset_Due_To_Not_Sending_Mqtt_Message(void){
	counterForWatchdogResetDueToNotSendingMqttMessage = 0;
}
uint8_t Is_Watchdog_Reset_Due_To_Not_Sending_Mqtt_Message(void){
	return (counterForWatchdogResetDueToNotSendingMqttMessage > DURATION_FOR_WATCH_DOG_RESET);
}
