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
		.host = "\"test.mosquitto.org\"",
		.port = 1883,
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
	bool internet_connected = false;
	switch (step) {
		case 0:
			netif_manager_is_connect_to_internet(&internet_connected);
			if(internet_connected){
				netif_log_info("netif_mqtt_connect");
				netif_mqtt_connect(&mqtt_client);
				step = 1;
			}
			break;
		case 1:
			if(connected){
				netif_log_info("netif_mqtt_subcribe");
				netif_mqtt_subcribe(&mqtt_client, "thodoxuan", 1);
				SCH_Add_Task(time_to_publish, 0, 500);
				step = 2;
			}
			break;
		case 2:
			if(time_for_publish && !published){
				published = true;
				time_for_publish = false;
				netif_log_info("netif_mqtt_publish");
				netif_mqtt_publish(&mqtt_client, "publish_topic", "123", 1, 0);
			}
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
	have_message = true;
}

static void on_publish_cb(uint8_t status){
	// reset
	published = false;
}
