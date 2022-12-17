/*
 * app_init.c
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_adc.h"
#include "app_accel.h"
#include "app_flash_database.h"
#include "app_gpio.h"
#include "app_led_display.h"
#include "app_sim3g.h"
#include "app_sim5320MQTT.h"
#include "app_temperature.h"
#include "app_timer.h"
#include "app_uart.h"
#include "app_system_clock_config.h"
#include "app_sim3g.h"
#include "app_spi.h"
#include "app_i2c.h"
#include "app_pcf8574.h"
#include "app_25LC512.h"
#include "app_init.h"
#include "app_relay.h"
#include "app_power.h"
#include "app_iwatchdog.h"
#include "app_scheduler.h"
#include "app_eeprom.h"

#include "app_test.h"



enum InitState initState = HAL_INIT;

void System_Initialization(void)
{
    while(initState != MAX_INIT_STATE){
        switch (initState) {
        case HAL_INIT:
        	HAL_Init();
            break;
        case SYSTEM_CLOCK_INIT:
        	SystemClock_Config();

            break;
        case UART_INIT:
        	UART3_Init();
        	UART1_Init();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"UART_INIT - Done \r\n"));
        	break;
        case GPIO_INIT:
        	MX_GPIO_Init();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"GPIO_INIT - ADC_DMA_Init - Done \r\n"));
            break;
        case LED_DISPLAY_INIT:
        	Led_Display_Init();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"LED_DISPLAY_INIT - Done \r\n"));
        	break;
        case RELAY_INIT:
        	Relay_Init();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"RELAY_INIT - Done \r\n"));
        	break;
        case FLASH_INIT:
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"FLASH_INIT - Done \r\n"));
        	break;
        case TIMER_INIT:
        	Timer_Init();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"TIMER_INIT - Done \r\n"));
            break;
        case SPI_INIT:
        	SPI1_Init();
        	SPI2_Init();

        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"SPI_INIT - Done \r\n"));
        	break;
        case SPI_25LCXXX_INIT:
        	Eeprom_Initialize();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"SPI_25LCXXX_INIT - Done \r\n"));
        	break;
        case I2C_INIT:

        	I2C_Init();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"I2C_Init \r\n"));
        	PCF_Init();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"I2C_Init - Done \r\n"));
        	break;
        case ACCELERATOR_INIT:
        	break;
        case POWER_SETUP_INIT:
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"POWER_SETUP_INIT - Done \r\n"));
        	MX_GPIO_Init();
        	Power_Setup();

        	break;
        case SIM_3G_INIT:
        	Sim3g_Init();
        	Set_Up_Topic_Names();
//        	Set_Sim3G_State(POWER_OFF_SIM3G);
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"SIM_3G_INIT - Done \r\n"));
        	break;
        case END_OF_INITIALISATION_STATE:
        	Turn_On_Buzzer();
			HAL_Delay(100);
			Turn_Off_Buzzer();
			HAL_Delay(100);
			Turn_On_Buzzer();
			HAL_Delay(100);
			Turn_Off_Buzzer();
        	break;
        case ADC_INIT:
        	ADC_DMA_Init();
        	ADC1_Init();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"ADC_INIT - Done \r\n"));

			HAL_Delay(100);
        	break;
        case START_DMA_ADC:
        	ADC_Start_Getting_Values();
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"START_DMA_ADC - Done \r\n"));
        	break;

        case WATCH_DOG_INIT:
        	DEBUG_INIT(UART3_SendToHost((uint8_t*)"WATCH_DOG_INIT - Done \r\n"));
            break;
        default:
            initState = HAL_INIT;
            break;
        }
        initState = initState + 1;
    }
}


