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

uint32_t GetADCValue(uint8_t channel);
void StartGettingADCValues(void);
void StopGettingADCValues(void);

#endif /* APP_ADC_H_ */
