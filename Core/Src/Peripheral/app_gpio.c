/*
 * app_gpio.c
 *
 *  Created on: Jul 26, 2021
 *      Author: peter
 */

#include "app_scheduler.h"
#include "main.h"
#include "Peripheral/app_gpio.h"

#define FLASH_BLINK_TIME			31		/*!< 31 ticks in 10ms timer ~ 300ms */
#define BEAT_BLINK_TIME				97		/*!< 97 ticks in 10ms timer ~ 1000ms */
#define SLOW_TIME					149		/*!< 149 ticks in 10ms timer ~ 1500ms */
#define QUICK_TIME					47		/*!< 47 ticks in 10ms timer ~ 500ms */

uint8_t boardID = 0;


//void bufer_245_init(void);
void urgent_init(void);
void LED_Init(void);
void GPIO_Relay_Init(void);
void Buzzer_Init(void);
//uint8_t Get_BoardID(void);
void Switch_Init(void);
void SPI_CS_Init(void);
void Sim7600_GPIO_Init(void);
void Read_BoardID();
uint8_t Get_BoardID();

/**
  * @brief 	GPIO Initialization Function: 74HC245, LED, Buzzer, SPI_CS, SwitchID, SIM7600_GPIO
  * @param 	None
  * @retval None
  */
void GPIO_Init(void)
{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

//	urgent_init();
//	LED_Init();
//	Buzzer_Init();
//	SPI_CS_Init();
//	Switch_Init();
	Sim7600_GPIO_Init();
//	Read_BoardID();
}

/**
  * @brief 	IC 74HC245 Enable Initialization Function
  * @param 	None
  * @retval None
  */
void urgent_init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0);

	GPIO_InitStruct.Pin = LP595_CLR;
	HAL_GPIO_Init(LP595_CLR_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(LP595_CLR_PORT, LP595_CLR, 0);
}

/**
  * @brief 	LED GPIO Initialization Function
  * @param 	None
  * @retval None
  */
void LED_Init(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = LED_1WIRE_PIN;
	HAL_GPIO_Init(LED_1WIRE_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LED2_PIN;
	HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief 	DIP Switch with GPIO pins Initialization Function
  * @param 	None
  * @retval None
  */
void Switch_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = SWITCH_PIN_0;
	HAL_GPIO_Init(SWITCH_PORT_0, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SWITCH_PIN_1;
	HAL_GPIO_Init(SWITCH_PORT_1, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SWITCH_PIN_2;
	HAL_GPIO_Init(SWITCH_PORT_2, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SWITCH_PIN_3;
	HAL_GPIO_Init(SWITCH_PORT_3, &GPIO_InitStruct);
}

/**
  * @brief 	Buzzer GPIO Initialization Function
  * @param 	None
  * @retval None
  */
void Buzzer_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = BUZZER_PIN;
	HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
}

void Sim7600_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = SIM7600_4G_PWRON;
	HAL_GPIO_Init(SIM7600_4G_PWRON_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SIM7600_4G_PERST;
	HAL_GPIO_Init(SIM7600_4G_PERST_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(SIM7600_4G_PWRON_PORT, SIM7600_4G_PWRON, SET);
	HAL_GPIO_WritePin(SIM7600_4G_PERST_PORT, SIM7600_4G_PERST, SET);
}




/**
 * @brief	Get the BoxID by reading the DIP Switch GPIO pins.
 * @param 	None
 * @retval 	uint8_t
 * TODO: customized this function, too long for read switchID
*/
void Read_BoardID(){
	// SW0
	if (HAL_GPIO_ReadPin(SWITCH_PORT_0, SWITCH_PIN_0) == GPIO_PIN_RESET) boardID+= 8;	// =2^3
	// SW1
	if (HAL_GPIO_ReadPin(SWITCH_PORT_1, SWITCH_PIN_1) == GPIO_PIN_RESET) boardID+= 4;	// = 2^2
	// SW2
	if (HAL_GPIO_ReadPin(SWITCH_PORT_2, SWITCH_PIN_2) == GPIO_PIN_RESET) boardID+= 2;	// = 2^1
	// SW3
	if (HAL_GPIO_ReadPin(SWITCH_PORT_3, SWITCH_PIN_3) == GPIO_PIN_RESET) boardID+= 1;
}


uint8_t Get_BoardID(){
	return boardID;
}
/**
 * @brief	Initialize the SPI Chip Select pin with GPIO pin init
 * @param 	None
 * @retval 	None
*/
void SPI_CS_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = SPI_CS_PIN;
	HAL_GPIO_Init(SPI_CS_PORT, &GPIO_InitStruct);
}

/**
 * @brief	Turn on Buzzer with GPIO_PIN_SET
 * @param 	None
 * @retval 	None
*/
void Turn_On_Buzzer(void){
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, SET);
}

/**
 * @brief	Turn off Buzzer with GPIO_PIN_RESET
 * @param 	None
 * @retval 	None
*/
void Turn_Off_Buzzer(void){
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, RESET);
}

/**
 * @brief	Turn on Led2 with GPIO_PIN_RESET
 * @param 	None
 * @retval 	None
*/
void Turn_On_Led2(void){
	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, RESET);
}

/**
 * @brief	Turn off Led2 with GPIO_PIN_SET
 * @param 	None
 * @retval 	None
*/
void Turn_Off_Led2(void){
	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, SET);
}

/**
 * @brief Led2 blinks ON_0.5s-OFF_1.5s twice after OK initialization
 * @param None
 * @retval None
*/
void Led2_Init_Mode_Done(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, QUICK_TIME, 0);
	SCH_Add_Task(Turn_On_Led2, QUICK_TIME + SLOW_TIME, 0);
	SCH_Add_Task(Turn_Off_Led2, QUICK_TIME * 2 + SLOW_TIME, 0);
}

/**
 * @brief	Led2 blinks ON_1.5s-OFF_0.5s twice after ERROR initialization
 * @param 	None
 * @retval 	None
*/
void Led2_Init_Mode_Error(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, SLOW_TIME, 0);
	SCH_Add_Task(Turn_On_Led2, SLOW_TIME + QUICK_TIME, 0);
	SCH_Add_Task(Turn_Off_Led2, SLOW_TIME*2 + QUICK_TIME, 0);
}

/**
 * @brief 	Led2 blinks ON_1s-OFF_1s continuously in normal working.
 * 			This function is called by Scheduler with PERIOD = 2s.
 * @param 	None
 * @retval 	None
*/
void Led2_Normal_Working(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME, 0);
}

/**
 * @brief 	Led2 blinks ON_0.3s-OFF_0.3s continuously if the system has any error.
 * @param 	None
 * @retval 	None
*/
void Led2_Error_Working(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, FLASH_BLINK_TIME, 0);
}

/**
 * @brief 	Led2 blinks ON_0.3s-OFF_0.3s twice after any OPEN operation
 * @param 	None
 * @retval 	None
*/
void Led2_Open_Cabinet(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, FLASH_BLINK_TIME, 0);
	SCH_Add_Task(Turn_Off_Led2, FLASH_BLINK_TIME * 2, 0);
	SCH_Add_Task(Turn_Off_Led2, FLASH_BLINK_TIME * 3, 0);
}

/**
 * @brief 	Led2 blinks ON_0.3s-OFF_0.3s once after any CLOSE operation
 * @param 	None
 * @retval 	None
*/
void Led2_Close_Cabinet(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, FLASH_BLINK_TIME, 0);
}

/**
 * @brief 	Led2 blinks ON_1.5s-OFF once after publish data
 * @param 	None
 * @retval 	None
*/
void Led2_Publish_Data(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, SLOW_TIME, 0);
}

/**
 * @brief Led2 blinks ON_1s-OFF_1s-(ON_0.3s-OFF_0.3s twice) after
 * 		  mainboard sends command to sub-board.
 * @param None
 * @retval None
*/
void Led2_MainB_Send_To_SubB(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME, 0);
	SCH_Add_Task(Turn_On_Led2, BEAT_BLINK_TIME * 2, 0);
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME * 2 + FLASH_BLINK_TIME, 0);
	SCH_Add_Task(Turn_On_Led2, BEAT_BLINK_TIME * 2 + FLASH_BLINK_TIME * 2, 0);
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME * 2 + FLASH_BLINK_TIME * 3, 0);
}

/**
 * @brief Led2 blinks ON_1s-OFF_1s-ON_0.3s-OFF after
 * 		  mainboard receives response from sub-board.
 * @param None
 * @retval None
*/
void Led2_MainB_Recv_From_SubB(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME, 0);
	SCH_Add_Task(Turn_On_Led2, BEAT_BLINK_TIME * 2, 0);
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME * 2 + FLASH_BLINK_TIME, 0);
}

/**
 * @brief Led2 blinks ON_1s-OFF_1s-ON_0.3s-OFF after
 * 		  sub-board sends response to mainboard.
 * @param None
 * @retval None
*/
void Led2_SubB_Send_To_MainB(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME, 0);
	SCH_Add_Task(Turn_On_Led2, BEAT_BLINK_TIME * 2, 0);
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME * 2 + FLASH_BLINK_TIME, 0);
}

/**
 * @brief Led2 blinks ON_1s-OFF_1s-(ON_0.3s-OFF_0.3s twice) after
 * 		  sub-board receives command from mainboard.
 * @param None
 * @retval None
*/
void Led2_SubB_Recv_From_MainB(void){
	Turn_On_Led2();
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME, 0);
	SCH_Add_Task(Turn_On_Led2, BEAT_BLINK_TIME * 2, 0);
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME * 2 + FLASH_BLINK_TIME, 0);
	SCH_Add_Task(Turn_On_Led2, BEAT_BLINK_TIME * 2 + FLASH_BLINK_TIME * 2, 0);
	SCH_Add_Task(Turn_Off_Led2, BEAT_BLINK_TIME * 2 + FLASH_BLINK_TIME * 3, 0);
}





