/*
 * app_mqtt_test.c
 *
 *  Created on: Dec 20, 2022
 *      Author: xuanthodo
 */

#include "app_mqtt_test.h"
#include "app_mqtt.h"
#include "app_scheduler.h"

#define PUBLISH_INTERVAL 100 	// 1s

static void timeout_for_publish();
static bool publish_timout_flag = true;

static mqtt_message_t message = {
	.topic = "topic_test",
	.payload = "payload_test",
	.qos = 1,
	.retain = 0
};

/**
 *0. Wait for Mqtt Ready
 *1. Publish Interval
 */
void mqtt_test_run(){
	if(mqtt_is_ready()){
		if(publish_timout_flag){
			publish_timout_flag = false;
			mqtt_sent_message(&message);
			SCH_Add_Task(timeout_for_publish, PUBLISH_INTERVAL, 0);
		}
	}
}

static void timeout_for_publish(){
	publish_timout_flag = true;
}