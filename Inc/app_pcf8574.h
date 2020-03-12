/*
 * app_pcf8574.h
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */

#ifndef APP_PCF8574_H_
#define APP_PCF8574_H_



void PCF_Init(void);
uint32_t Get_All_Relay_Fuse_Statuses(void);
uint8_t PCF_Scan();
void PCF_read(void);



#endif /* APP_PCF8574_H_ */
