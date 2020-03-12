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

#define		MAIN_INPUT		0
typedef struct PowerNodes {
	NodeStatus  nodeStatus;			//avaiable = 0; charging = 1;	chargefull = 2
	uint32_t current;	// in uA
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
	float currentTotal;			//sum of all channel
	float refTotalCurrent;		//read from Big CT
	SystemStatus status;				//as in enum SystemStatus
	uint8_t ambientTemp;
	uint8_t internalTemp;

} PowerSystem;

PowerSystem Main;
POWER_FSM_STATE powerFsmState = POWER_FINISH_STATE;
uint32_t lastTimeErr, lastReport;

void Node_Setup(void);
void Node_Check(void);
void Power_Setup(void);


void Node_Setup(void) {
	for(uint8_t outletID = 0; outletID <= NUMBER_OF_RELAYS; outletID ++){
		if (outletID == MAIN_INPUT) {	//setup for Main node
			Main.currentTotal = 0;
			Main.status = SYSTEM_NORMAL;
			Main.refTotalCurrent = 0;
			Main.valueRef = 0;
			Main.valueTotal = 0;
			Main.ambientTemp = 25;
			Main.internalTemp = 25;
		} else {
			Main.nodes[outletID - 1].voltage = 220;
	//		Main.nodes[outletID].frequency = 50;
			Main.nodes[outletID - 1].powerFactor = 100;
			Main.nodes[outletID - 1].power = 0;
			Main.nodes[outletID - 1].limitEnergy = 0;
			Main.nodes[outletID - 1].current = 0;
			Main.nodes[outletID - 1].workingTime = 0;
			Main.nodes[outletID - 1].energy = 0;
			Main.nodes[outletID - 1].nodeStatus = NODE_NORMAL;

			//for testing, enable first channel
	//		if(outletID == 0) Main.nodes[0].limitEnergy = 300000;
	//		Led_Display_Color(outletID, GREEN);
	//		Led_Display_Show();
		}
	}

}

void Node_Check(void) {

	uint32_t tempRelayFuseStatuses = Get_All_Relay_Fuse_Statuses();

	for(uint8_t outletID = 0; outletID <= NUMBER_OF_RELAYS; outletID ++){
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
		} else if (outletID <= NUMBER_OF_RELAYS) {

			uint8_t tempOutletID = outletID - 1;

			if ((tempRelayFuseStatuses >> (tempOutletID*2) & 0x02) > 0) {	//return NOFUSE
				if (outletID < 4) {
					Main.nodes[outletID].nodeStatus = NO_FUSE;
				}
			} else if ((tempRelayFuseStatuses >> (tempOutletID*2) & 0x01) > 0
					&& (Get_Relay_Status(tempOutletID) == SET)) {	//relay not working MUST and is Working
				if (tempOutletID < 4) {
					Main.nodes[tempOutletID].nodeStatus = NO_RELAY;
				}
			} else if (Main.nodes[tempOutletID].current > 300000) {	// nodeValue from 0 to 1860
				Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
			} else if (Main.nodes[tempOutletID].limitEnergy > 0
					&& Main.nodes[tempOutletID].energy >= Main.nodes[tempOutletID].limitEnergy) {
				Main.nodes[tempOutletID].nodeStatus = NODE_OVER_MONEY;
			} else if (Main.nodes[tempOutletID].limitEnergy == 0) {
				Main.nodes[tempOutletID].nodeStatus = NODE_NORMAL;
			} else if (Main.nodes[tempOutletID].current >= 1000) {
				Main.nodes[tempOutletID].nodeStatus = CHARGING;
			} else if (Main.nodes[tempOutletID].nodeStatus == CHARGING) {
				if (Main.nodes[tempOutletID].lastPower - Main.nodes[tempOutletID].power > 20) {
					Main.nodes[tempOutletID].nodeStatus = UNPLUG;
				} else {
					Main.nodes[tempOutletID].nodeStatus = CHARGEFULL;
				}
				Main.nodes[tempOutletID].lastPower = Main.nodes[tempOutletID].power;
			} else {
				Main.nodes[tempOutletID].nodeStatus = NODE_NORMAL;
			}
		}
	}
}

void Set_Limit_Energy(uint8_t outletID, uint32_t limit_energy){
	if(outletID < NUMBER_OF_RELAYS){
		Main.nodes[outletID].limitEnergy = limit_energy;
	}
}

void Node_Update(uint8_t outletID, uint32_t current, uint8_t voltage, uint8_t power_factor, uint8_t time_period) {	//update and return energy ???
	if (outletID == 0) {	//setup for Main node
//		Main.ambientTemp = AdcDmaBuffer[10] * 0.08056641;	//chua tinh, LM35 10mV/C => C = V/10mV = ADC*3.3/4096 *100 = adc*0.08056641
//		Main.internalTemp = ((1.43 - (AdcDmaBuffer[12] * 3.3 / 4096)) / 4.3) + 25 ;//AdcDmaBuffer[12];	//chua lay 1v8//Temp = (v25 - Vsense)/AVGSlope + 25 = (1.43 - Vsense)/4.3 + 25 = (1.43 - (ADC*3.3/4096) / 4.3 ) + 25
//		Main.valueRef =
	} else if (outletID <= NUMBER_OF_RELAYS) {
		uint8_t tempOutletID = outletID - 1;
		Main.nodes[tempOutletID].current = current;
		Main.nodes[tempOutletID].voltage = voltage;
		Main.nodes[tempOutletID].powerFactor = power_factor;
		Main.nodes[tempOutletID].power = Main.nodes[tempOutletID].voltage * Main.nodes[tempOutletID].current * Main.nodes[tempOutletID].powerFactor / 100000;	//in mA	// /100
		if (Main.nodes[tempOutletID].limitEnergy > 0){
			Main.nodes[tempOutletID].energy = Main.nodes[tempOutletID].energy + Main.nodes[tempOutletID].power*time_period;
		}
		else {
			Main.nodes[tempOutletID].energy = 0;
		}
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
			UART3_SendToHost((uint8_t*) "System over current \r\n");
			powerFsmState = POWER_ERROR_HANDLER_STATE;
			Led_Display_Set_All(RED);
		} else if (Main.status == SYSTEM_OVER_TEMP) {
			UART3_SendToHost((uint8_t*) "System over temp \r\n");
			powerFsmState = POWER_WARNING_HANDLER_STATE;
			Led_Display_Set_All(YELLOW);
		} else if (Main.status == NODE_OVER_TOTAL || Main.status == NODE_UNDER_TOTAL) {
			UART3_SendToHost((uint8_t*) "System Node != total \r\n");
			powerFsmState = POWER_WARNING_HANDLER_STATE;
			Main.status = SYSTEM_NORMAL;
			Led_Display_Set_All(YELLOW);
		} else if (Main.status == SYSTEM_NORMAL) {
			if (Main.nodes[relayIndex].nodeStatus == NODE_NORMAL) {
				//if command then set_node_energy(1000);
				if (Main.nodes[relayIndex].limitEnergy > Main.nodes[relayIndex].energy) {
					Set_Relay(relayIndex);
				} else {
					Reset_Relay(relayIndex);
				}

			} else if (Main.nodes[relayIndex].nodeStatus == CHARGING) {
			} else if (Main.nodes[relayIndex].nodeStatus == CHARGEFULL) {
				if (HAL_GetTick() - lastReport > 5000) {
					lastReport = HAL_GetTick();
				}
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

			relayIndex++;
			if (relayIndex >= NUMBER_OF_RELAYS) {
				relayIndex = 0;
				powerFsmState = POWER_FINISH_STATE;

//				Led_Display_Show();

//				sprintf((char*) strtmpMain, "i\t\t\t s\t\t\t I\t\t\t PF\t\t\t P\t\t\t E \r\n");
//				UART3_SendToHost((uint8_t*) strtmpMain);
//
//				for (uint8_t i = 0; i < 4; i++) {
//					sprintf((char*) strtmpMain, "%d\t\t\t %d\t\t\t %d\t\t\t\t %d\t\t\t %d\t\t\t %d  \r\n", (int) i, (int) Main.nodes[i].nodeStatus, (int) Main.nodes[i].current,
//							(int) Main.nodes[i].powerFactor, (int) Main.nodes[i].power, (int) Main.nodes[i].energy);
//					UART3_SendToHost((uint8_t*) strtmpMain);
//				}
//
//				UART3_SendToHost((uint8_t*) "\r\n");
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
