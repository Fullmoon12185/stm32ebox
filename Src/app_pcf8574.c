/*
 * app_pcf8574.c
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_uart.h"

#define PCF_WRITE_ADDRESS_1		64
#define PCF_WRITE_ADDRESS_2		72
#define PCF_WRITE_ADDRESS_3		76
#define PCF_READ_ADDRESS_1		65
#define PCF_READ_ADDRESS_2		73
#define PCF_READ_ADDRESS_3		77

//#define
extern I2C_HandleTypeDef I2cHandle;

uint8_t sfmReadPowerStateMode = 0;
uint8_t I2CReceiveBuffer[2];
uint32_t Relay_and_Fuse_Statuses = 0;


void Set_Input_PCF_Pins(void);





void PCF_Init(void){
	Relay_and_Fuse_Statuses = 0;
	Set_Input_PCF_Pins();
}

uint8_t PCF_Scan(){
	uint8_t i, count = 0;
	uint8_t strpcf[] = "                           ";
	HAL_StatusTypeDef result ;
	UART3_SendToHost((uint8_t*)"Scan I2C");
	for( i = 1; i< 128; i++){
		result = HAL_I2C_IsDeviceReady(&I2cHandle, (uint16_t) (i<<1), 2, 2);
		if(result != HAL_OK){
			UART3_SendToHost((uint8_t*)"."); // No ACK received at that address
		}
		else{
			count ++;
			sprintf((char*) strpcf, "%d\t", (int) i);
			UART3_SendToHost((uint8_t *)strpcf);
			return i;
		}
	}
	sprintf((char*) strpcf, "count: %d", count);
	UART3_SendToHost((uint8_t*)strpcf);
	return i;
}

uint32_t Get_All_Relay_Fuse_Statuses(void){
	return Relay_and_Fuse_Statuses;
}

void PCF_read(void){
	static uint8_t pcfReadState = 0;
	switch(pcfReadState){
	case 0:
		HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_1, (uint8_t*) I2CReceiveBuffer, 2, 100);
		Relay_and_Fuse_Statuses = Relay_and_Fuse_Statuses | I2CReceiveBuffer[0];
		pcfReadState = 1;
		break;
	case 1:
		HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_2, (uint8_t*) I2CReceiveBuffer, 2, 100);
		Relay_and_Fuse_Statuses = Relay_and_Fuse_Statuses | ((uint32_t)I2CReceiveBuffer[0] << 8);
		pcfReadState = 2;
		break;
	case 2:
		HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_3, (uint8_t*) I2CReceiveBuffer, 2, 100);
		Relay_and_Fuse_Statuses = Relay_and_Fuse_Statuses | ((uint32_t)I2CReceiveBuffer[0] << 16);
		pcfReadState = 0;
		break;
	default:
		pcfReadState = 0;
		break;
	}
}


void Set_Input_PCF_Pins(void){
	uint8_t initData[1] = {0xff};
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_1, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_2, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_3, (uint8_t *) initData, 1, 0xffff);
}


