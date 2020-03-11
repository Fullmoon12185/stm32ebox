/*
 * app_pcf8574.c
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */
#include "main.h"


#define PCF_WADDR1		64
#define PCF_WADDR2		72
#define PCF_WADDR3		76
#define PCF_RADDR1		65
#define PCF_RADDR2		73
#define PCF_RADDR3		77

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




#define SLAVE_ADDRESS_SENSOR 0x4F
#define BEGIN			0
//#define
extern I2C_HandleTypeDef I2cHandle;

uint8_t address1 = 0x20, addres2 = 0x24;
uint8_t flagReceiveCplt=0, flagTransmitCplt=0;
uint8_t sfmReadPowerStateMode = 0;
uint8_t aRxBuffer[16];


uint8_t I2CReceiveBuffer[2];

//uint8_t PCF_Scan(){
//	uint8_t i, count = 0;
//	HAL_StatusTypeDef result ;
//	printf("Scan I2C");
//	for( i = 1; i< 128; i++){
//		result = HAL_I2C_IsDeviceReady(&I2cHandle, (uint16_t) (i<<1), 2, 2);
//		if(result != HAL_OK){
//			printf("."); // No ACK received at that address
//		}
//		else{
//			count ++;
//			printf("0x%X", i);
//
////			return i;
//		}
//	}
//	printf("count: %d", count);
//	return i;
//}


uint8_t PCF_read(uint8_t address){
	if (address <= 3) {
			HAL_I2C_Master_Receive(&I2cHandle, PCF_RADDR1, (uint8_t*) I2CReceiveBuffer, 2, 100);
			return (0x03 & (I2CReceiveBuffer[0] >> (address * 2)));
		} else if (address <= 7) {
			HAL_I2C_Master_Receive(&I2cHandle, PCF_RADDR2, (uint8_t*) I2CReceiveBuffer, 2, 100);
			return (0x03 & (I2CReceiveBuffer[0] >> ((address - 4) * 2)));
		} else if (address <= 9) {
			HAL_I2C_Master_Receive(&I2cHandle, PCF_RADDR3, (uint8_t*) I2CReceiveBuffer, 2, 100);
			return (0x03 & (I2CReceiveBuffer[0] >> ((address - 8) * 2)));
		}
		return 0;
}
void Set_Input_PCF_Pins(void){
	uint8_t initData[1] = {0xff};
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WADDR1, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WADDR2, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WADDR3, (uint8_t *) initData, 1, 0xffff);
}


