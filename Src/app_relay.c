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
FlagStatus isUpdateRelayStatus = RESET;

void Update_Relay_Physical_Status(void);

uint16_t array_Of_Relay_Pins[NUMBER_OF_RELAYS] = {
		RELAY_PIN_6,
		RELAY_PIN_5,
		RELAY_PIN_4,
		RELAY_PIN_3,
		RELAY_PIN_2,
		RELAY_PIN_1,
		RELAY_PIN_0,
		RELAY_PIN_9,
		RELAY_PIN_8,
		RELAY_PIN_7
};

GPIO_TypeDef * array_Of_Relay_Ports[NUMBER_OF_RELAYS] = {
		RELAY_PORT_6,
		RELAY_PORT_5,
		RELAY_PORT_4,
		RELAY_PORT_3,
		RELAY_PORT_2,
		RELAY_PORT_1,
		RELAY_PORT_0,
		RELAY_PORT_9,
		RELAY_PORT_8,
		RELAY_PORT_7
};


void Relay_Init(void){
	uint8_t i;
	for (i = 0; i < NUMBER_OF_RELAYS; i ++){
		array_Of_Relay_Statuses[i] = RESET;
	}
	Update_Relay_Physical_Status();
}
void Set_Relay(uint8_t relayIndex){
	if(relayIndex > 9) return;
	if(array_Of_Relay_Statuses[relayIndex] == RESET){
		isUpdateRelayStatus = SET;
	}
	array_Of_Relay_Statuses[relayIndex] = SET;
	HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], SET);
}

void Reset_Relay(uint8_t relayIndex){
	if(relayIndex > 9) return;
	if(array_Of_Relay_Statuses[relayIndex] == SET){
		isUpdateRelayStatus = SET;
	}
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
FlagStatus Get_Is_Update_Relay_Status(void){
	if(isUpdateRelayStatus){
		isUpdateRelayStatus = RESET;
		return SET;
	} else {
		return RESET;
	}
}
