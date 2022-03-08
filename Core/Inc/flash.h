#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "string.h"

typedef enum {
	FLASH_OK,        ///< "Good, code with all of your anger"
	FLASH_PRG_ERROR, ///< Programming error, you didn't call flashErasePage() did you?
	FLASH_WRT_ERROR  ///< Write protection error, this section of flash is write protected.
} FlashError;

void deleteBuffer(char* data);
void 	Flash_Lock(void);
void 	Flash_Unlock(void);
void 	Flash_Erase(uint32_t addr,uint32_t num_bytes);
void 	Flash_Write_Int(uint32_t addr, int data);
uint16_t Flash_Read_Int(uint32_t addr);
void 	Flash_Write_Char(uint32_t addr, uint8_t* data ,uint32_t data_len);
void 	Flash_ReadChar(uint32_t addr1 , char* dataOut, uint32_t data_length);
void 	Flash_ProgramPage(char* dataIn, uint32_t addr1, uint32_t addr2);
