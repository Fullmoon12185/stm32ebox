/*
 * app_gpio.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#include "main.h"

void LED_Init(void);
void Relay_Init(void);
void Buzzer_Init(void);
void Sim5320_GPIO_Init(void);
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	LED_Init();
	Relay_Init();
	Buzzer_Init();
	Sim5320_GPIO_Init();
}

void LED_Init(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = LED1_PIN;
	HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED2_PIN;
	HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);


	GPIO_InitStruct.Pin = LED3_PIN;
	HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED4_PIN;
	HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED7_SDI;
	HAL_GPIO_Init(LED7_SDI_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LED7_SCK;
	HAL_GPIO_Init(LED7_SCK_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LED7_LE;
	HAL_GPIO_Init(LED7_LE_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LED7_OE;
	HAL_GPIO_Init(LED7_OE_PORT, &GPIO_InitStruct);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
}

void Relay_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = PA11_OUT0;
	HAL_GPIO_Init(PA11_OUT0_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PA12_OUT1;
	HAL_GPIO_Init(PA12_OUT1_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PA13_OUT2;
	HAL_GPIO_Init(PA13_OUT2_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PA14_OUT3;
	HAL_GPIO_Init(PA14_OUT3_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PA15_OUT4;
	HAL_GPIO_Init(PA15_OUT4_PORT, &GPIO_InitStruct);


	GPIO_InitStruct.Pin = PC10_OUT5;
	HAL_GPIO_Init(PC10_OUT5_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PC11_OUT6;
	HAL_GPIO_Init(PC11_OUT6_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PC12_OUT7;
	HAL_GPIO_Init(PC12_OUT7_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PB3_OUT8;
	HAL_GPIO_Init(PB3_OUT8_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PB4_OUT9;
	HAL_GPIO_Init(PB4_OUT9_PORT, &GPIO_InitStruct);
}

void Buzzer_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = PB5_BUZZER;
	HAL_GPIO_Init(PB5_BUZZER_PORT, &GPIO_InitStruct);
}

void Sim5320_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = PC7_3G_WAKEUP;
	HAL_GPIO_Init(PC7_3G_WAKEUP_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PC8_3G_PWRON;
	HAL_GPIO_Init(PC8_3G_PWRON_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PC9_3G_PERST;
	HAL_GPIO_Init(PC9_3G_PERST_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PA8_3G_REG_EN;
	HAL_GPIO_Init(PA8_3G_REG_EN_PORT, &GPIO_InitStruct);
}

