/*
 * app_version.h
 *
 *  Created on: Nov 17, 2021
 *      Author: DO XUAN THO
 */

#ifndef INC_APP_VERSION_H_
#define INC_APP_VERSION_H_

#include "main.h"

void Set_Is_Update_Firmware(void);
void Clear_Is_Update_Firmware(void);
uint8_t Is_Update_Firmware(void);


uint16_t Get_Current_Version();
uint16_t Get_Factory_Version();
void Set_Current_Version(uint16_t version);
void Set_Factory_Version(uint16_t version);
void Jump_To_Factory_Firmware();
void Jump_To_Current_Firmware();
void Jump_To_Fota_Firmware();


#endif /* INC_APP_VERSION_H_ */
