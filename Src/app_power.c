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


#include "app_gpio.h"
#include "app_adc.h"
#include "app_power_meter_485.h"
#include "app_fsm.h"


#define	OUTLET_AVAILABLE_STATE 					0
#define OUTLET_STABLIZED_STATE 					4
#define	OUTLET_CHARGING_STATE 					1
#define	OUTLET_ERROR_STATE						3

#define	OUTLET_ERROR_OVER_CURRENT_STATE 		8
#define OUTLET_RELAY_ERROR_STATE				9

#define	OUTLET_UNPLUG_STATE						5
#define OUTLET_CHARGE_FULL_STATE 				6
#define OUTLET_AFTER_CHARGE_FULL_STATE			7
#define OUTLET_PREPARE_TO_AVAILABLE_STATE 		2


#define OVERCURRENT_THRESHOLD_COUNT  			10





#define		DEBUG_POWER(X)							X
#if(VERSION_EBOX == VERSION_3_WITH_ALL_CT_5A)
	#define		MAX_CURRENT								800000
	#define		MAX_CURRENT_1							950000
static uint32_t MAX_POWER  = 85 * 230 * 10; //8.5A, 230V và PF = 100%
static uint32_t MAX_POWER_1  = 16 * 230 * 100; //16A, 230V và PF = 100%
#elif(VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A)
	#define		MAX_CURRENT								700000
	#define		MAX_CURRENT_1							1000000
static uint32_t MAX_POWER  = 85 * 230 * 10; //8.5A, 230V và PF = 100%
static uint32_t MAX_POWER_1  = 16 * 230 * 100; //16A, 230V và PF = 100%
#elif(VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
	#define		MAX_CURRENT								850000
	#define		MAX_CURRENT_1							1600000

static uint32_t MAX_POWER  = 85 * 230 * 10; //8.5A, 230V và PF = 100%
static uint32_t MAX_POWER_1  = 16 * 230 * 100; //16A, 230V và PF = 100%

#elif(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
//for testing
//	#define		MAX_CURRENT								402000
//	#define		MAX_CURRENT_1							402000
//for running
	#define		MAX_CURRENT								1600000
	#define		MAX_CURRENT_1							1600000

static uint32_t MAX_POWER_1  = 16 * 230 * 100; //16A, 230V và PF = 100%

#endif




#define 		MAX_CURRENT_FOR_CABLE_25				16000
#define 		MAX_CURRENT_FOR_CB20					20000
#define 		MAX_CURRENT_FOR_CABLE_40				20000
#define 		MAX_CURRENT_FOR_CABLE_60				31000


static uint32_t 	MAX_TOTAL_CURRENT = MAX_CURRENT_FOR_CABLE_60;		//in miliampere



#if(VERSION_EBOX == VERSION_TEST_EBOX)
#define		MIN_CURRENT								3000
#define		MIN_CURRENT_DETECTING_FULL_CHARGE		3000
#define		MIN_CURRENT_DETECTING_UNPLUG			3000
#define		MIN_CURRENT_FOR_START_CHARGING			3000
#define 	CURRENT_CHANGING_THRESHOLD				300
#define 	MIN_PF									1

#elif(VERSION_EBOX == 2 || VERSION_EBOX == VERSION_3_WITH_ALL_CT_5A || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A || VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A  || VERSION_EBOX == VERSION_6_WITH_8CT_20A)
#define		MIN_CURRENT											40000
#define		MIN_CURRENT_DETECTING_FULL_CHARGE_FOR_SMALL_BIKE	40000
#define		MIN_CURRENT_DETECTING_FULL_CHARGE					50000
#define		THRESHOLD_BETWEEN_SMALL_BIKE_AND_NORMAL_BIKE		200000

#define		MIN_CURRENT_DETECTING_UNPLUG						35000
#define		MIN_CURRENT_DETECTING_UNPLUG_HD_MON					30000

#define		MIN_CURRENT_FOR_START_CHARGING						40000
#define 	CURRENT_CHANGING_THRESHOLD							30000

#if(BOX_PLACE == BOX_AT_XI)
	#define 	MIN_PF									20
#elif(VERSION_EBOX == VERSION_3_WITH_ALL_CT_5A)
	#define 	MIN_PF									20
#else
	#define 	MIN_PF									30
#endif

#elif(VERSION_EBOX == 15)
#define		MIN_CURRENT								10000
#define 	CURRENT_CHANGING_THRESHOLD				10000
#define 	MIN_PF									25
#elif(VERSION_EBOX == 1)
#define		MIN_CURRENT								10000
#define 	CURRENT_CHANGING_THRESHOLD				10000
#define 	MIN_PF									25
#endif

#define		TIME_OUT_1_SECOND											(1000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_STABABILITY										(20000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_DETECT_TOTAL_OVER_CURRRENT					(20000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_UNPLUG										(30000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_DETECTING_NO_FUSE							(20000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_DETECTING_NO_RELAY							(30000/INTERRUPT_TIMER_PERIOD)

#if(ESTOP_BUTTON == 1)
	#define		TIME_OUT_AFTER_DETECTING_ESTOP_PRESSED					(2000/INTERRUPT_TIMER_PERIOD)
#endif


#define		TIME_OUT_AFTER_CHARGE_FULL									(30000/INTERRUPT_TIMER_PERIOD)
#define		TIME_OUT_AFTER_STOP_FROM_APP								(10000/INTERRUPT_TIMER_PERIOD)

#define	    TIME_OUT_AFTER_DETECTING_OVER_CURRENT   					(60000/INTERRUPT_TIMER_PERIOD)

#define	    TIME_OUT_FOR_SYSTEM_OVER_CURRENT							(5000/INTERRUPT_TIMER_PERIOD)
#define		COUNT_FOR_DETECT_MAX_CURRENT								10

#define		COUNT_FOR_DECIDE_UNPLUG										200
#define		COUNT_FOR_DECIDE_CHARGE_FULL								210


typedef struct PowerNodes {
	NodeStatus  nodeStatus;			//avaiable = 0; charging = 1;	chargefull = 2
	uint32_t current;	// in uA
	uint32_t previousCurrent;
	uint32_t previousCurrent_1;
	uint32_t previousCurrent_2;
	uint32_t maxNodeCurrent;
	uint16_t voltage;	// = 220
//	uint8_t frequency;	// = 50 in Hz
	uint8_t powerFactor; // = 1pF
	uint32_t lastPower;
	uint32_t power;		// in mW
	uint32_t workingTime;		//from start to now
	uint32_t preEnergy;
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
	uint64_t previousEnergy;
	uint64_t energy;
	uint64_t limitEnergy;
	uint32_t workingTime;
} PowerSystem;

PowerSystem Main;
POWER_FSM_STATE powerFsmState = POWER_FINISH_STATE;


uint32_t lastTimeErr, lastReport;

uint8_t strtmpPower[] = "                                                                              ";
static uint8_t power_TimeoutFlag[NUMBER_OF_RELAYS];
static uint32_t power_Timeout_Task_ID[NUMBER_OF_RELAYS];
static NodeStatus previousOutletStatus[NUMBER_OF_RELAYS];


static uint8_t power_Input_Source_TimeoutFlag = 0;
static uint32_t power_Input_Source_Task_ID = NO_TASK_ID;


static uint8_t outletState[NUMBER_OF_RELAYS] = {0,0,0,0,0,0,0,0};
static uint32_t outletCounter[NUMBER_OF_RELAYS] = {0,0,0,0,0,0,0,0};
static uint32_t outletCounterForUnplug[NUMBER_OF_RELAYS] = {0,0,0,0,0,0,0,0};

static uint32_t outletCounterMaxCurrent[NUMBER_OF_RELAYS];
static FlagStatus is_Node_Status_Changed = RESET;

static uint8_t chargingFullStatus[NUMBER_OF_RELAYS];

static uint8_t isStartCalculating[NUMBER_OF_RELAYS] = {0,0,0,0,0,0,0,0};

typedef struct
{
    uint16_t boxID;
    uint32_t maxCurrent;
} BoxCurrentConfig_t;

static const BoxCurrentConfig_t boxCurrentTable[] =
{
    { PEGASUS_BIENHOA_222,    MAX_CURRENT_FOR_CABLE_25 },
    { TARA_327,               MAX_CURRENT_FOR_CABLE_40 },
    { TARA_328,               MAX_CURRENT_FOR_CABLE_40 },
    { EHOME_3_BLOCK_A0_375,   MAX_CURRENT_FOR_CB20 },
};



void DelayReadingCurrent(uint8_t outletID);


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

#if(VERSION_EBOX != VERSION_6_WITH_8CT_20A)
void Power_Set_Timeout_Flag_8(void);
void Power_Set_Timeout_Flag_9(void);
#endif

void Set_MAX_TOTAL_CURRENT(void);
uint8_t Is_Power_Timeout_Flag(uint8_t outletID);
uint8_t Is_Main_Current_Over_Max_Current(void);

void Power_Init(void);

uint8_t isNewCurrents(uint8_t outletID);
void Process_Outlets(void);
SystemStatus Process_Input_Source(void);
void Process_Main_Currrent_Over_Max_Current(void);

void Set_Charging_Full_Status(uint8_t outletID);
uint8_t Is_Charging_Full_Status(uint8_t outletID);


void Update_Max_Node_Current(uint8_t index, uint32_t current);
void Checking_DMA_ADC_Reading(void);

void Node_Over_Current_Detected(uint8_t outletID);
void Detect_Stop_From_App(uint8_t outletID);
void Detect_Stop_From_App(uint8_t outletID);
void Detect_Charge_Full(uint8_t outletID, uint32_t threshold);

void Detect_Spike(uint8_t outletID, uint32_t threshold);
void Detect_Un_Plug(uint8_t outletID);

/////////////////////////////////////////////////////////////

void Power_Init(void){
	for(uint8_t idx = 0; idx < NUMBER_OF_RELAYS; idx++){
		power_Timeout_Task_ID[idx] = NO_TASK_ID;
		outletState[idx] = OUTLET_AVAILABLE_STATE;
		outletCounter[idx] = 0;
		outletCounterMaxCurrent[idx] = 0;
		previousOutletStatus[idx] = NODE_NORMAL;
		chargingFullStatus[idx] = 0;
	}
	Set_MAX_TOTAL_CURRENT();
}

void Clear_Charging_Full_Status(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS){
		chargingFullStatus[outletID] = 0;
	}
}
void Set_Charging_Full_Status(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS)
		chargingFullStatus[outletID] = 1;
}
uint8_t Is_Charging_Full_Status(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS)
		return chargingFullStatus[outletID];
	return 0;
}


void Set_MAX_TOTAL_CURRENT(void)
{
    uint16_t boxID = Get_Box_ID();

    MAX_TOTAL_CURRENT = MAX_CURRENT_FOR_CABLE_60;    // Default

    for (uint32_t i = 0; i < sizeof(boxCurrentTable)/sizeof(boxCurrentTable[0]); i++)
    {
        if (boxCurrentTable[i].boxID == boxID)
        {
            MAX_TOTAL_CURRENT = boxCurrentTable[i].maxCurrent;
            break;
        }
    }
}


//void Set_MAX_TOTAL_CURRENT(void){
//	uint16_t boxID = Get_Box_ID();
//	if(boxID == PEGASUS_BIENHOA_222){
//		MAX_TOTAL_CURRENT = MAX_CURRENT_FOR_CABLE_25; //in miliampere
//	}
//	else if (boxID == TARA_327 || boxID == TARA_328)
//	{
//		MAX_TOTAL_CURRENT = MAX_CURRENT_FOR_CABLE_40; //in miliampere
//	}
//	else if (boxID == EHOME_3_BLOCK_A0_375){
//		MAX_TOTAL_CURRENT = MAX_CURRENT_FOR_CB20;
//	}
//	else {
//		MAX_TOTAL_CURRENT = MAX_CURRENT_FOR_CABLE_60; //in miliampere
//	}
//}


void Process_System_Power(void){
	if(Process_Input_Source() == SYSTEM_NORMAL){
		Process_Outlets();
		Checking_DMA_ADC_Reading();
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

#if(VERSION_EBOX != VERSION_6_WITH_8CT_20A)
void Set_Power_Timeout_Flag_8(void){
	power_TimeoutFlag[8] = 1;
}
void Set_Power_Timeout_Flag_9(void){
	power_TimeoutFlag[9] = 1;
}
#endif

uint8_t Is_Power_Timeout_Flag(uint8_t outletID){
	if(outletID >= NUMBER_OF_RELAYS) return 0;
	uint8_t tempStatus = power_TimeoutFlag[outletID];
	if(tempStatus == 1)
		Clear_Power_Timeout_Flag(outletID);
	return tempStatus;
}

void Set_Power_Timeout_Flags(uint8_t outletID, uint32_t PowerTimeOut){

	if(outletID >= NUMBER_OF_RELAYS) return;

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
	}
#if(VERSION_EBOX != VERSION_6_WITH_8CT_20A)
	else if(outletID == 8){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_8, PowerTimeOut, 0);
	} else if(outletID == 9){
		power_Timeout_Task_ID[outletID] = SCH_Add_Task(Set_Power_Timeout_Flag_9, PowerTimeOut, 0);
	}
#endif
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
			{
				Main.nodes[outletID].energy = Eeprom_Get_Outlet_Energy(outletID);
				Main.nodes[outletID].nodeStatus = NODE_NORMAL;
			}
			sprintf((char*) strtmpPower, "i:%d\t s:%d\t e:%lu\r\n", (int) outletID, (int)Main.nodes[outletID].nodeStatus,
					(uint32_t)Main.nodes[outletID].energy);
			UART3_SendToHost((uint8_t *)strtmpPower);
			Main.nodes[outletID].power = 0;
			Main.nodes[outletID].current = 0;
			power_TimeoutFlag[outletID] = 0;
			power_Timeout_Task_ID[outletID] = NO_TASK_ID;
		}

	}
}



void Start_Working_Time(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS){
		Main.nodes[outletID].workingTime = 0;
	}
}

uint8_t Is_Charging_More_Than_An_Hour(uint8_t outletID, uint32_t number_of_seconds){
	if(outletID < NUMBER_OF_RELAYS){
		if(Main.nodes[outletID].workingTime >= number_of_seconds){
			return 1;
		} 
		return 0;
	} else {
		return 0;
	}	
}


void Clear_Max_Node_Current(uint8_t outletID){
	if(outletID < NUMBER_OF_RELAYS){
		Main.nodes[outletID].maxNodeCurrent = 0;
	}
			
}

uint64_t Get_Main_Power_Consumption(void)
{
	return (uint64_t)(Main.energy);
}

void Set_Outlet_Energy(uint8_t outletID, uint32_t outlet_energy){
	if(outletID >= NUMBER_OF_RELAYS) return;
	Main.nodes[outletID].energy = outlet_energy;
	sprintf((char*) strtmpPower, "Set_Outlet_Energy:%d\t s:%d\t e:%lu\r\n", (int) outletID, (int)Main.nodes[outletID].energy, outlet_energy);
	UART3_SendToHost((uint8_t *)strtmpPower);

}

void Set_Main_Power_Consumption(uint64_t totalPowerConsumption){
	Main.energy = (uint64_t)totalPowerConsumption;
	Eeprom_Update_Main_Energy_Immediately(Main.energy);
}

uint8_t Get_Main_Power_Factor(void){
	return Main.powerFactor;
}
//in milliampere
uint32_t Get_Main_Current(void){
	Main.currentTotal = 0;
	for(uint8_t outletIndex = 0; outletIndex < NUMBER_OF_RELAYS; outletIndex ++){
		Main.currentTotal = Main.currentTotal + Get_Current(outletIndex);
	}
	return Main.currentTotal;
}

uint8_t Is_Main_Current_Over_Max_Current(void)
{
    static uint32_t last_check_tick = 0;
    static uint8_t over_count = 0;

    uint32_t now = HAL_GetTick();

    if ((uint32_t)(now - last_check_tick) < 1000U)
    {
        return 0;
    }

    last_check_tick = now;

    uint16_t current = PowerCurrent();

    if (current <= 0)
    {
        current = Get_Main_Current();
    }

    if (current > MAX_TOTAL_CURRENT)
    {

        if (++over_count >= OVERCURRENT_THRESHOLD_COUNT)
        {
            over_count = 0;
            return 1;
        }
    }
    else
    {
        over_count = 0;
    }

    return 0;
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
	float tempCurrent = 0.0;
	float tempPower = 0.0;
	uint32_t tempEnergy = 0;
	static uint8_t counter_to_reset_energy_outlet[NUMBER_OF_RELAYS] = {0,0,0,0,0,0,0,0};

	if (outletID == MAIN_INPUT) {	//setup for Main node
		Main.energy +=  POWER_CONSUMPTION_OF_MCU;
		Eeprom_Update_Main_Energy(Main.energy);
		Main.workingTime++;
	} else if (outletID < MAIN_INPUT) {
		uint8_t tempOutletID = outletID;
		DelayReadingCurrent(tempOutletID);
		Main.nodes[tempOutletID].previousCurrent = Main.nodes[tempOutletID].previousCurrent_2;
		Main.nodes[tempOutletID].previousCurrent_2 = Main.nodes[tempOutletID].previousCurrent_1;
		Main.nodes[tempOutletID].previousCurrent_1 = Main.nodes[tempOutletID].current;


		if ((Get_Relay_Status(tempOutletID) == RESET) ||
				(power_factor < MIN_PF) ||
				(isStartCalculating[tempOutletID] == 0))
		{
			Main.nodes[tempOutletID].current = 0;
			Main.nodes[tempOutletID].powerFactor = 0;
		} else	{
			Main.nodes[tempOutletID].current = current;
			Main.nodes[tempOutletID].powerFactor = power_factor;
		}

		Main.nodes[tempOutletID].voltage = PowerVoltage();

#if (VERSION_EBOX == VERSION_6_WITH_8CT_20A)
		if(Main.nodes[tempOutletID].current < MAX_CURRENT_1 && (Main.nodes[tempOutletID].nodeStatus != NODE_READY)){
			tempCurrent = (float)(Main.nodes[tempOutletID].current)/100000;//in mA	// /100
			tempPower = (float)(Main.nodes[tempOutletID].voltage * tempCurrent * Main.nodes[tempOutletID].powerFactor);
		} else {
			tempPower = 0.0;
		}

		if (isStartCalculating[tempOutletID] == 1){

			counter_to_reset_energy_outlet[tempOutletID] = 0;

			Main.nodes[tempOutletID].power = round((float)tempPower);
			if(Main.nodes[tempOutletID].power < MAX_POWER_1){

				tempEnergy = Main.nodes[tempOutletID].power/100;

				Main.energy += tempEnergy;
				Main.nodes[tempOutletID].energy = Main.nodes[tempOutletID].energy + tempEnergy;

				Main.nodes[tempOutletID].workingTime++;
				Eeprom_Update_Outlet_Energy(tempOutletID, Main.nodes[tempOutletID].energy, 0);
			}
		} else {
			if(Is_Publishing_Message()){
				if(counter_to_reset_energy_outlet[tempOutletID] < 60){
					counter_to_reset_energy_outlet[tempOutletID]++;
				} else {
					Main.nodes[tempOutletID].energy = 0;
				}
			}
		}
#else
		if(tempOutletID >= 2){
			if(Main.nodes[tempOutletID].current < MAX_CURRENT){
				tempCurrent = (float)(Main.nodes[tempOutletID].current)/100000;//in mA	// /100
				tempPower = (float)(Main.nodes[tempOutletID].voltage * tempCurrent * Main.nodes[tempOutletID].powerFactor);
			} else {
				tempPower = 0.0;
			}

			if (isStartCalculating[tempOutletID] == 1){

				Main.nodes[tempOutletID].power = round((float)tempPower);
				if(Main.nodes[tempOutletID].power < MAX_POWER){

					tempEnergy = Main.nodes[tempOutletID].power/100;

					Main.energy += tempEnergy;
					Main.nodes[tempOutletID].energy = Main.nodes[tempOutletID].energy + tempEnergy;

					Main.nodes[tempOutletID].workingTime++;
//					Eeprom_Update_Outlet_Energy(tempOutletID, Main.nodes[tempOutletID].energy);
				}

			}
		} else {
			if(Main.nodes[tempOutletID].current < MAX_CURRENT_1){
				tempCurrent = (float)(Main.nodes[tempOutletID].current)/100000;//in mA	// /100
				tempPower = (float)(Main.nodes[tempOutletID].voltage * tempCurrent * Main.nodes[tempOutletID].powerFactor);
			} else {
				tempPower = 0.0;
			}

			if (isStartCalculating[tempOutletID] == 1){

				Main.nodes[tempOutletID].power = round((float)tempPower);
				if(Main.nodes[tempOutletID].power < MAX_POWER_1){

					tempEnergy = Main.nodes[tempOutletID].power/100;

					Main.energy += tempEnergy;
					Main.nodes[tempOutletID].energy = Main.nodes[tempOutletID].energy + tempEnergy;

					Main.nodes[tempOutletID].workingTime++;
//					Eeprom_Update_Outlet_Energy(tempOutletID, Main.nodes[tempOutletID].energy);
				}

			} else {
				Main.nodes[tempOutletID].energy = 0;
				Main.nodes[tempOutletID].workingTime = 0;
			}
		}

#endif
		Update_Max_Node_Current(tempOutletID, Main.nodes[tempOutletID].current);
		if(tempOutletID == 0){
//			DEBUG_POWER(sprintf((char*) strtmpPower, "%d\t", (int) tempOutletID););
//			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
////			DEBUG_POWER(sprintf((char*) strtmpPower, "pf:%d\t", (int) Main.nodes[tempOutletID].powerFactor););
////			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
////			DEBUG_POWER(sprintf((char*) strtmpPower, "mNC:%d\t", (int) Main.nodes[tempOutletID].maxNodeCurrent););
////			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
//			DEBUG_POWER(sprintf((char*) strtmpPower, "t1:%d:%d\t", (int) Main.workingTime/60, (int) Main.workingTime%60););
//			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
////			DEBUG_POWER(sprintf((char*) strtmpPower, "v:%d\t", (int) Main.nodes[tempOutletID].voltage););
////			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
////
////			DEBUG_POWER(sprintf((char*) strtmpPower, "rc:%d\t", (int) current););
////			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
////			DEBUG_POWER(sprintf((char*) strtmpPower, "c:%d\t", (int) Main.nodes[tempOutletID].current););
////			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
////			DEBUG_POWER(sprintf((char*) strtmpPower, "p:%d\t", (int) Main.nodes[tempOutletID].power););
////			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
////
////			DEBUG_POWER(sprintf((char*) strtmpPower, "ne:%d\r\n", (int) Main.nodes[tempOutletID].energy););
////			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
//			DEBUG_POWER(UART3_SendToHost((uint8_t *)"\r\n"););
		}
	}
}

static uint8_t preStatus[NUMBER_OF_RELAYS];
static uint8_t preStatus1[NUMBER_OF_RELAYS];
static uint8_t preStatus2[NUMBER_OF_RELAYS];
static uint8_t preStatus3[NUMBER_OF_RELAYS];

static uint8_t curStatus[NUMBER_OF_RELAYS];

void DelayReadingCurrent(uint8_t outletID){
	if (Get_Relay_Status(outletID) == RESET){

		isStartCalculating[outletID] = 0;

		preStatus3[outletID] = SET;
		preStatus2[outletID] = SET;
		preStatus1[outletID] = SET;
		preStatus[outletID] = SET;
		curStatus[outletID] = RESET;
	}
	else{
		preStatus3[outletID] = preStatus2[outletID];
		preStatus2[outletID] = preStatus1[outletID];
		preStatus1[outletID] = preStatus[outletID];
		preStatus[outletID] = curStatus[outletID];
		curStatus[outletID] = Get_Relay_Status(outletID);

		if(preStatus3[outletID] == RESET && curStatus[outletID] == SET){
			isStartCalculating[outletID] = 1;
		}
	}
}

void Update_Max_Node_Current(uint8_t index, uint32_t current){
	if (isStartCalculating[index] == 1){
		if(Main.nodes[index].maxNodeCurrent < current){
			Main.nodes[index].maxNodeCurrent = current;
		}
	} else{
		Main.nodes[index].maxNodeCurrent = 0;
	}

}

void Power_Setup(void) {
	powerFsmState = POWER_FINISH_STATE;
	Node_Setup();
	Led_Display_Clear_All();
	Power_Init();
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
			} else {
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

	if(previousOutletStatus[outletID] != Main.nodes[outletID].nodeStatus){
		previousOutletStatus[outletID] = Main.nodes[outletID].nodeStatus;
		is_Node_Status_Changed = SET;

		switch(Main.nodes[outletID].nodeStatus){
		case NODE_NORMAL:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NODE_NORMAL=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case NODE_READY:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NODE_READY=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case CHARGING:
			DEBUG_POWER(sprintf((char*) strtmpPower, "CHARGING=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
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
		case TOTAL_OVER_CURRRENT:
			DEBUG_POWER(sprintf((char*) strtmpPower, "G_M_C() = %d, M_TO_C = %d, relayIndex = %d\r\n", (int) Get_Main_Current(), (int)MAX_TOTAL_CURRENT, (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););

			DEBUG_POWER(sprintf((char*) strtmpPower, "TOTAL_OVER_CURRRENT=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case NODE_OVER_MONEY:
			DEBUG_POWER(sprintf((char*) strtmpPower, "NODE_OVER_MONEY=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case NODE_OVER_TIME:
			break;
		case RELAY_BROKEN:
			DEBUG_POWER(sprintf((char*) strtmpPower, "RELAY_BROKEN=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
			break;
		case ESTOP_PRESSED:
			DEBUG_POWER(sprintf((char*) strtmpPower, "ESTOP_PRESSED=%d\r\n", (int) outletID););
			DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););

			break;
		default:
			break;
		}
	}
}


uint8_t isChargingInProgress(void){
	uint8_t outletID;
	for (outletID = 0; outletID < NUMBER_OF_RELAYS; outletID++){
		if(Main.nodes[outletID].nodeStatus == CHARGING || Main.nodes[outletID].nodeStatus == NODE_READY){
			return 1;
		}
	}
	return 0;
}


void Node_Over_Current_Detected(uint8_t outletID){
	Main.nodes[outletID].nodeStatus = NODE_OVER_CURRENT;
	Set_Power_Timeout_Flags(outletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
	Reset_Relay(outletID);
	outletState[outletID] = OUTLET_ERROR_OVER_CURRENT_STATE;
}
void Detect_Stop_From_App(uint8_t outletID){
	Main.nodes[outletID].nodeStatus = CHARGEFULL;
	outletCounter[outletID] = 0;
	Set_Power_Timeout_Flags(outletID, TIME_OUT_AFTER_STOP_FROM_APP);
	outletState[outletID] = OUTLET_PREPARE_TO_AVAILABLE_STATE;
}

void Detect_Charge_Full(uint8_t outletID, uint32_t threshold)
{

	if(outletID >= NUMBER_OF_RELAYS)
		return;

	uint32_t current = Get_Current(outletID);

	uint8_t pf = Get_Power_Factor(outletID);

	// sanity check
	if(current > MAX_CURRENT || pf > 100)
		return;

	uint32_t realCurrent = current * pf;


	if (realCurrent < threshold)
	{
		outletCounter[outletID]++;
		if(outletCounter[outletID] >= COUNT_FOR_DECIDE_CHARGE_FULL)
		{
			outletCounter[outletID] = 0;
			Set_Power_Timeout_Flags(outletID, TIME_OUT_AFTER_CHARGE_FULL);
			Main.nodes[outletID].nodeStatus = CHARGEFULL;
			Set_Charging_Full_Status(outletID);
			outletState[outletID] = OUTLET_CHARGE_FULL_STATE;
		}
	} else if (realCurrent > threshold ) {
		if(outletCounter[outletID] > 0)
			outletCounter[outletID]--;
		else
			outletCounter[outletID] = 0;
	}
}

void Detect_Spike(uint8_t outletID, uint32_t threshold){

	if(Main.nodes[outletID].current < MIN_CURRENT_DETECTING_UNPLUG){
		int32_t tempDefference = (int32_t)(Main.nodes[outletID].previousCurrent - Main.nodes[outletID].current);
		if ((int32_t)tempDefference > (int32_t)threshold)	{
			outletCounter[outletID] = 0;
			if(Is_Charging_Full_Status(outletID)){
				Main.nodes[outletID].nodeStatus = CHARGEFULL;
				Set_Power_Timeout_Flags(outletID, TIME_OUT_AFTER_CHARGE_FULL);
				outletState[outletID] = OUTLET_CHARGE_FULL_STATE;
			} else {
				outletCounterForUnplug[outletID] = 0;
				outletState[outletID] = OUTLET_UNPLUG_STATE;
			}
		}
	}
}

void Detect_Un_Plug(uint8_t outletID){

	if(Main.nodes[outletID].previousCurrent < MIN_CURRENT_DETECTING_UNPLUG
			&& Main.nodes[outletID].current < MIN_CURRENT_DETECTING_UNPLUG){
		outletCounterForUnplug[outletID]++;
		if(outletCounterForUnplug[outletID] > 5){
			Main.nodes[outletID].nodeStatus = UNPLUG;
			Set_Power_Timeout_Flags(outletID, TIME_OUT_AFTER_UNPLUG);
			outletState[outletID] = OUTLET_PREPARE_TO_AVAILABLE_STATE;
		}
	}
	else if (Main.nodes[outletID].previousCurrent >= MIN_CURRENT_DETECTING_UNPLUG
			&&  Main.nodes[outletID].current >= MIN_CURRENT_DETECTING_UNPLUG)
	{
		Main.nodes[outletID].nodeStatus = CHARGING;
		outletState[outletID] = OUTLET_CHARGING_STATE;
	}
}

uint8_t isNewCurrents(uint8_t outletID){

	if(outletID >= NUMBER_OF_RELAYS)
		return 0;

	static uint32_t previousWorkingTime[NUMBER_OF_RELAYS];
	if(previousWorkingTime[outletID] != Main.workingTime){
		previousWorkingTime[outletID] = Main.workingTime;
		return 1;
	}
	return 0;

}


void Checking_DMA_ADC_Reading(void)
{
    static uint16_t fault_counter = 0;
    static uint32_t last_recover_tick = 0;

    uint32_t now = HAL_GetTick();
    if (now - last_recover_tick < 1000){
    	return;
    }else {
    	last_recover_tick = now;
    }

    if (!isChargingInProgress()) {
        fault_counter = 0;
        return;
    }

    // Detect abnormal condition
    bool is_fault = (Get_Main_Current() < 100) && (PowerCurrent() > 500);

    if (!is_fault) {
        fault_counter = 0;
        return;
    }

    fault_counter++;

    // 🟡 Step 1: Try recovery every 20000ms
    if (fault_counter%20 == 0) {
        ZeroPoint_Detection_Pin_Clear_Interrupt_Flag();
        ADC_Recover();

    }

    // 🔴 Step 2: Hard reset if persistent failure
    if (fault_counter >= 100) {
        fault_counter = 0;
        NVIC_SystemReset();
    }
}

void Process_Outlets(void){
	static uint8_t tempOutletID = 0;
//	int32_t tempDefference;
//	uint32_t tempRelayFuseStatuses = Get_All_Relay_Fuse_Statuses();

	Display_OutLet_Status(tempOutletID);
#if(ESTOP_BUTTON == 1)
	if(isEstopPressed()){
		Main.nodes[tempOutletID].nodeStatus = ESTOP_PRESSED;
		Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_ESTOP_PRESSED);
		outletState[tempOutletID] = OUTLET_ERROR_STATE;
	}
	else
#endif

	if (isNoFuseAvailable(tempOutletID)){
		if(Get_Relay_Status(tempOutletID) == RESET) {	//return NOFUSE
			if(outletState[tempOutletID] != OUTLET_ERROR_STATE){
				Main.nodes[tempOutletID].nodeStatus = NO_FUSE;
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_NO_FUSE);
				outletState[tempOutletID] = OUTLET_ERROR_STATE;
			}
		}
	}
	else if (isRelayOff(tempOutletID)
			&& (Get_Relay_Status(tempOutletID) == SET)
			&& is_Set_Relay_Timeout()
			&& Main.nodes[tempOutletID].current < MIN_CURRENT) {	//relay not working MUST and is Working
		if(outletState[tempOutletID] != OUTLET_RELAY_ERROR_STATE && outletState[tempOutletID] != OUTLET_ERROR_STATE){
//				Main.nodes[tempOutletID].nodeStatus = NO_RELAY;
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_NO_RELAY*10);
				outletState[tempOutletID] = OUTLET_RELAY_ERROR_STATE;
		}
	}

	else if(isRelayOn(tempOutletID)
			&& (Get_Relay_Status(tempOutletID) == RESET)
			&& is_Set_Relay_Timeout()){
		if(outletState[tempOutletID] != OUTLET_ERROR_STATE){
			Main.nodes[tempOutletID].nodeStatus = RELAY_BROKEN;
			Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_NO_RELAY);
			outletState[tempOutletID] = OUTLET_ERROR_STATE;
		}
	}

#if (VERSION_EBOX == VERSION_3_WITH_ALL_CT_5A)
	else if (Main.nodes[tempOutletID].current > MAX_CURRENT) {	// nodeValue from 0 to 1860
		if(tempOutletID == 0 || tempOutletID == 1){
			if (Main.nodes[tempOutletID].current > MAX_CURRENT_1){
				Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
				Reset_Relay(tempOutletID);
				outletState[tempOutletID] = OUTLET_ERROR_STATE;
			}
		} else {
			Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
			Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
			Reset_Relay(tempOutletID);
			outletState[tempOutletID] = OUTLET_ERROR_STATE;
		}
	}
#elif(VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A)
	else if (Main.nodes[tempOutletID].current > MAX_CURRENT) {	// nodeValue from 0 to 1860
		if(tempOutletID == CT_10A_1 || tempOutletID == CT_10A_2){
			if (Main.nodes[tempOutletID].current > MAX_CURRENT_1){
				Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
				Reset_Relay(tempOutletID);
				outletState[tempOutletID] = OUTLET_ERROR_STATE;
			}
		} else {
			Main.nodes[tempOutletID].nodeStatus = NODE_OVER_CURRENT;
			Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
			Reset_Relay(tempOutletID);
			outletState[tempOutletID] = OUTLET_ERROR_STATE;
		}
	}
#elif(VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
	else if (Main.nodes[tempOutletID].current > MAX_CURRENT) {	// nodeValue from 0 to 1860
		if(isNewCurrents(tempOutletID) == 1){
			if(tempOutletID == CT_20A_1 || tempOutletID == CT_20A_2){
				if (Main.nodes[tempOutletID].current > MAX_CURRENT_1){
					outletCounterMaxCurrent[tempOutletID] ++;
					if(outletCounterMaxCurrent[tempOutletID] > COUNT_FOR_DETECT_MAX_CURRENT){
						Node_Over_Current_Detected(tempOutletID);
					}
				}
			} else {
				outletCounterMaxCurrent[tempOutletID] ++;
				if(outletCounterMaxCurrent[tempOutletID] > COUNT_FOR_DETECT_MAX_CURRENT){
					Node_Over_Current_Detected(tempOutletID);
				}
			}
		}
	}
#elif (VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	else if (Main.nodes[tempOutletID].current > MAX_CURRENT) {
		if(isNewCurrents(tempOutletID) == 1){
			if (Main.nodes[tempOutletID].current > MAX_CURRENT_1) {
				outletCounterMaxCurrent[tempOutletID] ++;
				ADC_Recover();
				if(outletCounterMaxCurrent[tempOutletID] > COUNT_FOR_DETECT_MAX_CURRENT) {
					if(PowerCurrent() < MAX_CURRENT_1){ // if power meter measure lower than sensor, it means sensor reading has an issue, need to reset.
						DEBUG_POWER(sprintf((char*) strtmpPower, "over current = %d\r\n", (int) tempOutletID););
						DEBUG_POWER(UART3_SendToHost((uint8_t *)strtmpPower););
						NVIC_SystemReset();//reset
					} else {
						Node_Over_Current_Detected(tempOutletID);
					}
				}
			}
		}
	}
#endif

	else {
		outletCounterMaxCurrent[tempOutletID] = 0;
		switch(outletState[tempOutletID]){
		case OUTLET_AVAILABLE_STATE:
			 if (Main.nodes[tempOutletID].current >= MIN_CURRENT_FOR_START_CHARGING && Main.nodes[tempOutletID].previousCurrent >= MIN_CURRENT_FOR_START_CHARGING) {
				Main.nodes[tempOutletID].nodeStatus = CHARGING;
				outletCounter[tempOutletID] = 0;
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_STABABILITY);
				outletState[tempOutletID] = OUTLET_STABLIZED_STATE;
			} else {
				if(Get_Relay_Status(tempOutletID) == SET){
					Main.nodes[tempOutletID].nodeStatus = NODE_READY;
				} else {
					Main.nodes[tempOutletID].nodeStatus = NODE_NORMAL;
				}
			}
			break;
		case OUTLET_STABLIZED_STATE:
			if(Is_Power_Timeout_Flag(tempOutletID)){
				outletState[tempOutletID] = OUTLET_CHARGING_STATE;
			} else if(Get_Relay_Status(tempOutletID) == RESET){
				outletState[tempOutletID] = OUTLET_AVAILABLE_STATE;
			}
			break;
		case OUTLET_CHARGING_STATE:
			if(Main.nodes[tempOutletID].nodeStatus == TOTAL_OVER_CURRRENT) {
				Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECT_TOTAL_OVER_CURRRENT);
				outletState[tempOutletID] = OUTLET_ERROR_STATE;
			} else if(Main.nodes[tempOutletID].nodeStatus == CHARGING) {
				if(isNewCurrents(tempOutletID) == 1){
					if(Get_Relay_Status(tempOutletID) == SET){
						if(Is_Charging_More_Than_An_Hour(tempOutletID, 30*60)) {
							Detect_Spike(tempOutletID, Main.nodes[tempOutletID].maxNodeCurrent*2/3);
						} else {
							Detect_Spike(tempOutletID, Main.nodes[tempOutletID].maxNodeCurrent*3/4);
						}

						if(Main.nodes[tempOutletID].maxNodeCurrent > THRESHOLD_BETWEEN_SMALL_BIKE_AND_NORMAL_BIKE){
							Detect_Charge_Full(tempOutletID, MIN_CURRENT_DETECTING_FULL_CHARGE);
						}
						else {
							Detect_Charge_Full(tempOutletID, MIN_CURRENT_DETECTING_FULL_CHARGE_FOR_SMALL_BIKE);
						}

					} else {
						Detect_Stop_From_App(tempOutletID);
					}
				}
			}
			break;

		case OUTLET_CHARGE_FULL_STATE:
			if(Is_Power_Timeout_Flag(tempOutletID)){
				outletState[tempOutletID] = OUTLET_AFTER_CHARGE_FULL_STATE;
			}
			if(Get_Relay_Status(tempOutletID) == RESET){
				Detect_Stop_From_App(tempOutletID);
			}
			break;
		case OUTLET_AFTER_CHARGE_FULL_STATE:
			if(Get_Relay_Status(tempOutletID) == SET){
				uint32_t tempThreshold = 0;

				if(Is_Charging_More_Than_An_Hour(tempOutletID, 6*60*60)){
					tempThreshold = MIN_CURRENT*3;
				}
				else if(Is_Charging_More_Than_An_Hour(tempOutletID, 3*60*60)){
					tempThreshold = MIN_CURRENT*2;
				}else {
					tempThreshold = MIN_CURRENT;
				}
				if (Main.nodes[tempOutletID].current >= tempThreshold) {
					Main.nodes[tempOutletID].nodeStatus = CHARGING;
					outletCounter[tempOutletID] = 0;
					outletState[tempOutletID] = OUTLET_CHARGING_STATE;
				} else {
					Main.nodes[tempOutletID].nodeStatus = NODE_READY;
				}
			} else {
				Detect_Stop_From_App(tempOutletID);
			}
			break;
		case OUTLET_UNPLUG_STATE:
			if(isNewCurrents(tempOutletID) == 1){
				Detect_Un_Plug(tempOutletID);
			}

			break;

		case OUTLET_ERROR_STATE:
			if(Is_Power_Timeout_Flag(tempOutletID)){
				outletState[tempOutletID] = OUTLET_AVAILABLE_STATE;
			}
			break;
		case OUTLET_ERROR_OVER_CURRENT_STATE:
			if(Is_Power_Timeout_Flag(tempOutletID)){
				outletState[tempOutletID] = OUTLET_AVAILABLE_STATE;

			}
			break;
		case OUTLET_RELAY_ERROR_STATE:
			if (Main.nodes[tempOutletID].current > MIN_CURRENT){
				outletState[tempOutletID] = OUTLET_AVAILABLE_STATE;
			}
			if(Is_Power_Timeout_Flag(tempOutletID)){
				if (isRelayOff(tempOutletID)
					&& (Get_Relay_Status(tempOutletID) == SET)
					&& Main.nodes[tempOutletID].current < MIN_CURRENT){

					Main.nodes[tempOutletID].nodeStatus = NO_RELAY;
					Set_Power_Timeout_Flags(tempOutletID, TIME_OUT_AFTER_DETECTING_NO_RELAY);
					outletState[tempOutletID] = OUTLET_ERROR_STATE;
				} else {
					outletState[tempOutletID] = OUTLET_AVAILABLE_STATE;
				}

			}
			break;
		case OUTLET_PREPARE_TO_AVAILABLE_STATE:
			if(Is_Power_Timeout_Flag(tempOutletID)){
				outletState[tempOutletID] = OUTLET_AVAILABLE_STATE;
			}
			break;
		default:
			outletState[tempOutletID] = OUTLET_AVAILABLE_STATE;
			break;
		}
	}
	Led_Update_Status_Buffer(tempOutletID, Main.nodes[tempOutletID].nodeStatus);
	tempOutletID = (tempOutletID + 1) % NUMBER_OF_RELAYS;
}

void Process_Main_Current_Over_Max_Current(void){
	uint8_t relayIndex = NUMBER_OF_RELAYS;
	if(Is_Main_Current_Over_Max_Current() == 1)
	{
		relayIndex = Reset_Latest_Relay();
		if (relayIndex >= NUMBER_OF_RELAYS) {
			return;
		}

		Main.nodes[relayIndex].nodeStatus = TOTAL_OVER_CURRRENT;
		Set_Power_Timeout_Flags(relayIndex, TIME_OUT_AFTER_DETECTING_OVER_CURRENT);
		outletState[relayIndex] = OUTLET_ERROR_STATE;
		Clear_Counter_For_Checking_Total_Current();

	} else if(Is_Timeout_For_Checking_Total_Current() == 0){
		Increase_Counter_For_Checking_Total_Current();
	} else {
		Clear_Latest_Relay_Buffer();
		Clear_Counter_For_Checking_Total_Current();
	}
}





