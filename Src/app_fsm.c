/*
 * app_fsm.c
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_sim3g.h"
#include "app_scheduler.h"
#include "app_mqtt.h"
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

#include "app_power_meter_485.h"

#include "utils_logger.h"

#define		TIME_FOR_PING_REQUEST		6000 //5s
#define		TIME_FOR_PUBLISH_MESSAGE	300 //5s


#define 	TIME_FOR_BUZZER				3


extern uint8_t PUBLISH_TOPIC_STATUS[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_FOTA_STATUS[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_POWER[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_VOLTAGE[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_CURRENT[MAX_TOPIC_LENGTH];
extern uint8_t PUBLISH_TOPIC_POWERFACTOR[MAX_TOPIC_LENGTH];


static uint8_t publish_message[MQTT_MESSAGE_BUFFER_LENGTH];
static uint8_t publishTopicIndex;
static uint8_t publish_message_length;

extern int32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_ADC_CHANNELS];

extern FlagStatus array_Of_Outlet_Status[NUMBER_OF_ADC_CHANNELS];

uint8_t ping_Request_TimeoutFlag = 0;
uint32_t ping_Request_TimeoutIndex = NO_TASK_ID;

uint8_t publish_message_TimeoutFlag = 1;
uint32_t publish_message_TimeoutIndex = NO_TASK_ID;

uint8_t update_Firmware_TimeoutFlag = 0;

typedef enum {
	SIM3G_SETUP_PUBLISH_TOPICS,
	SIM3G_RECEIVE_MQTT_MESSAGE,
	SIM3G_SEND_SMS_MESSAGE,
	SIM3G_UPDATE_FIRMWARE,
	MAX_SERVER_COMMUNICATION_FSM_NUMBER_STATES
} SERVER_COMMUNICATION_FSM_STATE;

SERVER_COMMUNICATION_FSM_STATE serverCommunicationFsmState = SIM3G_SETUP_PUBLISH_TOPICS;



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
void Update_Publish_Power_Message(uint8_t outletID, int32_t displayData);
void Update_Publish_PowerMeter_Message(void);

void Update_Publish_Power_Message_All_Outlets(void);
void Update_Publish_Power_Factor_Message_All_Outlets(void);
void Update_Publish_Voltage_Message_All_Outlets(void);
void Update_Publish_Ampere_Message_All_Outlets(void);

void Process_Subcribe_Command_Message(char * payload);
void Process_Subcribe_Update_Power_Consumption_Message(char * payload);
void Process_Subcribe_Retained_Message(char * payload);


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


void Clear_Publish_Message_Buffer(void){
	for(uint8_t idx = 0; idx < MQTT_MESSAGE_BUFFER_LENGTH; idx ++){
		publish_message[idx] = 0;
	}

}

void Update_Publish_Power_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint64_t tempValue;
	uint8_t* tempCharBuffer;

	Clear_Publish_Message_Buffer();
	publishMessageIndex = 0;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){

		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Power_Consumption(outletID);
		} else {
			if(outletID >= 10){
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID + 0x30;
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
	publish_message_length = publishMessageIndex;
}


void Update_Publish_Power_Factor_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint8_t tempValue;
	uint8_t * tempCharBuffer;


	Clear_Publish_Message_Buffer();
	publishMessageIndex = 0;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){
		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
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
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Voltage_Message_All_Outlets(void){

	uint8_t publishMessageIndex = 0;
	uint8_t tempValue;
	uint8_t * tempCharBuffer;

	Clear_Publish_Message_Buffer();
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

	Clear_Publish_Message_Buffer();
	publishMessageIndex = 0;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID++){

		if(outletID < MAIN_INPUT){
			publish_message[publishMessageIndex++] = outletID + 0x30;
			publish_message[publishMessageIndex++] = '-';
			tempValue = Get_Current(outletID);
		} else {
			if(outletID <= 9){
				publish_message[publishMessageIndex++] = outletID + 0x30;
			} else {
				publish_message[publishMessageIndex++] = outletID/10 + 0x30;
				publish_message[publishMessageIndex++] = outletID%10 + 0x30;
			}
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
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Status_Message(void){
	uint8_t publishMessageIndex = 0;

	Clear_Publish_Message_Buffer();
	publishMessageIndex = 0;
	for(uint8_t outletID = 0; outletID < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; outletID ++){
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
			publish_message[publishMessageIndex++] = (uint8_t)Get_Main_Status() + 0x30;
		}
		if(outletID < MAIN_INPUT)
			publish_message[publishMessageIndex++] = ',';
	}
	publish_message_length = publishMessageIndex;
}

void Update_Publish_Fota_Status_Message(uint16_t fota_status){
	uint8_t publishMessageIndex = 0;

	Clear_Publish_Message_Buffer();
	publishMessageIndex = 0;
	publish_message[publishMessageIndex++] = fota_status >> 8;
	publish_message[publishMessageIndex++] = fota_status;
	publish_message_length = publishMessageIndex;
}



void Update_Publish_PowerMeter_Message(void){
	POWER_t* power = POWERMETER485_get_lastest();
	publish_message_length = snprintf((char*)publish_message,
									MQTT_MESSAGE_BUFFER_LENGTH,
									"%ld,%ld,%ld,%ld,%ld,%ld", //voltage, current, active_power, power_factor, frequency, total_active_power
									power->voltage,
									power->current,
									power->active_power,
									power->power_factor,
									power->frequency,
									power->total_active_power);
}

void Process_Subcribe_Command_Message(char * payload){
	utils_log_debug("\r\nProcess_Subcribe_Command_Message\r\n");
	uint8_t relayIndex;
	uint8_t relayStatus;
	if(payload[0] == '1'){
		relayIndex = payload[1] - 0x30;
		relayStatus = payload[2] - 0x30;
		utils_log_debug("\r\nrelayIndex %d, relayStatus %d\r\n", relayIndex, relayStatus);
		if(relayStatus == SET){
			Set_Relay(relayIndex);
			Set_Limit_Energy(relayIndex, 0xffffffff);
			Start_Working_Time(relayIndex);
			Clear_Max_Node_Current(relayIndex);
		} else {
			Reset_Relay(relayIndex);
			Set_Limit_Energy(relayIndex, 0);
		}
	} else if(payload[0] == '0' && payload[1] == 'x') {
		// FOTA
		utils_log_debug("Jump_To_Fota_Firmware");
		Jump_To_Fota_Firmware();
	} else if (payload[0] == '*'){
		Process_Subcribe_Update_Power_Consumption_Message(payload);
	}
}

void Process_Subcribe_Update_Power_Consumption_Message(char * payload){
	utils_log_debug("Process_Subcribe_Firmware_Update_Message\r\n");
	uint64_t updateTotalPowerConsumption = 0;

	if(payload[0] == '*'){
		for(uint8_t idx = 1; idx <= 10; idx++){
			if(payload[idx] == '#') break;
			updateTotalPowerConsumption = updateTotalPowerConsumption*10 + (payload[idx] - 0x30);
		}
		utils_log_debug("\r\n updateTotalPowerConsumption %d\r\n", updateTotalPowerConsumption);
		Set_Main_Power_Consumption(updateTotalPowerConsumption);
	}
}


void Process_Subcribe_Retained_Message(char * payload){
	utils_log_debug("Process_Subcribe_Firmware_Update_Message\r\n");
	uint8_t relayIndex;
	uint8_t relayStatus;
	for(uint8_t i = 0; i < NUMBER_OF_RELAYS; i++){
		relayIndex = payload[i*4 + 0] - 0x30;
		relayStatus = payload[i*4 + 2] - 0x30;
		if(relayStatus == SET){
			Set_Relay(relayIndex);
			Set_Limit_Energy(relayIndex, 0xffffffff);
		} else {
			Reset_Relay(relayIndex);
			Set_Limit_Energy(relayIndex, 0);
		}
	}
}


void FSM_handle_subcribe_message(void){
	static mqtt_message_t message;
	if(mqtt_receive_message(&message)){
		switch (message.topic_id) {
			case SUBTOPIC_COMMAND:
				Process_Subcribe_Command_Message(message.payload);
				break;
//			case SUBTOPIC_UPDATE_POWER_CONSUMPTION:
//				Process_Subcribe_Update_Power_Consumption_Message(message.payload);
//				break;
			case SUBTOPIC_RETAINED:
				Process_Subcribe_Retained_Message(message.payload);
				break;
			default:
				utils_log_error("Topic Invalid: Drop All Received Message\r\n");
				mqtt_receive_message_drop_all();
				break;
		}
	}
}



void Server_Communication(void){
//	static uint8_t publishChannelIndex = 0;
//	static uint16_t whTest = 0;
//	Led_Status_Display();
	switch(serverCommunicationFsmState){
	case SIM3G_SETUP_PUBLISH_TOPICS:
		if(Is_Set_Send_Sms_Flag()) {
			Start_Sending_Sms_Message();
			serverCommunicationFsmState = SIM3G_SEND_SMS_MESSAGE;
		} else if(Is_Update_Firmware()){
			Power_Off_Sim3g();
			Clear_Update_Firmware_Timeout_Flag();
			SCH_Add_Task(Set_Update_Firmware_Timeout_Flag, 3000, 0);
			serverCommunicationFsmState = SIM3G_UPDATE_FIRMWARE;
		}
		else if(is_Set_Relay_Timeout()){
			if(Get_Is_Update_Relay_Status() == SET || Get_Is_Node_Status_Changed() == SET){
				publishTopicIndex = 0;
			} else if (is_Publish_Message_Timeout()){
				mqtt_message_t message;
				if (publishTopicIndex == 0) {
					publishTopicIndex = 1;
					Update_Publish_Status_Message();
					message.topic_id = PUBTOPIC_STATUS;
					memset(message.payload, 0 , sizeof(message.payload));
					memcpy(message.payload, publish_message, publish_message_length);
					message.qos = 1;
					message.retain = 1;
					mqtt_sent_message(&message);
				} else if (publishTopicIndex == 1) {
					publishTopicIndex = 2;
					Update_Publish_Power_Message_All_Outlets();
					message.topic_id = PUBTOPIC_POWER;
					memset(message.payload, 0 , sizeof(message.payload));
					memcpy(message.payload, publish_message, publish_message_length);
					message.qos = 1;
					message.retain = 1;
					mqtt_sent_message(&message);
				} else if (publishTopicIndex == 2) {
					publishTopicIndex = 3;
					Update_Publish_Power_Factor_Message_All_Outlets();
					message.topic_id = PUBTOPIC_POWER_FACTOR;
					memset(message.payload, 0 , sizeof(message.payload));
					memcpy(message.payload, publish_message, publish_message_length);
					message.qos = 1;
					message.retain = 1;
					mqtt_sent_message(&message);
				} else if (publishTopicIndex == 3) {
					publishTopicIndex = 4;
					Update_Publish_Voltage_Message_All_Outlets();
					message.topic_id = PUBTOPIC_VOLTAGE;
					memset(message.payload, 0 , sizeof(message.payload));
					memcpy(message.payload, publish_message, publish_message_length);
					message.qos = 1;
					message.retain = 1;
					mqtt_sent_message(&message);
				} else if (publishTopicIndex == 4) {
					publishTopicIndex = 5;
					Update_Publish_Current_Message_All_Outlets();
					message.topic_id = PUBTOPIC_CURRENT;
					memset(message.payload, 0 , sizeof(message.payload));
					memcpy(message.payload, publish_message, publish_message_length);
					message.qos = 1;
					message.retain = 1;
					mqtt_sent_message(&message);
				} else if (publishTopicIndex == 5) {
					publishTopicIndex = 0;
					Update_Publish_PowerMeter_Message();
					message.topic_id = PUBTOPIC_POWERMETER485;
					memset(message.payload, 0 , sizeof(message.payload));
					memcpy(message.payload, publish_message, publish_message_length);
					message.qos = 1;
					message.retain = 1;
					mqtt_sent_message(&message);
				}

				Clear_Publish_Message_Timeout_Flag();
				publish_message_TimeoutIndex = SCH_Add_Task(Set_Publish_Message_Timeout_Flag, TIME_FOR_PUBLISH_MESSAGE, 0);

				ClearCounter();

				Turn_On_Buzzer();
				SCH_Add_Task(Turn_Off_Buzzer, TIME_FOR_BUZZER, 0);

			}
		}
		break;
	case SIM3G_SEND_SMS_MESSAGE:
		FSM_For_Sending_SMS_Message();
		if(Is_Done_Sending_Sms_Message()){
			serverCommunicationFsmState = SIM3G_SETUP_PUBLISH_TOPICS;
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




