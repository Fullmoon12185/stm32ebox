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



void Clear_Sim3g_Receive_Buffer(void);
void Copy_Received_Data_To_Processing_Buffer(void);
HAL_StatusTypeDef Setup_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

/* UART handler declaration */
UART_HandleTypeDef Uart1Handle;
UART_HandleTypeDef Uart2Handle;
UART_HandleTypeDef Uart3Handle;
/* Buffer used for transmission */
uint8_t  aUART_TxBuffer[] = "";


/* Buffer used for reception */
uint8_t aUART_RxBuffer[RXBUFFERSIZE];
uint8_t Sim3gDataProcessingBuffer[RXBUFFERSIZE];

__IO ITStatus UartTransmitReady = SET;
__IO ITStatus UartReceiveReady = RESET;

__IO ITStatus isGreaterThanSymbolReceived = RESET;





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
//PUTCHAR_PROTOTYPE
//{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the USART1 and Loop until the end of transmission */
//  HAL_UART_Transmit(&Uart3Handle, (uint8_t *)&ch, 1, 0xFFFF);
//
//  return ch;
//}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete */
  UartTransmitReady = SET;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete */
	UartReceiveReady = SET;
//	Setup_UART_Receive_IT(&Uart1Handle, (uint8_t *)aUART_RxBuffer, RXBUFFERSIZE);

//	UART3_SendToHost((uint8_t *)"a\r");
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle){
    Error_Handler();
}



void UART3_SendToHost(uint8_t * buffer){
	uint8_t buffer_len = GetStringLength((uint8_t *) buffer);
	HAL_UART_Transmit(&Uart3Handle, (uint8_t *)buffer, buffer_len, 0xFFFFFFFF);
}

void UART3_SendReceivedBuffer(void){
	UART3_SendToHost((uint8_t *)Sim3gDataProcessingBuffer);
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

HAL_StatusTypeDef Setup_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{

    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;
    huart->RxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;


    return HAL_OK;

}

HAL_StatusTypeDef Custom_UART_Receive_IT(UART_HandleTypeDef *huart)
{
  uint8_t ch;
  static uint8_t receiveBufferIndex = 0;

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
	ch = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
	if (ch == '\n')
	{
		aUART_RxBuffer[receiveBufferIndex] = 0;
		receiveBufferIndex = 0;
		UartReceiveReady = SET;
		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->RxState = HAL_UART_STATE_BUSY_RX;

		Copy_Received_Data_To_Processing_Buffer();

	} else if(ch == '>') {
		isGreaterThanSymbolReceived = SET;
	} else {
		aUART_RxBuffer[receiveBufferIndex] = (uint8_t)(ch & (uint8_t)0x00FF);
		receiveBufferIndex ++;
	}
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
void Clear_Sim3g_Receive_Buffer(void){
	uint8_t i;
	for(i = 0; i < RXBUFFERSIZE; i++){
		aUART_RxBuffer[i] = 0;
	}
}

void Copy_Received_Data_To_Processing_Buffer(void){
	uint8_t i;
	for(i = 0; i < RXBUFFERSIZE; i++){

		Sim3gDataProcessingBuffer[i] = aUART_RxBuffer[i];
	}
	Clear_Sim3g_Receive_Buffer();
}
HAL_StatusTypeDef Sim3g_Receive_Setup(void){
	Clear_Sim3g_Receive_Buffer();
	if(HAL_UART_Receive_IT(&Uart1Handle, (uint8_t *)aUART_RxBuffer, RXBUFFERSIZE) != HAL_OK){
		return HAL_ERROR;
	}
	return HAL_OK;

}
void ATcommandSending(uint8_t * buffer){
	if(isSim3gTransmissionReady() == SET){
		UART1_Transmit(buffer);
	}
//	Sim3g_Receive_Setup();
}

void MQTTCommandSending(uint8_t * buffer, uint8_t buffer_len){
	if(isSim3gTransmissionReady() == SET){
		Sim3g_Transmit(buffer, buffer_len);
	}
//	Sim3g_Receive_Setup();
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


FlagStatus isReceivedData1(const char * str){
	return strcmp((char *)Sim3gDataProcessingBuffer, str) == 0;
}

FlagStatus isReceivedData(const uint8_t * str){
	uint8_t i = 0;
	uint8_t str_len = sizeof(str);
	uint8_t str_len1 = GetStringLength(Sim3gDataProcessingBuffer);
	while(i < str_len){
		if(Sim3gDataProcessingBuffer[i] != str[i]){
			return RESET;
		}
		i++;
	}
	return SET;
}

FlagStatus isPBDone(void){
	if(Sim3gDataProcessingBuffer[0] == 'P' && Sim3gDataProcessingBuffer[1] == 'B'){
		return SET;
	}
	return RESET;
}

FlagStatus isOK(void){
	if(Sim3gDataProcessingBuffer[0] == 'O' && Sim3gDataProcessingBuffer[1] == 'K'){
		return SET;
	}
	return RESET;
}

FlagStatus isERROR(void){
	if(Sim3gDataProcessingBuffer[0] == 'E' && Sim3gDataProcessingBuffer[1] == 'R' && Sim3gDataProcessingBuffer[2] == 'R' && Sim3gDataProcessingBuffer[3] == 'O' && Sim3gDataProcessingBuffer[4] == 'R'){
		return SET;
	}
	return RESET;
}

FlagStatus isConnect_OK(void){
	if(Sim3gDataProcessingBuffer[0] == 'C' && Sim3gDataProcessingBuffer[1] == 'o' && Sim3gDataProcessingBuffer[2] == 'n' && Sim3gDataProcessingBuffer[3] == 'n' && aUART_RxBuffer[4] == 'e'
			&& Sim3gDataProcessingBuffer[5] == 'c' && Sim3gDataProcessingBuffer[6] == 't' && Sim3gDataProcessingBuffer[7] == ' ' && Sim3gDataProcessingBuffer[8] == 'o' && Sim3gDataProcessingBuffer[9] == 'k'){
		return SET;
	}
	return RESET;
}

FlagStatus isGreaterThanSymbol(void){
	if(isGreaterThanSymbolReceived == SET){
		isGreaterThanSymbolReceived = RESET;
		return SET;
	}
	return RESET;
}
