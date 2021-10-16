/*
 * app_bootloader.c
 *
 *  Created on: Jul 29, 2021
 *      Author: Nguyet
 */
#include "main.h"
#include "app_bootloader.h"

/* USER CODE BEGIN PV */
typedef  void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t JumpAddress;


uint8_t OTA_hadRequested();
uint8_t OTA_hadSucceeded();




uint8_t OTA_hasRequested()
{
  uint8_t ota_buffer[12];

  FLASH_READ(OTA_FLAG_ADDRESS, ota_buffer, OTA_FLAG_OFFSET);
  if (ota_buffer[0] == 'T')
  {
    return 1;
  }

  return 0;
}

uint8_t OTA_hasSucceeded()
{
	uint8_t ota_buffer[12];

	FLASH_READ(OTA_FB_DONE_ADDRESS, ota_buffer, OTA_FB_DONE_OFFSET);
	if (ota_buffer[0] == 'D')
	{
		return 1;
	}
	return 0;
}


uint8_t Jump_To_Bootloader_To_Update_Firmware(void){
//	if (((*(__IO uint32_t*)BOOTLOADER_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
//		{
//
//			printf("---> Bootloader available!\r\n");
//			if (OTA_hasRequested() == 1)
//			{
//				printf("---> OTA requested. Jump to bootloader!\r\n");
//				HAL_Delay(1000);
//
//				/* Jump to user application */
//				JumpAddress = *(__IO uint32_t*) (BOOTLOADER_ADDRESS + 4);
//				JumpToApplication = (pFunction) JumpAddress;
//				/* Initialize user application's Stack Pointer */
//				__set_MSP(*(__IO uint32_t*) BOOTLOADER_ADDRESS);
//				JumpToApplication();
//			}
//		}
//		else
//		{
//			printf("---> Bootloader not found!\r\n");
//		}
	return 0;
}




