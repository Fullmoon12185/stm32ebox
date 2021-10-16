/*
 * app_sim5320MQTT.h
 *
 *  Created on: Dec 17, 2019
 *      Author: VAIO
 */

#ifndef APP_SIM5320MQTT_H_
#define APP_SIM5320MQTT_H_

//#include "main.h"

#define		MQTT_MESSAGE_BUFFER_LENGTH	128


#define	RETAIN_MESSAGE			0x01
#define	QoS_MESSAGE				0x02

#define MQTT_MSG_CONNECT        0x10
#define MQTT_MSG_CONNACK        0x20
#define MQTT_MSG_PUBLISH        0x30
#define MQTT_SUBSCRIBE          0x82
#define MQTT_SUBACK             0x90
#define MQTT_PINGREQ            0xc0
#define MQTT_PINGRESP           0xd0

#define CR                      '\r'        // 0x0D
#define LF                      '\n'        // 0x0A

typedef enum {
	MQTT_OPEN_STATE = 0,
	MQTT_WAIT_FOR_RESPONSE_FROM_OPEN_STATE,
	MQTT_CONNECT_STATE,
	MQTT_RECEIVE_GREATER_THAN_SYMBOL_CONNECT_STATE,
	MQTT_WAIT_FOR_RESPONSE_FROM_CONNECT_STATE,
	MQTT_SUBSCRIBE_STATE,
	MQTT_RECEIVE_GREATER_THAN_SYMBOL_SUBSCRIBE_STATE,
	MQTT_WAIT_FOR_RESPONSE_FROM_SUBSCRIBE_STATE,
	MQTT_PUBLISH_STATE,
	MQTT_RECEIVE_GREATER_THAN_SYMBOL_PUBLISH_STATE,
	MQTT_WAIT_FOR_RESPONSE_FROM_PUBLISH_STATE,
	MQTT_PING_REQUEST_STATE,
	MQTT_RECEIVE_GREATER_THAN_SYMBOL_PING_REQUEST_STATE,
	MQTT_WAIT_FOR_RESPONSE_FROM_REQUEST_STATE,
	MQTT_DISCONNECT_STATE,
	MQTT_WAIT_FOR_RESPONSE_FROM_DISCONNECT_STATE,
	MQTT_WAIT_FOR_NEW_COMMAND,
	MAX_MQTT_NUMBER_STATES
} MQTT_STATE;

typedef struct {
	MQTT_STATE state;
	void (*func)(void);
}MQTT_Machine_Type;

void Set_Up_Topic_Names(void);
uint8_t MQTT_Run(void);
void Set_Mqtt_State(MQTT_STATE newState);
MQTT_STATE Get_Mqtt_State(void);

void Setup_Mqtt_Connect_Message(void);
void Setup_Mqtt_Subscribe_Message(uint8_t * topic);
void Setup_Mqtt_Publish_Message(uint8_t * topic, uint8_t * message, uint8_t lenOfMessage);
void Setup_Mqtt_Publish_Message_No_Retain(uint8_t * topic, uint8_t * message, uint8_t lenOfMessage);

#endif /* APP_SIM5320MQTT_H_ */
