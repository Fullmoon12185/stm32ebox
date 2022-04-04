/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "locker_config.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/*
 * Firmware Choosen
 */
#define FOTA_FIRMWARE_CHOOSEN		0
#define FACTORY_FIRMWARE_CHOOSEN	1
#define CURRENT_FIRMWARE_CHOOSEN	2
/*
 * Update Firmware Status
 */
#define UPDATE_SUCCESS				0
#define UPDATE_FAILED				1
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */


#define BOOTLOADER_FIRMWARE_ADDR	0x08000000			// Address of Bootloader Firmware
#define FACTORY_FIRMWARE_ADDR		0x08005000			// Bootloader have 20KBytes from 0x08000000
#define FOTA_FIRMWARE_ADDR			0x0801E000			// Factory have 100KBytes from 0x08005800
#define CURRENT_FIRMWARE_ADDR		0x08040000			// FOTA have 50Kbytes from 0x0801E000
#define FIRMWARE_CHOOSEN			0x08060000			// Address for choose what firmware MCU will run
#define FACTORY_VERSION_ADDR		0x08070000			// Address contain FACTORY Firmware Version
#define CURRENT_VERSION_ADDR		0x08075000			// Address contain CURRENT Firmware Version
#define UPDATE_STATUS_ADDR			0x08076000			// Address contain FACTORY Firmware Version


#define FIRMWARE_READ_SIZE_PER_TIME	1024
#define TEMP_BUFFER_SIZE			2048
#define FIRMWARE_PAGE_LENGTH		50					// 50Pages
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define LOG	UART_DEBUG_Transmit
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
