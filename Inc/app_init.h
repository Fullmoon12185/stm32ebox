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
    FLASH_INIT,
    TIMER_INIT,
    WATCH_DOG_INIT,
    MAX_INIT_STATE
};

void System_Initialization(void);



#endif /* APP_INIT_H_ */
