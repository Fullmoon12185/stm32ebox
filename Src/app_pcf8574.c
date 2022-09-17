/*
 * app_pcf8574.c
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_uart.h"
#include "app_pcf8574.h"
#include "app_relay.h"


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

//uint8_t sfmReadPowerStateMode = 0;
uint8_t I2CReceiveBuffer[2];
//uint32_t Relay_and_Fuse_Statuses = 0;


uint8_t strpcf[] = "                           ";

typedef union {
	uint8_t bytePCFData[4];
	uint16_t wordPCFData[2];
	uint32_t Relay_and_Fuse_Statuses;
}Union_PCF_Data;

Union_PCF_Data pcfData;

void Set_Input_PCF_Pins(void);




void PCF_Init(void){
	Set_Input_PCF_Pins();
	for(uint8_t i = 0; i < 4; i++){
		PCF_read();
		HAL_Delay(100);
	}

	sprintf((char*) strpcf, "boxID = %d\r\n", (int) Get_Box_ID());
	UART3_SendToHost((uint8_t *)strpcf);
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
			sprintf((char*) strpcf, "%d\t", (int) i<<1);
			UART3_SendToHost((uint8_t *)strpcf);
			uint8_t initData[1] = {0xFF};
			HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)(i<<1), (uint8_t *) initData, 1, 0xffff);

		}
	}
	sprintf((char*) strpcf, "count: %d", count);
	UART3_SendToHost((uint8_t*)strpcf);
	return i;
}

uint32_t Get_All_Relay_Fuse_Statuses(void){
	return pcfData.Relay_and_Fuse_Statuses;
}

uint16_t Get_Box_ID(void){
#if(VERSION_EBOX == 2  || VERSION_EBOX == 3 || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A || VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
	uint8_t tempHi = pcfData.bytePCFData[2] & 0xf0;
	uint8_t tempHiReversed = 0, temp;
	for (uint8_t i = 0; i < 8; i++)
	{
		temp = (tempHi & (1 << i));
		if(temp)
			tempHiReversed |= (1 << ((8 - 1) - i));
	}

	return (uint16_t)((tempHiReversed & 0x0f) << 8) | pcfData.bytePCFData[3];
#else
	return 5;
#endif
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
//		sprintf((char*) strpcf, "%08X\r\n", (int) pcfData.Relay_and_Fuse_Statuses);
//		UART3_SendToHost((uint8_t *)strpcf);
		break;
	default:
		pcfReadState = 0;
		break;
	}


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
	HAL_Delay(100);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_2, (uint8_t *) initData, 1, 0xffff);
	HAL_Delay(100);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_3, (uint8_t *) initData, 1, 0xffff);
	HAL_Delay(100);
	HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)PCF_WRITE_ADDRESS_4, (uint8_t *) initData, 1, 0xffff);
	HAL_Delay(100);

}


uint8_t isNoFuseAvailable(uint8_t outletId){
	uint32_t tempRelayFuseStatuses = Get_All_Relay_Fuse_Statuses();
	return ((tempRelayFuseStatuses >> (outletId*2) & 0x00000002) > 0);
}

uint8_t isRelayOff(uint8_t outletId){
	uint32_t tempRelayFuseStatuses = Get_All_Relay_Fuse_Statuses();
	return ((tempRelayFuseStatuses >> (outletId*2) & 0x00000001) > 0);
}

uint8_t isRelayOn(uint8_t outletId){
	uint32_t tempRelayFuseStatuses = Get_All_Relay_Fuse_Statuses();
	return ((tempRelayFuseStatuses >> (outletId*2) & 0x00000001) == 0);
}

