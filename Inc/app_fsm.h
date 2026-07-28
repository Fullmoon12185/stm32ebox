/*
 * app_fsm.h
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */

#ifndef APP_FSM_H_
#define APP_FSM_H_


void Server_Communication(void);

uint8_t Is_Publishing_Message(void);
void Allow_To_Send_Specific_Topic(uint8_t topicIndex);

#endif /* APP_FSM_H_ */
