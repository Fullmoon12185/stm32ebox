/*
 * app_mqtt_test.c
 *
 *  Created on: Dec 20, 2022
 *      Author: xuanthodo
 */

#include "app_mqtt_test.h"
#include "netif.h"
#include "stdbool.h"
#include "app_scheduler.h"
#include "utils/netif_logger.h"

static void time_to_publish();
static void on_connect_cb(uint8_t status);
static void on_disconnect_cb(uint8_t status);
static void on_message_cb(char * topic, char * payload);
static void on_publish_cb(uint8_t status);

static bool time_for_publish = false;
static bool connected = false;
static bool subcribed = false;
static bool have_message = false;
static bool published = false;

static netif_mqtt_client_t mqtt_client = {
		.client_id = "\"netif_test_123\"",
		.host = "\"test.mosquitto.org\"",
		.port = 1883,
		.username = "\"\"",
		.password = "\"\"",
		.reconnect = 1,
		.on_connect = on_connect_cb,
		.on_disconnect = on_disconnect_cb,
		.on_message = on_message_cb,
		.on_publish = on_publish_cb
};

/**
 *0. Connect
 *1. Subcribe
 *2. Publish
 */
void mqtt_run(){
	static uint8_t step = 0;
	static uint32_t last_sent = 0;
	netif_status_t ret;
	bool internet_connected = false;
	switch (step) {
		case 0:
			netif_manager_is_connect_to_internet(&internet_connected);
			if(internet_connected){
				netif_log_info("Internet Connected");
				step = 1;
			}
			break;
		case 1:
			ret = netif_mqtt_config(&mqtt_client);
			if(ret == NETIF_OK){
				netif_log_info("netif_mqtt_config OK");
				last_sent = NETIF_GET_TIME_MS();
				step = 2;
			}
			break;
		case 2:
			if(NETIF_GET_TIME_MS() - last_sent < 1000){
				break;
			}
			ret = netif_mqtt_connect(&mqtt_client);
			if(ret == NETIF_OK){
				netif_log_info("netif_mqtt_connect OK");
				last_sent = NETIF_GET_TIME_MS();
				step = 3;
			}
			break;
		case 3:
			if(NETIF_GET_TIME_MS() - last_sent < 1000){
				break;
			}
			ret = netif_mqtt_subcribe(&mqtt_client, "\"thodoxuan\"", 1);
			if(ret == NETIF_OK){
				netif_log_info("netif_mqtt_subcribe OK");
				last_sent = NETIF_GET_TIME_MS();
				step = 4;
			}
			break;
		case 4:
//			if(NETIF_GET_TIME_MS() - last_sent > 100){
//				ret = netif_mqtt_publish(&mqtt_client, "\"publish_topic\"", "\"123\"", 1, 0);
//				if(ret == NETIF_OK){
//					last_sent = NETIF_GET_TIME_MS();
//					netif_log_info("netif_mqtt_publish OK");
//				}
//			}
			break;
		default:
			break;
	}
}

static void time_to_publish(){
	time_for_publish = true;
}


static void on_connect_cb(uint8_t status){
	connected = true;
}

static void on_disconnect_cb(uint8_t status){
	connected = false;
}

static void on_message_cb(char * topic, char * payload){
	netif_log_info("on_message_cb: topic %s, payload %s", topic,payload);
	have_message = true;
}

static void on_publish_cb(uint8_t status){
	// reset
	published = false;
}
