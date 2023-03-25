/*
 * app_fsm.h
 *
 *  Created on: Nov 23, 2019
 *      Author: VAIO
 */

#ifndef APP_FSM_H_
#define APP_FSM_H_


void Server_Communication(void);
void FSM_handle_subcribe_message(void);
void UpdatePublishMessage(uint8_t outletID, int32_t displayData);

#endif /* APP_FSM_H_ */
