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
        case GPIO_INIT:
        	MX_GPIO_Init();
            break;
        case UART_INIT:
        	MX_USART2_UART_Init();
            break;
        case FLASH_INIT:
        	break;
        case TIMER_INIT:
        	Timer_Init();
            break;
        case WATCH_DOG_INIT:
            break;
        default:
            initState = HAL_INIT;
            break;
        }
        initState = initState + 1;
    }
}


