/*
 * app_uart.h
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */

#ifndef APP_UART_H_
#define APP_UART_H_
#include "main.h"


void UART1_Init(void);
void UART1_Transmit(uint8_t* buffer);
void Sim3g_Transmit(uint8_t * buffer, uint8_t buffer_len);

uint8_t Uart1_Received_Buffer_Available(void);
uint8_t Uart1_Read_Received_Buffer(void);

void ATcommandSending(uint8_t * buffer);
void MQTTCommandSending(uint8_t * buffer, uint8_t buffer_len);
HAL_StatusTypeDef Custom_UART_Receive_IT(UART_HandleTypeDef *huart);
ITStatus isSim3gReceiveReady();
ITStatus isSim3gTransmissionReady(void);



void UART2_Init(void);
void UART3_Init(void);
void UART3_SendToHost(uint8_t * buffer);
void UART3_SendReceivedBuffer(void);
void UART3_Transmit(uint8_t * buffer, uint8_t buffer_len);
#endif /* APP_UART_H_ */
