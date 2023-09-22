/*
 * app_fsm.c
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_sim3g.h"
#include "app_sim5320MQTT.h"
#include "app_scheduler.h"
#include "app_uart.h"
#include "app_test.h"
#include "app_relay.h"
#include "app_gpio.h"
#include "app_power.h"
#include "app_i2c_lcd.h"

#include "app_send_sms.h"
#include "app_iwatchdog.h"
#include "app_version.h"

#include "app_string.h"

#include "app_led_display.h"


#define		TIME_FOR_PING_REQUEST		6000 //5s
#define		TIME_FOR_PUBLISH_MESSAGE	300 //5s


#define 	TIME_FOR_BUZZER				3


extern uint8_t PUBLISH_TOPIC_STATUS[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_FOTA_STATUS[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_POWER[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_VOLTAGE[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_CURRENT[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_POWERFACTOR[MAX_TOPIC_LENGTH];


extern uint8_t publish_message[MQTT_MESSAGE_BUFFER_LENGTH];
extern uint8_t publishTopicIndex;
extern uint8_t publish_message_length;

extern int32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_ADC_CHANNELS];

extern FlagStatus array_Of_Outlet_Status[NUMBER_OF_ADC_CHANNELS];

uint8_t ping_Request_TimeoutFlag = 0;
uint32_t ping_Request_TimeoutIndex = NO_TASK_ID;

uint8_t publish_message_TimeoutFlag = 0;
uint32_t publish_message_TimeoutIndex = NO_TASK_ID;

uint8_t update_Firmware_TimeoutFlag = 0;

typedef enum {
	SIM3G_OPEN_CONNECTION = 0,
	SIM3G_WAIT_FOR_A_MOMENT,
	SIM3G_SETUP_SUBSCRIBE_TOPICS,
	SIM3G_SETUP_PUBLISH_TOPICS,
	SIM3G_SEND_SMS_MESSAGE,
	SIM3G_UPDATE_FIRMWARE,
	MAX_SERVER_COMMUNICATION_FSM_NUMBER_STATES
} SERVER_COMMUNICATION_FSM_STATE;

SERVER_COMMUNICATION_FSM_STATE serverCommunicationFsmState = SIM3G_OPEN_CONNECTION;



uint8_t is_Update_Firmware_Timeout(void);
void Set_Update_Firmware_Timeout_Flag(void);
void Clear_Update_Firmware_Timeout_Flag(void);

uint8_t is_Ping_Request_Timeout(void);
void Set_Ping_Request_Timeout_Flag(void);
void Clear_Ping_Request_Timeout_Flag(void);

void Set_Publish_Message_Timeout_Flag(void);
void Clear_Publish_Message_Timeout_Flag(void);
uint8_t is_Publish_Message_Timeout(void);

void Update_Publish_Status_Message(void);
void Update_Publish_Fota_Status_Message(uint16_t fota_status);
void Update_Publish_Power_Message(uint8_t outletID, int32_t displayData);

void Update_Publish_Power_Message_All_Outlets(void);
void Update_Publish_Power_Factor_Message_All_Outlets(void);
void Update_Publish_Voltage_Message_All_Outlets(void);
void Update_Publish_Ampere_Message_All_Outlets(void);

uint8_t is_Update_Firmware_Timeout(void){
	return update_Firmware_TimeoutFlag == 1;
}
void Set_Update_Firmware_Timeout_Flag(void){
	update_Firmware_TimeoutFlag = 1;
}
void Clear_Update_Firmware_Timeout_Flag(void){
	update_Firmware_TimeoutFlag = 0;
}

void Clear_Ping_Request_Timeout_Flag(void){
	ping_Request_TimeoutFlag = 0;
}
void Set_Ping_Request_Timeout_Flag(void){
	ping_Request_TimeoutFlag = 1;
}
uint8_t is_Ping_Request_Timeout(void){
	return ping_Request_TimeoutFlag;
}

void Clear_Publish_Message_Timeout_Flag(void){
	publish_message_TimeoutFlag = 0;
}
void Set_Publish_Message_Timeout_Flag(void){
	publish_message_TimeoutFlag = 1;
}
uint8_t is_Publish_Message_Timeout(void){
	return publish_message_TimeoutFlag;
}



void Update_Publish_Power_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint64_t tempValue;
	uint8_t * tempCharBuffer;
	uint8_t outletID = 0;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;

#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	for(outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){

		if(outletID < MAIN_INPUT){
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Power_Consumption(outletID);
			tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
			for(uint8_t i = 0; i < 21; i ++){
				if(tempCharBuffer[i] == 0) break;
				publish_message[publishMessageIndex++] = tempCharBuffer[i];
			}
		} else {
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			publish_message[publishMessageIndex++] = 0 + 0x30;

		}

		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
	publish_message[publishMessageIndex++] = ',';
	outletID = 9;
	publish_message[publishMessageIndex++] = outletID + 0x30;
	publish_message[publishMessageIndex++] = '-';
	publish_message[publishMessageIndex++] = 0 + 0x30;

	outletID = 10;
	publish_message[publishMessageIndex++] = ',';
	publish_message[publishMessageIndex++] = outletID/10 + 0x30;
	publish_message[publishMessageIndex++] = outletID%10 + 0x30;
	publish_message[publishMessageIndex++] = '-';
	tempValue = Get_Main_Power_Consumption();
	tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
	for(uint8_t i = 0; i < 21; i ++){
		if(tempCharBuffer[i] == 0) break;
		publish_message[publishMessageIndex++] = tempCharBuffer[i];
	}


#else
	for(outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){

		if(outletID < MAIN_INPUT){
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Power_Consumption(outletID);
		} else {
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Main_Power_Consumption();

		}

		tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
		for(uint8_t i = 0; i < 21; i ++){
			if(tempCharBuffer[i] == 0) break;
			publish_message[publishMessageIndex++] = tempCharBuffer[i];
		}
		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
#endif
	publish_message_length = publishMessageIndex;
}


void Update_Publish_Power_Factor_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint8_t tempValue;
	uint8_t * tempCharBuffer;
	uint8_t outletID;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
#if (VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	for( outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){
		if(outletID < MAIN_INPUT){
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Power_Factor(outletID);
			tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
			for(uint8_t i = 0; i < 21; i ++){
				if(tempCharBuffer[i] == 0) break;
				publish_message[publishMessageIndex++] = tempCharBuffer[i];
			}
		} else {
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			publish_message[publishMessageIndex++] = 0 + 0x30;
		}

		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}

	publish_message[publishMessageIndex++] = ',';
	outletID = 9;
	publish_message[publishMessageIndex++] = outletID + 0x30;
	publish_message[publishMessageIndex++] = '-';
	publish_message[publishMessageIndex++] = 0 + 0x30;

	outletID = 10;
	publish_message[publishMessageIndex++] = ',';
	publish_message[publishMessageIndex++] = outletID/10 + 0x30;
	publish_message[publishMessageIndex++] = outletID%10 + 0x30;
	publish_message[publishMessageIndex++] = '-';
	tempValue = Get_Main_Power_Factor();
	tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
	for(uint8_t i = 0; i < 21; i ++){
		if(tempCharBuffer[i] == 0) break;
		publish_message[publishMessageIndex++] = tempCharBuffer[i];
	}

#else
	for( outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){
		if(outletID < MAIN_INPUT){
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Power_Factor(outletID);
		} else {
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Main_Power_Factor();
		}

		tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
		for(uint8_t i = 0; i < 21; i ++){
			if(tempCharBuffer[i] == 0) break;
			publish_message[publishMessageIndex++] = tempCharBuffer[i];
		}

		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}

#endif
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Voltage_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint8_t tempValue;
	uint8_t * tempCharBuffer;

	for(publishMessageIndex = 0; publishMessageIndex < 20; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
	for(uint8_t outletID = 0; outletID < 1; outletID++){
		publish_message[publishMessageIndex++] = outletID + 0x30;
		publish_message[publishMessageIndex++] = '-';
		tempValue = Get_Voltage(outletID);
		tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
		for(uint8_t i = 0; i < 21; i ++){
			if(tempCharBuffer[i] == 0) break;
			publish_message[publishMessageIndex++] = tempCharBuffer[i];
		}
	}
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Current_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint32_t tempValue;
	uint8_t * tempCharBuffer;
	uint8_t outletID = 0;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;

#if (VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	for(outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){

		if(outletID < MAIN_INPUT){
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Current(outletID);
			tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
			for(uint8_t i = 0; i < 21; i ++){
				if(tempCharBuffer[i] == 0) break;
				publish_message[publishMessageIndex++] = tempCharBuffer[i];
			}
		} else {
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			publish_message[publishMessageIndex++] = 0 + 0x30;

		}


		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}

	publish_message[publishMessageIndex++] = ',';
	outletID = 9;
	publish_message[publishMessageIndex++] = outletID + 0x30;
	publish_message[publishMessageIndex++] = '-';
	publish_message[publishMessageIndex++] = 0 + 0x30;

	outletID = 10;
	publish_message[publishMessageIndex++] = ',';
	publish_message[publishMessageIndex++] = outletID/10 + 0x30;
	publish_message[publishMessageIndex++] = outletID%10 + 0x30;
	publish_message[publishMessageIndex++] = '-';
	tempValue = Get_Main_Current();
	tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
	for(uint8_t i = 0; i < 21; i ++){
		if(tempCharBuffer[i] == 0) break;
		publish_message[publishMessageIndex++] = tempCharBuffer[i];
	}


#else
//	publish_message[publishMessageIndex++] = BOX_ID + 0x30;
	for(outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){

		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Current(outletID);
		} else {
			publish_message[publishMessageIndex++] = outletID/10 + 0x30;
			publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Main_Current();
		}
		tempCharBuffer = (uint8_t*)ConvertUint64ToString(tempValue);
		for(uint8_t i = 0; i < 21; i ++){
			if(tempCharBuffer[i] == 0) break;
			publish_message[publishMessageIndex++] = tempCharBuffer[i];
		}

		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}

#endif
	publish_message_length = publishMessageIndex;
}


void Update_Publish_Status_Message(void){
	uint8_t publishMessageIndex = 0;
	uint8_t outletID = 0;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	for(outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID ++){
		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
				if((uint8_t)Get_Node_Status(outletID) <= 9){
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID) + 0x30;
				} else {
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID)/10 + 0x30;
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID)%10 + 0x30;
				}
		} else {
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
			publish_message[publishMessageIndex++] = '-';
			publish_message[publishMessageIndex++] = NO_FUSE +  0x30;
		}
		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
	publish_message[publishMessageIndex++] = ',';
	outletID = 9;
	publish_message[publishMessageIndex++] = outletID + 0x30;
	publish_message[publishMessageIndex++] = '-';
	publish_message[publishMessageIndex++] = NO_FUSE + 0x30;

	outletID = 10;
	publish_message[publishMessageIndex++] = ',';
	publish_message[publishMessageIndex++] = outletID/10 + 0x30;
	publish_message[publishMessageIndex++] = outletID%10 + 0x30;
	publish_message[publishMessageIndex++] = '-';
	publish_message[publishMessageIndex++] = (uint8_t)Get_Main_Status() + 0x30;

#else
	for(outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID ++){
		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
			// if(Get_Relay_Status(outletID)){
				if((uint8_t)Get_Node_Status(outletID) <= 9){
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID) + 0x30;
				} else {
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID)/10 + 0x30;
					publish_message[publishMessageIndex++] = (uint8_t)Get_Node_Status(outletID)%10 + 0x30;
				}
			// } else {
			// 	publish_message[publishMessageIndex++] = 0x30;
			// }
		} else {
			publish_message[publishMessageIndex++] = outletID/10 + 0x30;
			publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			publish_message[publishMessageIndex++] = '-';
			publish_message[publishMessageIndex++] = (uint8_t)Get_Main_Status() + 0x30;
		}
		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
#endif
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Fota_Status_Message(uint16_t fota_status){
	uint8_t publishMessageIndex = 0;
	for(publishMessageIndex = 0; publishMessageIndex < MQTT_MESSAGE_BUFFER_LENGTH; publishMessageIndex ++){
		publish_message[publishMessageIndex] = 0;
	}
	publishMessageIndex = 0;
	publish_message[publishMessageIndex++] = fota_status >> 8;
	publish_message[publishMessageIndex++] = fota_status;
	publish_message_length = publishMessageIndex;
}





void Server_Communication(void){
	if(Is_Reset_Module_Sim()){
		UART3_SendToHost((uint8_t*)"Is_Reset_Module_Sim\r\n");
		Clear_Counter_For_Reset_Module_Sim();
		Set_Sim3G_State(POWER_OFF_SIM3G);
		Set_Mqtt_State(MQTT_WAIT_FOR_NEW_COMMAND);
		serverCommunicationFsmState = SIM3G_OPEN_CONNECTION;
	}
	switch(serverCommunicationFsmState){
	case SIM3G_OPEN_CONNECTION:
		
		if(Sim3g_Run()){
			Set_Mqtt_State(MQTT_OPEN_STATE);
			SCH_Delete_Task(ping_Request_TimeoutIndex);
			Clear_Ping_Request_Timeout_Flag();
			ping_Request_TimeoutIndex = SCH_Add_Task(Set_Ping_Request_Timeout_Flag, 200, 0);
			serverCommunicationFsmState = SIM3G_WAIT_FOR_A_MOMENT;

		}
		break;
	case SIM3G_WAIT_FOR_A_MOMENT:
		if(is_Ping_Request_Timeout()){
			Clear_Ping_Request_Timeout_Flag();
			ping_Request_TimeoutIndex = SCH_Add_Task(Set_Ping_Request_Timeout_Flag, TIME_FOR_PING_REQUEST, 0);

			Clear_Publish_Message_Timeout_Flag();
			publish_message_TimeoutIndex = SCH_Add_Task(Set_Publish_Message_Timeout_Flag,TIME_FOR_PUBLISH_MESSAGE, 0);

			serverCommunicationFsmState = SIM3G_SETUP_SUBSCRIBE_TOPICS;
		}
		break;
	case SIM3G_SETUP_SUBSCRIBE_TOPICS:
		if(Uart1_Received_Buffer_Available() == 0){
			if(MQTT_Run()){
				Set_Sim3G_State(RESET_SIM3G);
				serverCommunicationFsmState = SIM3G_OPEN_CONNECTION;
			} else {
				if(Is_Ready_To_Send_MQTT_Data()){
					serverCommunicationFsmState = SIM3G_SETUP_PUBLISH_TOPICS;
					
				}
			}
		}
		break;
	case SIM3G_SETUP_PUBLISH_TOPICS:
		if(!Is_Ready_To_Send_MQTT_Data()){
			serverCommunicationFsmState = SIM3G_SETUP_SUBSCRIBE_TOPICS;
		} else if(Is_Set_Send_Sms_Flag()) {
			Start_Sending_Sms_Message();
			serverCommunicationFsmState = SIM3G_SEND_SMS_MESSAGE;
		} else if(Is_Update_Firmware()){
			Power_Off_Sim3g();
			Clear_Update_Firmware_Timeout_Flag();
			SCH_Add_Task(Set_Update_Firmware_Timeout_Flag, 3000, 0);
			serverCommunicationFsmState = SIM3G_UPDATE_FIRMWARE;
		} else {
			if(is_Set_Relay_Timeout()){
				if(Get_Is_Update_Relay_Status() == SET || Get_Is_Node_Status_Changed() == SET){
					publishTopicIndex = 0;
				} else if (is_Publish_Message_Timeout()){
					if (publishTopicIndex == 0) {
						publishTopicIndex = 1;
						Update_Publish_Status_Message();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_STATUS,
								publish_message, publish_message_length);
					} else if (publishTopicIndex == 1) {
						publishTopicIndex = 2;
						Update_Publish_Power_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_POWER,
								publish_message, publish_message_length);
					} else if (publishTopicIndex == 2) {
						publishTopicIndex = 3;
						Update_Publish_Power_Factor_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_POWERFACTOR,
								publish_message, publish_message_length);
					} else if (publishTopicIndex == 3) {
						publishTopicIndex = 4;
						Update_Publish_Voltage_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_VOLTAGE,
								publish_message, publish_message_length);
					} else if (publishTopicIndex == 4) {
						publishTopicIndex = 0;
						Update_Publish_Current_Message_All_Outlets();
						Setup_Mqtt_Publish_Message(PUBLISH_TOPIC_CURRENT,
								publish_message, publish_message_length);
					}

					Set_Mqtt_State(MQTT_PUBLISH_STATE);
					Clear_Publish_Message_Timeout_Flag();
					publish_message_TimeoutIndex = SCH_Add_Task(Set_Publish_Message_Timeout_Flag, TIME_FOR_PUBLISH_MESSAGE, 0);

					ClearCounter();
					Clear_Counter_For_Reset_Module_Sim();
					Clear_For_Watchdog_Reset_Due_To_Not_Sending_Mqtt_Message();
					Turn_On_Buzzer();
					SCH_Add_Task(Turn_Off_Buzzer, TIME_FOR_BUZZER, 0);
					#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
						Network_Connected();
					#endif
				}
			}
		}
		break;
	case SIM3G_SEND_SMS_MESSAGE:
		FSM_For_Sending_SMS_Message();
		if(Is_Done_Sending_Sms_Message()){
			serverCommunicationFsmState = SIM3G_SETUP_SUBSCRIBE_TOPICS;
		}
		break;
	case SIM3G_UPDATE_FIRMWARE:
		if(is_Update_Firmware_Timeout()){
			UART3_SendToHost((uint8_t*)"Jump_To_Fota_Firmware");
			Jump_To_Fota_Firmware();
		}
		break;
	default:
		break;
	}
}




