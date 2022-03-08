/*
 * app_ATProcessing.h
 *
 *  Created on: Sep 10, 2021
 *      Author: thodo
 */

#ifndef INC_FSM_APP_ATPROCESSING_H_
#define INC_FSM_APP_ATPROCESSING_H_

#include "main.h"

typedef enum {
	AT_OK=0,
	AT_ERROR,
	AT_INPUT,
	AT_PB_DONE,
	AT_SUBCRIBE_MESSAGE,
	AT_HTTP_RESPONSE,
	AT_IMEI,
	AT_FIRMWARE_VERSION,
	AT_FIRMWARE_DATA,
	AT_FIRMWARE_CHECKSUM,
	AT_NONE
}AT_Result;

void AT_Processing();
AT_Result Get_AT_Result();
void Clear_AT_Result();

#endif /* INC_FSM_APP_ATPROCESSING_H_ */
