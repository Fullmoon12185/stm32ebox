/**
 * @file app_uart.h
 * @author thodo
 * @brief Header file for app_uart.c file. This file for some initiations and operation relate to UART.
 */

#ifndef INC_PERIPHERAL_APP_UART_H_
#define INC_PERIPHERAL_APP_UART_H_

#include "main.h"

//Function declare area
void Start_Receive_IT(void);
//Function declare area
void Stop_Receive_IT(void);
// UART for SIM7600 - UART1
HAL_StatusTypeDef UART_SIM7600_Init(void);
void UART_SIM7600_Transmit(uint8_t *buffer);
void UART_SIM7600_Transmit_Size(uint8_t *buffer , uint16_t buffer_size);
uint8_t UART_SIM7600_Received_Buffer_Available(void);
uint8_t UART_SIM7600_Read_Received_Buffer(void);

// UART for 485 Board Control - UART2
void UART_485_Init(void);
void UART_485_Transmit(uint8_t *buffer);
void UART_485_Transmit_Size(uint8_t *buffer , uint16_t buffer_len);
uint8_t UART_485_Received_Buffer_Available(void);
uint8_t UART_485_Read_Received_Buffer(void);


// UART for DEBUG - UART3
HAL_StatusTypeDef UART_DEBUG_Init(void);
void UART_DEBUG_Transmit(uint8_t *buffer);
void UART_DEBUG_Transmit_Size(uint8_t *buffer ,uint16_t buffer_size);



HAL_StatusTypeDef Custom_UART_Receive_IT(UART_HandleTypeDef *huart);


void Clear_Reiceive_Buffer(void);


uint16_t* Get_RxBuffer_Address();
uint16_t* Get_rxHead_Address();

#endif /* INC_PERIPHERAL_APP_UART_H_ */
