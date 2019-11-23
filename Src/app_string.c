/*
 * app_string.c
 *
 *  Created on: Nov 25, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_string.h"


uint8_t GetStringLength(uint8_t* buffer){
	uint8_t len = 0;
	if(*buffer == 0){
		return 0;
	}
	while(*buffer != 0){
		len ++;
		if(*buffer == '\n') break;
		buffer ++;

	}
	return len;
}

