/*
 * app_gpio.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_gpio.h"
#include "app_uart.h"


#define EXTI_PREEMP_PRIORITY_LEVEL 0
#define EXTI_SUB_PRIORITY_LEVEL 0


void LED_Init(void);
void GPIO_Relay_Init(void);
void Buzzer_Init(void);
void ZeroPoint_Detection_Pin_Init(void);


#if(ESTOP_BUTTON == 1)

void Estop_Init(void);
static uint8_t estopPressed = 0;
uint8_t strtmpGPIO[] = "                                                                              ";
#endif

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
	SPI_CS_Init();
	ZeroPoint_Detection_Pin_Init();
#if(ESTOP_BUTTON == 1)
	Estop_Init();
#endif
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

#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
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


	GPIO_InitStruct.Pin = PD2_RELAY_ENABLE_PIN;
	HAL_GPIO_Init(PD2_RELAY_ENABLE_PORT, &GPIO_InitStruct);

#elif(VERSION_EBOX == 2 || VERSION_EBOX == 3 || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A || VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
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

#elif (VERSION_EBOX == 15)
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
#endif
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

#if(VERSION_EBOX == VERSION_6_WITH_8CT_20A)
	/* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, EXTI_PREEMP_PRIORITY_LEVEL, EXTI_SUB_PRIORITY_LEVEL);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#elif(VERSION_EBOX == 2 || VERSION_EBOX == 3 || VERSION_EBOX == VERSION_4_WITH_8CT_5A_2CT_10A || VERSION_EBOX == VERSION_5_WITH_8CT_10A_2CT_20A)
	/* EXTI interrupt init*/
	  HAL_NVIC_SetPriority(EXTI9_5_IRQn, EXTI_PREEMP_PRIORITY_LEVEL, EXTI_SUB_PRIORITY_LEVEL);
	  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#elif(VERSION_EBOX == 15)
	/* EXTI interrupt init*/
	  HAL_NVIC_SetPriority(EXTI9_5_IRQn, EXTI_PREEMP_PRIORITY_LEVEL, EXTI_SUB_PRIORITY_LEVEL);
	  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#elif(VERSION_EBOX == 1)
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, EXTI_PREEMP_PRIORITY_LEVEL, EXTI_SUB_PRIORITY_LEVEL);
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


void Turn_On_LED(void){
	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, RESET);
}

void Turn_Off_LED(void){
	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, SET);
}


#if(ESTOP_BUTTON == 1)

void Estop_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = ESTOP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ESTOP_PORT, &GPIO_InitStruct);

	estopPressed = 0;
}

uint8_t isEstopPressed(void){

	return estopPressed;
}
void Estop_Processing(void){
	static GPIO_PinState bitstatus, bitstatus1, bitstatus2;
	bitstatus2 = bitstatus1;
	bitstatus1 = bitstatus;
	bitstatus = HAL_GPIO_ReadPin(ESTOP_PORT, (uint16_t)ESTOP_PIN);
	static uint8_t previousBitStatus = 0;

	if((bitstatus == bitstatus1) && (bitstatus == bitstatus2)){
		if (bitstatus == GPIO_PIN_RESET){
			estopPressed = 1;
			if(previousBitStatus != estopPressed){
				previousBitStatus = estopPressed;
				sprintf((char*) strtmpGPIO, "estopPressed = %d %d \n", (int) estopPressed, (int)HAL_GPIO_ReadPin(ESTOP_PORT, (uint16_t)ESTOP_PIN));
				UART3_SendToHost((uint8_t *)strtmpGPIO);
			}
		}
		else {
			estopPressed = 0;
		}
	}


}

#endif


