/*
 * app_relay.c
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_pcf8574.h"
#include "app_scheduler.h"

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


static uint32_t relay_TimeoutFlag_Index = NO_TASK_ID;
uint8_t set_Relay_TimeoutFlag = 1;

void Clear_Relay_Timeout_Flag(void){
	set_Relay_TimeoutFlag = 0;
}
void Set_Relay_Timeout_Flag(void){
	set_Relay_TimeoutFlag = 1;
}
uint8_t is_Set_Relay_Timeout(void){
	return set_Relay_TimeoutFlag;
}

static void Relay_Output_Control_Enable(void){
#if(VERSION_EBOX == 2 || VERSION_EBOX == 3  || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A )
	HAL_GPIO_WritePin(PD2_RELAY_ENABLE_PORT, PD2_RELAY_ENABLE_PIN, RESET);
#elif(VERSION_EBOX == 15)
	HAL_GPIO_WritePin(PD2_RELAY_ENABLE_PORT, PD2_RELAY_ENABLE_PIN, RESET);
#endif
}

void Relay_Init(void){
	uint8_t i;
	for (i = 0; i < NUMBER_OF_RELAYS; i ++){
		array_Of_Relay_Statuses[i] = RESET;
	}
	Update_Relay_Physical_Status();
	set_Relay_TimeoutFlag = 1;
	Relay_Output_Control_Enable();
}

void Set_Relay1(uint8_t relayIndex){
	if(relayIndex > 9) return;
	if(array_Of_Relay_Statuses[relayIndex] == RESET){
		isUpdateRelayStatus = SET;
	}
	array_Of_Relay_Statuses[relayIndex] = SET;
	HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], SET);

}
void Set_Relay(uint8_t relayIndex){
	if(relayIndex > 9) return;
	if(array_Of_Relay_Statuses[relayIndex] == RESET){
		isUpdateRelayStatus = SET;
	}
	array_Of_Relay_Statuses[relayIndex] = SET;
	HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], SET);

	SCH_Delete_Task(relay_TimeoutFlag_Index);
	Clear_Relay_Timeout_Flag();
	relay_TimeoutFlag_Index = SCH_Add_Task(Set_Relay_Timeout_Flag, 100, 0);
}

void Reset_Relay(uint8_t relayIndex){
	if(relayIndex > 9) return;
	if(array_Of_Relay_Statuses[relayIndex] == SET){
		isUpdateRelayStatus = SET;
	}
	array_Of_Relay_Statuses[relayIndex] = RESET;
	HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], RESET);

	SCH_Delete_Task(relay_TimeoutFlag_Index);
	Clear_Relay_Timeout_Flag();
	relay_TimeoutFlag_Index = SCH_Add_Task(Set_Relay_Timeout_Flag, 100, 0);
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
