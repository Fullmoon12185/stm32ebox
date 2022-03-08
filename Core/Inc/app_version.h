/*
 * app_version.h
 *
 *  Created on: Nov 17, 2021
 *      Author: DO XUAN THO
 */

#ifndef INC_APP_VERSION_H_
#define INC_APP_VERSION_H_

#include "main.h"

char* Get_Current_Version();
char* Get_Factory_Version();
uint8_t Set_Current_Version(char* version);
uint8_t Set_Factory_Version(char* version);
void Jump_To_Factory_Firmware();
void Jump_To_Current_Firmware();
void Jump_To_Fota_Firmware();


#endif /* INC_APP_VERSION_H_ */
