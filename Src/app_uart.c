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
void Clear_UART1_Receive_Buffer(void);

/* UART handler declaration */
UART_HandleTypeDef Uart1Handle;
UART_HandleTypeDef Uart2Handle;
/* Buffer used for transmission */
uint8_t  aTxBuffer[] = " ****UART_TwoBoards_ComIT****  ****UART_TwoBoards_ComIT****  ****UART_TwoBoards_ComIT**** \r\n";


/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

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
	UART1_Transmit(aTxBuffer);
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
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&Uart2Handle, (uint8_t *)&ch, 1, 0xFFFF);

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

void Clear_UART1_Receive_Buffer(void){
	uint8_t i;
	for(i = 0; i < RXBUFFERSIZE; i++){
		aRxBuffer[i] = 0;
	}
}
void UART1_Receive(void){
	Clear_UART1_Receive_Buffer();
	HAL_UART_Receive_IT(&Uart1Handle, (uint8_t *)aRxBuffer, RXBUFFERSIZE);
}

ITStatus isUART1TransmissionReady(void){
	return UartTransmitReady;
}

FlagStatus isReceivedData(uint8_t * str){
	uint8_t i = 0;
	uint8_t str_len = GetStringLength((uint8_t*)str);

	while(i < str_len){
		if(aRxBuffer[i] != str[i]){
			return RESET;
		}
		i++;
	}
	return SET;
}
FlagStatus isOK(void){
	if(aRxBuffer[0] == 'O' && aRxBuffer[1] == 'K'){
		return SET;
	}
	return RESET;
}

FlagStatus isERROR(void){
	if(aRxBuffer[0] == 'E' && aRxBuffer[1] == 'R' && aRxBuffer[2] == 'R' && aRxBuffer[3] == 'O' && aRxBuffer[4] == 'R'){
		return SET;
	}
	return RESET;
}

FlagStatus isConnect_OK(void){
	if(aRxBuffer[0] == 'C' && aRxBuffer[1] == 'o' && aRxBuffer[2] == 'n' && aRxBuffer[3] == 'n' && aRxBuffer[4] == 'e'
			&& aRxBuffer[5] == 'c' && aRxBuffer[6] == 't' && aRxBuffer[7] == ' ' && aRxBuffer[8] == 'o' && aRxBuffer[9] == 'k'){
		return SET;
	}
	return RESET;
}

FlagStatus isGreaterThanSymbol(void){
	if(aRxBuffer[0] == '>'){
		return SET;
	}
	return RESET;
}
