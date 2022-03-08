/**
 *@file app_string.h
 *@author thodo
 *@brief Header file for app_string.c file. This file contains some string handling function support for UART Handling.
 */

#ifndef INC_APP_STRING_H_
#define INC_APP_STRING_H_
#include "main.h"

FlagStatus isReceiveData(char *buffer , uint16_t buffer_len , const char * data);
FlagStatus isReceiveData_New(char *buffer , uint16_t buffer_len , uint16_t buffer_size , const char * data);
uint16_t String2Int(char * buffer , uint16_t buffer_length);

#endif /* INC_APP_STRING_H_ */
