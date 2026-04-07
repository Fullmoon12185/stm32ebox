/*
 * app_gpio.h
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#ifndef APP_GPIO_H_
#define APP_GPIO_H_


void Turn_On_Buzzer(void);
void Turn_Off_Buzzer(void);

void Turn_On_LED(void);
void Turn_Off_LED(void);


void SPI_CS_Init(void);
void MX_GPIO_Init(void);

void LED_Init(void);
void GPIO_Relay_Init(void);
void Buzzer_Init(void);
void ZeroPoint_Detection_Pin_Init(void);
void ZeroPoint_Detection_Pin_Clear_Interrupt_Flag(void);

#if(ESTOP_BUTTON == 1)
	uint8_t isEstopPressed(void);
	void Estop_Processing(void);
#endif

#endif /* APP_GPIO_H_ */


