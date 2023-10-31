#ifndef MODBUS_H
#define MODBUS_H

#include <modbus_opts.h>
#include "stdbool.h"
#include "stdint.h"

typedef enum{
    MODBUS_READ_COILS               = 0x01,
    MODBUS_READ_DISCRETE_INPUT      = 0x02,
    MODBUS_READ_HOLDING_REGISTER    = 0x03,
    MODBUS_READ_INPUT_REGISTER      = 0x04,
    MODBUS_WRITE_SINGLE_COIL        = 0x05,
    MODBUS_WRITE_SINGLE_REGISTER    = 0x06,
	MODBUS_WRITE_MULTIPLE_COILS		= 0x0F,
	MODBUS_WRITE_MULTIPLE_REGISTER 	= 0x10
}MODBUS_function_t;

typedef union {
	// Read Function
	struct {
		uint16_t addr;
		uint16_t quantity;
	}read_req;
	struct {
		uint8_t byte_cnt;
		uint8_t data[MODBUS_DATA_MAX_SIZE];
	}read_res;
	// Write Single
	struct {
		uint16_t addr;
		uint16_t data;
	}write_single_req;
	struct {
		uint16_t addr;
		uint16_t data;
	}write_single_res;
	// Write multiple
	struct {
		uint16_t addr;
		uint16_t quantity;
		uint16_t byte_cnt;
		uint8_t data[MODBUS_DATA_MAX_SIZE];
	}write_multiple_req;
	struct {
		uint16_t addr;
		uint16_t quantity;
	}write_multiple_res;
}MODBUS_data_t;

/*
 * Modbus PDU
 * 1: Address (1 byte)
 * 2: Function Code (1 byte))
 * 3: Data (N bytes)
 * 4-5: CRC (2 bytes)
 */
typedef struct {
    uint8_t address;            			// Address of Slave Device
    MODBUS_function_t function_code;      	// Function code of Modbus
    MODBUS_data_t data;						// Modbus Data
    uint16_t crc;               			// Can be calculated automatically
}MODBUS_t;




void MODBUS_init(void);
bool MODBUS_run();
bool MODBUS_transmit(MODBUS_t* tx_message);
bool MODBUS_receive(MODBUS_t* rx_message);


#endif //MODBUS_H
