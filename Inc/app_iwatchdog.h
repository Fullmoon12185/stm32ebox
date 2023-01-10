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


void Clear_Counter_For_Reset_Module_Sim(void);
uint8_t Is_Reset_Module_Sim(void);

void Clear_For_Watchdog_Reset_Due_To_Not_Sending_Mqtt_Message(void);
uint8_t Is_Watchdog_Reset_Due_To_Not_Sending_Mqtt_Message(void);
#endif /* APP_IWATCHDOG_H_ */
