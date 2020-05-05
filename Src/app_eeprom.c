/*
 * app_eeprom.c
 *
 *  Created on: 24 Mar 2020
 *      Author: phamvanvinh
 */


#include "app_eeprom.h"
#include "app_25LC512.h"
#include "app_uart.h"

#define		TIME_INTERVAL_TO_UPDATE_FLASH		1 //IN SECOND

#define	FIRST_TIME_READING_FLASH				10
#define	FIRST_TIME_READING_FLASH_SIZE			1

#define	PAGE_0									0
#define PAGE_1									64
#define PAGE_LENGTH								64

#define EEPROM_BLOCK_SIZE						17

#define	EEPROM_MAIN_OUTLET_ENERGY_ADDRESS						(PAGE_0 + 0)
#define	EEPROM_MAIN_OUTLET_ENERGY_SIZE							5


#define	EEPROM_OUTLET_STATUS_ADDRESS					(PAGE_1 + 0)
#define	EEPROM_OUTLET_STATUS_SIZE						2
#define	EEPROM_OUTLET_ENERGY_ADDRESS					(EEPROM_OUTLET_STATUS_ADDRESS + EEPROM_OUTLET_STATUS_SIZE)
#define	EEPROM_OUTLET_ENERGY_SIZE						5
#define	EEPROM_OUTLET_LIMIT_ENERGY_ADDRESS				(EEPROM_OUTLET_ENERGY_ADDRESS + EEPROM_OUTLET_ENERGY_SIZE)
#define	EEPROM_OUTLET_LIMIT_ENERGY_SIZE 				5
#define	EEPROM_OUTLET_WORKING_TIME_ADDRESS				(EEPROM_OUTLET_LIMIT_ENERGY_ADDRESS + EEPROM_OUTLET_LIMIT_ENERGY_SIZE)
#define	EEPROM_OUTLET_WORKING_TIME_SIZE 				5


uint8_t strtmp2[] = "                                     ";
typedef struct {
		uint8_t status;			//save when have change
		uint8_t cksumStatus;
		uint32_t energy;		//save when have change and status is working
		uint8_t cksumEnengy;
		uint32_t limitEnergy;	//save at first set command to outlet
		uint8_t cksumLimitEnergy;
		uint32_t workingTime;
		uint8_t cksumWorkingTime;
}EEPROM_BLOCK;

typedef union EEPROM_BLOCKS_ARRAY{
	EEPROM_BLOCK block_element;
	uint8_t eepromBuffer[EEPROM_BLOCK_SIZE];
}EEPROM_BLOCKS_ARRAY ;

EEPROM_BLOCKS_ARRAY block[NUMBER_OF_RELAYS];


uint32_t Eeprom_Get_LimitEnergy(uint8_t outletID);
uint8_t Eeprom_Get_Status(uint8_t outletID);
uint32_t Eeprom_Get_Energy(uint8_t outletID);


void Eeprom_Initialize(){
	MC25LC512_Initialize();
	for(uint8_t j = 0; j < NUMBER_OF_RELAYS; j ++){
		for(uint8_t i = 0; i< EEPROM_BLOCK_SIZE; i++) {
			block[j].eepromBuffer[i] = 0;
		}
	}
}


void Setup_Eeprom(void){
	for(uint8_t i = 0; i < NUMBER_OF_RELAYS; i ++){
		Eeprom_Update_Status(i, 0);
		HAL_Delay(100);
		Eeprom_Update_LimitEnergy(i, 0xffffffff);
		HAL_Delay(100);
		Eeprom_Update_Energy(i, 1);
		HAL_Delay(100);
		Eeprom_Update_WorkingTime(i, 0);
		HAL_Delay(100);
		Eeprom_Update_Main_Energy(0);
		HAL_Delay(100);
	}
}
uint8_t Read_First_Byte(void){
	uint8_t tempBuffer[2];
	MC25LC512_Read_Bytes(FIRST_TIME_READING_FLASH, tempBuffer, FIRST_TIME_READING_FLASH_SIZE);
	return tempBuffer[1];
}
void Write_First_Byte(uint8_t value){
	uint8_t tempBuffer[2];
	tempBuffer[0] = value;
	tempBuffer[1] = value;
	MC25LC512_Write_Bytes(FIRST_TIME_READING_FLASH, tempBuffer, FIRST_TIME_READING_FLASH_SIZE);
}





uint8_t Eeprom_Read_Outlet(uint8_t outletID, uint8_t *status, uint32_t *energy, uint32_t *limitEnergy, uint32_t *workingTime) {
//	MC25LC512_Read_Bytes((outletID* PAGE_LENGTH) + EEPROM_OUTLET_STATUS_ADDRESS, block[outletID].eepromBuffer, EEPROM_BLOCK_SIZE);
	*status = Eeprom_Get_Status(outletID);
	*energy = Eeprom_Get_Energy(outletID);
	*limitEnergy = Eeprom_Get_LimitEnergy(outletID);
	*workingTime = 0;
	block[outletID].block_element.status = *status;
	block[outletID].block_element.energy = *energy;
	block[outletID].block_element.limitEnergy = *limitEnergy;
	if(*status > 2){
		return 0;
	} else {
		return 1;
	}
}

uint8_t Eeprom_Get_Status(uint8_t outletID){
		uint8_t tempBuffer[2];
		MC25LC512_Read_Bytes((outletID* PAGE_LENGTH) + EEPROM_OUTLET_STATUS_ADDRESS, tempBuffer, EEPROM_OUTLET_STATUS_SIZE);
		if(tempBuffer[0] == (tempBuffer[1] ^ 0xff)){
			return tempBuffer[0];
		}
		return 0;
}
void Eeprom_Update_Status(uint8_t outletID, uint8_t status){
	if(block[outletID].block_element.status != status)
	{
		sprintf((char*) strtmp2, "status=%d\r\n", (int) status);
		UART3_SendToHost((uint8_t *)strtmp2);

		block[outletID].block_element.status = status;
		uint8_t tempBuffer[2];
		tempBuffer[0] = status;
		tempBuffer[1] = status^0xff;
		MC25LC512_Write_Bytes((outletID* PAGE_LENGTH) + EEPROM_OUTLET_STATUS_ADDRESS, tempBuffer, EEPROM_OUTLET_STATUS_SIZE);
	}
}
uint32_t Eeprom_Get_LimitEnergy(uint8_t outletID){
	uint8_t tempBuffer[5];
	uint32_t tempLE = 0xffffffff;
	MC25LC512_Read_Bytes((outletID* PAGE_LENGTH) + EEPROM_OUTLET_LIMIT_ENERGY_ADDRESS, tempBuffer, EEPROM_OUTLET_LIMIT_ENERGY_SIZE);
	if(tempBuffer[4] == (tempBuffer[0] ^ tempBuffer[1] ^ tempBuffer[2] ^ tempBuffer[3])){
		tempLE = (uint32_t)tempBuffer[0];
		tempLE |= (uint32_t)(tempBuffer[1] << 8);
		tempLE |= (uint32_t)(tempBuffer[2] << 16);
		tempLE |= (uint32_t)(tempBuffer[3] << 24);
	}
	return tempLE;
}
void Eeprom_Update_LimitEnergy(uint8_t outletID, uint32_t limitEnergy){
	if(block[outletID].block_element.limitEnergy != limitEnergy){
		block[outletID].block_element.limitEnergy = limitEnergy;
		uint8_t tempBuffer[5];
		tempBuffer[0] = (uint8_t)(limitEnergy & 0xff);
		tempBuffer[1] = (uint8_t)(limitEnergy>>8 & 0xff);
		tempBuffer[2] = (uint8_t)(limitEnergy>>16 & 0xff);
		tempBuffer[3] = (uint8_t)(limitEnergy>>24 & 0xff);
		tempBuffer[4] = tempBuffer[0] ^ tempBuffer[1] ^ tempBuffer[2] ^ tempBuffer[3];
		MC25LC512_Write_Bytes((outletID* PAGE_LENGTH) + EEPROM_OUTLET_LIMIT_ENERGY_ADDRESS, tempBuffer, EEPROM_OUTLET_LIMIT_ENERGY_SIZE);
	}
}

uint32_t Eeprom_Get_Energy(uint8_t outletID){
		uint8_t tempBuffer[5];
		uint32_t tempE = 0;
		MC25LC512_Read_Bytes((outletID* PAGE_LENGTH) + EEPROM_OUTLET_ENERGY_ADDRESS, tempBuffer, EEPROM_OUTLET_ENERGY_SIZE);
		if(tempBuffer[4] == (tempBuffer[0] ^ tempBuffer[1] ^ tempBuffer[2] ^ tempBuffer[3])){
			tempE = (uint32_t)tempBuffer[0];
			tempE |= (uint32_t)(tempBuffer[1] << 8);
			tempE |= (uint32_t)(tempBuffer[2] << 16);
			tempE |= (uint32_t)(tempBuffer[3] << 24);
		}
		return tempE;
}
void Eeprom_Update_Energy(uint8_t outletID, uint32_t energy){

	if(block[outletID].block_element.energy != energy){
		block[outletID].block_element.energy = energy;
		uint8_t tempBuffer[5];
		tempBuffer[0] = (uint8_t)(energy & 0xff);
		tempBuffer[1] = (uint8_t)(energy>>8 & 0xff);
		tempBuffer[2] = (uint8_t)(energy>>16 & 0xff);
		tempBuffer[3] = (uint8_t)(energy>>24 & 0xff);
		tempBuffer[4] = tempBuffer[0] ^ tempBuffer[1] ^ tempBuffer[2] ^ tempBuffer[3];
		MC25LC512_Write_Bytes((outletID* PAGE_LENGTH) + EEPROM_OUTLET_ENERGY_ADDRESS, tempBuffer, EEPROM_OUTLET_ENERGY_SIZE);
	}

}

void Eeprom_Update_WorkingTime(uint8_t outletID, uint32_t workingTime){

	if(block[outletID].block_element.workingTime != workingTime){
		block[outletID].block_element.workingTime = workingTime;
		uint8_t tempBuffer[5];
		tempBuffer[0] = (uint8_t)(workingTime & 0xff);
		tempBuffer[1] = (uint8_t)(workingTime>>8 & 0xff);
		tempBuffer[2] = (uint8_t)(workingTime>>16 & 0xff);
		tempBuffer[3] = (uint8_t)(workingTime>>24 & 0xff);
		tempBuffer[4] = tempBuffer[0] ^ tempBuffer[1] ^ tempBuffer[2] ^ tempBuffer[3];
		MC25LC512_Write_Bytes((outletID* PAGE_LENGTH) + EEPROM_OUTLET_WORKING_TIME_ADDRESS, tempBuffer, EEPROM_OUTLET_WORKING_TIME_SIZE);
	}

}

uint32_t Eeprom_Get_Main_Energy(void){
	uint8_t tempBuffer[5];
	uint32_t mainEnergy = 0;
	MC25LC512_Read_Bytes(EEPROM_MAIN_OUTLET_ENERGY_ADDRESS, tempBuffer, EEPROM_MAIN_OUTLET_ENERGY_SIZE);
	if(tempBuffer[4] == (tempBuffer[0] ^ tempBuffer[1] ^ tempBuffer[2] ^ tempBuffer[3])){
		mainEnergy = (uint32_t)tempBuffer[0];
		mainEnergy |= (uint32_t)(tempBuffer[1] << 8);
		mainEnergy |= (uint32_t)(tempBuffer[2] << 16);
		mainEnergy |= (uint32_t)(tempBuffer[3] << 24);
	}
	return mainEnergy;
}

void Eeprom_Update_Main_Energy(uint32_t main_energy){
	uint8_t tempBuffer[5];
	static uint8_t countForUpdateMainEnergy = 0;
	countForUpdateMainEnergy = (countForUpdateMainEnergy + 1)%10;
	if(countForUpdateMainEnergy == 0){
		tempBuffer[0] = (uint8_t)(main_energy & 0xff);
		tempBuffer[1] = (uint8_t)(main_energy>>8 & 0xff);
		tempBuffer[2] = (uint8_t)(main_energy>>16 & 0xff);
		tempBuffer[3] = (uint8_t)(main_energy>>24 & 0xff);
		tempBuffer[4] = tempBuffer[0] ^ tempBuffer[1] ^ tempBuffer[2] ^ tempBuffer[3];
		MC25LC512_Write_Bytes(EEPROM_MAIN_OUTLET_ENERGY_ADDRESS, tempBuffer, EEPROM_MAIN_OUTLET_ENERGY_SIZE);
	}

}
