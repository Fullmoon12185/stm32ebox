/*
 * app_adc.h
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */


#ifndef APP_ADC_H_
#define APP_ADC_H_


typedef enum {
	SETUP_TIMER_ONE_SECOND = 0,
	FIND_ZERO_VOLTAGE_POINT,
	START_GETTING_ADC,
	WAIT_FOR_DATA_COMPLETE_TRANSMIT,
	STOP_GETTING_ADC,
	REPORT_POWER_DATA,
	COMPUTE_ROOT_MEAN_SQUARE,
	COMPUTE_PEAK_TO_PEAK_VOLTAGE,
	COMPUTE_POWER_CONSUMPTION,
	PREPARE_FOR_THE_NEXT_CONVERSION,
	MAX_NUMBER_OF_ADC_STATES
}ADC_STATE;

void ADC1_Init(void);
void ADC_DMA_Init(void);

void ADC_Start_Getting_Values(void);
void ADC_Stop_Getting_Values(void);
void PowerConsumption_FSM(void);

void Zero_Point_Detection(void);
uint8_t get_PowerConsumption_FSM_State();
//Vinh add begin
//uint8_t get_power_factor();
//uint8_t get_power();
//uint8_t get_energy();
//Vinh add end
#endif /* APP_ADC_H_ */
