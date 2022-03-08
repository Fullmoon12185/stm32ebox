#include <flash.h>
#include "main.h"

#define page_size 0x800

void deleteBuffer(char* data)
{
	uint8_t len = strlen(data);
	for(uint8_t i = 0; i < len; i++)
	{
		data[i] = 0;
	}
}

void Flash_Lock()
{
	HAL_FLASH_Lock();
}

void Flash_Unlock()
{
	HAL_FLASH_Unlock();
}

void Flash_Erase(uint32_t addr,uint32_t num_bytes)
{
	uint32_t num_page = (uint32_t)(num_bytes/FLASH_PAGE_SIZE)+1;
	uint32_t page_error;
	Flash_Unlock();
	FLASH_EraseInitTypeDef erase_instance;
	erase_instance.TypeErase = FLASH_TYPEERASE_PAGES;
	erase_instance.NbPages = num_page;
	erase_instance.PageAddress = addr;
	HAL_FLASHEx_Erase(&erase_instance, &page_error);
	Flash_Lock();
	LOG("Erase done\r\n");
}

void Flash_Write_Int(uint32_t addr, int data)
{
	Flash_Unlock();
	FLASH->CR |= FLASH_CR_PG;				/*!< Programming */
	while((FLASH->SR&FLASH_SR_BSY));
	*(__IO uint16_t*)addr = data;
	while((FLASH->SR&FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_CR_PG;
	Flash_Lock();
}

uint16_t Flash_Read_Int(uint32_t addr)
{
	uint16_t* val = (uint16_t *)addr;
	return *val;
}


void Flash_Write_Char(uint32_t addr, uint8_t* data ,uint32_t data_len)
{
	char log[20];
	sprintf(log,"leng_char: %d\r\n",data_len);
	LOG(log);
	int i;
	uint16_t temp;
	Flash_Unlock();
	FLASH->CR |= FLASH_CR_PG;
	for(i=0; i<data_len; i+=2)
	{
	  while((FLASH->SR&FLASH_SR_BSY));
	  *(__IO uint16_t*)(addr + i) = *(uint16_t*)(data+i);
	}
	while((FLASH->SR&FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_CR_PG;
	Flash_Lock();
	LOG("Write DOne");
}

//void Flash_Write_Char(uint32_t addr, uint8_t* data,uint32_t data_len)
//{
//	Flash_Unlock();
//	int i;
//  FLASH->CR |= FLASH_CR_PG;
//  for(i=0; i<data_len; i+=2)
//  {
//    while((FLASH->SR&FLASH_SR_BSY));
//    *(__IO uint16_t*)(addr + i) = *(uint16_t*)(data+i);
//  }
//	while((FLASH->SR&FLASH_SR_BSY)){};
//  FLASH->CR &= ~FLASH_CR_PG;
//  Flash_Lock();
//}


void Flash_ReadChar(uint32_t addr1 , char* dataOut, uint32_t data_length)
{
	uint16_t temp_data;
	deleteBuffer(dataOut);
	for (int var = 0; var < data_length; var+=2) {
		temp_data = Flash_Read_Int(addr1 + var);
		dataOut[var+1] = (temp_data&0xFF00)>> 8;
		dataOut[var] = temp_data;
	}
	return;
}




