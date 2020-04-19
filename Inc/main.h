/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"

#include "stdio.h"
#include "string.h"


///* Private function prototypes -----------------------------------------------*/
//#ifdef __GNUC__
///* With GCC, small printf (option LD Linker->Libraries->Small printf
//   set to 'Yes') calls __io_putchar() */
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */

typedef enum
{
  ABNORMAL = 0,
  NORMAL = !ABNORMAL
} WorkingStatus;

#define DEBUG_INIT(X) 					X


/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/

#define		VERSION_EBOX				2
#define		INTERRUPT_TIMER_PERIOD		10 //ms
#define		WATCHDOG_ENABLE 			0

//#define B1_Pin 							GPIO_PIN_13
//#define B1_GPIO_Port 					GPIOC
//#define B1_EXTI_IRQn 					EXTI15_10_IRQn
//#define LD2_Pin 						GPIO_PIN_5
//#define LD2_GPIO_Port 					GPIOA

//#define TMS_Pin 						GPIO_PIN_13
//#define TMS_GPIO_Port 					GPIOA
//#define TCK_Pin 						GPIO_PIN_14
//#define TCK_GPIO_Port					GPIOA
//#define SWO_Pin 						GPIO_PIN_3
//#define SWO_GPIO_Port 					GPIOB



//#define LED1_PIN                         GPIO_PIN_5
//#define LED1_GPIO_PORT                   GPIOA
#define LED2_PIN                         GPIO_PIN_2
#define LED2_GPIO_PORT                   GPIOB
//#define LED3_PIN                         GPIO_PIN_8
//#define LED3_GPIO_PORT                   GPIOC
//#define LED4_PIN                         GPIO_PIN_5
//#define LED4_GPIO_PORT                   GPIOC




//LED output control signals
#define LED_SDI							GPIO_PIN_6
#define LED_SDI_PORT					GPIOC
#define LED_SCK							GPIO_PIN_3
#define LED_SCK_PORT					GPIOC
#define LED_LE							GPIO_PIN_4
#define LED_LE_PORT						GPIOC
#define LED_OE							GPIO_PIN_5
#define LED_OE_PORT						GPIOC

//BUZZER
#define PB5_BUZZER_PIN					GPIO_PIN_5
#define PB5_BUZZER_PORT					GPIOB
#define BUZZER_PIN						PB5_BUZZER_PIN
#define BUZZER_PORT						PB5_BUZZER_PORT

//SPI CS pin
#define SPI_CS_PIN						GPIO_PIN_12
#define SPI_CS_PORT						GPIOB

//3G control signals pins ports
#if(VERSION_EBOX != 2)
#define PC7_3G_WAKEUP					GPIO_PIN_7
#define PC7_3G_WAKEUP_PORT				GPIOC
#endif

#define PC8_3G_PWRON					GPIO_PIN_8
#define PC8_3G_PWRON_PORT				GPIOC
#define PC9_3G_PERST					GPIO_PIN_9
#define PC9_3G_PERST_PORT				GPIOC
#define PA8_3G_REG_EN					GPIO_PIN_8
#define PA8_3G_REG_EN_PORT				GPIOA

#if(VERSION_EBOX != 2)
#define SIM5320_3G_WAKEUP					PC7_3G_WAKEUP
#define SIM5320_3G_WAKEUP_PORT				PC7_3G_WAKEUP_PORT
#endif
#define SIM5320_3G_PWRON					PC8_3G_PWRON
#define SIM5320_3G_PWRON_PORT				PC8_3G_PWRON_PORT
#define SIM5320_3G_PERST					PC9_3G_PERST
#define SIM5320_3G_PERST_PORT				PC9_3G_PERST_PORT
#define SIM5320_3G_REG_EN					PA8_3G_REG_EN
#define SIM5320_3G_REG_EN_PORT				PA8_3G_REG_EN_PORT

//timer
#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()


/* Definition for TIMx's NVIC */
#define TIMx_IRQn                      TIM3_IRQn
#define TIMx_IRQHandler                TIM3_IRQHandler



/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
#define USART1_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define DMA1_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART1_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USART1_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_GPIO_PORT              GPIOA




/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      250




#define USART2_TX_PIN 					GPIO_PIN_2
#define USART2_TX_GPIO_PORT 				GPIOA
#define USART2_RX_PIN 					GPIO_PIN_3
#define USART2_RX_GPIO_PORT 				GPIOA

#define USART2_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
#define DMA2_CLK_ENABLE()                __HAL_RCC_DMA2_CLK_ENABLE()
#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART2_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USART2_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

///////////////////////////////////////////////////////////////////////////////
#define USART3_TX_PIN 					GPIO_PIN_10
#define USART3_TX_GPIO_PORT 				GPIOB
#define USART3_RX_PIN 					GPIO_PIN_11
#define USART3_RX_GPIO_PORT 				GPIOB

#define USART3_CLK_ENABLE()              		__HAL_RCC_USART3_CLK_ENABLE();
#define DMA3_CLK_ENABLE()                		__HAL_RCC_DMA3_CLK_ENABLE()
#define USART3_RX_GPIO_CLK_ENABLE()      		__HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_TX_GPIO_CLK_ENABLE()      		__HAL_RCC_GPIOB_CLK_ENABLE()

#define USART3_FORCE_RESET()             		__HAL_RCC_USART3_FORCE_RESET()
#define USART3_RELEASE_RESET()           		__HAL_RCC_USART3_RELEASE_RESET()


#define		SAMPLE_STEPS							2
#define 	NUMBER_OF_SAMPLES_PER_AVERAGE			(1 << SAMPLE_STEPS)
#define 	NUMBER_OF_SAMPLES_PER_SECOND			275
#define 	NUMBER_OF_ADC_CHANNELS					14

#if(VERSION_EBOX == 2)
#define 	ZERO_POINT_DETECTION_PIN				GPIO_PIN_7
#define 	ZERO_POINT_DETECTION_PORT				GPIOC
#else
#define 	ZERO_POINT_DETECTION_PIN				GPIO_PIN_14
#define 	ZERO_POINT_DETECTION_PORT				GPIOC
#endif

//SPI2
#define MASTER_BOARD
#define SPI2_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define SPI2_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI2_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI2_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

/* Definition for SPIx Pins */
#define SPI2_NSS_PIN                     GPIO_PIN_12
#define SPI2_NSS_GPIO_PORT               GPIOB

#define SPI2_SCK_PIN                     GPIO_PIN_13
#define SPI2_SCK_GPIO_PORT               GPIOB
#define SPI2_MISO_PIN                    GPIO_PIN_14
#define SPI2_MISO_GPIO_PORT              GPIOB
#define SPI2_MOSI_PIN                    GPIO_PIN_15
#define SPI2_MOSI_GPIO_PORT              GPIOB




//I2C
#define I2C1_CLK_ENABLE()               __HAL_RCC_I2C1_CLK_ENABLE()
#define I2C1_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C1_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2C1_FORCE_RESET()              __HAL_RCC_I2C1_FORCE_RESET()
#define I2C1_RELEASE_RESET()            __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2C1_SCL_PIN                    GPIO_PIN_6
#define I2C1_SCL_GPIO_PORT              GPIOB
#define I2C1_SDA_PIN                    GPIO_PIN_7
#define I2C1_SDA_GPIO_PORT              GPIOB




//pcf8574 inputs
#define I2C_BUFFERSIZE 		1
#define I2C_ADDRESS_GPIO0	0
#define I2C_ADDRESS_GPIO1	4
#define I2C_ADDRESS_GPIO2	6

#define SAC10				0
#define SAC20				1
#define SAC11				2
#define SAC21				3
#define SAC12				4
#define SAC22				5
#define SAC13				6
#define SAC23				7
#define SAC14				8
#define SAC24				9
#define SAC15				10
#define SAC25				11
#define SAC16				12
#define SAC26				13
#define SAC17				14
#define SAC27				15
#define SAC18				16
#define SAC28				17
#define SAC19				18
#define SAC29				19

//Relay
#define	NUMBER_OF_RELAYS	10
#define RELAY_0				0
#define RELAY_1				1
#define RELAY_2				2
#define RELAY_3				3
#define RELAY_4				4
#define RELAY_5				5
#define RELAY_6				6
#define RELAY_7				7
#define RELAY_8				8
#define RELAY_9				9

//Relay control pins and ports
#define PA11_OUT0						GPIO_PIN_11
#define PA11_OUT0_PORT					GPIOA
#define PA12_OUT1						GPIO_PIN_12
#define PA12_OUT1_PORT					GPIOA
#define PB8_OUT2						GPIO_PIN_8
#define PB8_OUT2_PORT					GPIOB
#define PB9_OUT3						GPIO_PIN_9
#define PB9_OUT3_PORT					GPIOB
#define PA15_OUT4						GPIO_PIN_15
#define PA15_OUT4_PORT					GPIOA

#define PC10_OUT5						GPIO_PIN_10
#define PC10_OUT5_PORT					GPIOC
#define PC11_OUT6						GPIO_PIN_11
#define PC11_OUT6_PORT					GPIOC
#define PC12_OUT7						GPIO_PIN_12
#define PC12_OUT7_PORT					GPIOC

#define PB3_OUT8						GPIO_PIN_3
#define PB3_OUT8_PORT					GPIOB

#define PB4_OUT9						GPIO_PIN_4
#define PB4_OUT9_PORT					GPIOB



#define RELAY_PIN_0						PA11_OUT0
#define RELAY_PIN_1						PA12_OUT1
#define RELAY_PIN_2						PB8_OUT2
#define RELAY_PIN_3						PB9_OUT3
#define RELAY_PIN_4						PA15_OUT4
#define RELAY_PIN_5						PC10_OUT5
#define RELAY_PIN_6						PC11_OUT6
#define RELAY_PIN_7						PC12_OUT7
#define RELAY_PIN_8						PB3_OUT8
#define RELAY_PIN_9						PB4_OUT9

#define RELAY_PORT_0						PA11_OUT0_PORT
#define RELAY_PORT_1						PA12_OUT1_PORT
#define RELAY_PORT_2						PB8_OUT2_PORT
#define RELAY_PORT_3						PB9_OUT3_PORT
#define RELAY_PORT_4						PA15_OUT4_PORT
#define RELAY_PORT_5						PC10_OUT5_PORT
#define RELAY_PORT_6						PC11_OUT6_PORT
#define RELAY_PORT_7						PC12_OUT7_PORT
#define RELAY_PORT_8						PB3_OUT8_PORT
#define RELAY_PORT_9						PB4_OUT9_PORT


#if(VERSION_EBOX == 2)
#define PD2_RELAY_ENABLE_PIN						GPIO_PIN_2
#define PD2_RELAY_ENABLE_PORT						GPIOD
#endif


#define		NUMBER_OF_SUBSCRIBE_TOPIC						2
#define		NUMBER_OF_PUBLISH_TOPIC							2
#define 	MAX_TOPIC_LENGTH								14

#define 	SUBSCRIBE_RECEIVE_MESSAGE_TYPE 					48
#define 	LEN_SUBSCRIBE_RECEIVE_MESSAGE_TYPE 				15

#define		NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION	(NUMBER_OF_RELAYS + 1)
#define		MAIN_INPUT										(NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION - 1)



#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
