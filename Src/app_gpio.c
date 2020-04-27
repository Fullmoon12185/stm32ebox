/*
 * app_gpio.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_gpio.h"
void LED_Init(void);
void GPIO_Relay_Init(void);
void Buzzer_Init(void);
void ZeroPoint_Detection_Pin_Init(void);

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

//	/*Configure GPIO pin Output Level */
//	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
//


	LED_Init();
	GPIO_Relay_Init();
	Buzzer_Init();
//	SPI_CS_Init();
	ZeroPoint_Detection_Pin_Init();
}

void LED_Init(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	GPIO_InitStruct.Pin = LED1_PIN;
//	HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED2_PIN;
	HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);

}




void GPIO_Relay_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = RELAY_PIN_0;
	HAL_GPIO_Init(RELAY_PORT_0, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = RELAY_PIN_1;
	HAL_GPIO_Init(RELAY_PORT_1, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = RELAY_PIN_2;
	HAL_GPIO_Init(RELAY_PORT_2, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = RELAY_PIN_3;
	HAL_GPIO_Init(RELAY_PORT_3, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = RELAY_PIN_4;
	HAL_GPIO_Init(RELAY_PORT_4, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = RELAY_PIN_5;
	HAL_GPIO_Init(RELAY_PORT_5, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = RELAY_PIN_6;
	HAL_GPIO_Init(RELAY_PORT_6, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = RELAY_PIN_7;
	HAL_GPIO_Init(RELAY_PORT_7, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = RELAY_PIN_8;
	HAL_GPIO_Init(RELAY_PORT_8, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = RELAY_PIN_9;
	HAL_GPIO_Init(RELAY_PORT_9, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PD2_RELAY_ENABLE_PIN;
	HAL_GPIO_Init(PD2_RELAY_ENABLE_PORT, &GPIO_InitStruct);

}



void Buzzer_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = BUZZER_PIN;
	HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
}



void ZeroPoint_Detection_Pin_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = ZERO_POINT_DETECTION_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ZERO_POINT_DETECTION_PORT, &GPIO_InitStruct);

#if(VERSION_EBOX == 2)
	/* EXTI interrupt init*/
	  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#else
	  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
#endif
}



void SPI_CS_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = SPI_CS_PIN;
	HAL_GPIO_Init(SPI_CS_PORT, &GPIO_InitStruct);
}


void Turn_On_Buzzer(void){
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, SET);
}

void Turn_Off_Buzzer(void){
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, RESET);
}





