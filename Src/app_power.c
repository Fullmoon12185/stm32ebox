/*
 * app_power.c
 *
 *  Created on: Mar 11, 2020
 *      Author: Nguyen
 */

#include "main.h"
#include "app_power.h"
#include "app_uart.h"
#include "app_pcf8574.h"
#include "app_led_display.h"
#include "app_relay.h"
#include "app_scheduler.h"
#include "app_eeprom.h"
#include "app_string.h"
#include <math.h>


#define		DEBUG_POWER(X)							X
#if(VERSION_EBOX == VERSION_3_WITH_ALL_CT_5A)
#define		MAX_CURRENT								700000
//for distrist
//#define		MAX_CURRENT_1							700000
#define		MAX_CURRENT_1							700000
#elif(VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A)
#define		MAX_CURRENT								700000
//for distrist
//#define		MAX_CURRENT_1							700000
#define		MAX_CURRENT_1							1000000

#endif

#if(VERSION_EBOX == 2 || VERSION_EBOX == 3 || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A)
#define		MIN_CURRENT								30000
#define		MIN_CURRENT_FOR_START_CHARGING			60000
#define 	CURRENT_CHANGING_THRESHOLD				30000
#define 	MIN_PF									30

#elif(VERSION_EBOX == 15)
#define		MIN_CURRENT								10000
#define 	CURRENT_CHANGING_THRESHOLD				10000
#define 	MIN_PF									25
#elif(VERSION_EBOX == 1)
#define		MIN_CURRENT								10000
#define 	CURRENT_CHANGING_THRESHOLD				10000
#define 	MIN_PF									25
#endif

#define		TIME_OUT_1_SECOND						(1000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_STABABILITY					(3000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_UNPLUG					(5000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_DETECTING_NO_FUSE		(20000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_DETECTING_NO_RELAY		(20000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_CHARGE_FULL				(10000/INTERRUPT_TIMER_PERIOD)
#define	    TIME_OUT_AFTER_DETECTING_OVER_CURRENT   (1000/INTERRUPT_TIMER_PERIOD)

#define	    TIME_OUT_AFTER_DETECTING_OVER_MONEY		(10000/INTERRUPT_TIMER_PERIOD)
#define	    TIME_OUT_FOR_SYSTEM_OVER_CURRENT		(5000/INTERRUPT_TIMER_PERIOD)

#define			COUNT_FOR_DECIDE_CHARGE_FULL		30


typedef struct PowerNodes {
	NodeStatus  nodeStatus;			//avaiable = 0; charging = 1;	chargefull = 2
	uint32_t current;	// in uA
	uint32_t previousCurrent;
	uint8_t voltage;	// = 220
//	uint8_t frequency;	// = 50 in Hz
	uint8_t powerFactor; // = 1pF
	uint32_t lastPower;
	uint32_t power;		// in mW
	uint32_t workingTime;		//from start to now
	uint32_t energy; 	// in mWs
	uint32_t limitEnergy;		//set this value from web app
} PowerNode;

typedef struct PowerSystems {
	PowerNode nodes[NUMBER_OF_RELAYS];
	uint16_t valueRef;	//read from 1v8
	uint16_t valueTotal;			//total Adc PP from Main CT
	uint32_t currentTotal;			//sum of all channel
	uint32_t refTotalCurrent;		//read from Big CT
	SystemStatus status;				//as in enum SystemStatus
	uint8_t ambientTemp;
	uint8_t internalTemp;

	uint8_t powerFactor;
	uint32_t previousEnergy;
	uint32_t energy;
	uint32_t limitEnergy;
	uint32_t workingTime;

} PowerSystem;

PowerSystem Main;
POWER_FSM_STATE powerFsmState = POWER_FINISH_STATE;
uint32_t lastTimeErr, lastReport;

uint8_t strtmpPower[] = "                                      ";
uint8_t power_TimeoutFlag[NUMBER_OF_RELAYS];
uint32_t power_Timeout_Task_ID[NUMBER_OF_RELAYS]= {
		NO_TASK_ID, NO_TASK_ID,
		NO_TASK_ID, NO_TASK_ID,
		NO_TASK_ID, NO_TASK_ID,
		NO_TASK_ID, NO_TASK_ID,
		NO_TASK_ID, NO_TASK_ID
};

uint8_t power_Input_Source_TimeoutFlag = 0;
uint32_t power_Input_Source_Task_ID = NO_TASK_ID;


static uint8_t outletState[NUMBER_OF_RELAYS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static uint32_t outletCounter[NUMBER_OF_RELAYS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

FlagStatus is_Node_Status_Changed = RESET;


static void Node_Setup(void);
void Power_Clear_Timeout_Flag(uint8_t outletID);
void Power_Set_Timeout_Flag_0(void);
void Power_Set_Timeout_Flag_1(void);
void Power_Set_Timeout_Flag_2(void);
void Power_Set_Timeout_Flag_3(void);
void Power_Set_Timeout_Flag_4(void);
void Power_Set_Timeout_Flag_5(void);
void Power_Set_Timeout_Flag_6(void);
void Power_Set_Timeout_Flag_7(void);
void Power_Set_Timeout_Flag_8(void);
void Power_Set_Timeout_Flag_9(void);
uint8_t Is_Power_Timeout_Flag(uint8_t outletID);


void Process_Outlets(void);
SystemStatus Process_Input_Source(void);

/////////////////////////////////////////////////////////////

void Process_System_Power(void){
	if(Process_Input_Source() == SYSTEM_NORMAL){
		Process_Outlets();
	}
}


void Set_Power_Input_Source_Timeout_Flag(void){
	power_Input_Source_TimeoutFlag = 1;
}
void Clear_Power_Input_Source_Timeout_Flag(void){
	power_Input_Source_TimeoutFlag = 0;
}
uint8_t Is_Power_Input_Source_Timeout_Flag(void){
	return power_Input_Source_TimeoutFlag;
}

void Clear_Power_Timeout_Flag(uint8_t outletID){
	power_TimeoutFlag[outletID] = 0;
}
void Set_Power_Timeout_Flag_0(void){
	power_TimeoutFlag[0] = 1;
}
void Set_Power_Timeout_Flag_1(void){
	power_TimeoutFlag[1] = 1;
}
void Set_Power_Timeout_Flag_2(void){
	power_TimeoutFlag[2] = 1;
}
void Set_Power_Timeout_Flag_3(void){
	power_TimeoutFlag[3] = 1;
}
void Set_Power_Timeout_Flag_4(void){
	power_TimeoutFlag[4] = 1;
}
void Set_Power_Timeout_Flag_5(void){
	power_TimeoutFlag[5] = 1;
}
void Set_Power_Timeout_Flag_6(void){
	power_TimeoutFlag[6] = 1;
}
void Set_Power_Timeout_Flag_7(void){
	power_TimeoutFlag[7] = 1;
}
void Set_Power_Timeout_Flag_8(void){
	power_TimeoutFlag[8] = 1;
}
void Set_Power_Timeout_Flag_9(void){
	power_TimeoutFlag[9] = 1;
}
uint8_t Is_Power_Timeout_Flag(uint8_t outletID){
	uint8_t tempStatus = power_TimeoutFlag[outletID];
	if(tempStatus == 1)
		Clear_Power_Timeout_Flag(outletID);
	return tempStatus;
}

void Set_Power_Timeout_Flags(uint8_t outletID, uint32_t PowerTimeOut){
	SCH_Delete_Task(power_Timeout_Task_ID[outletID]);
	Clear_Power_Timeout_Flag(outletID);
	if(outletID == 0){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_0, PowerTimeOut, 0);
	} else if(outletID == 1){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_1, PowerTimeOut, 0);
	} else if(outletID == 2){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_2, PowerTimeOut, 0);
	} else if(outletID == 3){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_3, PowerTimeOut, 0);
	} else if(outletID == 4){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_4, PowerTimeOut, 0);
	} else if(outletID == 5){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_5, PowerTimeOut, 0);
	} else if(outletID == 6){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_6, PowerTimeOut, 0);
	} else if(outletID == 7){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_7, PowerTimeOut, 0);
	} else if(outletID == 8){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_8, PowerTimeOut, 0);
	} else if(outletID == 9){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_9, PowerTimeOut, 0);
	}
}



static void Node_Setup(void) {
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID ++){
		if (outletID == MAIN_INPUT) {	//setup for Main node
			{
				Main.currentTotal = 0;
				Main.status = SYSTEM_NORMAL;
				Main.refTotalCurrent = 0;
				Main.valueRef = 0;
				Main.valueTotal = 0;
				Main.ambientTemp = 25;
				Main.internalTemp = 25;
				Main.energy = Eeprom_Get_Main_Energy();
				Main.previousEnergy = Main.energy;
				UART3_SendToHost((uint8_t *)ConvertUint64ToString((uint64_t)Main.energy));
				Main.limitEnergy = 0xffffffff;
			}
		} else {
			Main.nodes[outletID].voltage = VOLTAGE_USAGE;
			Main.nodes[outletID].powerFactor = 100;
//			if(Eeprom_Read_Outlet(outletID,
//					&Main.nodes[outletID].nodeStatus,
//					&Main.nodes[outletID].energy,
//					&Main.nodes[outletID].limitEnergy,
//					&Main.nodes[outletID].workingTime)){
//				if(Main.nodes[outletID].nodeStatus == CHARGING){
//					Set_Relay(outletID);
//				} else if(Main.nodes[outletID].nodeStatus == NODE_READY){
//					Main.nodes[outletID].limitEnergy = 0xffffffff;
//					Main.nodes[outletID].energy = 0;
//					Set_Relay(outletID);
//				} else {
//					Main.nodes[outletID].limitEnergy = 0xffffffff;
//					Main.nodes[outletID].energy = 0;
//				}
//
//				HAL_Delay(100);
//			} else
			{

				Main.nodes[outletID].limitEnergy = 0xffffffff;
				Main.nodes[outletID].energy = 0;
				Main.nodes[outletID].nodeStatus = NODE_NORMAL;
				Main.nodes[outletID].workingTime = 0;
			}
			sprintf((char*) strtmpPower, "i:%d\t s:%d\t e:%lu\t l:%lu\t \r\n", (int) outletID, (int)Main.nodes[outletID].nodeStatus,
					(uint32_t)Main.nodes[outletID].energy, (uint32_t)Main.nodes[outletID].limitEnergy);
			UART3_SendToHost((uint8_t *)strtmpPower);
			Main.nodes[outletID].power = 0;
			Main.nodes[outletID].current = 0;
			power_TimeoutFlag[outletID] = 0;
			power_Timeout_Task_ID[outletID] = NO_TASK_ID;
		}

	}
}





void Set_Limit_Energy(uint8_t outletID, uint32_t limit_energy){
	if(outletID < NUMBER_OF_RELAYS){
		Main.nodes[outletID].limitEnergy = limit_energy;
		if(Main.nodes[outletID].nodeStatus != CHARGING){
			Main.nodes[outletID].energy = 0;
			Main.nodes[outletID].workingTime = 0;
		}
//		Eeprom_Update_LimitEnergy(outletID, limit_energy);
	}
}
uint32_t Get_Main_Power_Consumption(void){
//	uint_t derivativeEnergy = Main.energy - Main.previousEnergy;
//	Main.previousEnergy = Main.energy;
	return (uint32_t)(Main.energy);
}
void Set_Main_Power_Consumption(uint64_t totalPowerConsumption){
	Main.energy = (uint32_t)totalPowerConsumption;
	Eeprom_Update_Main_Energy_Immediately(Main.energy);
}

uint8_t Get_Main_Power_Factor(void){
	return Main.powerFactor;
}
uint32_t Get_Main_Current(void){
	return Main.refTotalCurrent/100;
}
SystemStatus Get_Main_Status(void){
	return Main.status;
}
uint32_t Get_Power_Consumption(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS){
		return Main.nodes[outletID].energy;
	}
	return 0;
}
uint8_t Get_Power_Factor(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS){
		return Main.nodes[outletID].powerFactor;
	}
	return 0;
}

uint8_t Get_Voltage(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS){
		return Main.nodes[outletID].voltage;
	}
	return 0;
}

uint32_t Get_Current(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS){
		return Main.nodes[outletID].current/100;
	}
	return 0;
}
NodeStatus Get_Node_Status(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS){
		return Main.nodes[outletID].nodeStatus;
	}
	return 9;
}

FlagStatus Get_Is_Node_Status_Changed(void){
	if(is_Node_Status_Changed == SET){
		is_Node_Status_Changed = RESET;
		return SET;
	}
	return RESET;
}

void Node_Update(uint8_t outletID, uint32_t current, uint8_t voltage, uint8_t power_factor, uint8_t time_period) {	//update and return energy ???
	float tempCurrent = 0;
	float tempPower = 0;
	if (outletID == MAIN_INPUT) {	//setup for Main node
		Main.energy +=  POWER_CONSUMPTION_OF_MCU;
		Eeprom_Update_Main_Energy(Main.energy);
		Main.workingTime++;
	} else if (outletID < MAIN_INPUT) {
		uint8_t tempOutletID = outletID;
		Main.nodes[tempOutletID].previousCurrent = Main.nodes[tempOutletID].current;
		if (current <= CURRENT_CHANGING_THRESHOLD
				|| (Get_Relay_Status(tempOutletID) == RESET)
				|| power_factor < MIN_PF)
		{
			Main.nodes[tempOutletID].current = 0;
			Main.nodes[tempOutletID].powerFactor = 0;
		} else	{
			Main.nodes[tempOutletID].current = current;
			Main.nodes[tempOutletID].powerFactor = power_factor;
		}

		Main.nodes[tempOutletID].voltage = voltage;
		tempCurrent = (float)(Main.nodes[tempOutletID].current)/100000;//in mA	// /100
		tempPower = (float)(Main.nodes[tempOutletID].voltage * tempCurrent * Main.nodes[tempOutletID].powerFactor);

		Main.nodes[tempOutletID].power = round((float)tempPower);

		if (Main.nodes[tempOutletID].limitEnergy > 0 || Get_Relay_Status(tempOutletID) == SET){
			Main.energy += Main.nodes[tempOutletID].power*time_period/100;
			Main.nodes[tempOutletID].energy = Main.nodes[tempOutletID].energy + Main.nodes[tempOutletID].power*time_period/100;
			Main.nodes[tempOutletID].workingTime++;
			Eeprom_Update_Energy(tempOutletID, Main.nodes[tempOutletID].energy);

		} else {
			Main.nodes[tempOutletID].energy = 0;
		}
		if(tempOutletID == 9){
			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) tempOutletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) Main.nodes[tempOutletID].powerFactor););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) Main.workingTime););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) Main.nodes[tempOutletID].current););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) Main.nodes[tempOutletID].power););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););

			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\r\n", (int) Main.nodes[tempOutletID].energy););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)"\r\n"););
		}
	}
}




void Power_Setup(void) {
	powerFsmState = POWER_FINISH_STATE;
	Node_Setup();
	Led_Display_Clear_All();
}



SystemStatus Process_Input_Source(void){
	static uint8_t inputSourceState = 0;

		switch(inputSourceState){
		case 0:
			if (Main.valueTotal > 1800 || Main.valueRef > 1800) {
				Main.status = SYSTEM_OVER_CURRENT;
				Relay_Init();
				SCH_Delete_Task(power_Input_Source_Task_ID);
				Clear_Power_Input_Source_Timeout_Flag();
				power_Input_Source_Task_ID = SCH_Add_Task(Set_Power_Input_Source_Timeout_Flag, TIME_OUT_FOR_SYSTEM_OVER_CURRENT, 0);
				inputSourceState = 1;
				DEBUG_POWER(UART3_SendToHost((uint8_t *)"SYSTEM_OVER_CURRENT"););
			}
//			else if ((Main.valueTotal - Main.valueRef) > 100) {
//				Main.status = NODE_OVER_TOTAL;
//				SCH_Delete_Task(power_Input_Source_Task_ID);
//				Clear_Power_Input_Source_Timeout_Flag();
//				power_Input_Source_Task_ID = SCH_Add_Task(Set_Power_Input_Source_Timeout_Flag, TIME_OUT_FOR_SYSTEM_OVER_CURRENT, 0);
//				inputSourceState = 2;
//			} else if (Main.valueRef - Main.valueTotal > 100) {
//				Main.status = NODE_UNDER_TOTAL;
//				SCH_Delete_Task(power_Input_Source_Task_ID);
//				Clear_Power_Input_Source_Timeout_Flag();
//				power_Input_Source_Task_ID = SCH_Add_Task(Set_Power_Input_Source_Timeout_Flag, TIME_OUT_FOR_SYSTEM_OVER_CURRENT, 0);
//				inputSourceState = 3;
//			} else if (Main.ambientTemp > 80) {
//				Main.status = SYSTEM_OVER_TEMP;
//				SCH_Delete_Task(power_Input_Source_Task_ID);
//				Clear_Power_Input_Source_Timeout_Flag();
//				power_Input_Source_Task_ID = SCH_Add_Task(Set_Power_Input_Source_Timeout_Flag, TIME_OUT_FOR_SYSTEM_OVER_CURRENT, 0);
//				inputSourceState = 4;
//			} else if (Main.internalTemp > 80) {
//				Main.status = SYSTEM_OVER_TEMP;
//				SCH_Delete_Task(power_Input_Source_Task_ID);
//				Clear_Power_Input_Source_Timeout_Flag();
//				power_Input_Source_Task_ID = SCH_Add_Task(Set_Power_Input_Source_Timeout_Flag, TIME_OUT_FOR_SYSTEM_OVER_CURRENT, 0);
//				inputSourceState = 4;
//			}
			else {
//				for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++) {
//					if (Main.nodes[i].nodeStatus == NODE_OVER_CURRENT) {
//						Main.status = SYSTEM_OVER_CURRENT;
//						Relay_Init();
//						SCH_Delete_Task(power_Input_Source_Task_ID);
//						Clear_Power_Input_Source_Timeout_Flag();
//						power_Input_Source_Task_ID = SCH_Add_Task(Set_Power_Input_Source_Timeout_Flag, TIME_OUT_FOR_SYSTEM_OVER_CURRENT, 0);
//						inputSourceState = 1;
//						break;
//					}
//				}
				Main.status = SYSTEM_NORMAL;
			}
			break;
		case 1:
			if(Is_Power_Input_Source_Timeout_Flag()){
				inputSourceState = 0;
			}
			break;
		case 2:
			break;
		default:
			break;
		}

	return Main.status;
}
void Display_OutLet_Status(uint8_t outletID){
	static NodeStatus previousOutletStatus[NUMBER_OF_RELAYS] = {
			NODE_NORMAL,
			NODE_NORMAL,
			NODE_NORMAL,
			NODE_NORMAL,
			NODE_NORMAL,
			NODE_NORMAL,
			NODE_NORMAL,
			NODE_NORMAL,
			NODE_NORMAL,
			NODE_NORMAL
	};
	if(previousOutletStatus[outletID] != Main.nodes[outletID].nodeStatus){
		previousOutletStatus[outletID] = Main.nodes[outletID].nodeStatus;
		is_Node_Status_Changed = SET;

		switch(Main.nodes[outletID].nodeStatus){
		case NODE_NORMAL:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NODE_NORMAL=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			Eeprom_Update_Status(outletID, Main.nodes[outletID].nodeStatus);
			break;
		case NODE_READY:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NODE_READY=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			Eeprom_Update_Status(outletID, Main.nodes[outletID].nodeStatus);
			break;
		case CHARGING:
			DEBUG_POWER(sprintf((char*) strtmpPower, "CHARGING=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			Eeprom_Update_Status(outletID, Main.nodes[outletID].nodeStatus);
			break;
		case CHARGEFULL:
			DEBUG_POWER(sprintf((char*) strtmpPower, "CHARGEFULL=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case UNPLUG:
			DEBUG_POWER(sprintf((char*) strtmpPower, "UNPLUG=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case NO_POWER:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NO_POWER=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case NO_FUSE:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NO_FUSE=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case NO_RELAY:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NO_RELAY=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;

		case NODE_OVER_CURRENT:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NODE_OVER_CURRENT=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case NODE_OVER_MONEY:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NODE_OVER_MONEY=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case NODE_OVER_TIME:
			break;
		default:
			break;
		}
	}
}
void Process_Outlets(void){
	static uint8_t tempOutletID = 0;
	int32_t tempDefference;
	uint32_t tempRelayFuseStatuses = Get_All_Relay_Fuse_Statuses();
	Display_OutLet_Status(tempOutletID);
	if ((tempRelayFuseStatuses >> (tempOutletID*2) & 0x02) > 0) {	//return NOFUSE
		Main.nodes[tempOutletID].nodeStatus = NO_FUSE;
		Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_NO_FUSE);
		outletState[tempOutletID] = 3;

	} else if ((tempRelayFuseStatuses >> (tempOutletID*2) & 0x01) > 0
			&& (Get_Relay_Status(tempOutletID) == SET)
			&& is_Set_Relay_Timeout()) {	//relay not working MUST and is Working

		Main.nodes[tempOutletID].nodeStatus = NO_RELAY;
		Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_NO_RELAY);
		outletState[tempOutletID] = 3;

	}
#if (VERSION_EBOX == VERSION_3_WITH_ALL_CT_5A)
	else if (Main.nodes[tempOutletID].current > MAX_CURRENT) {	// nodeValue from 0 to 1860
		if(tempOutletID == 0 || tempOutletID == 1){
			if (Main.nodes[tempOutletID].current > MAX_CURRENT_1){
				Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
				Reset_Relay(tempOutletID);
				outletState[tempOutletID] = 3;
			}
		} else {
			Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
			Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
			Reset_Relay(tempOutletID);
			outletState[tempOutletID] = 3;
		}
	}
#elif(VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A)
	else if (Main.nodes[tempOutletID].current > MAX_CURRENT) {	// nodeValue from 0 to 1860
		if(tempOutletID == CT_10A_1 || tempOutletID == CT_10A_2){
			if (Main.nodes[tempOutletID].current > MAX_CURRENT_1){
				Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
				Reset_Relay(tempOutletID);
				outletState[tempOutletID] = 3;
			}
		} else {
			Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
			Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
			Reset_Relay(tempOutletID);
			outletState[tempOutletID] = 3;
		}
	}

#endif
	else if (Main.nodes[tempOutletID].limitEnergy > 0
			&& Main.nodes[tempOutletID].energy >= Main.nodes[tempOutletID].limitEnergy) {

		Main.nodes[tempOutletID].nodeStatus = NODE_OVER_MONEY;
		Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_MONEY);
		outletState[tempOutletID] = 3;

	} else if (Main.nodes[tempOutletID].limitEnergy == 0) {
		Main.nodes[tempOutletID].nodeStatus = NODE_NORMAL;
		outletState[tempOutletID] = 0;
	} else {
		switch(outletState[tempOutletID]){
		case 0:
			 if (Main.nodes[tempOutletID].current >= MIN_CURRENT_FOR_START_CHARGING) {
//				Main.nodes[tempOutletID].nodeStatus = CHARGING;
				outletCounter[tempOutletID] = 0;
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_STABABILITY);
				outletState[tempOutletID] = 4;
			} else {
				if(Get_Relay_Status(tempOutletID) == SET){
					Main.nodes[tempOutletID].nodeStatus = NODE_READY;
				} else {
					Main.nodes[tempOutletID].nodeStatus = NODE_NORMAL;
				}
			}
			break;
		case 4:
			if(Is_Power_Timeout_Flag(tempOutletID)){
				if (Main.nodes[tempOutletID].current >= MIN_CURRENT) {
					Main.nodes[tempOutletID].nodeStatus = CHARGING;
					outletCounter[tempOutletID] = 0;
					outletState[tempOutletID] = 1;
				} else {
					outletState[tempOutletID] = 0;
				}
			}
			break;
		case 1:
			if (Main.nodes[tempOutletID].current < MIN_CURRENT){
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_1_SECOND);
				outletState[tempOutletID] = 5;
			} else {
				outletCounter[tempOutletID] = 0;
			}
			break;
		case 2:
			if(Is_Power_Timeout_Flag(tempOutletID)){
				outletState[tempOutletID] = 0;
			}

			break;
//		case 3:
//			if(Main.nodes[tempOutletID].current >= MIN_CURRENT){
//				outletState[tempOutletID] = 0;
//			} else if(Is_Power_Timeout_Flag(tempOutletID)){
//				Main.nodes[tempOutletID].nodeStatus = UNPLUG;
//				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_UNPLUG*2);
//				outletState[tempOutletID] = 2;
//			}
//			break;
		case 5:
			if(Is_Power_Timeout_Flag(tempOutletID)){
				outletCounter[tempOutletID]++;
				if(outletCounter[tempOutletID] >= COUNT_FOR_DECIDE_CHARGE_FULL){
					Main.nodes[tempOutletID].nodeStatus = CHARGEFULL;
					outletCounter[tempOutletID] = 0;
					Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_CHARGE_FULL);
					outletState[tempOutletID] = 2;
				} else {
					outletState[tempOutletID] = 1;
				}

			}
			break;
		default:
			outletState[tempOutletID] = 0;
			break;
		}
	}
	Led_Update_Status_Buffer(tempOutletID, Main.nodes[tempOutletID].nodeStatus);
	tempOutletID = (tempOutletID + 1) % NUMBER_OF_RELAYS;
}


