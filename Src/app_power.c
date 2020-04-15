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

#define		DEBUG_POWER(X)							X

#define		MAX_CURRENT								250000
#define		MIN_CURRENT								10000
#define 	CURRENT_CHANGING_THRESHOLD				10000


#define		TIME_OUT_AFTER_UNPLUG					(20000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_DETECTING_NO_FUSE		(20000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_DETECTING_NO_RELAY		(20000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_CHARGE_FULL				(10000/INTERRUPT_TIMER_PERIOD)
#define	    TIME_OUT_AFTER_DETECTING_OVER_CURRENT   (1000/INTERRUPT_TIMER_PERIOD)

#define	    TIME_OUT_AFTER_DETECTING_OVER_MONEY		(10000/INTERRUPT_TIMER_PERIOD)
#define	    TIME_OUT_FOR_SYSTEM_OVER_CURRENT		(5000/INTERRUPT_TIMER_PERIOD)

#define			COUNT_FOR_DECIDE_CHARGE_FULL		10


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
	uint32_t energy;
	uint32_t limitEnergy;
	uint32_t workingTime;

} PowerSystem;

PowerSystem Main;
POWER_FSM_STATE powerFsmState = POWER_FINISH_STATE;
uint32_t lastTimeErr, lastReport;

uint8_t strtmpPower[] = "                                 ";
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


uint8_t strPower[] = "                                         ";

static void Node_Setup(void);
static void Node_Check(void);
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
			if(Eeprom_Read_Outlet(outletID,
					&Main.status,
					&Main.energy,
					&Main.limitEnergy,
					&Main.workingTime)){

			} else {
				Main.currentTotal = 0;
				Main.status = SYSTEM_NORMAL;
				Main.refTotalCurrent = 0;
				Main.valueRef = 0;
				Main.valueTotal = 0;
				Main.ambientTemp = 25;
				Main.internalTemp = 25;
				Main.energy = 0;
				Main.limitEnergy = 0xffffffff;
			}


		} else {
			Main.nodes[outletID].voltage = 230;
			Main.nodes[outletID].powerFactor = 100;
			if(Eeprom_Read_Outlet(outletID,
					&Main.nodes[outletID].nodeStatus,
					&Main.nodes[outletID].energy,
					&Main.nodes[outletID].limitEnergy,
					&Main.nodes[outletID].workingTime)){
				if(Main.nodes[outletID].nodeStatus == NODE_READY ||
						Main.nodes[outletID].nodeStatus == CHARGING){
					Set_Relay(outletID);
				}

			} else {
				Main.nodes[outletID].limitEnergy = 0;
				Main.nodes[outletID].energy = 0;
				Main.nodes[outletID].nodeStatus = NODE_NORMAL;
				Main.nodes[outletID].workingTime = 0;
			}
			Main.nodes[outletID].power = 0;
			Main.nodes[outletID].current = 0;
			power_TimeoutFlag[outletID - 1] = 0;
			power_Timeout_Task_ID[outletID - 1] = NO_TASK_ID;
		}

	}
}



static void Node_Check(void) {

	uint32_t tempRelayFuseStatuses = Get_All_Relay_Fuse_Statuses();

	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID ++){
		if (outletID == MAIN_INPUT) {
			if (Main.valueTotal > 1800 || Main.valueRef > 1800) {
				Main.status = SYSTEM_OVER_CURRENT;
			} else if ((Main.valueTotal - Main.valueRef) > 100) {
				Main.status = NODE_OVER_TOTAL;
			} else if (Main.valueRef - Main.valueTotal > 100) {
				Main.status = NODE_UNDER_TOTAL;
			} else if (Main.ambientTemp > 80) {
				Main.status = SYSTEM_OVER_TEMP;
			} else if (Main.internalTemp > 80) {
				Main.status = SYSTEM_OVER_TEMP;
			} else {
				for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++) {
					if (Main.nodes[i].nodeStatus == NODE_OVER_CURRENT) {
						Main.status = SYSTEM_OVER_CURRENT;
					}
				}
				Main.status = SYSTEM_NORMAL;
			}
		} else if (outletID < MAIN_INPUT) {

			uint8_t tempOutletID = outletID;

			if ((tempRelayFuseStatuses >> (tempOutletID*2) & 0x02) > 0) {	//return NOFUSE
				Main.nodes[tempOutletID].nodeStatus = NO_FUSE;
			} else if ((tempRelayFuseStatuses >> (tempOutletID*2) & 0x01) > 0
					&& (Get_Relay_Status(tempOutletID) == SET)) {	//relay not working MUST and is Working
				Main.nodes[tempOutletID].nodeStatus = NO_RELAY;
			} else if (Main.nodes[tempOutletID].current > MAX_CURRENT) {	// nodeValue from 0 to 1860
				Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
			} else if (Main.nodes[tempOutletID].limitEnergy > 0
					&& Main.nodes[tempOutletID].energy >= Main.nodes[tempOutletID].limitEnergy) {
				Main.nodes[tempOutletID].nodeStatus = NODE_OVER_MONEY;
			} else if (Main.nodes[tempOutletID].limitEnergy == 0) {
				Main.nodes[tempOutletID].nodeStatus = NODE_NORMAL;
			} else if (Main.nodes[tempOutletID].current >= MIN_CURRENT) {
				Main.nodes[tempOutletID].nodeStatus = CHARGING;
			} else if (Main.nodes[tempOutletID].nodeStatus == CHARGING) {
				if (Main.nodes[tempOutletID].previousCurrent - Main.nodes[tempOutletID].current > CURRENT_CHANGING_THRESHOLD) {
					Main.nodes[tempOutletID].nodeStatus = UNPLUG;
				} else {
					Main.nodes[tempOutletID].nodeStatus = CHARGEFULL;
				}
			} else if(Main.nodes[tempOutletID].nodeStatus == UNPLUG && Get_Relay_Status(tempOutletID) == SET){

			} else if(Main.nodes[tempOutletID].nodeStatus == CHARGEFULL && Get_Relay_Status(tempOutletID) == SET){

			}
			else {
				if(Get_Relay_Status(tempOutletID) == SET){
					Main.nodes[tempOutletID].nodeStatus = NODE_READY;
				} else {
					Main.nodes[tempOutletID].nodeStatus = NODE_NORMAL;
				}
			}
		}
	}
}

void Set_Limit_Energy(uint8_t outletID, uint32_t limit_energy){
	if(outletID < NUMBER_OF_RELAYS){
		Main.nodes[outletID].limitEnergy = limit_energy;
		Main.nodes[outletID].energy = 0;
		Main.nodes[outletID].workingTime = 0;
		Eeprom_Update_LimitEnergy(outletID, limit_energy);
	}
}
uint32_t Get_Main_Power_Consumption(void){
	return Main.energy;
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

void Node_Update(uint8_t outletID, uint32_t current, uint8_t voltage, uint8_t power_factor, uint8_t time_period) {	//update and return energy ???
	if (outletID == MAIN_INPUT) {	//setup for Main node
//		Main.ambientTemp = AdcDmaBuffer[10] * 0.08056641;	//chua tinh, LM35 10mV/C => C = V/10mV = ADC*3.3/4096 *100 = adc*0.08056641
//		Main.internalTemp = ((1.43 - (AdcDmaBuffer[12] * 3.3 / 4096)) / 4.3) + 25 ;//AdcDmaBuffer[12];	//chua lay 1v8//Temp = (v25 - Vsense)/AVGSlope + 25 = (1.43 - Vsense)/4.3 + 25 = (1.43 - (ADC*3.3/4096) / 4.3 ) + 25
//		Main.valueRef =
	} else if (outletID < MAIN_INPUT) {
		uint8_t tempOutletID = outletID;
		Main.nodes[tempOutletID].previousCurrent = Main.nodes[tempOutletID].current;
		if (current <= 10000){
			Main.nodes[tempOutletID].current = 0;
		} else	{
			Main.nodes[tempOutletID].current = current;
		}
		Main.nodes[tempOutletID].voltage = voltage;
		Main.nodes[tempOutletID].powerFactor = power_factor;
		Main.nodes[tempOutletID].power = Main.nodes[tempOutletID].voltage * Main.nodes[tempOutletID].current * Main.nodes[tempOutletID].powerFactor / 100000;	//in mA	// /100
		if (Main.nodes[tempOutletID].limitEnergy > 0 || Get_Relay_Status(tempOutletID) == SET){
			Main.nodes[tempOutletID].energy = Main.nodes[tempOutletID].energy + Main.nodes[tempOutletID].power*time_period/100;
			Main.nodes[tempOutletID].workingTime++;
			Eeprom_Update_Energy(tempOutletID, Main.nodes[tempOutletID].energy, Main.nodes[tempOutletID].workingTime);

		} else {
			Main.nodes[tempOutletID].energy = 0;
		}
//		if(tempOutletID <= 9 && tempOutletID >= 0){
//			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) Main.nodes[tempOutletID].powerFactor););
//			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
//			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) Main.nodes[tempOutletID].current););
//			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
//			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) Main.nodes[tempOutletID].power););
//			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
//
//			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\r\n", (int) Main.nodes[tempOutletID].energy););
//			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
//			DEBUG_POWER(UART3_SendToHost((uint8_t *)"\r\n"););
//		}


	}
}




void Power_Setup(void) {
	powerFsmState = POWER_FINISH_STATE;
	Node_Setup();
	Led_Display_Clear_All();
}

void Power_Loop(void) {
	static uint8_t relayIndex = 0;
	switch (powerFsmState) {
	case POWER_FINISH_STATE:
		powerFsmState = POWER_BEGIN_STATE;
		break;
	case POWER_BEGIN_STATE:
		powerFsmState = POWER_CHECK_STATUS_STATE;
		break;
	case POWER_CHECK_STATUS_STATE:
		Node_Check();
		powerFsmState = POWER_CHECK_COMMAND_STATE;
		break;
	case POWER_CHECK_COMMAND_STATE:
		if (Main.status == SYSTEM_OVER_CURRENT) {
			DEBUG_POWER(UART3_SendToHost((uint8_t*) "System over current \r\n"););
			powerFsmState = POWER_ERROR_HANDLER_STATE;
			Led_Display_Set_All(RED);
		} else if (Main.status == SYSTEM_OVER_TEMP) {
			DEBUG_POWER(UART3_SendToHost((uint8_t*) "System over temp \r\n"););
			powerFsmState = POWER_WARNING_HANDLER_STATE;
			Led_Display_Set_All(YELLOW);
		} else if (Main.status == NODE_OVER_TOTAL || Main.status == NODE_UNDER_TOTAL) {
			UART3_SendToHost((uint8_t*) "System Node != total \r\n");
			powerFsmState = POWER_WARNING_HANDLER_STATE;
			Main.status = SYSTEM_NORMAL;
			Led_Display_Set_All(YELLOW);
		} else if (Main.status == SYSTEM_NORMAL) {
			if (Main.nodes[relayIndex].nodeStatus == NODE_NORMAL) {
			} else if (Main.nodes[relayIndex].nodeStatus == CHARGING) {
			} else if (Main.nodes[relayIndex].nodeStatus == CHARGEFULL) {
			} else if (Main.nodes[relayIndex].nodeStatus == UNPLUG) {
			} else if (Main.nodes[relayIndex].nodeStatus == NO_FUSE) {
			} else if (Main.nodes[relayIndex].nodeStatus == NO_RELAY) {
			} else if (Main.nodes[relayIndex].nodeStatus == NODE_OVER_MONEY) {
				if (Get_Relay_Status(relayIndex) == SET) {
					Reset_Relay(relayIndex);
				}
				Main.nodes[relayIndex].limitEnergy = 0;
			} else if (Main.nodes[relayIndex].nodeStatus == NODE_OVER_TIME) {
				if (Get_Relay_Status(relayIndex) == 1) {
					Reset_Relay(relayIndex);
				}
				Main.nodes[relayIndex].nodeStatus = NODE_NORMAL;
			}

			Led_Update_Status_Buffer(relayIndex, Main.nodes[relayIndex].nodeStatus);
			if(Is_Power_Timeout_Flag(relayIndex)){
				Reset_Relay(relayIndex);
			}
			relayIndex++;
			if (relayIndex >= NUMBER_OF_RELAYS) {
				relayIndex = 0;
				powerFsmState = POWER_FINISH_STATE;
			}
		}

		break;
	case POWER_ERROR_HANDLER_STATE:
		if (HAL_GetTick() - lastTimeErr > 5000) {
			lastTimeErr = HAL_GetTick();
			for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++)
				Reset_Relay(i);
			powerFsmState = POWER_FINISH_STATE;
		}
		break;
	case POWER_WARNING_HANDLER_STATE:
		powerFsmState = POWER_FINISH_STATE;
		break;
	default:
		powerFsmState = POWER_FINISH_STATE;
		break;
	}
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
	static NodeStatus previousOutletStatus[NUMBER_OF_RELAYS];
	if(previousOutletStatus[outletID] != Main.nodes[outletID].nodeStatus){
		previousOutletStatus[outletID] = Main.nodes[outletID].nodeStatus;

		switch(Main.nodes[outletID].nodeStatus){
		case NODE_NORMAL:
			DEBUG_POWER(sprintf((char*) strPower, "NODE_NORMAL=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;
		case NODE_READY:
			DEBUG_POWER(sprintf((char*) strPower, "NODE_READY=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;
		case CHARGING:
			DEBUG_POWER(sprintf((char*) strPower, "CHARGING=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;
		case CHARGEFULL:
			DEBUG_POWER(sprintf((char*) strPower, "CHARGEFULL=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;
		case UNPLUG:
			DEBUG_POWER(sprintf((char*) strPower, "UNPLUG=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;
		case NO_POWER:
			DEBUG_POWER(sprintf((char*) strPower, "NO_POWER=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;
		case NO_FUSE:
			DEBUG_POWER(sprintf((char*) strPower, "NO_FUSE=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;
		case NO_RELAY:
			DEBUG_POWER(sprintf((char*) strPower, "NO_RELAY=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;

		case NODE_OVER_CURRENT:
			DEBUG_POWER(sprintf((char*) strPower, "NODE_OVER_CURRENT=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
			break;
		case NODE_OVER_MONEY:
			DEBUG_POWER(sprintf((char*) strPower, "NODE_OVER_MONEY=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strPower););
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

	} else if (Main.nodes[tempOutletID].current > MAX_CURRENT) {	// nodeValue from 0 to 1860

		Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
		Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
		Reset_Relay(tempOutletID);
		outletState[tempOutletID] = 3;

	} else if (Main.nodes[tempOutletID].limitEnergy > 0
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
			 if (Main.nodes[tempOutletID].current >= MIN_CURRENT) {

				Main.nodes[tempOutletID].nodeStatus = CHARGING;
				outletCounter[tempOutletID] = 0;
				outletState[tempOutletID] = 1;

			} else {
				if(Get_Relay_Status(tempOutletID) == SET){
					Main.nodes[tempOutletID].nodeStatus = NODE_READY;
				} else {
					Main.nodes[tempOutletID].nodeStatus = NODE_NORMAL;
				}
			}
			break;
		case 1:
			if (Main.nodes[tempOutletID].current < MIN_CURRENT){
				if (Main.nodes[tempOutletID].previousCurrent - Main.nodes[tempOutletID].current > CURRENT_CHANGING_THRESHOLD) {
					Main.nodes[tempOutletID].nodeStatus = UNPLUG;
					Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_UNPLUG);
					outletState[tempOutletID] = 3;
				} else {
					outletCounter[tempOutletID]++;
					if(outletCounter[tempOutletID] >= COUNT_FOR_DECIDE_CHARGE_FULL){
						Main.nodes[tempOutletID].nodeStatus = CHARGEFULL;
						outletCounter[tempOutletID] = 0;
						Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_CHARGE_FULL);
						outletState[tempOutletID] = 2;
					}
				}
			}
			break;
		case 2:
			if(Is_Power_Timeout_Flag(tempOutletID)){
//				Reset_Relay(tempOutletID);
				outletState[tempOutletID] = 0;
			}
			break;
		case 3:
			if(Is_Power_Timeout_Flag(tempOutletID)){
//				Reset_Relay(tempOutletID);
				outletState[tempOutletID] = 0;
			} else if(Main.nodes[tempOutletID].current >= MIN_CURRENT){
				outletState[tempOutletID] = 0;
			}
			break;
		default:
			outletState[tempOutletID] = 0;
			break;
		}
	}
	Led_Update_Status_Buffer(tempOutletID, Main.nodes[tempOutletID].nodeStatus);
	Eeprom_Update_Status(tempOutletID, Main.nodes[tempOutletID].nodeStatus);
	tempOutletID = (tempOutletID + 1) % NUMBER_OF_RELAYS;
}

void Process_System_Power(void){
	if(Process_Input_Source() == SYSTEM_NORMAL){
		Process_Outlets();
	}

}
