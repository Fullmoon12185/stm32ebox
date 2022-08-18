/*
 * app_relay.h
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */

#ifndef APP_RELAY_H_
#define APP_RELAY_H_


void Relay_Init(void);

void Clear_Relay_Timeout_Flag(void);
void Set_Relay_Timeout_Flag(void);
uint8_t is_Set_Relay_Timeout(void);




void Set_Relay1(uint8_t relayIndex);
void Set_Relay(uint8_t relayIndex);
void Reset_Relay(uint8_t relayIndex);


void Update_Relay_Physical_Status(void);
FlagStatus Get_Relay_Status(uint8_t relayIndex);
FlagStatus Get_Is_Update_Relay_Status(void);
void Check_Physical_Working_Statuses_Of_Relays_Fuses(uint8_t relayIndex);
void Check_All_Physical_Working_Statuses_Of_Relays_Fuses(void);

void Clear_Counter_For_Checking_Total_Current(void);
void Increase_Counter_For_Checking_Total_Current(void);
uint8_t Is_Timeout_For_Checking_Total_Current(void);

uint8_t Reset_Latest_Relay(void);
void Clear_Latest_Relay_Buffer(void);
void Update_Latest_Relay(uint8_t index);
#endif /* APP_RELAY_H_ */
