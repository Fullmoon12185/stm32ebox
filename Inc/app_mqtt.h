#ifndef APP_MQTT_H
#define APP_MQTT_H

#include "stdint.h"
#include "stdbool.h"

#define COMMAND_INTERVAL	1000		// 1000ms
#define CLIENTID_MAX_LEN	64
#define TOPIC_MAX_LEN       128
#define PAYLOAD_MAX_LEN     512


enum {
	SUBTOPIC_COMMAND,
	SUBTOPIC_UPDATE_POWER_CONSUMPTION,
	SUBTOPIC_RETAINED
};

enum{
	PUBTOPIC_STATUS,
	PUBTOPIC_POWER,
	PUBTOPIC_VOLTAGE,
	PUBTOPIC_CURRENT,
	PUBTOPIC_POWER_FACTOR,
	PUBTOPIC_POWERMETER485
};

typedef struct {
	uint8_t topic_id;
    char topic[TOPIC_MAX_LEN];
    char payload[PAYLOAD_MAX_LEN];
    uint8_t qos;
    uint8_t retain;
}mqtt_message_t;

void mqtt_init();
void mqtt_run();


bool mqtt_is_ready();
bool mqtt_sent_message(mqtt_message_t * message);
bool mqtt_receive_message(mqtt_message_t * message);


#endif //APP_MQTT_H
