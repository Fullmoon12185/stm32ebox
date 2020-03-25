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

#define PCF_WRITE_ADDRESS_4		78

#define PCF_READ_ADDRESS_1		65
#define PCF_READ_ADDRESS_2		73
#define PCF_READ_ADDRESS_3		77
#define PCF_READ_ADDRESS_4		79

//#define
extern I2C_HandleTypeDef I2cHandle;

uint8_t sfmReadPowerStateMode = 0;
uint8_t I2CReceiveBuffer[2];
//uint32_t Relay_and_Fuse_Statuses = 0;


uint8_t strpcf[] = "                           ";

typedef union {
	uint8_t bytePCFData[4];
	uint32_t Relay_and_Fuse_Statuses;
}Union_PCF_Data;

Union_PCF_Data pcfData;

void Set_Input_PCF_Pins(void);





void PCF_Init(void){
	Set_Input_PCF_Pins();
}

uint8_t PCF_Scan(){
	uint8_t i, count = 0;
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
	return pcfData.Relay_and_Fuse_Statuses;
}

void PCF_read(void){
	static uint8_t pcfReadState = 0;
	switch(pcfReadState){
	case 0:
		HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_1, (uint8_t*) I2CReceiveBuffer, 2, 0xffff);
		pcfData.bytePCFData[pcfReadState] = I2CReceiveBuffer[0];
		pcfReadState = 1;
		break;
	case 1:
		HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_2, (uint8_t*) I2CReceiveBuffer, 2, 0xffff);
		pcfData.bytePCFData[pcfReadState] = I2CReceiveBuffer[0];
		pcfReadState = 2;
		break;
	case 2:
		HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_3, (uint8_t*) I2CReceiveBuffer, 2, 0xffff);
		pcfData.bytePCFData[pcfReadState] = I2CReceiveBuffer[0];
		pcfReadState = 3;
		break;
	case 3:
		HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_4, (uint8_t*) I2CReceiveBuffer, 2, 0xffff);
		pcfData.bytePCFData[pcfReadState] = I2CReceiveBuffer[0];
		pcfReadState = 0;
		break;
	default:
		pcfReadState = 0;
		break;
	}
	sprintf((char*) strpcf, "%08X\r\n", (int) pcfData.Relay_and_Fuse_Statuses);
	UART3_SendToHost((uint8_t *)strpcf);

}



uint8_t PCF_read1(uint8_t address){
//	HAL_I2C_Master_Receive(&I2cHandle, PCF_RADDR1, (uint8_t*) I2CReceiveBuffer, 2, 100);
//				return ( I2CReceiveBuffer[0] );
	if (address <= 3) {
			HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_1, (uint8_t*) I2CReceiveBuffer, 2, 100);
			return (0x03 & (I2CReceiveBuffer[0] >> (address * 2)));
		} else if (address <= 7) {
			HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_2, (uint8_t*) I2CReceiveBuffer, 2, 100);
			return (0x03 & (I2CReceiveBuffer[0] >> ((address - 4) * 2)));
		} else if (address <= 9) {
			HAL_I2C_Master_Receive(&I2cHandle, PCF_READ_ADDRESS_3, (uint8_t*) I2CReceiveBuffer, 2, 100);
			return (0x03 & (I2CReceiveBuffer[0] >> ((address - 8) * 2)));
		}
		return 0;
}

void Set_Input_PCF_Pins(void){
	uint8_t initData[1] = {0xff};
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_1, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_2, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_3, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_4, (uint8_t *) initData, 1, 0xffff);
}


