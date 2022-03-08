/*
 * fota_fsm.h
 *
 *  Created on: Nov 17, 2021
 *      Author: DO XUAN THO
 */

#ifndef INC_FOTA_FSM_H_
#define INC_FOTA_FSM_H_
#include "main.h"

typedef enum{
	START_SIMCOM,
	VERSION_CHECKING_HTTP,
	COPY_NEW_FIRMWARE,
	FSM_MAX_STATE
}FSM_State_Typedef;



typedef struct{
	FSM_State_Typedef fsm_state;		/*!< This is HTTP_State of State Machine. It help to manage state easier*/
	void(*func)(void);			/*!< This is function pointer when State Machine get onto http_state above so It will run*/
}FSM_Machine_TypeDef;

uint8_t FSM_Run();
FSM_State_Typedef FSM_Get_State();
void FSM_Set_State(FSM_State_Typedef fsm_state);
void FSM_Display_State(void);

#endif /* INC_FOTA_FSM_H_ */
