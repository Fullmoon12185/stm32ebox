/*
 * app_send_sms.h
 *
 *  Created on: Nov 26, 2022
 *      Author: Nguyet
 */

#ifndef APP_SEND_SMS_H_
#define APP_SEND_SMS_H_

void Set_Send_Sms_Flag(void);
void Clear_Send_Sms_Flag(void);
uint8_t Is_Set_Send_Sms_Flag(void);

void Start_Sending_Sms_Message(void);
void FSM_For_Sending_SMS_Message(void);

#endif /* APP_SEND_SMS_H_ */
