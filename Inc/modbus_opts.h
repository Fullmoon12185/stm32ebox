#ifndef MODBUS_OPTS_H
#define MODBUS_OPTS_H

#include "main.h"
#include "app_uart.h"


#define MODBUS_DATA_MAX_SIZE        256     // Max data size for modbus
#define MODBUS_TX_BUFFER_SIZE		256
#define MODBUS_RX_BUFFER_SIZE       256       // Max Rx Buffer Size
#define MODBUS_TIMEOUT              3000     // Timeout for get response data

/*Portable Modbus to your system*/ 

// Physical Layer [UART]
#define MODBUS_TRANSMIT(data, data_size)		UART5_transmit(data, data_size)
#define MODBUS_RECEIVE_IS_AVAILABLE()			UART5_Read_Available()
#define MODBUS_RECEIVE()						UART5_Read_Received_Buffer()

// Tick                                     
#define MODBUS_GET_TIME_MS()			HAL_GetTick()

#endif //MODBUS_OPTS_H


