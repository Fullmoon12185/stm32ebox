/*
 * app_25LC512.c
 *
 *  Created on: Dec 14, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_spi.h"
#include "app_25LC512.h"



extern SPI_HandleTypeDef Spi2Handle;

const uint16_t TIME_DELAY = 10;

void MC25LC512_CS(uint8_t CS_Status);
void MC25LC512_WriteEnableOrDisable(unsigned char EnableOrDisable);
unsigned char MC25LC512_ReleaseDeepPowerDownMode(void);




void MC25LC512_CS(uint8_t CS_Status)
{

	// For Cs of the EEprom
	if(CS_Status == EEPROM_CS_PIN_RESET)
	{
		ResetChipSelect();
	}
	else
	{
		SetChipSelect();

	}

}
void MC25LC512_WriteEnableOrDisable(uint8_t EnableOrDisable)
{
	uint8_t SendOneByte = 0;
	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	for(uint16_t i = 0; i < 10; i ++);
	if(EnableOrDisable==EEPROM_Enable)
	{
			SendOneByte = MC25LCxxx_SPI_WREN;
	}
	else
	{
			SendOneByte = MC25LCxxx_SPI_WRDI;
	}
	HAL_SPI_Transmit(&Spi2Handle , &SendOneByte, 1, 200) ;
	for(uint16_t i = 0; i < 10; i ++);
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Set The spi Chip //Set means Disable
}
uint8_t MC25LC512_ReleaseDeepPowerDownMode(void)
{

	uint8_t SendOneByte;
	uint8_t RecieveByteOfReleaseDeepPowerMode = 0;
	SendOneByte = MC25LCxxx_SPI_RDID;

	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	for(uint16_t i = 0; i < 1000; i ++);
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);

	HAL_SPI_Receive(&Spi2Handle , &RecieveByteOfReleaseDeepPowerMode, 1,200) ;//Address of Manufaturer id High
	HAL_SPI_Receive(&Spi2Handle , &RecieveByteOfReleaseDeepPowerMode, 1,200) ;//Address of Manufaturer id Low
	HAL_SPI_Receive(&Spi2Handle , &RecieveByteOfReleaseDeepPowerMode, 1,200) ;//Manufaturer id
	for(uint16_t i = 0; i < 1000; i ++);
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Set The spi Chip //Set means Disable

	return RecieveByteOfReleaseDeepPowerMode;

}

void MC25LC512_Initialize(void)
{

	MC25LC512_CS(EEPROM_CS_PIN_SET);// Reset The spi Chip //Reset means Enable
	MC25LC512_ReleaseDeepPowerDownMode();
	MC25LC512_ReadStatusRegister();
	MC25LC512_WriteEnableOrDisable(EEPROM_Enable);

}
void MC25LC512_Write_Bytes(uint16_t AddresOfData, uint8_t *WriteArrayOfEEProm, uint16_t SizeOfArray)
{

	uint8_t SendOneByte;
	MC25LC512_WriteEnableOrDisable(EEPROM_Enable);
	for(uint16_t i = 0; i < TIME_DELAY; i ++);
	MC25LC512_CS(EEPROM_CS_PIN_SET);
	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	for(uint16_t i = 0; i < TIME_DELAY; i ++);
	SendOneByte = MC25LCxxx_SPI_WRITE;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);
	SendOneByte = AddresOfData>>8;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//High byte of address
	SendOneByte = AddresOfData & 0x00FF;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//Low byte of address
	//
	HAL_SPI_Transmit(&Spi2Handle, WriteArrayOfEEProm, SizeOfArray, SizeOfArray*50) ;
	for(uint16_t i = 0; i < TIME_DELAY; i ++);
	MC25LC512_CS(EEPROM_CS_PIN_RESET);
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Reset The spi Chip //Reset means Enable
	for(uint16_t i = 0; i < 10000; i ++);
	for(uint16_t i = 0; i < 10000; i ++);
//	MC25LC512_WriteEnableOrDisable(EEPROM_Enable);

}
void MC25LC512_Write_HalfWords(uint16_t AddresOfData, uint16_t *WriteArrayOfEEProm, uint16_t sizeOfArray)
{

	uint8_t SendOneByte;

	uint16_t i = 0,j = 0;
	uint8_t tmpArrayForSend[2];

	MC25LC512_WriteEnableOrDisable(EEPROM_Enable);
	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	HAL_Delay(1);
	SendOneByte=MC25LCxxx_SPI_WRITE;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);
	SendOneByte= AddresOfData >> 8;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//High byte of address
	SendOneByte= AddresOfData & 0x00FF;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//Low byte of address
	////
	for(j = 0; j < sizeOfArray; j ++){
		for(i = 0; i < 2; i ++)
			tmpArrayForSend[i] = (((WriteArrayOfEEProm[j]) >> ((1 - i) * 8)) & 0x000000FF);
		HAL_SPI_Transmit(&Spi2Handle, tmpArrayForSend, 2, 50) ;
	}
	HAL_Delay(4);
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Reset The spi Chip //Reset means Enable
}
void MC25LC512_Write_Words(uint16_t AddresOfData, uint32_t *WriteArrayOfEEProm, uint16_t sizeOfArray)
{

	uint8_t SendOneByte;

	uint16_t i = 0,j = 0;
	uint8_t tmpArrayForSend[4];
	MC25LC512_WriteEnableOrDisable(EEPROM_Enable);
	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	HAL_Delay(1);
	SendOneByte = MC25LCxxx_SPI_WRITE;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);
	SendOneByte= AddresOfData >> 8;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//High byte of address
	SendOneByte= AddresOfData & 0x00FF;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//Low byte of address
	////
	for(j = 0; j < sizeOfArray; j ++){
		for(i = 0; i < 4; i ++)
			tmpArrayForSend[i] = (((WriteArrayOfEEProm[j]) >> ((3 - i) * 8)) & 0x000000FF);
		HAL_SPI_Transmit(&Spi2Handle, tmpArrayForSend, 4 ,50) ;
	}
	HAL_Delay(4);
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Reset The spi Chip //Reset means Enable

}
uint8_t MC25LC512_ReadStatusRegister(void)
{

	uint8_t SendOneByte = 0;
	uint8_t ReceiveOneByte;
	SendOneByte = MC25LCxxx_SPI_RDSR;
	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);
	HAL_SPI_Receive(&Spi2Handle, &ReceiveOneByte, 1, 200) ;//Address of Manufaturer id High
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Set The spi Chip //Set means Disable
	return ReceiveOneByte;
}



uint8_t MC25LC512_Read_Bytes(uint16_t AddresOfData, uint8_t *DataArrayOfEEProm, uint16_t SizeOfArray)
{

	uint8_t SendOneByte;
	//	uint8_t RecieveByteFromEEProm[1];
	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	for(uint16_t i = 0; i < TIME_DELAY; i ++);
	SendOneByte = MC25LCxxx_SPI_READ;//Config the Device
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);

	SendOneByte= AddresOfData >> 8;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//High byte of address
	SendOneByte= AddresOfData & 0x00FF;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//Low byte of address

	HAL_SPI_Receive(&Spi2Handle, DataArrayOfEEProm, SizeOfArray, SizeOfArray*30) ;//Receive Amount of  Data from EEPROM
	for(uint16_t i = 0; i < TIME_DELAY; i ++);
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Reset The spi Chip //Reset means Enable
	return 0;

}
uint8_t MC25LC512_Read_HalfWords(uint16_t AddresOfData, uint16_t *DataArrayOfEEProm, uint16_t SizeOfArray)
{
	uint8_t SendOneByte;
	uint8_t ucDataArrayOfEEProm[2];
	uint16_t i = 0;
	//	uint8_t RecieveByteFromEEProm[1];
	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	HAL_Delay(1);
	SendOneByte=MC25LCxxx_SPI_READ;//Config the Device
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);

	SendOneByte= AddresOfData>>8;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//High byte of address
	SendOneByte= AddresOfData & 0x00FF;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//Low byte of address

	for(i = 0; i < SizeOfArray; i ++){
		HAL_SPI_Receive(&Spi2Handle, ucDataArrayOfEEProm, 2, 150) ;//Receive Amount of  Data from EEPROM
		DataArrayOfEEProm[i] =	(uint16_t)((uint8_t)(ucDataArrayOfEEProm[0]) << 8 | (uint8_t)(ucDataArrayOfEEProm[1]));
	}
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Reset The spi Chip //Reset means Enable
	HAL_Delay(1);
	return 0;

}
uint8_t  MC25LC512_Read_Words(uint16_t AddresOfData, uint32_t *DataArrayOfEEProm, uint16_t SizeOfArray)
{
	uint8_t SendOneByte;
	uint8_t ucDataArrayOfEEProm[4];
	uint16_t i = 0;
	//	uint8_t RecieveByteFromEEProm[1];
	MC25LC512_CS(EEPROM_CS_PIN_RESET);// Reset The spi Chip //Reset means Enable
	HAL_Delay(1);
	SendOneByte=MC25LCxxx_SPI_READ;//Config the Device
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);

	SendOneByte= AddresOfData>>8;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//High byte of address
	SendOneByte= AddresOfData & 0x00FF;
	HAL_SPI_Transmit(&Spi2Handle, &SendOneByte, 1, 200);//Low byte of address

	for(i = 0; i < SizeOfArray; i ++){
		HAL_SPI_Receive(&Spi2Handle, ucDataArrayOfEEProm, 4, 150) ;//Receive Amount of  Data from EEPROM
		DataArrayOfEEProm[i] =(uint32_t)((uint8_t)(ucDataArrayOfEEProm[0]) << 24 |
															(uint8_t)(ucDataArrayOfEEProm[1]) << 16 |
															(uint8_t)(ucDataArrayOfEEProm[2]) << 8 	|
															(uint8_t)(ucDataArrayOfEEProm[3]));
	}
	MC25LC512_CS(EEPROM_CS_PIN_SET);// Reset The spi Chip //Reset means Enable
	HAL_Delay(1);
	return 0;
}





