/*
 * app_adc.h
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */


#ifndef APP_ADC_H_
#define APP_ADC_H_

void ADC1_Init(void);
void ADC_DMA_Init(void);

void ADC_Start_Getting_Values(void);
void ADC_Stop_Getting_Values(void);
void PowerConsumption_FSM(void);

void Zero_Point_Detection(void);

uint8_t Is_Done_Getting_ADC(void);
#endif /* APP_ADC_H_ */
