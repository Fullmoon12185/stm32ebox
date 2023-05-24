/*
 * app_modbus_test.c
 *
 *  Created on: Feb 23, 2023
 *      Author: xuanthodo
 */

#include <app_power_meter_485.h>
#include "modbus.h"
#include "utils/utils_logger.h"

#define POWERMETER485_REQUEST_INTERVAL		5000
#define POWERMETER485_APP_TIMEOUT			5000


enum {
	POWERMETER485_INIT,
	POWERMETER485_SEND_REQUEST,
	POWERMETER485_WAIT_FOR_GET_RESPONSE,
};

typedef union {
	float f32;
	uint8_t u8_p[4];
}f32_cast_t;


typedef enum {
	READ_VOLTAGE,
	READ_CURRENT,
	READ_ACTIVE_POWER,
	READ_POWER_FACTOR,
	READ_FREQUENCY,
	READ_TOTAL_ACTIVE_POWER
}POWER_idx_t;

typedef struct {
	POWER_idx_t idx;
	uint16_t address;
	char * name;
	float data;
}POWER_field_t;


#if defined(MODBUS_MODEL) && (MODBUS_MODEL == DDX)
static POWER_field_t power_mapping_table[] = {
		[READ_VOLTAGE] = {
			.idx = READ_VOLTAGE,
			.address = 0x0000,
			.name = "Voltage"
		},
		[READ_CURRENT] = {
			.idx = READ_CURRENT,
			.address = 0x0008,
			.name = "Current"
		},
		[READ_ACTIVE_POWER] = {
			.idx = READ_ACTIVE_POWER,
			.address = 0x0012,
			.name = "Active Power"
		},
		[READ_POWER_FACTOR] = {
			.idx = READ_POWER_FACTOR,
			.address = 0x002A,
			.name = "Power Factor"
		},
		[READ_FREQUENCY] = {
			.idx = READ_FREQUENCY,
			.address = 0x0036,
			.name = "Frequency"
		},
		[READ_TOTAL_ACTIVE_POWER] = {
			.idx = READ_TOTAL_ACTIVE_POWER,
			.address = 0x0100,
			.name = "Total Active Power"
		},
};

#elif defined(MODBUS_MODEL) && (MODBUS_MODEL == SDM120)
static POWER_field_t power_mapping_table[] = {
		[READ_VOLTAGE] = {
			.idx = READ_VOLTAGE,
			.address = 0x0000,
			.name = "Voltage"
		},
		[READ_CURRENT] = {
			.idx = READ_CURRENT,
			.address = 0x0006,
			.name = "Current"
		},
		[READ_ACTIVE_POWER] = {
			.idx = READ_ACTIVE_POWER,
			.address = 0x000C,
			.name = "Active Power"
		},
		[READ_POWER_FACTOR] = {
			.idx = READ_POWER_FACTOR,
			.address = 0x001E,
			.name = "Power Factor"
		},
		[READ_FREQUENCY] = {
			.idx = READ_FREQUENCY,
			.address = 0x0046,
			.name = "Frequency"
		},
		[READ_TOTAL_ACTIVE_POWER] = {
			.idx = READ_TOTAL_ACTIVE_POWER,
			.address = 0x0156,
			.name = "Total Active Power"
		},
};
#endif

static MODBUS_t read_req = {
		.address = 1,
		.function_code = MODBUS_READ_INPUT_REGISTER,
		.data = {
			.read_req = {
				.addr = 0x0000,	// Pass Address of Data to here
				.quantity = 2	// read 2 bytes
			}
		},
};

static bool address_found = false;

// Power Field
static uint8_t powermeter485_state = POWERMETER485_SEND_REQUEST;
static uint8_t power_mapping_table_size = sizeof(power_mapping_table) / sizeof(POWER_field_t);
static uint8_t power_index = 0;
// Singleton Power
static POWER_t power;
// Modbus Response
static MODBUS_t response;
// Internal variable
static bool powermeter485_process_done = true;

// Internal functions
static float POWERMETER485_calculate_float(uint8_t * data_p, uint8_t data_size);
static void POWERMETER_assign_data_by_index(POWER_t * power, POWER_idx_t index , float data);

void POWERMETER485_fsm(void){
	static uint32_t start_tx_time = 0;
	MODBUS_run();
	switch (powermeter485_state) {
		case POWERMETER485_INIT:
			// Scan 485 address from 0 -> 255
			if(!address_found){
				read_req.address++;
				utils_log_debug("Change address to 0x%x\r\n", read_req.address);
			}
			powermeter485_state = POWERMETER485_SEND_REQUEST;
			break;
		case POWERMETER485_SEND_REQUEST:
			// Check if got all field of power
			if(power_index == power_mapping_table_size){
				if(HAL_GetTick() - start_tx_time < POWERMETER485_REQUEST_INTERVAL){
					break;
				}
				// Mark process done
				powermeter485_process_done = true;
				// Reset Index
				power_index = 0;
				break;
			}
			powermeter485_process_done = false;
			start_tx_time = HAL_GetTick();
			// Assign Register Address of Power Index
			read_req.data.read_req.addr = power_mapping_table[power_index].address;
			// Sent Request
			MODBUS_transmit(&read_req);
			powermeter485_state = POWERMETER485_WAIT_FOR_GET_RESPONSE;
			break;
		case POWERMETER485_WAIT_FOR_GET_RESPONSE:
			if(MODBUS_receive(&response)){
				float data = POWERMETER485_calculate_float(response.data.read_res.data , response.data.read_res.byte_cnt);
				POWERMETER_assign_data_by_index(&power,power_index,data);
				
				// utils_log_debug("%s: %f\r\n",power_mapping_table[power_index].name,
				// 							data);
				// Increase Power Index
				power_index++;
				powermeter485_state = POWERMETER485_SEND_REQUEST;
			}
			if (HAL_GetTick() - start_tx_time > POWERMETER485_APP_TIMEOUT){
				utils_log_error("Modbus timeout for receive -> Try to other address\r\n");
				address_found = false;
				powermeter485_state = POWERMETER485_INIT;
			}
			break;
		default:
			break;
	}
}

POWER_t* POWERMETER485_get_lastest(void){
	return &power;
}


// Internal functions
static void POWERMETER_assign_data_by_index(POWER_t * power, POWER_idx_t index , float data){
	switch (index) {
		case READ_VOLTAGE:
			power->voltage = data;
			break;
		case READ_CURRENT:
			power->current = data;
			break;
		case READ_ACTIVE_POWER:
			power->active_power = data;
			break;
		case READ_POWER_FACTOR:
			power->power_factor = data;
			break;
		case READ_FREQUENCY:
			power->frequency = data;
			break;
		case READ_TOTAL_ACTIVE_POWER:
			power->total_active_power = data;
			break;
		default:
			break;
	}
}

static float POWERMETER485_calculate_float(uint8_t * data_p, uint8_t data_size){
	if(data_size != 4){
		utils_log_error("Float data size invalid\r\n");
		return 0;
	}
	f32_cast_t cast;
	cast.u8_p[0] = data_p[3];
	cast.u8_p[1] = data_p[2];
	cast.u8_p[2] = data_p[1];
	cast.u8_p[3] = data_p[0];
	return cast.f32;
}
