/*
 * app_eeprom.c
 *
 *  Created on: 24 Mar 2020
 *      Author: phamvanvinh
 */


#include "app_eeprom.h"
#include "app_25LC512.h"
#include "string.h"
#include "stdio.h"
#define FIRST_EEPROM_ADDR 0x0000
#define EEPROM_BLOCK_SIZE	10

uint8_t strtmp2[] = "                                     ";
uint8_t rxBuffer[1], txBuffer[1];
union EEPROM_BLOCKS_ARRAY{
	struct EEPROM_BLOCK{
		uint8_t channelID;		//remove
		uint8_t status;			//save when have change
		uint32_t energy;		//save when have change and status is working
	//	uint32_t altEnergy;		//alternative storage for energy

		uint32_t limitEnergy;	//save at first set command to outlet
	}block_element;
	uint8_t eepromBuffer[EEPROM_BLOCK_SIZE];
}block ;

void eeprom_initilize(){
	MC25LC512_Initilize();
	for(uint8_t i = 0; i< EEPROM_BLOCK_SIZE; i++)	block.eepromBuffer[i] = 0;
}

void eeprom_write_outlet(uint8_t outletIndex, uint8_t status, uint32_t energy, uint32_t limitEnergy){
	block.block_element.channelID = outletIndex;
	block.block_element.status = status;
	block.block_element.energy = energy;
	block.block_element.limitEnergy = limitEnergy;
	MC25LC512_Write_Bytes(FIRST_EEPROM_ADDR + (EEPROM_BLOCK_SIZE * outletIndex), block.eepromBuffer, EEPROM_BLOCK_SIZE);
}


void eeprom_read_outlet(uint8_t *outletIndex, uint8_t *status, uint32_t *energy, uint32_t *limitEnergy){
	MC25LC512_Read_Bytes(FIRST_EEPROM_ADDR + (EEPROM_BLOCK_SIZE * *outletIndex), block.eepromBuffer, EEPROM_BLOCK_SIZE);
	* status = block.block_element.status;
	* energy = block.block_element.energy;
	* limitEnergy = block.block_element.limitEnergy;
}


