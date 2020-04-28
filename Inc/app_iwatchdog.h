/*
 * app_iwatchdog.h
 *
 *  Created on: Mar 23, 2020
 *      Author: VAIO
 */

#ifndef APP_IWATCHDOG_H_
#define APP_IWATCHDOG_H_

void MX_IWDG_Init(void);
void Watchdog_Refresh(void);
void Reset_Watchdog_Counting(void);
void Watchdog_Counting(void);
uint8_t Is_Watchdog_Reset(void);
#endif /* APP_IWATCHDOG_H_ */
