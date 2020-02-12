/*
 * app_uart.c
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_uart.h"
#include "app_string.h"


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
void Clear_Sim3g_Receive_Buffer(void);

/* UART handler declaration */
UART_HandleTypeDef Uart1Handle;
UART_HandleTypeDef Uart2Handle;
UART_HandleTypeDef Uart3Handle;
/* Buffer used for transmission */
uint8_t  aUART_TxBuffer[] = " ****UART_TwoBoards_ComIT****  ****UART_TwoBoards_ComIT****  ****UART_TwoBoards_ComIT**** \r\n";


/* Buffer used for reception */
uint8_t aUART_RxBuffer[RXBUFFERSIZE];

__IO ITStatus UartTransmitReady = SET;
__IO ITStatus UartReceiveReady = RESET;





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
	UART1_Transmit(aUART_TxBuffer);
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
  /* Output a message on Hyperterminal using printf function */
   printf("\n\r UART Printf Example: retarget the C library printf function to the UART\n\r");
   printf("** Test finished successfully. ** \n\r");

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
  /* Output a message on Hyperterminal using printf function */
   printf("\n\r UART Printf Example: retarget the C library printf function to the UART\n\r");
   printf("** Test finished successfully. ** \n\r");

}
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&Uart3Handle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete */
  UartTransmitReady = SET;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete */
	UartReceiveReady = SET;

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle){
    Error_Handler();
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


void Clear_Sim3g_Receive_Buffer(void){
	uint8_t i;
	for(i = 0; i < RXBUFFERSIZE; i++){
		aUART_RxBuffer[i] = 0;
	}
}

void Sim3g_Receive_Setup(void){
	UartReceiveReady = RESET;
	Clear_Sim3g_Receive_Buffer();
	HAL_UART_Receive_IT(&Uart1Handle, (uint8_t *)aUART_RxBuffer, RXBUFFERSIZE);
}

void ATcommandSending(uint8_t * buffer){
	if(isSim3gTransmissionReady() == SET){
		UART1_Transmit(buffer);
	}
	Sim3g_Receive_Setup();
}

ITStatus isSim3gReceiveReady(){
	return UartReceiveReady;
}
ITStatus isSim3gTransmissionReady(void){
	return UartTransmitReady;
}


FlagStatus isReivedData1(const char * str){
	return strcmp((char *)aUART_RxBuffer, str) == 0;
}

FlagStatus isReceivedData(const uint8_t * str){
	uint8_t i = 0;
	uint8_t str_len = GetStringLength((uint8_t*)str);

	while(i < str_len){
		if(aUART_RxBuffer[i] != str[i]){
			return RESET;
		}
		i++;
	}
	return SET;
}

FlagStatus isOK(void){
	if(aUART_RxBuffer[0] == 'O' && aUART_RxBuffer[1] == 'K'){
		return SET;
	}
	return RESET;
}

FlagStatus isERROR(void){
	if(aUART_RxBuffer[0] == 'E' && aUART_RxBuffer[1] == 'R' && aUART_RxBuffer[2] == 'R' && aUART_RxBuffer[3] == 'O' && aUART_RxBuffer[4] == 'R'){
		return SET;
	}
	return RESET;
}

FlagStatus isConnect_OK(void){
	if(aUART_RxBuffer[0] == 'C' && aUART_RxBuffer[1] == 'o' && aUART_RxBuffer[2] == 'n' && aUART_RxBuffer[3] == 'n' && aUART_RxBuffer[4] == 'e'
			&& aUART_RxBuffer[5] == 'c' && aUART_RxBuffer[6] == 't' && aUART_RxBuffer[7] == ' ' && aUART_RxBuffer[8] == 'o' && aUART_RxBuffer[9] == 'k'){
		return SET;
	}
	return RESET;
}

FlagStatus isGreaterThanSymbol(void){
	if(aUART_RxBuffer[0] == '>'){
		return SET;
	}
	return RESET;
}
