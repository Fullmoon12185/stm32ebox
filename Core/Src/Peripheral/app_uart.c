/*
 * @file app_uart.c
 * @author thodo
 */

#define DEBUG	1

#include <Peripheral/app_uart.h>
#include <Utility/app_string.h>
#include "stdio.h"
#include <string.h>



//Variable declare area
uint8_t temp;

UART_HandleTypeDef huart1;							/*!< UART Handler for SIMCOM7600*/
UART_HandleTypeDef huart3;							/*!< UART Handler for DEBUG*/

volatile uint8_t aUART_RxBuffer[UART_RX_BUFFERSIZE];				/*!< Buffer for UART RX*/
volatile uint16_t receiveBufferIndexHead = 0;					/*!< Head index of RX buffer*/
volatile uint16_t receiveBufferIndexTail = 0;					/*!< Tail index of RX buffer*/

__IO ITStatus UartTransmitReady = SET;				/*!< Manage status of Uart transmit interrupt service*/
__IO ITStatus UartReceiveReady = RESET;				/*!< Manage status of Uart receive interrupt service*/


/**
  * @brief USART SIMCOM7600 Initialization Function
  * @param None
  * @retval None
  */
HAL_StatusTypeDef UART_SIM7600_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
	  return HAL_ERROR;
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  Start_Receive_IT();
  /* USER CODE END USART1_Init 2 */
  return HAL_OK;
}


/**
  * @brief UART_SIM7600_Transmit Initialization Function
  * @param None
  * @retval None
  */
void UART_SIM7600_Transmit(uint8_t *buffer){
	uint16_t tmpLen = strlen((char*)buffer);
	if(tmpLen == 0) {
		return;
	} else {
#ifdef DEBUG
		LOG(buffer);
#endif
//		if (DEBUG){
////			while(!UartTransmitReady);
//			UART_DEBUG_Transmit(buffer);
//		}
		//TODO: never do while in state machine
		while(!UartTransmitReady);
		if(HAL_UART_Transmit_IT(&huart1, buffer, tmpLen)!= HAL_OK){
			Error_Handler();
		}
		UartTransmitReady = RESET;
	}
	return;
}

/**
  * @brief UART_SIM7600_Transmit_Size Initialization Function
  * @param buffer: buffer contain data for transmit
  * @param buffer_size: size of data to transmit
  * @retval None
  */
void UART_SIM7600_Transmit_Size(uint8_t *buffer , uint16_t buffer_size){
	if(buffer_size == 0) {
		return;
	} else {
		if (DEBUG){
//			while(!UartTransmitReady);
			UART_DEBUG_Transmit_Size(buffer,buffer_size);
		}
		while(!UartTransmitReady);
		if(HAL_UART_Transmit_IT(&huart1, (uint8_t*)buffer, buffer_size)!= HAL_OK){
			Error_Handler();
		}
		UartTransmitReady = RESET;
	}
	return;
}


/**
  * @brief UART_SIM7600_Received_Buffer_Available Initialization Function
  * @param None
  * @param None
  * @retval None
  */
uint8_t UART_SIM7600_Received_Buffer_Available(void){
	if(receiveBufferIndexTail != receiveBufferIndexHead){
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief UART_SIM7600_Read_Received_Buffer Initialization Function
  * @param None
  * @param None
  * @retval ch character at Tail index of buffer and Tail will increase 1
  */
char log[10];
uint8_t UART_SIM7600_Read_Received_Buffer(void){
	if(receiveBufferIndexTail == receiveBufferIndexHead) return 0xff;
	uint8_t ch = aUART_RxBuffer[receiveBufferIndexTail];
	receiveBufferIndexTail = (receiveBufferIndexTail + 1) % UART_RX_BUFFERSIZE;
//	sprintf(log,"%d\r\n",receiveBufferIndexTail,receiveBufferIndexHead);
//	LOG("1111");
	return ch;
}


/**
  * @brief UART_DEBUG_Init Initialization Function
  * @param None
  * @retval None
  */
HAL_StatusTypeDef UART_DEBUG_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
	  return HAL_ERROR;
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */
  return HAL_OK;
}

/**
  * @brief USART Transmit Initialization Function
  * @param buffer buffer to transmit via UART DEBUG without known size, it will use strlen(buffer) to get size.
  * @retval None
  */
void UART_DEBUG_Transmit(uint8_t *buffer){
	uint16_t tmpLen = strlen((char*)buffer);
	if(tmpLen == 0){
		return;
	}
	if(HAL_UART_Transmit(&huart3, (uint8_t*)buffer, tmpLen ,0xFF)!= HAL_OK){
		Error_Handler();
	}
//	UartTransmitReady = RESET;
}


/**
  * @brief UART_DEBUG_Transmit_Size Initialization Function
  * @param buffer: buffer contain data for transmit
  * @param buffer_size: size of data to transmit
  * @retval None
  */
void UART_DEBUG_Transmit_Size(uint8_t *buffer ,uint16_t buffer_size){
	if(HAL_UART_Transmit(&huart3, (uint8_t*)buffer, buffer_size,200)!= HAL_OK){
		Error_Handler();
	}
//	UartTransmitReady = RESET;
}


/**
  * @brief Setup_Receive_IT Initialization Function
  * It will receive 1 bytes.
  * @param None
  * @retval None
  */
void Start_Receive_IT(void){
	if(HAL_UART_Receive_IT(&huart1, &temp, 1)!= HAL_OK){
		Error_Handler();
	}
}


/**
  * @brief Setup_Receive_IT Initialization Function
  * It will receive 1 bytes.
  * @param None
  * @retval None
  */
void Stop_Receive_IT(void){
	if(HAL_UART_AbortReceive_IT(&huart1)!= HAL_OK){
		Error_Handler();
	}
}
/**
  * @brief HAL_UART_RxCpltCallback Initialization Function
  * When Simcom7600 transmit data to UART, so HAL_UART_RxCpltCallback will be called.
  * HAL_UART_RxCpltCallback will recalled HAL_UART_Receive_IT to continous UART Interrupt Service.
  * @param huart UART_HandleTypeDef be handled in Callback
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		aUART_RxBuffer[receiveBufferIndexHead] = temp;
		receiveBufferIndexHead = (receiveBufferIndexHead + 1) % UART_RX_BUFFERSIZE;
		HAL_UART_Receive_IT(huart, &temp, 1);
	}
}


/**
  * @brief HAL_UART_TxCpltCallback Initialization Function
  * @param UartHandle UART_HandleTypeDef be handled in Callback
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete */
  UartTransmitReady = SET;
}


/**
  * @brief Clear_Reiceive_Buffer Initialization Function
  * Clear all data in RX buffer.
  * @param None
  * @retval None
  */
void Clear_Reiceive_Buffer(void){
	memset(aUART_RxBuffer,0,UART_RX_BUFFERSIZE);
	receiveBufferIndexHead = 0;
	receiveBufferIndexTail = 0;
}

uint16_t* Get_rxHead_Address(){
	return &receiveBufferIndexHead;
}

uint16_t* Get_rxTail_Address(){
	return &receiveBufferIndexTail;
}

uint16_t* Get_RxBuffer_Address(){
	return aUART_RxBuffer;
}


