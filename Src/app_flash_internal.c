/*
 * app_flash_internal.c
 *
 *  Created on: Jul 29, 2021
 *      Author: Nguyet
 */
/**
  ******************************************************************************
  * @file    combros_peripheral_flash.c
  * @author  Combros
  * @version 1.0
  * @date    9/4/2019 11:27:22 AM
  * @brief   $brief$
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_flash_internal.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t	Flash_PageSize_Buffer[FLASH_PAGE_SIZE+1];

/* Private function prototypes -----------------------------------------------*/
#ifdef MCU_STM32L4
static uint32_t getpage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}
#endif

/* Exported functions --------------------------------------------------------*/
void cb_periph_flash_init()
{
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();

  /* Clear all FLASH flags */
	#if 	defined (MCU_STM32L0)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR);
	#elif defined (MCU_STM32L1)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR);
	#elif defined (MCU_STM32L4)
//	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	#elif defined (MCU_STM32F1)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	#elif defined (MCU_STM32F3)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	#endif
//	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
  /* Unlock the Program memory */
  HAL_FLASH_Lock();
}

#ifndef MCU_STM32L4
uint8_t cb_periph_flash_write_firmware(uint32_t Addr, uint8_t* pData, uint16_t Length)
{
  uint8_t ret_val = 0;
  uint32_t AddressTemp;
  uint32_t DataTemp;
  uint8_t DataBuffer[4];
  uint16_t Index = 0;

  AddressTemp = Addr;

  HAL_FLASH_Unlock();
  while (Index < Length)
  {
    for ( uint8_t i = 0; i < 4; i++ )
    {
      if (Index < Length)
      {
        DataBuffer[i] = pData[Index];
      }
      else
      {
        DataBuffer[i] = 0xFF;
      }
      Index++;
    }

    DataTemp = DataBuffer[3];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[2];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[1];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[0];

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, AddressTemp, DataTemp) == HAL_OK)
    {
      AddressTemp = AddressTemp + 4;
    }
    else
    {
      return ret_val;
    }
    DataTemp = 0;
  }

  HAL_FLASH_Lock();
  return 1;
}

uint8_t cb_periph_flash_write(uint32_t Addr, uint8_t* pBuffer, uint16_t Length)
{
  uint8_t ret_val = 0;
  uint32_t AddressTemp;
  uint32_t DataTemp;
  uint8_t DataBuffer[4];
  uint16_t Index = 0;

  AddressTemp = Addr;

  HAL_FLASH_Unlock();
  while (Index < Length)
  {
    for ( uint8_t i = 0; i < 4; i++ )
    {
      if (Index < Length)
      {
        DataBuffer[i] = pBuffer[Index];
      }
      else
      {
        DataBuffer[i] = 0xFF;
      }
			Index++;
    }

    DataTemp = DataBuffer[3];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[2];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[1];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[0];

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, AddressTemp, DataTemp) == HAL_OK)
    {
      AddressTemp = AddressTemp + 4;
    }
    else
    {
      return ret_val;
    }
    DataTemp = 0;
  }

  HAL_FLASH_Lock();
  return 1;
}

uint8_t cb_periph_flash_write_string(uint32_t Addr, uint8_t* pData, uint16_t Length)
{
  uint8_t   ret_val = 0;
  uint32_t  AddressTemp;
  uint32_t  DataTemp;
  uint8_t   DataBuffer[4];
  uint16_t  DataLength;
  uint16_t  Index = 0;

  AddressTemp = Addr;
  DataLength  = strlen((char *)pData);

  HAL_FLASH_Unlock();
  while (Index < Length)
  {
    for ( uint8_t i = 0; i < 4; i++ )
    {
      if (Index < Length)
      {
        if (Index < DataLength)
        {
          DataBuffer[i] = pData[Index];
        }
        else
        {
          DataBuffer[i] = 0x00;
        }
      }
      else
      {
        DataBuffer[i] = 0xFF;
      }
      Index++;
    }

    DataTemp = DataBuffer[3];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[2];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[1];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[0];

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, AddressTemp, DataTemp) == HAL_OK)
    {
      AddressTemp = AddressTemp + 4;
    }
    else
    {
      return ret_val;
    }
    DataTemp = 0;
  }

  HAL_FLASH_Lock();
  return 1;

}
#else
uint8_t cb_periph_flash_write_firmware(uint32_t Addr, uint8_t* pData, uint16_t Length)
{
  uint8_t ret_val = 0;
  uint32_t AddressTemp;
  uint64_t DataTemp;
  uint8_t DataBuffer[8];
  uint16_t Index = 0;

  AddressTemp = Addr;

  HAL_FLASH_Unlock();
  while (Index < Length)
  {
    for ( uint8_t i = 0; i < 8; i++ )
    {
      if (Index < Length)
      {
        DataBuffer[i] = pData[Index];
      }
      else
      {
        DataBuffer[i] = 0xFF;
      }
      Index++;
    }

    DataTemp = DataBuffer[7];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[6];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[5];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[4];
	DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[3];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[2];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[1];
	DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[0];

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, AddressTemp, DataTemp) == HAL_OK)
    {
      AddressTemp = AddressTemp + 8;
    }
    else
    {
      return ret_val;
    }
    DataTemp = 0;
  }

  HAL_FLASH_Lock();
  return 1;
}

uint8_t cb_periph_flash_write(uint32_t Addr, uint8_t* pBuffer, uint16_t Length)
{
  uint8_t ret_val = 0;
  uint32_t AddressTemp;
  uint64_t DataTemp;
  uint8_t DataBuffer[8];
  uint16_t Index = 0;

  AddressTemp = Addr;

  HAL_FLASH_Unlock();
  while (Index < Length)
  {
    for ( uint8_t i = 0; i < 8; i++ )
    {
      if (Index < Length)
      {
        DataBuffer[i] = pBuffer[Index];
      }
      else
      {
        DataBuffer[i] = 0xFF;
      }
			Index++;
    }

    DataTemp = DataBuffer[7];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[6];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[5];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[4];
	DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[3];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[2];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[1];
	DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[0];

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, AddressTemp, DataTemp) == HAL_OK)
    {
      AddressTemp = AddressTemp + 8;
    }
    else
    {
      return ret_val;
    }
    DataTemp = 0;
  }

  HAL_FLASH_Lock();
  return 1;
}

uint8_t cb_periph_flash_write_string(uint32_t Addr, uint8_t* pData, uint16_t Length)
{
  uint8_t   ret_val = 0;
  uint32_t  AddressTemp;
  uint64_t  DataTemp;
  uint8_t   DataBuffer[8];
  uint16_t  DataLength;
  uint16_t  Index = 0;

  AddressTemp = Addr;
  DataLength  = strlen((char *)pData);

  HAL_FLASH_Unlock();
  while (Index < Length)
  {
    for ( uint8_t i = 0; i < 8; i++ )
    {
      if (Index < Length)
      {
        if (Index < DataLength)
        {
          DataBuffer[i] = pData[Index];
        }
        else
        {
          DataBuffer[i] = 0x00;
        }
      }
      else
      {
        DataBuffer[i] = 0xFF;
      }
      Index++;
    }

    DataTemp = DataBuffer[7];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[6];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[5];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[4];
		DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[3];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[2];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[1];
		DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[0];

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, AddressTemp, DataTemp) == HAL_OK)
    {
      AddressTemp = AddressTemp + 8;
    }
    else
    {
      return ret_val;
    }
    DataTemp = 0;
  }

  HAL_FLASH_Lock();
  return 1;

}

#endif

uint8_t* cb_periph_flash_read(uint32_t Addr, uint8_t* pData, uint16_t Length)
{
	uint32_t Address = 0;
	uint32_t datatemp = 0;
	Address = Addr;
	uint8_t word[4];

	for (uint16_t i = 0; i < Length; i = i + 4)
	{
		datatemp = *(__IO uint32_t*)Address;

		word[0] = datatemp & 0xFF;
		*(pData + i) = word[0];

		word[1] = (datatemp>>8) & 0xFF;
		*(pData + i + 1) = word[1];

		word[2] = (datatemp>>16) & 0xFF;
		*(pData + i + 2) = word[2];

		word[3] = (datatemp>>24) & 0xFF;
		*(pData + i + 3) = word[3];

		Address += 4;
	}
	return pData;
}

uint32_t cb_periph_flash_erase(uint32_t start, uint8_t num_page)
{
	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef pEraseInit;
	HAL_StatusTypeDef status = HAL_OK;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Get the sector where start the user flash area */

	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	#ifndef MCU_STM32L4
	pEraseInit.PageAddress = start;
	#else
	pEraseInit.Page = getpage(start);
	#endif
	pEraseInit.NbPages = num_page;
	status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	if (status != HAL_OK){
		/* Error occurred while page erase */
		return 0;
	}

  return 1;

}

uint8_t	cb_periph_flash_write_replace(uint32_t Addr, uint8_t* pData, uint16_t Length)
{
	uint32_t PageAddr;
	uint16_t startIndex;
	uint16_t dataLength;

	PageAddr = Addr - (Addr % FLASH_PAGE_SIZE);
	startIndex = Addr % FLASH_PAGE_SIZE;
	dataLength = strlen((char *)pData);

	cb_periph_flash_read(PageAddr, Flash_PageSize_Buffer, FLASH_PAGE_SIZE);

	for (uint16_t i = 0; i < Length; i++)
	{
		if (i < dataLength)
		{
			Flash_PageSize_Buffer[startIndex + i] = *(pData++);
		}
		else
		{
			Flash_PageSize_Buffer[startIndex + i] = 0x00;
		}
	}
	cb_periph_flash_erase(PageAddr, 1);
	cb_periph_flash_write(PageAddr, Flash_PageSize_Buffer, FLASH_PAGE_SIZE);

	return 0;
}



