/*
 * locker_config.h
 *
 *  Created on: Sep 23, 2021
 *      Author: thodo
 */

#ifndef INC_LOCKER_CONFIG_H_
#define INC_LOCKER_CONFIG_H_

/*
 * Deploy Environment
 */
#define DEV					0
#define PROD				1


/*
 * Version Control
 */
#define VERSION				"v0.0.0"
#define VERSION_LENGTH		6
#define ENVIRONMENT			DEV

#if ENVIRONMENT == DEV
	#define CLOUDFUNCTION_REGION		"asia-southeast2"
	#define CLOUDFUNCTION_PROJECTID		"hw-smartlocker"
	#define CONNECTION_TOPIC			"connection_dev"
	#define RESPONSE_TOPIC				"response_dev"
	#define STATUS_TOPIC				"status_dev"
	#define ERROR_TOPIC					"error_dev"
	#define CERTIFICATE_TOPIC			"certificate_dev"
	#define DEPOSIT_TOPIC				"deposit_dev"

#elif ENVIRONMENT == PROD
	#define CLOUDFUNCTION_REGION		"asia-southeast2"
	#define CLOUDFUNCTION_PROJECTID		"hw-smartlocker-prod"
	#define CONNECTION_TOPIC			"connection"
	#define RESPONSE_TOPIC				"response"
	#define STATUS_TOPIC				"status"
	#define ERROR_TOPIC					"error"
	#define CERTIFICATE_TOPIC			"certificate"
	#define DEPOSIT_TOPIC				"deposit"
#endif

#define MQTT_SERVER					"\"tcp://a2a8bfp9xqpfl6-ats.iot.ap-northeast-1.amazonaws.com:8883\""
//note that ClientID be differenced between lockers
#define USER_NAME					"\"logUser\""
#define PASS_WORD					"\"logPwd\""

#define TIMER_CYCLE				 		10 /*!< 10ms*/

/*
 * Timeout OpenCabinet Configuration
 */
#define TIMEOUT_TO_OPENCABINET						5000	// 5 seconds
#define TIMEOUT_UPDATE_STATUS_AFTER_OPENCABINET		500 		/*!< 1000ms*/
#define TIMEOUT_OPENCABINET_TO_CLOSECABINET			240000 		/*!< 5000ms .... Follow ticker*/
#define TIMEOUT_CHECK_SURE_CABINET_CLOSED			500 		/*!< 500ms .... Follow ticker*/
#define TIME_TURN_OFF_LED_AFTER_TIMEOUT				3000	/*!< Turn off Red Led after Time Out*/



#define  DEVICE_CLIENTCERT_FILENAME 	"\"main_clientcert.pem\""
#define  DEVICE_CLIENTKEY_FILENAME  	"\"main_clientkey.pem\""

// Cabinet Configuration
#define MAX_TOTAL_CABINET				360 		/*!< This is number of cabinet gotten from Firestore */
#define MAX_CABINET_IN_1_BOARD			24 			/*!< This is number of cabinet gotten from Firestore */
#define MAX_BOARD						15
//IMEI
#define IMEI_LENGTH						15
#define IMEI_INDEX						20

//DEPOSIT
#define DEPOSIT_LENGTH					10

//PHONE NUMBER
#define PHONE_NUMBER_LENGTH				15

//AT Command Timeout
#define COMMAND_TIMEOUT 				200 //200 ms



/*
 * Peripheral Definition
 */

//Led GPIO
#define LED_PORT					GPIOA
#define LED_STATUS_PIN				GPIO_PIN_1
#define LED_RX_PIN					GPIO_PIN_2
#define LED_TX_PIN					GPIO_PIN_3

//BUZZER
#define PB5_BUZZER_PIN						GPIO_PIN_5
#define PB5_BUZZER_PORT						GPIOB
#define BUZZER_PIN							PB5_BUZZER_PIN
#define BUZZER_PORT							PB5_BUZZER_PORT

// Switch with GPIO pin
#define SWITCH_PIN_0						GPIO_PIN_4
#define SWITCH_PORT_0						GPIOA
#define SWITCH_PIN_1 						GPIO_PIN_5
#define SWITCH_PORT_1						GPIOA
#define SWITCH_PIN_2						GPIO_PIN_6
#define SWITCH_PORT_2						GPIOA
#define SWITCH_PIN_3						GPIO_PIN_7
#define SWITCH_PORT_3						GPIOA

//SIMCOM GPIO
#define SIM7600_4G_PWRON_PORT		GPIOC
#define SIM7600_4G_PWRON			GPIO_PIN_8
#define SIM7600_4G_PERST_PORT		GPIOC
#define SIM7600_4G_PERST			GPIO_PIN_9

/*UART Constant*/

//Max buffer for RX
#define UART_RX_BUFFERSIZE 			4096
#define CAN_RX_BUFFER				20
#define RX3_BUFFERSIZE				5


//UART Simcom7600 Constant
#define UART_SIMCOM_INSTANCE		USART1
#define UART_DEBUG_INSTANCE			USART3


/*I2C Constant*/

//EEPROM I2C
#define I2C_EEPROM_INSTANCE			I2C1


/*Timer Constant*/
#define TIMER_INSTANCE				TIM3

/*LED*/
#define LED_1WIRE_PIN				GPIO_PIN_7
#define LED_1WIRE_PORT				GPIOC


/*
 *@define
 *@brief LED GPIO for 595ic
 */
//LP595 (Control lockpad with ic595) output control signals
#define LP595_LE							GPIO_PIN_4
#define LP595_LE_PORT						GPIOC
#define LP595_SDI							GPIO_PIN_5
#define LP595_SDI_PORT						GPIOC
#define LP595_CLR							GPIO_PIN_6
#define LP595_CLR_PORT						GPIOC
#define LP595_SCK							GPIO_PIN_10
#define LP595_SCK_PORT						GPIOC

// LP165 (Read lockpad status with ic165) output control signals and input
#define LP165_SHLD							GPIO_PIN_12	// Shift-load / Latch
#define LP165_SHLD_PORT						GPIOC
#define LP165_CI							GPIO_PIN_11	// Clock inhibit / CE
#define LP165_CI_PORT						GPIOC
#define LP165_CLK							GPIO_PIN_10	// Clock input of IC / CLK
#define LP165_CLK_PORT						GPIOC
#define LP165_DATAIN						GPIO_PIN_3	// Data in of MCU / MISO
#define LP165_DATAIN_PORT					GPIOC


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


#define LED2_PIN							GPIO_PIN_2
#define LED2_GPIO_PORT						GPIOB


//SPI CS pin
#define SPI_CS_PIN							GPIO_PIN_12
#define SPI_CS_PORT							GPIOB



#endif /* INC_LOCKER_CONFIG_H_ */
