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
	GPIO_INIT,
    UART_INIT,
	RELAY_INIT,
	LED_DISPLAY_INIT,
	FLASH_INIT,
    TIMER_INIT,
	SPI_INIT,
	SPI_25LCXXX_INIT,
	I2C_INIT,
	ACCELERATOR_INIT,
	POWER_SETUP_INIT,
	SIM_3G_INIT,
	END_OF_INITIALISATION_STATE,
	ADC_INIT,
	START_DMA_ADC,
	WATCH_DOG_INIT,

    MAX_INIT_STATE
};

void System_Initialization(void);



#endif /* APP_INIT_H_ */
