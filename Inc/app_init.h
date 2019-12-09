/*
 * app_init.h
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */

#ifndef APP_INIT_H_
#define APP_INIT_H_
enum InitState {

    HAL_INIT = 0,
    SYSTEM_CLOCK_INIT,
    UART_INIT,
	GPIO_INIT,
	FLASH_INIT,
    TIMER_INIT,
	ADC_INIT,
    WATCH_DOG_INIT,
	START_DMA_ADC,
	SIM_3G_INIT,
    MAX_INIT_STATE
};

void System_Initialization(void);



#endif /* APP_INIT_H_ */
