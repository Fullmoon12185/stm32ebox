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

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}


