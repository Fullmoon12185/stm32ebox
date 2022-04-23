/*
 * app_version.c
 *
 *  Created on: Nov 17, 2021
 *      Author: DO XUAN THO
 */

#include "app_version.h"
#include "main.h"
#include "app_flash.h"



uint16_t Get_Current_Version(){
	return Flash_Read_Int(CURRENT_VERSION_ADDR);
}

void Set_Current_Version(uint16_t version){
	Flash_Erase(CURRENT_VERSION_ADDR,1);
	Flash_Write_Int(CURRENT_VERSION_ADDR, version);
}

uint16_t Get_Factory_Version(){
	return Flash_Read_Int(FACTORY_VERSION_ADDR);
}

void Set_Factory_Version(uint16_t version){
	Flash_Erase(FACTORY_VERSION_ADDR,1);
	Flash_Write_Int(FACTORY_VERSION_ADDR, (int)version);
}

uint8_t Have_Current_Firmware(){
	return Get_Current_Version() == VERSION_EBOX;
}

uint8_t Have_Factory_Firmware(){
	return Get_Factory_Version() == VERSION_EBOX;
}


void Jump_To_Fota_Firmware(){
	Flash_Erase(FIRMWARE_CHOOSEN,1);
    Flash_Write_Int(FIRMWARE_CHOOSEN, FOTA_FIRMWARE_CHOOSEN);
//	void (*app_reset_handler)(void) = (void*)(*(volatile uint32_t*) (FOTA_FIRMWARE_ADDR + 4));
//	app_reset_handler();
	NVIC_SystemReset();
}

void Jump_To_Current_Firmware(){
	Flash_Erase(FIRMWARE_CHOOSEN,1);
    Flash_Write_Int(FIRMWARE_CHOOSEN, CURRENT_FIRMWARE_CHOOSEN);
//	void (*app_reset_handler)(void) = (void*)(*(volatile uint32_t*) (CURRENT_FIRMWARE_ADDR + 4));
//	app_reset_handler();
	NVIC_SystemReset();
}







