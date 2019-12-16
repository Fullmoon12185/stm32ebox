/*
 * app_pcf8574.c
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */
#include "main.h"



uint8_t Read_PCF8574(uint8_t address);

uint32_t sacInput = 0;
extern I2C_HandleTypeDef I2cHandle;


uint8_t Read_PCF8574(uint8_t address){
	uint8_t i2cRxBuffer[1];
	if(HAL_I2C_Master_Receive(&I2cHandle, (uint8_t)address, (uint8_t *)i2cRxBuffer, I2C_BUFFERSIZE, 10000) != HAL_OK)
		return ERROR;
	return i2cRxBuffer[0];
}

void Read_All_PCF8574_Inputs(void){
	uint8_t tempInput = 0;
	tempInput = Read_PCF8574(I2C_ADDRESS_GPIO2);
	sacInput = ((uint32_t)tempInput);
	tempInput = Read_PCF8574(I2C_ADDRESS_GPIO1);
	sacInput = (sacInput << 8) | ((uint32_t)tempInput);
	tempInput = Read_PCF8574(I2C_ADDRESS_GPIO0);
	sacInput = (sacInput << 8) | ((uint32_t)tempInput);
}

FlagStatus SAC_Status(uint8_t sacIndex){
	if(sacInput & (((uint32_t)1 << sacIndex)))
		return RESET;
	return SET;
}



