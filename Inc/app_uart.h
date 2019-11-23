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

void UART1_Receive(void);
ITStatus isUART1TransmissionReady(void);
FlagStatus isReceivedData(uint8_t * str);
FlagStatus isOK(void);
FlagStatus isERROR(void);
FlagStatus isGreaterThanSymbol(void);


void UART2_Init(void);
#endif /* APP_UART_H_ */
