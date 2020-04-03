/*
 * app_eeprom.c
 *
 *  Created on: 24 Mar 2020
 *      Author: phamvanvinh
 */


#include "app_eeprom.h"
#include "app_25LC512.h"

#define	FIRST_TIME_READING_FLASH				0
#define	FIRST_TIME_READING_FLASH_SIZE			1
#define FIRST_EEPROM_ADDR 						(FIRST_TIME_READING_FLASH_SIZE)
#define EEPROM_BLOCK_SIZE						11

uint8_t strtmp2[] = "                                     ";
uint8_t rxBuffer[1], txBuffer[1];
union EEPROM_BLOCKS_ARRAY{
	struct EEPROM_BLOCK{
		uint8_t outletID;		//remove
		uint8_t status;			//save when have change
		uint32_t energy;		//save when have change and status is working
	//	uint32_t altEnergy;		//alternative storage for energy

		uint32_t limitEnergy;	//save at first set command to outlet
		uint8_t checksum;
	}block_element;
	uint8_t eepromBuffer[EEPROM_BLOCK_SIZE];
}block[NUMBER_OF_RELAYS] ;

void Eeprom_Initialize(){
	MC25LC512_Initialize();
	for(uint8_t j = 0; j < NUMBER_OF_RELAYS; j ++){
		for(uint8_t i = 0; i< EEPROM_BLOCK_SIZE; i++) {
			block[j].eepromBuffer[i] = 0;
		}
	}
}

uint8_t Read_First_Byte(void){
	uint8_t tempBuffer[1];
	MC25LC512_Read_Bytes(FIRST_TIME_READING_FLASH, tempBuffer, FIRST_TIME_READING_FLASH_SIZE);
	return tempBuffer[0];
}
void Write_First_Byte(uint8_t value){
	uint8_t tempBuffer[1];
	tempBuffer[0] = value;
	MC25LC512_Write_Bytes(FIRST_TIME_READING_FLASH, tempBuffer, FIRST_TIME_READING_FLASH_SIZE);
}

void Eeprom_Write_Outlet(uint8_t outletID, uint8_t status, uint32_t energy, uint32_t limitEnergy) {
	block[outletID].block_element.outletID = outletID;
	block[outletID].block_element.status = status;
	block[outletID].block_element.energy = energy;
	block[outletID].block_element.limitEnergy = limitEnergy;
	block[outletID].block_element.checksum = 0;
	for(uint8_t i = 0; i < EEPROM_BLOCK_SIZE - 1; i ++){
		block[outletID].block_element.checksum = block[outletID].block_element.checksum ^ block[outletID].eepromBuffer[i];
	}
	MC25LC512_Write_Bytes(FIRST_EEPROM_ADDR + (EEPROM_BLOCK_SIZE * outletID), block[outletID].eepromBuffer, EEPROM_BLOCK_SIZE);
}


uint8_t Eeprom_Read_Outlet(uint8_t outletID, uint8_t *status, uint32_t *energy, uint32_t *limitEnergy) {
	uint8_t checksum = 0;
	MC25LC512_Read_Bytes(FIRST_EEPROM_ADDR + (EEPROM_BLOCK_SIZE * outletID), block[outletID].eepromBuffer, EEPROM_BLOCK_SIZE);
	for(uint8_t i = 0; i < EEPROM_BLOCK_SIZE - 1; i ++){
		checksum = checksum ^ block[outletID].eepromBuffer[i];
	}
	if(outletID == block[outletID].block_element.outletID
			&& checksum == block[outletID].block_element.checksum){

		*status = block[outletID].block_element.status;
		*energy = block[outletID].block_element.energy;
		*limitEnergy = block[outletID].block_element.limitEnergy;
		return 1;
	}
	return 0;
}

void Eeprom_Save(uint8_t outletID){
	block[outletID].block_element.checksum = 0;
	for(uint8_t i = 0; i < EEPROM_BLOCK_SIZE - 1; i ++){
		block[outletID].block_element.checksum = block[outletID].block_element.checksum ^ block[outletID].eepromBuffer[i];
	}
	MC25LC512_Write_Bytes(FIRST_EEPROM_ADDR + (EEPROM_BLOCK_SIZE * outletID), block[outletID].eepromBuffer, EEPROM_BLOCK_SIZE);
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
void Eeprom_Update_Energy(uint8_t outletID, uint32_t energy){
	if(block[outletID].block_element.energy != energy){
		block[outletID].block_element.energy = energy;
		Eeprom_Save(outletID);
	}
}

