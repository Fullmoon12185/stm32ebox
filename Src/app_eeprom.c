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

#define	FIRST_TIME_READING_FLASH				0
#define	FIRST_TIME_READING_FLASH_SIZE			0
#define FIRST_EEPROM_ADDR 						(FIRST_TIME_READING_FLASH_SIZE)
#define PAGE_LENGTH								64
#define EEPROM_BLOCK_SIZE						14

uint8_t strtmp2[] = "                                     ";
uint8_t rxBuffer[1], txBuffer[1];
typedef struct {
		uint8_t outletID;		//remove
		uint8_t status;			//save when have change
		uint32_t energy;		//save when have change and status is working
		uint32_t workingTime;
		uint32_t limitEnergy;	//save at first set command to outlet
}EEPROM_BLOCK;

typedef union EEPROM_BLOCKS_ARRAY{
	EEPROM_BLOCK block_element;
	uint8_t eepromBuffer[EEPROM_BLOCK_SIZE];
}EEPROM_BLOCKS_ARRAY ;

EEPROM_BLOCKS_ARRAY block[NUMBER_OF_RELAYS];

void Eeprom_Initialize(){
	MC25LC512_Initialize();
	for(uint8_t j = 0; j < NUMBER_OF_RELAYS; j ++){
		for(uint8_t i = 0; i< EEPROM_BLOCK_SIZE; i++) {
			block[j].eepromBuffer[i] = 0;
		}
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

void Eeprom_Write_Outlet(uint8_t outletID, uint8_t status, uint32_t energy, uint32_t limitEnergy, uint32_t workingTime) {
	block[outletID].block_element.outletID = outletID;
	block[outletID].block_element.status = status;
	block[outletID].block_element.energy = energy;
	block[outletID].block_element.limitEnergy = limitEnergy;
	block[outletID].block_element.workingTime = workingTime;
//	block[outletID].block_element.checksum = 0;
//	for(uint8_t i = 0; i < EEPROM_BLOCK_SIZE - 1; i ++){
//		block[outletID].block_element.checksum = block[outletID].block_element.checksum ^ block[outletID].eepromBuffer[i];
//	}
	MC25LC512_Write_Bytes(FIRST_EEPROM_ADDR + (outletID*PAGE_LENGTH), block[outletID].eepromBuffer, EEPROM_BLOCK_SIZE);
}



uint8_t Eeprom_Read_Outlet(uint8_t outletID, uint8_t *status, uint32_t *energy, uint32_t *limitEnergy, uint32_t *workingTime) {
//	uint8_t checksum = 0;
	MC25LC512_Read_Bytes(FIRST_EEPROM_ADDR + (outletID*PAGE_LENGTH), block[outletID].eepromBuffer, EEPROM_BLOCK_SIZE);
//	for(uint8_t i = 0; i < EEPROM_BLOCK_SIZE - 1; i ++){
//		checksum = checksum ^ block[outletID].eepromBuffer[i];
//	}
//	uint8_t tempChecksum = block[outletID].block_element.checksum;
//	uint8_t tempoutletID = block[outletID].block_element.outletID;
//	if( tempoutletID == outletID && checksum == tempChecksum)
	{
		*status = block[outletID].block_element.status;
		*energy = block[outletID].block_element.energy;
		*limitEnergy = block[outletID].block_element.limitEnergy;
		*workingTime = block[outletID].block_element.workingTime;
		return 1;
	}

	return 0;
}

void Eeprom_Save(uint8_t outletID){
//	block[outletID].block_element.checksum = 0;
//	for(uint8_t i = 0; i < EEPROM_BLOCK_SIZE - 1; i ++){
//		block[outletID].block_element.checksum = block[outletID].block_element.checksum ^ block[outletID].eepromBuffer[i];
//	}

	MC25LC512_Write_Bytes(FIRST_EEPROM_ADDR + (PAGE_LENGTH * outletID), block[outletID].eepromBuffer, EEPROM_BLOCK_SIZE);
}

void Eeprom_Update_Status(uint8_t outletID, uint8_t status){
	if(block[outletID].block_element.status != status){
		block[outletID].block_element.status = status;
		Eeprom_Save(outletID);
	}
}
void Eeprom_Update_LimitEnergy(uint8_t outletID, uint32_t limitEnergy){
	if(block[outletID].block_element.limitEnergy != limitEnergy){
		block[outletID].block_element.limitEnergy = limitEnergy;
		Eeprom_Save(outletID);
	}
}
void Eeprom_Update_Energy(uint8_t outletID, uint32_t energy, uint32_t workingTime){

//	static uint8_t countUpdateFlash[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//	countUpdateFlash[outletID] = (countUpdateFlash[outletID] + 1)%TIME_INTERVAL_TO_UPDATE_FLASH;
//	if(countUpdateFlash[outletID] == 0){
//		if(block[outletID].block_element.energy != energy){
//			block[outletID].block_element.energy = energy;
//			block[outletID].block_element.workingTime = workingTime;
//			Eeprom_Save(outletID);
//		}
//	}

}

