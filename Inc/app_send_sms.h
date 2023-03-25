/*
 * app_send_sms.h
 *
 *  Created on: Nov 26, 2022
 *      Author: Nguyet
 */

#ifndef APP_SEND_SMS_H_
#define APP_SEND_SMS_H_

typedef enum {
	SMS_FIRST_MESSAGE_AFTER_REBOOT = 0,
	SMS_NORMAL_MESSAGE,
	SMS_LOST_CONNECTION
}SMS_MESSAGE_TYPE;


void Send_Sms_Message_Init(void);
void Set_Send_Sms_Flag(void);
void Clear_Send_Sms_Flag(void);
uint8_t Is_Set_Send_Sms_Flag(void);
uint8_t Is_Done_Sending_Sms_Message(void);

void Start_Sending_Sms_Message(void);
void FSM_For_Sending_SMS_Message(void);

void Set_Sms_Message_Type(SMS_MESSAGE_TYPE msgType);

void Start_Sending_Lost_Connection_Message(void);
#endif /* APP_SEND_SMS_H_ */
