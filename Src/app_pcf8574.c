/*
 * app_pcf8574.c
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */
#include "main.h"


#define PCF_WADR1		64
#define PCF_WADR2		72
#define PCF_WADR3		76
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

//void PCF_init(){
////scan for address1 and 2
////	address1 = PCF_Scan();
////	HAL_I2C_Master_Receive_DMA(&I2cHandle, address1, (uint8_t *) aRxBuffer, 16);
//	uint8_t initData[1] = { 0xff };
//		HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t) PCF_WADR1, (uint8_t*) initData, 1, 10);
//		HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t) PCF_WADR2, (uint8_t*) initData, 1, 10);
//		HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t) PCF_WADR3, (uint8_t*) initData, 1, 10);
//}


uint8_t PCF_read(uint8_t address){
//	HAL_I2C_Master_Receive(&I2cHandle, PCF_RADDR1, (uint8_t*) I2CReceiveBuffer, 2, 100);
//				return ( I2CReceiveBuffer[0] );
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
//void PCF_write(uint8_t);

//void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
//{
//  /* Toggle LED2: Transfer in reception process is correct */
////  BSP_LED_Toggle(LED2);
//	flagReceiveCplt = 1;
//	HAL_I2C_Master_Receive_DMA(&I2cHandle, address1, (uint8_t *) aRxBuffer, 16);
//}
//
//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * I2cHandle){
//	flagTransmitCplt = 1;
//}
//
//void FSM_ReadPowerState(){	//call in main loop(1)
//
//
//}
//
void Set_Input_PCF_Pins(void){
	uint8_t initData[1] = {0xff};
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)64, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)72, (uint8_t *) initData, 1, 0xffff);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)76, (uint8_t *) initData, 1, 0xffff);
//	//	HAL_I2C_Master_Transmit(hi2c, DevAddress, pData, Size, Timeout)/
//
//		for(uint16_t i = 0; i< 100; i++){
//			while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
//			while(HAL_I2C_Master_Receive(&hi2c1, (uint16_t) 65, (uint8_t *)receiveI2C, 2, 100) != HAL_OK );
//			storeBuffer[i] = receiveI2C[0];
//	//		printBuffer ="";
//			sprintf(printBuffer,"\r%d  ", receiveI2C[0]);
//					HAL_UART_Transmit(&huart3, printBuffer, 4, 100);
//			HAL_Delay(1);
//		}
}


