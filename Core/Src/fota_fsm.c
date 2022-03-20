/*
 * fota_fsm.c
 *
 *  Created on: Nov 17, 2021
 *      Author: DO XUAN THO
 */
#include "fota_fsm.h"
#include "ServerCommunication/app_http.h"
#include "app_version.h"

#define FLASH_BUFFER_SIZE 	2048

FSM_State_Typedef fsm_curr_state = START_SIMCOM;
FSM_State_Typedef fsm_prev_state = START_SIMCOM;
uint8_t flash_buffer[2048];
static uint8_t ret;

void Start_Simcom();
void Version_Checking_Http();

FSM_Machine_TypeDef fsm_state_machine[]={
		{START_SIMCOM				, 		Start_Simcom				},
		{VERSION_CHECKING_HTTP		, 		Version_Checking_Http		},
};

/**
 * HTTP_Display_State()
 * @brief This is function for display state of State Machine. It only show State when having a state changation.
 */
void FSM_Display_State(void){
	if(fsm_curr_state!=fsm_prev_state){
		fsm_prev_state = fsm_curr_state;
		switch (fsm_curr_state) {
			case START_SIMCOM:
				LOG("\r\nSTART SIMCOM\r\n");
				break;
			case VERSION_CHECKING_HTTP:
				LOG("\r\nVERSION CHECKING HTTP\r\n");
				break;
			default:
				break;
		}
	}

}

/**
 * HTTP_Run()
 * @brief This is function can be called from external file. It run follow state machine method. Not have param.
 */
uint8_t FSM_Run(void){
	FSM_Display_State();
	if(fsm_curr_state < FSM_MAX_STATE){
		(*fsm_state_machine[fsm_curr_state].func)();
		return 0;
	}
	else{
		return 1;
	}
}


void Start_Simcom(){
	if(Sim7600_Run()){
		fsm_curr_state = VERSION_CHECKING_HTTP;
	}
}
void Version_Checking_Http(){
	ret = HTTP_Run();
	if(ret == 1){
		LOG("Get into\r\n");
		if(Get_Update_Firmware_Status() == UPDATE_FAILED){
			LOG("Get into Update Failed\r\n");
			Jump_To_Factory_Firmware();
		}
		else{
			LOG("Get into Update Success\r\n");
			fsm_curr_state = COPY_NEW_FIRMWARE;
			Jump_To_Current_Firmware();
		}
	}
	else if(ret == 2){
		Reset_SimConfiguration_State();
		HTTP_Set_State(HTTP_INIT);
		fsm_curr_state = START_SIMCOM;
	}
}


FSM_State_Typedef FSM_Get_State(){
	return fsm_curr_state;
}
void FSM_Set_State(FSM_State_Typedef fsm_state){
	fsm_curr_state = fsm_state;
}
