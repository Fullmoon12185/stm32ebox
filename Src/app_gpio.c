/*
 * app_gpio.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#include "main.h"

void LED_Init(void);
void GPIO_Relay_Init(void);
void Buzzer_Init(void);



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
//	/*Configure GPIO pin : B1_Pin */
//	GPIO_InitStruct.Pin = B1_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
//
//	/*Configure GPIO pin : LD2_Pin */
//	GPIO_InitStruct.Pin = LD2_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
//
//	/* EXTI interrupt init*/
//	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	LED_Init();
	GPIO_Relay_Init();
	Buzzer_Init();
//	Sim5320_GPIO_Init();
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


//	GPIO_InitStruct.Pin = LED3_PIN;
//	HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);
//
//	GPIO_InitStruct.Pin = LED4_PIN;
//	HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	return;
	//	HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
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
}



void Buzzer_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = BUZZER_PIN;
	HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
}








