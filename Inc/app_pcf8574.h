/*
 * app_pcf8574.h
 *
 *  Created on: Dec 16, 2019
 *      Author: VAIO
 */

#ifndef APP_PCF8574_H_
#define APP_PCF8574_H_

//void Read_All_PCF8574_Inputs(void);
FlagStatus SAC_Status(uint8_t sacIndex);

//void PCF_init();
//uint8_t PCF_Scan();
uint8_t PCF_read(uint8_t address);
void Set_Input_PCF_Pins(void);
#endif /* APP_PCF8574_H_ */
