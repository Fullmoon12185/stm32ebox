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

void Sim3g_Receive_Setup(void);
void ATcommandSending(uint8_t * buffer);
ITStatus isSim3gReceiveReady();
ITStatus isSim3gTransmissionReady(void);
FlagStatus isReceivedData(const uint8_t * str);
FlagStatus isOK(void);
FlagStatus isERROR(void);
FlagStatus isGreaterThanSymbol(void);


void UART2_Init(void);
void UART3_Init(void);
#endif /* APP_UART_H_ */
