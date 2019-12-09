/*
 * app_init.c
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_adc.h"
#include "app_accel.h"
#include "app_flash.h"
#include "app_gpio.h"
#include "app_sim3g.h"
#include "app_temperature.h"
#include "app_timer.h"
#include "app_uart.h"
#include "app_system_clock_config.h"
#include "app_sim3g.h"
#include "app_init.h"




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
        	UART2_Init();
        	UART1_Init();
        	DEBUG_INIT(printf("UART_INIT - Done \r\n"));
        	break;
        case GPIO_INIT:
        	MX_GPIO_Init();
        	ADC_DMA_Init();
        	DEBUG_INIT(printf("GPIO_INIT - ADC_DMA_Init - Done \r\n"));
            break;
        case FLASH_INIT:
        	DEBUG_INIT(printf("FLASH_INIT - Done \r\n"));
        	break;
        case TIMER_INIT:
        	Timer_Init();
        	DEBUG_INIT(printf("TIMER_INIT - Done \r\n"));
            break;
        case ADC_INIT:
        	ADC1_Init();
        	DEBUG_INIT(printf("ADC_INIT - Done \r\n"));
        	break;
        case WATCH_DOG_INIT:

        	DEBUG_INIT(printf("WATCH_DOG_INIT - Done \r\n"));
            break;
        case START_DMA_ADC:
        	StartGettingADCValues();
        	DEBUG_INIT(printf("START_DMA_ADC - Done \r\n"));
        	break;
        case SIM_3G_INIT:
        	Sim3g_Init();
        	DEBUG_INIT(printf("SIM_3G_INIT - Done \r\n"));
        	break;
        default:
            initState = HAL_INIT;
            break;
        }
        initState = initState + 1;
    }
}


