/*
 * app_bootloader.h
 *
 *  Created on: Jul 29, 2021
 *      Author: Nguyet
 */

#ifndef APP_BOOTLOADER_H_
#define APP_BOOTLOADER_H_

#include "app_flash_internal.h"
/* Config VECT_TAB_OFFSET in system_stm32f1xx.c ------------------------------*/
/*!< Vector Table base offset field.
     This value must be a multiple of 0x200. */
#define APPLICATION_ADDRESS     			ADDR_FLASH_PAGE_0
#define BOOTLOADER_ADDRESS     				ADDR_FLASH_PAGE_50
#define FWOTA_ADDRESS						ADDR_FLASH_PAGE_75

/* Flash memory layout -------------------------------------------------------*/
#define FLASH_PAGESIZE_APPLICATION		    50	// First 100KB 	/ 256KB
#define FLASH_PAGESIZE_BOOTLOADER			25	// Next  50KB 	/ 256KB
#define FLASH_PAGESIZE_FWOTA				50	// Next  100KB 	/ 256KB

/* Address User's Application info -------------------------------------------*/
#define USER_INFO_ADDRESS        			ADDR_FLASH_PAGE_125

/* Address OTA update firmware info ------------------------------------------*/
#define OTA_INFO_ADDRESS        			ADDR_FLASH_PAGE_126

#define OTA_FLAG_OFFSET         			4
#define OTA_FTP_HOST_OFFSET          	    100
#define OTA_FTP_PORT_OFFSET       		    8
#define OTA_USER_OFFSET     				32
#define OTA_PASS_OFFSET           		    32
#define OTA_PATH_OFFSET         			100
#define OTA_VERSION_OFFSET    	 			12
#define	OTA_FB_DONE_OFFSET					4
#define	OTA_SSID_WIFI_OFFSET				32
#define	OTA_PASS_WIFI_OFFSET				64

#define OTA_FLAG_ADDRESS        			OTA_INFO_ADDRESS
#define OTA_FTP_HOST_ADDRESS         	    OTA_FLAG_ADDRESS      + OTA_FLAG_OFFSET
#define OTA_FTP_PORT_ADDRESS      		    OTA_FTP_HOST_ADDRESS  + OTA_FTP_HOST_OFFSET
#define OTA_USER_ADDRESS    				OTA_FTP_PORT_ADDRESS  + OTA_FTP_PORT_OFFSET
#define OTA_PASS_ADDRESS					OTA_USER_ADDRESS 			+ OTA_USER_OFFSET
#define OTA_PATH_ADDRESS          		    OTA_PASS_ADDRESS   		+ OTA_PASS_OFFSET
#define OTA_VERSION_ADDRESS        		    OTA_PATH_ADDRESS      + OTA_PATH_OFFSET
#define OTA_SSID_WIFI_ADDRESS				OTA_VERSION_ADDRESS		+ OTA_VERSION_OFFSET
#define OTA_PASS_WIFI_ADDRESS				OTA_SSID_WIFI_ADDRESS + OTA_SSID_WIFI_OFFSET

#define	OTA_FB_DONE_ADDRESS 				OTA_INFO_ADDRESS

#endif /* APP_BOOTLOADER_H_ */
