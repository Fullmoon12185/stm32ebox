/*
 * app_relay.c
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_pcf8574.h"
#include "app_scheduler.h"
#include "app_relay.h"


FlagStatus array_Of_Relay_Statuses[NUMBER_OF_RELAYS];
WorkingStatus array_Of_Relay_Physical_Statuses[NUMBER_OF_RELAYS];
WorkingStatus array_Of_Fuse_Statuses[NUMBER_OF_RELAYS];
FlagStatus isUpdateRelayStatus = RESET;

void Update_Relay_Physical_Status(void);
#if(VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
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
#elif(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
uint16_t array_Of_Relay_Pins[NUMBER_OF_RELAYS] = {
		RELAY_PIN_0,
		RELAY_PIN_1,
		RELAY_PIN_2,
		RELAY_PIN_3,
		RELAY_PIN_4,
		RELAY_PIN_5,
		RELAY_PIN_6,
		RELAY_PIN_7
};

GPIO_TypeDef * array_Of_Relay_Ports[NUMBER_OF_RELAYS] = {
		RELAY_PORT_0,
		RELAY_PORT_1,
		RELAY_PORT_2,
		RELAY_PORT_3,
		RELAY_PORT_4,
		RELAY_PORT_5,
		RELAY_PORT_6,
		RELAY_PORT_7
};
#else
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

#endif
static uint32_t relay_TimeoutFlag_Index = NO_TASK_ID;
uint8_t set_Relay_TimeoutFlag = 1;

uint8_t latestRelay[NUMBER_OF_RELAYS];
uint8_t latestRelayIndex = 0;
static uint8_t timeoutForResetLatestRelayBuffer = 0;

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
#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	HAL_GPIO_WritePin(PD2_RELAY_ENABLE_PORT, PD2_RELAY_ENABLE_PIN, RESET);
#elif(VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
	HAL_GPIO_WritePin(PD2_RELAY_ENABLE_PORT, PD2_RELAY_ENABLE_PIN, RESET);
#elif(VERSION_EBOX == 2 || VERSION_EBOX == 3  || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A )
	HAL_GPIO_WritePin(PD2_RELAY_ENABLE_PORT, PD2_RELAY_ENABLE_PIN, RESET);
#elif(VERSION_EBOX == 15)
	HAL_GPIO_WritePin(PD2_RELAY_ENABLE_PORT, PD2_RELAY_ENABLE_PIN, RESET);
#endif
}

void Relay_Init(void){
	uint8_t i;
	for (i = 0; i < NUMBER_OF_RELAYS; i ++){
		array_Of_Relay_Statuses[i] = RESET;
		latestRelay[i] = NUMBER_OF_RELAYS;
	}
	Update_Relay_Physical_Status();
	set_Relay_TimeoutFlag = 1;
	Relay_Output_Control_Enable();
}

void Set_All(void){
	Set_Relay(0);
	HAL_Delay(1000);
	Set_Relay(1);
	HAL_Delay(1000);
	Set_Relay(2);
	HAL_Delay(1000);
	Set_Relay(3);
	HAL_Delay(1000);
	Set_Relay(4);
	HAL_Delay(1000);
	Set_Relay(5);
	HAL_Delay(1000);
	Set_Relay(6);
	HAL_Delay(1000);
	Set_Relay(7);
}
void Set_Relay1(uint8_t relayIndex){
	if(relayIndex >= NUMBER_OF_RELAYS) return;
	if(array_Of_Relay_Statuses[relayIndex] == RESET){
		isUpdateRelayStatus = SET;
	}
	array_Of_Relay_Statuses[relayIndex] = SET;
	HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], SET);

}
void Set_Relay(uint8_t relayIndex){
	if(relayIndex >= NUMBER_OF_RELAYS) return;
	if(array_Of_Relay_Statuses[relayIndex] == RESET){
		isUpdateRelayStatus = SET;
	}
	array_Of_Relay_Statuses[relayIndex] = SET;
	Update_Latest_Relay(relayIndex);
	Clear_Counter_For_Checking_Total_Current();
	HAL_GPIO_WritePin(array_Of_Relay_Ports[relayIndex], array_Of_Relay_Pins[relayIndex], SET);

	SCH_Delete_Task(relay_TimeoutFlag_Index);
	Clear_Relay_Timeout_Flag();
	relay_TimeoutFlag_Index = SCH_Add_Task(Set_Relay_Timeout_Flag, 100, 0);
}

void Reset_Relay(uint8_t relayIndex){
	if(relayIndex >= NUMBER_OF_RELAYS) return;
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



//For latest Relay
void Clear_Counter_For_Checking_Total_Current(void){
	timeoutForResetLatestRelayBuffer = 0;
}
void Increase_Counter_For_Checking_Total_Current(void){
	timeoutForResetLatestRelayBuffer = timeoutForResetLatestRelayBuffer + 1;
}
uint8_t Is_Timeout_For_Checking_Total_Current(void){
	if(timeoutForResetLatestRelayBuffer >= 60){
		return 1;
	} else {
		return 0;
	}
}

void Clear_Latest_Relay_Buffer(void){
	for (uint8_t index = 0; index < NUMBER_OF_RELAYS; index ++){
		latestRelay[index] = NUMBER_OF_RELAYS;
	}
	latestRelayIndex = 0;
}
uint8_t Reset_Latest_Relay(void){
	uint8_t relayIndex = latestRelay[latestRelayIndex];
	Reset_Relay(latestRelay[latestRelayIndex]);
	latestRelay[latestRelayIndex] = NUMBER_OF_RELAYS;
	if(latestRelayIndex == 0){
		latestRelayIndex = NUMBER_OF_RELAYS - 1;
	} else {
		latestRelayIndex = latestRelayIndex - 1;
	}
	return relayIndex;
}

void Update_Latest_Relay(uint8_t index){
	if(index >= NUMBER_OF_RELAYS) return;
	latestRelayIndex = (latestRelayIndex + 1)%NUMBER_OF_RELAYS;
	latestRelay[latestRelayIndex] = index;
	
}


