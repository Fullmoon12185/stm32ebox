#include <app_flash.h>
#include "main.h"
uint8_t lengthPage;

char log[50];
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

void Flash_Write_Char(uint32_t addr, char* data)
{
	Flash_Unlock();
	int i;
  FLASH->CR |= FLASH_CR_PG;
	int var = 0;
	lengthPage = strlen(data);
  for(i=0; i<lengthPage; i+=2)
  {
    while((FLASH->SR&FLASH_SR_BSY));
    *(__IO uint16_t*)(addr + i) = *(uint16_t *)(data+i);
  }
	while((FLASH->SR&FLASH_SR_BSY)){};
  FLASH->CR &= ~FLASH_CR_PG;
  Flash_Lock();
}

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

void Flash_ProgramPage(char* dataIn, uint32_t addr1, uint32_t addr2)
{
	//FLASH_Unlock
	Flash_Unlock();
	//Flash_Erase Page
	Flash_Erase(addr1,1);
	//FLASH_Program HalfWord
	Flash_Write_Char(addr1,dataIn);
	HAL_Delay(100);
	char tempbuf[5] = {0};
	sprintf(tempbuf,"%d",lengthPage);
	//FLASH_Unlock
	Flash_Unlock();
	//Flash_Erase Page
	Flash_Erase(addr2,1);
	//FLASH_Program HalfWord
	Flash_Write_Char(addr2,tempbuf);
}


uint8_t is_Empty(char *data, uint8_t data_size){
	for (int var = 0; var < data_size; ++var) {
		if(data[var] != 0xFF){
			return 0;
		}
	}
	return 1;
}
