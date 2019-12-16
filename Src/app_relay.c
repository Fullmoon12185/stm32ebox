/*
 * app_relay.c
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_pcf8574.h"

FlagStatus array_Of_Relay_Statuses[NUMBER_OF_RELAYS];
WorkingStatus array_Of_Relay_Physical_Statuses[NUMBER_OF_RELAYS];
WorkingStatus array_Of_Fuse_Statuses[NUMBER_OF_RELAYS];

uint16_t array_Of_Relay_Pins[NUMBER_OF_RELAYS] = {
		RELAY_PIN_0,
		RELAY_PIN_1,
		RELAY_PIN_2,
		RELAY_PIN_3,
		RELAY_PIN_4,
		RELAY_PIN_5,
		RELAY_PIN_6,
		RELAY_PIN_7,
		RELAY_PIN_8,
		RELAY_PIN_9
};

GPIO_TypeDef * array_Of_Relay_Ports[NUMBER_OF_RELAYS] = {
		RELAY_PORT_0,
		RELAY_PORT_1,
		RELAY_PORT_2,
		RELAY_PORT_3,
		RELAY_PORT_4,
		RELAY_PORT_5,
		RELAY_PORT_6,
		RELAY_PORT_7,
		RELAY_PORT_8,
		RELAY_PORT_9
};


void Relay_Init(void){
	uint8_t i;
	for (i = 0; i < NUMBER_OF_RELAYS; i ++){
		array_Of_Relay_Statuses[i] = RESET;
	}
}
void Set_Relay(uint8_t relayIndex){
	array_Of_Relay_Statuses[relayIndex] = SET;
	HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], SET);
}

void Reset_Relay(uint8_t relayIndex){
	array_Of_Relay_Statuses[relayIndex] = RESET;
	HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], RESET);
}

void Update_Relay_Physical_Status(void){
	uint8_t relayIndex;
	for (relayIndex = 0; relayIndex <  NUMBER_OF_RELAYS; relayIndex ++){
		HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], array_Of_Relay_Statuses[relayIndex]);
	}
}

FlagStatus Get_Relay_Status(uint8_t relayIndex){
	return array_Of_Relay_Statuses[relayIndex];
}


void Check_Physical_Working_Statuses_Of_Relays_Fuses(uint8_t relayIndex){

	uint8_t aclRelay;
	uint8_t aclFuse;
	aclRelay = 2 * relayIndex;
	aclFuse = 2 * relayIndex + 1;
	if(array_Of_Relay_Statuses[relayIndex] == SET){
		if(SAC_Status(aclRelay) == SET && SAC_Status(aclFuse) == SET){
			array_Of_Relay_Physical_Statuses[relayIndex] = NORMAL;
			array_Of_Fuse_Statuses[relayIndex] = NORMAL;
		} else if(SAC_Status(aclRelay) == RESET && SAC_Status(aclFuse) == SET){
			array_Of_Relay_Physical_Statuses[relayIndex] = ABNORMAL;
			array_Of_Fuse_Statuses[relayIndex] = NORMAL;
		} else if(SAC_Status(aclRelay) == SET && SAC_Status(aclFuse) == RESET){
			array_Of_Relay_Physical_Statuses[relayIndex] = NORMAL;
			array_Of_Fuse_Statuses[relayIndex] = ABNORMAL;
		} else {
			array_Of_Relay_Physical_Statuses[relayIndex] = ABNORMAL;
			array_Of_Fuse_Statuses[relayIndex] = ABNORMAL;
		}
	} else {
		if(SAC_Status(aclRelay) == RESET && SAC_Status(aclFuse) == SET){
			array_Of_Relay_Physical_Statuses[relayIndex] = NORMAL;
			array_Of_Fuse_Statuses[relayIndex] = NORMAL;
		} else if(SAC_Status(aclRelay) == SET && SAC_Status(aclFuse) == SET){
			array_Of_Relay_Physical_Statuses[relayIndex] = ABNORMAL;
			array_Of_Fuse_Statuses[relayIndex] = NORMAL;
		} else if(SAC_Status(aclRelay) == RESET && SAC_Status(aclFuse) == RESET){
			array_Of_Relay_Physical_Statuses[relayIndex] = NORMAL;
			array_Of_Fuse_Statuses[relayIndex] = ABNORMAL;
		} else {
			array_Of_Relay_Physical_Statuses[relayIndex] = ABNORMAL;
			array_Of_Fuse_Statuses[relayIndex] = ABNORMAL;
		}
	}
}

void Check_All_Physical_Working_Statuses_Of_Relays_Fuses(void){
	uint8_t relayIndex;
	for(relayIndex = 0; relayIndex < NUMBER_OF_RELAYS; relayIndex ++){
		Check_Physical_Working_Statuses_Of_Relays_Fuses(relayIndex);
	}
}





