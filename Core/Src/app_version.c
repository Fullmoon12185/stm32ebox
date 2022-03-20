/*
 * app_version.c
 *
 *  Created on: Nov 17, 2021
 *      Author: DO XUAN THO
 */

#include "app_version.h"
#include "main.h"
#include "flash.h"
#include "Peripheral/app_uart.h"



char current_version[50];
char factory_version[50];
uint8_t update_status;

char* Get_Current_Version(){
	Flash_ReadChar(CURRENT_VERSION_ADDR,current_version,strlen(VERSION));
	return current_version;
}

uint8_t Set_Current_Version(char * version){
	Flash_Erase(CURRENT_VERSION_ADDR,1);
	Flash_Write_Char(CURRENT_VERSION_ADDR, version,strlen(VERSION));
}


void Jump_To_Current_Firmware(){
	Flash_Erase(FIRMWARE_CHOOSEN,1);
    Flash_Write_Int(FIRMWARE_CHOOSEN, CURRENT_FIRMWARE_CHOOSEN);
//	void (*app_reset_handler)(void) = (void*)(*(volatile uint32_t*) (CURRENT_FIRMWARE_ADDR + 4));
//	app_reset_handler();
	NVIC_SystemReset();
}

void Jump_To_Factory_Firmware(){
	Flash_Erase(FIRMWARE_CHOOSEN,1);
    Flash_Write_Int(FIRMWARE_CHOOSEN, FACTORY_FIRMWARE_CHOOSEN);
//	void (*app_reset_handler)(void) = (void*)(*(volatile uint32_t*) (FOTA_FIRMWARE_ADDR + 4));
//	app_reset_handler();
	NVIC_SystemReset();
}

void Jump_To_Fota_Firmware(){
	Flash_Erase(FIRMWARE_CHOOSEN,1);
    Flash_Write_Int(FIRMWARE_CHOOSEN, FOTA_FIRMWARE_CHOOSEN);
//	void (*app_reset_handler)(void) = (void*)(*(volatile uint32_t*) (FOTA_FIRMWARE_ADDR + 4));
//	app_reset_handler();
	NVIC_SystemReset();
}

void Update_Firmware_Failed(){
	Flash_Erase(UPDATE_STATUS_ADDR,1);
    Flash_Write_Int(UPDATE_STATUS_ADDR, UPDATE_FAILED);
}
void Update_Firmware_Success(){
	Flash_Erase(UPDATE_STATUS_ADDR,1);
    Flash_Write_Int(UPDATE_STATUS_ADDR, UPDATE_SUCCESS);
}

uint8_t Get_Update_Firmware_Status(){
	return update_status;
}

void Clear_Update_Firmware_Status(){
    Flash_Erase(UPDATE_STATUS_ADDR, 1);
}





