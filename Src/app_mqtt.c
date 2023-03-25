/*
 * app_mqtt_test.c
 *
 *  Created on: Dec 20, 2022
 *      Author: xuanthodo
 */

#include "app_mqtt.h"
#include "netif.h"
#include "stdbool.h"
#include "app_scheduler.h"

#include "utils_buffer.h"
#include "utils_logger.h"

// Get BoxID
#include "app_pcf8574.h"

enum {
    MQTT_WAIT_FOR_INTERNET_CONNECTED,
    MQTT_CLIENT_CONFIG,
    MQTT_CLIENT_CONNECT,
    MQTT_CLIENT_SUBCRIBE,
    MQTT_CLIENT_PUBLISH,
    MQTT_CLIENT_DISCONNECT,
    MQTT_CLIENT_IDLE,
};

static void time_to_publish();
static void on_connect_cb(uint8_t status);
static void on_disconnect_cb(uint8_t status);
static void on_message_cb(char * topic, char * payload);
static void on_publish_cb(uint8_t status);
static uint8_t mqtt_subtopic_to_id(char * topic);

// Internal State
static uint8_t mqtt_state = MQTT_WAIT_FOR_INTERNET_CONNECTED;

// Flag for limit time between 2 publish message
static bool timeout_to_publish_flag = false;

// Tx-Rx Buffer
static utils_buffer_t mqtt_tx_buffer;
static utils_buffer_t mqtt_rx_buffer;

// Callback flag - Unused
static bool connected = false;
static bool subcribed = false;
static bool have_message = false;
static bool published = false;

static char client_id[CLIENTID_MAX_LEN];

static char subtopic_entry[][TOPIC_MAX_LEN] = {
		[SUBTOPIC_COMMAND] = "CEbox_%.4d",
		[SUBTOPIC_UPDATE_POWER_CONSUMPTION] = "FEbox_%.4d",
		[SUBTOPIC_RETAINED] = "REbox_%.4d",
};

static char pubtopic_entry[][TOPIC_MAX_LEN] = {
		[PUBTOPIC_STATUS] = "SEbox_%.4d",
		[PUBTOPIC_POWER] = "PEbox_%.4d",
		[PUBTOPIC_VOLTAGE] = "VEbox_%.4d",
		[PUBTOPIC_CURRENT] = "AEbox_%.4d",
		[PUBTOPIC_POWER_FACTOR] = "PFEbox_%.4d",
};

static netif_mqtt_client_t mqtt_client = {
		.client_id = "netif_test_123",
		.host = "35.240.158.2",
		.port = 8883,
		.username = "eboost-k2",
		.password = "ZbHzPb5W",
		.reconnect = 1,
		.keep_alive = 120,
		.on_connect = on_connect_cb,
		.on_disconnect = on_disconnect_cb,
		.on_message = on_message_cb,
		.on_publish = on_publish_cb
};

void mqtt_init(){
	char topic_temp[TOPIC_MAX_LEN];
	uint16_t boxID = Get_Box_ID();
	// Init Client ID
	snprintf(client_id, CLIENTID_MAX_LEN,"Eb%d", boxID);
	mqtt_client.client_id = client_id;
	// Init SubTopic
	for (int var = 0; var < sizeof(subtopic_entry)/sizeof(subtopic_entry[0]); ++var) {
		// Append boxID to Topic
		snprintf(topic_temp, TOPIC_MAX_LEN, subtopic_entry[var], boxID);
		memcpy(subtopic_entry[var], topic_temp, TOPIC_MAX_LEN);
	}
	// Init PubTopic
	for (int var = 0; var < sizeof(pubtopic_entry)/sizeof(pubtopic_entry[0]); ++var) {
		// Append boxID to Topic
		snprintf(topic_temp, TOPIC_MAX_LEN, pubtopic_entry[var], boxID);
		memcpy(pubtopic_entry[var], topic_temp, TOPIC_MAX_LEN);
	}
    // Init Tx-Rx Buffer;
    utils_buffer_init(&mqtt_tx_buffer, sizeof(mqtt_message_t));
    utils_buffer_init(&mqtt_rx_buffer, sizeof(mqtt_message_t));
}

/**
 *0. Connect
 *1. Subcribe
 *2. Publish
 */
void mqtt_run(){
    static mqtt_message_t publish_message;
	static uint32_t last_sent = 0;
	static uint8_t subtopic_idx = 0;
	static uint8_t subtopic_size = sizeof(subtopic_entry) / sizeof(subtopic_entry[0]);
	bool internet_connected = false;
	netif_status_t ret;
	switch (mqtt_state) {
		case MQTT_WAIT_FOR_INTERNET_CONNECTED:
			netif_manager_is_connect_to_internet(&internet_connected);
			if(internet_connected){
				utils_log_info("Internet Connected\r\n");
				mqtt_state = MQTT_CLIENT_CONFIG;
			}
			break;
		case MQTT_CLIENT_CONFIG:
			ret = netif_mqtt_config(&mqtt_client);
			if(ret == NETIF_OK){
				utils_log_info("Mqtt Config OK\r\n");
				last_sent = NETIF_GET_TIME_MS();
				mqtt_state = MQTT_CLIENT_CONNECT;
			}else if(ret != NETIF_IN_PROCESS){
				Error_Handler();
			}

			break;
		case MQTT_CLIENT_CONNECT:
			if(NETIF_GET_TIME_MS() - last_sent < COMMAND_INTERVAL){
				break;
			}
			ret = netif_mqtt_connect(&mqtt_client);
			if(ret == NETIF_OK){
				utils_log_info("Mqtt Connect OK\r\n");
				last_sent = NETIF_GET_TIME_MS();
				mqtt_state = MQTT_CLIENT_SUBCRIBE;
			}else if(ret != NETIF_IN_PROCESS){
				Error_Handler();
			}
			break;
		case MQTT_CLIENT_SUBCRIBE:
			if(NETIF_GET_TIME_MS() - last_sent < 2 * COMMAND_INTERVAL){
				break;
			}
			ret = netif_mqtt_subcribe(&mqtt_client, subtopic_entry[subtopic_idx], 1);
			if(ret == NETIF_OK){
				subtopic_idx ++;
				if(subtopic_idx >= subtopic_size){
					subtopic_idx = 0;
					mqtt_state = MQTT_CLIENT_IDLE;
				}
				utils_log_info("Subcribe OK\r\n");
				last_sent = NETIF_GET_TIME_MS();
			}else if(ret  != NETIF_IN_PROCESS){
				Error_Handler();
			}
			break;
        case MQTT_CLIENT_PUBLISH:
        	if(NETIF_GET_TIME_MS() - last_sent < COMMAND_INTERVAL){
				break;
			}
			ret = netif_mqtt_publish(&mqtt_client, publish_message.topic,
													publish_message.payload,
													publish_message.qos,
													publish_message.retain);
			if(ret == NETIF_OK){
				last_sent = NETIF_GET_TIME_MS();
				utils_log_info("Mqtt Publish OK\r\n");
                mqtt_state = MQTT_CLIENT_IDLE;
			}else if(ret  != NETIF_IN_PROCESS ){
				Error_Handler();
			}
			break;
		case MQTT_CLIENT_IDLE:
			// Check if Mqtt disconnected
            if(!connected){
                mqtt_state = MQTT_CLIENT_CONNECT;
            }
            // Check if Mqtt have message to sent
            else if(utils_buffer_is_available(&mqtt_tx_buffer)){
                utils_buffer_pop(&mqtt_tx_buffer, &publish_message);
                mqtt_state = MQTT_CLIENT_PUBLISH;
            }
			break;
		default:
			break;
	}
}

bool mqtt_is_ready(){
    return mqtt_state == MQTT_CLIENT_IDLE;
}

bool mqtt_sent_message(mqtt_message_t * message){
	// Get Topic correspond with topic_idx
	memcpy(message->topic, pubtopic_entry[message->topic_id], TOPIC_MAX_LEN);
    if(utils_buffer_is_full(&mqtt_tx_buffer)){
        return false;
    }
    utils_buffer_push(&mqtt_tx_buffer,message);
    return true;
}

bool mqtt_receive_message(mqtt_message_t * message){
    if(!utils_buffer_is_available(&mqtt_rx_buffer)){
        return false;
    }
    utils_buffer_pop(&mqtt_rx_buffer,message);
    return true;
}

bool mqtt_receive_message_drop_all(){
	utils_buffer_drop_all(&mqtt_rx_buffer);
}



static void timeout_to_publish(){
	timeout_to_publish_flag = true;
}


static void on_connect_cb(uint8_t status){
	connected = true;
}

static void on_disconnect_cb(uint8_t status){
	connected = false;
}

static void on_message_cb(char * topic, char * payload){

	utils_log_debug("on_message_cb: topic %s, payload %s", topic,payload);
    mqtt_message_t message;
    message.topic_id = mqtt_subtopic_to_id(topic);
    memcpy(message.payload, payload , strlen(payload));
	utils_buffer_push(&mqtt_rx_buffer, &message);
}

static void on_publish_cb(uint8_t status){
	// reset
	published = false;
}

static uint8_t mqtt_subtopic_to_id(char * topic){
	for (int var = 0; var < sizeof(subtopic_entry)/sizeof(subtopic_entry[0]); ++var) {
		if(!strstr(subtopic_entry[var], topic)){
			return var;
		}
	}
	return 0xFF;
}
