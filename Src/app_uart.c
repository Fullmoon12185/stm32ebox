/*
 * app_uart.c
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_uart.h"
#include "app_string.h"
#include "app_relay.h"







/* UART handler declaration */
UART_HandleTypeDef Uart1Handle;
UART_HandleTypeDef Uart2Handle;
UART_HandleTypeDef Uart3Handle;
/* Buffer used for transmission */
uint8_t  aUART_TxBuffer[] = "";


/* Buffer used for reception */
uint8_t aUART_RxBuffer[RXBUFFERSIZE];
uint16_t receiveBufferIndexHead = 0;
uint16_t receiveBufferIndexTail = 0;

__IO ITStatus UartTransmitReady = SET;
__IO ITStatus UartReceiveReady = RESET;


HAL_StatusTypeDef Sim3g_Receive_Setup(void);

void UART1_Init(void){
	Uart1Handle.Instance          = USART1;

	Uart1Handle.Init.BaudRate     = 115200;
	Uart1Handle.Init.WordLength   = UART_WORDLENGTH_8B;
	Uart1Handle.Init.StopBits     = UART_STOPBITS_1;
	Uart1Handle.Init.Parity       = UART_PARITY_NONE;
	Uart1Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	Uart1Handle.Init.Mode         = UART_MODE_TX_RX;

	if(HAL_UART_DeInit(&Uart1Handle) != HAL_OK){
		Error_Handler();
	}
	if(HAL_UART_Init(&Uart1Handle) != HAL_OK){
		Error_Handler();
	}
	Sim3g_Receive_Setup();
}
/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void UART2_Init(void)
{
	Uart2Handle.Instance = USART2;
	Uart2Handle.Init.BaudRate = 115200;
	Uart2Handle.Init.WordLength = UART_WORDLENGTH_8B;
	Uart2Handle.Init.StopBits = UART_STOPBITS_1;
	Uart2Handle.Init.Parity = UART_PARITY_NONE;
	Uart2Handle.Init.Mode = UART_MODE_TX_RX;
	Uart2Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	Uart2Handle.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&Uart2Handle) != HAL_OK) {
    Error_Handler();
  }
//  /* Output a message on Hyperterminal using printf function */
//   printf("\n\r UART Printf Example: retarget the C library printf function to the UART\n\r");
//   printf("** Test finished successfully. ** \n\r");

}
/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
void UART3_Init(void)
{
	Uart3Handle.Instance = USART3;
	Uart3Handle.Init.BaudRate = 115200;
	Uart3Handle.Init.WordLength = UART_WORDLENGTH_8B;
	Uart3Handle.Init.StopBits = UART_STOPBITS_1;
	Uart3Handle.Init.Parity = UART_PARITY_NONE;
	Uart3Handle.Init.Mode = UART_MODE_TX_RX;
	Uart3Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	Uart3Handle.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&Uart3Handle) != HAL_OK) {
    Error_Handler();
  }
//  /* Output a message on Hyperterminal using printf function */
//   printf("\n\r UART Printf Example: retarget the C library printf function to the UART\n\r");
//   printf("** Test finished successfully. ** \n\r");

}

HAL_StatusTypeDef Sim3g_Receive_Setup(void){
	if(HAL_UART_Receive_IT(&Uart1Handle, (uint8_t *)aUART_RxBuffer, RXBUFFERSIZE) != HAL_OK){
		return HAL_ERROR;
	}
	return HAL_OK;

}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete */
  UartTransmitReady = SET;
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle){
    Error_Handler();
}



void UART3_SendToHost(uint8_t * buffer){
	uint8_t buffer_len = GetStringLength((uint8_t *) buffer);
	HAL_UART_Transmit(&Uart3Handle, (uint8_t *)buffer, buffer_len, 0xFFFFFFFF);
}


void UART3_Transmit(uint8_t * buffer, uint8_t buffer_len){
	if(buffer_len == 0) {
		return;
	} else {
		if(HAL_UART_Transmit_IT(&Uart3Handle, (uint8_t*)buffer, buffer_len)!= HAL_OK){
			Error_Handler();
		}
		UartTransmitReady = RESET;
	}
	return;
}

void UART1_Transmit(uint8_t * buffer){
	uint8_t buffer_len = GetStringLength((uint8_t*)buffer);
	if(buffer_len == 0) {
		return;
	} else {
		if(HAL_UART_Transmit_IT(&Uart1Handle, (uint8_t*)buffer, buffer_len)!= HAL_OK){
			Error_Handler();
		}
		UartTransmitReady = RESET;
	}
	return;
}

void Sim3g_Transmit(uint8_t * buffer, uint8_t buffer_len){
	if(buffer_len == 0) {
		return;
	} else {
		if(HAL_UART_Transmit_IT(&Uart1Handle, (uint8_t*)buffer, buffer_len)!= HAL_OK){
			Error_Handler();
		}
		UartTransmitReady = RESET;
	}
	return;
}



HAL_StatusTypeDef Custom_UART_Receive_IT(UART_HandleTypeDef *huart)
{
  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
	  huart->ErrorCode = HAL_UART_ERROR_NONE;
	  huart->RxState = HAL_UART_STATE_BUSY_RX;
	  aUART_RxBuffer[receiveBufferIndexHead] = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
	  receiveBufferIndexHead = (receiveBufferIndexHead + 1) % RXBUFFERSIZE;
	  return HAL_OK;
  } else {
    return HAL_BUSY;
  }
}

uint8_t Uart1_Received_Buffer_Available(void){
	if(receiveBufferIndexTail != receiveBufferIndexHead){
		return 1;
	} else {
		return 0;
	}
}
uint8_t Uart1_Read_Received_Buffer(void){
	uint8_t buffer[2];
	if(receiveBufferIndexTail == receiveBufferIndexHead) return 0xff;
	uint8_t ch = aUART_RxBuffer[receiveBufferIndexTail];
	receiveBufferIndexTail = (receiveBufferIndexTail + 1) % RXBUFFERSIZE;
	buffer[0] = ch;
	buffer[1] = 0;
	UART3_SendToHost(buffer);
	return ch;
}


void ATcommandSending(uint8_t * buffer){
	if(isSim3gTransmissionReady() == SET){
		UART1_Transmit(buffer);
	}
}

void MQTTCommandSending(uint8_t * buffer, uint8_t buffer_len){
	if(isSim3gTransmissionReady() == SET){
		Sim3g_Transmit(buffer, buffer_len);
	}
}


ITStatus isSim3gReceiveReady(){

	if(UartReceiveReady == SET){
		UartReceiveReady = RESET;
		return SET;
	} else {
		return RESET;
	}
}
ITStatus isSim3gTransmissionReady(void){

	if(UartTransmitReady == SET){
		UartTransmitReady = RESET;

		return SET;
	} else {
		return RESET;
	}

}





