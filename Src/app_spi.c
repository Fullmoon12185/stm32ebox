/*
 * app_spi.c
 *
 *  Created on: Dec 14, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_spi.h"

#define BUFFERSIZE 10
SPI_HandleTypeDef Spi2Handle;
/* Buffer used for transmission */
uint8_t aSPITxBuffer[] = "****SPI - Two Boards communication based on Polling **** SPI Message ******** SPI Message ******** SPI Message ****";

/* Buffer used for reception */
uint8_t aSPIRxBuffer[BUFFERSIZE];


void SPI1_Init(void){

}


void SPI2_Init(void){
	/*##-1- Configure the SPI peripheral #######################################*/
		  /* Set the SPI parameters */
		  Spi2Handle.Instance               = SPI2;
		  Spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
		  Spi2Handle.Init.Direction         = SPI_DIRECTION_2LINES;
		  Spi2Handle.Init.CLKPhase          = SPI_PHASE_1EDGE;
		  Spi2Handle.Init.CLKPolarity       = SPI_POLARITY_LOW;
		  Spi2Handle.Init.DataSize          = SPI_DATASIZE_8BIT;
		  Spi2Handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
		  Spi2Handle.Init.TIMode            = SPI_TIMODE_DISABLE;
		  Spi2Handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
		  Spi2Handle.Init.CRCPolynomial     = 7;
//		  Spi2Handle.Init.NSS               = SPI_NSS_SOFT;
		  Spi2Handle.Init.NSS               = SPI_NSS_HARD_OUTPUT;

		#ifdef MASTER_BOARD
		  Spi2Handle.Init.Mode = SPI_MODE_MASTER;
		#else
		  Spi2Handle.Init.Mode = SPI_MODE_SLAVE;
		#endif /* MASTER_BOARD */

		  if(HAL_SPI_Init(&Spi2Handle) != HAL_OK)
		  {
		    /* Initialization Error */
		    Error_Handler();
		  }

		#ifdef MASTER_BOARD
		  /* SPI block is enabled prior calling SPI transmit/receive functions, in order to get CLK signal properly pulled down.
		     Otherwise, SPI CLK signal is not clean on this board and leads to errors during transfer */
		  __HAL_SPI_ENABLE(&Spi2Handle);

		#endif /* MASTER_BOARD */
}

void ResetChipSelect(void){
	HAL_GPIO_WritePin(SPI2_NSS_GPIO_PORT,SPI2_NSS_PIN, GPIO_PIN_RESET);
}

void SetChipSelect(void){
	HAL_GPIO_WritePin(SPI2_NSS_GPIO_PORT,SPI2_NSS_PIN, GPIO_PIN_SET);
}
