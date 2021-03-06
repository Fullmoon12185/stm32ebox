 /*
 * app_adc.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "math.h"

#include "app_scheduler.h"
#include "app_uart.h"
#include "app_fsm.h"
#include "app_test.h"
#include "app_power.h"


#define		DEBUG_ADC(X)								X
#define 	NUMBER_OF_SAMPLES_FOR_SMA					3
#define		REFERENCE_1V8_VOLTAGE_INDEX					12
#define		DIFFERENCE_ADC_VALUE_THRESHOLD				10

ADC_HandleTypeDef ADC1Handle;
DMA_HandleTypeDef Hdma_adc1Handle;


//uint32_t ADCValues[NUMBER_OF_ADC_CHANNELS];

int32_t AdcDmaBuffer[NUMBER_OF_ADC_CHANNELS];
int32_t AdcBufferSUM[NUMBER_OF_ADC_CHANNELS];

int32_t AdcBufferSMA[NUMBER_OF_ADC_CHANNELS];

int32_t AdcBufferSMA_Head[NUMBER_OF_ADC_CHANNELS];

int32_t PowerFactor[NUMBER_OF_RELAYS];

int32_t AdcBuffer[NUMBER_OF_RELAYS][NUMBER_OF_SAMPLES_PER_SECOND];
//int32_t AdcBufferFilter[NUM_OF_FILTER][NUMBER_OF_ADC_CHANNELS];
int32_t AdcBufferPeakMax[NUMBER_OF_RELAYS],
		AdcBufferPeakMin[NUMBER_OF_RELAYS],
		AdcBufferPeakPeak[NUMBER_OF_RELAYS];
int32_t AdcDmaBufferIndex = 0, AdcDmaBufferIndexFilter = 0;
int32_t AdcBufferCurrent[NUMBER_OF_RELAYS];

uint8_t strtmp[] = "Begin read ADcs \r\n";
uint32_t array_Of_Vrms_ADC_Values[NUMBER_OF_RELAYS];
uint32_t array_Of_Average_Vrms_ADC_Values[NUMBER_OF_RELAYS];
//
uint32_t array_Of_Irms_ADC_Values[NUMBER_OF_RELAYS];
uint32_t array_Of_Power_Consumption[NUMBER_OF_RELAYS];
uint32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_RELAYS];



FlagStatus AdcDmaStoreFlag = RESET;
#define ADC_READING_TIME_OUT	95

typedef enum {
	ADC_SETUP_TIMER_ONE_SECOND = 0,
	ADC_FIND_ZERO_VOLTAGE_POINT,
	ADC_START_GETTING,
	ADC_WAIT_FOR_DATA_COMPLETE_TRANSMIT,
	ADC_COMPUTE_PEAK_TO_PEAK_VOLTAGE,
	ADC_REPORT_POWER_DATA,
	MAX_NUMBER_OF_ADC_STATES
}ADC_STATE;

uint8_t adc_TimeoutFlag = 0;
uint8_t adc_Timeout_Task_Index = SCH_MAX_TASKS;
FlagStatus is_Ready_To_Find_Min_Max_Voltage = RESET;
ADC_STATE adcState = ADC_SETUP_TIMER_ONE_SECOND;
ADC_STATE pre_adcState = MAX_NUMBER_OF_ADC_STATES;



void Adc_State_Display(void);

/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  AdcHandle : AdcHandle handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
{
	AdcDmaStoreFlag = SET;
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
void ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  ADC1Handle.Instance = ADC1;
  ADC1Handle.Init.ScanConvMode = ADC_SCAN_ENABLE;
  ADC1Handle.Init.ContinuousConvMode = ENABLE;
  ADC1Handle.Init.DiscontinuousConvMode = DISABLE;
  ADC1Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  ADC1Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  ADC1Handle.Init.NbrOfConversion = 14;
  if (HAL_ADC_Init(&ADC1Handle) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_9;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_10;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_11;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_12;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_13;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_14;
  if (HAL_ADC_ConfigChannel(&ADC1Handle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	is_Ready_To_Find_Min_Max_Voltage = SET;
//	test2();
	return;
}
/**
  * Enable DMA controller clock
  */
void ADC_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}
void ADC_Start_Getting_Values(void){
	HAL_ADC_Start_DMA(&ADC1Handle, (uint32_t*) AdcDmaBuffer, NUMBER_OF_ADC_CHANNELS);
}

void ADC_Stop_Getting_Values(void){
	HAL_ADC_Stop_DMA(&ADC1Handle);
}


void Adc_Clear_Timeout_Flag(void){
	adc_TimeoutFlag = 0;
}
void Adc_Reading_Timeout(void){
	adc_TimeoutFlag = 1;
}
uint8_t is_Adc_Reading_Timeout(void){
	return adc_TimeoutFlag;
}



void Zero_Point_Detection(void){
	static uint8_t zeroPointState = 0;
	switch(zeroPointState){
	case 0:
		if(HAL_GPIO_ReadPin(ZERO_POINT_DETECTION_PORT, ZERO_POINT_DETECTION_PIN) == GPIO_PIN_SET){
			zeroPointState = 1;
			is_Ready_To_Find_Min_Max_Voltage = SET;
		}
		break;
	case 1:
		if(HAL_GPIO_ReadPin(ZERO_POINT_DETECTION_PORT, ZERO_POINT_DETECTION_PIN) == GPIO_PIN_RESET){
			zeroPointState = 0;
		}
		break;
	}
}

FlagStatus Is_Done_Getting_ADC(void){
	if(adcState == ADC_REPORT_POWER_DATA)
		return SET;
	return RESET;
}


uint8_t filterNoiseState = 0;

void PowerConsumption_FSM(void){
	static uint8_t externalInterruptCounter = 0;
	static uint8_t cycleCounter = 0;
	Adc_State_Display();
	switch(adcState){
	case ADC_SETUP_TIMER_ONE_SECOND:
		SCH_Delete_Task(adc_Timeout_Task_Index);
		Adc_Clear_Timeout_Flag();
		adc_Timeout_Task_Index = SCH_Add_Task(Adc_Reading_Timeout, 100, 0);
		is_Ready_To_Find_Min_Max_Voltage = RESET;
		externalInterruptCounter = 0;
		AdcDmaBufferIndexFilter = 0;
		cycleCounter = 0;
		adcState = ADC_FIND_ZERO_VOLTAGE_POINT;
		break;
	case ADC_FIND_ZERO_VOLTAGE_POINT:
		if(is_Ready_To_Find_Min_Max_Voltage == SET){
			is_Ready_To_Find_Min_Max_Voltage = RESET;
			externalInterruptCounter++;
			AdcDmaBufferIndexFilter = 0;
			adcState = ADC_START_GETTING;
		}
		break;

	case ADC_START_GETTING:
		if(is_Ready_To_Find_Min_Max_Voltage == SET){
			is_Ready_To_Find_Min_Max_Voltage = RESET;
			externalInterruptCounter++;
		} else {
			if(externalInterruptCounter < 3){
				ADC_Start_Getting_Values();
				adcState = ADC_WAIT_FOR_DATA_COMPLETE_TRANSMIT;
			} else if(externalInterruptCounter >= 3) {
				adcState = ADC_COMPUTE_PEAK_TO_PEAK_VOLTAGE;
			}
		}
		break;

	case ADC_WAIT_FOR_DATA_COMPLETE_TRANSMIT:
		if(AdcDmaStoreFlag == SET){
			ADC_Stop_Getting_Values();
			AdcDmaStoreFlag = RESET;
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex++) {
				AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] = AdcDmaBuffer[channelIndex] - AdcDmaBuffer[REFERENCE_1V8_VOLTAGE_INDEX];
//				if(AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] < 10 && AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] > -10){
//					AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] = 0;
//				}
			}
			AdcDmaBufferIndexFilter++;
			if(AdcDmaBufferIndexFilter % NUMBER_OF_SAMPLES_PER_SECOND == 0){
				AdcDmaBufferIndexFilter = 0;
			}
			adcState = ADC_START_GETTING;
		}
		break;
	case ADC_COMPUTE_PEAK_TO_PEAK_VOLTAGE:
		for(uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex ++){
			for(uint8_t sampleIndex = 0; sampleIndex < AdcDmaBufferIndexFilter; sampleIndex ++){
//				if(channelIndex == 3){
//					sprintf((char*) strtmp, "%d\r\n", (int) AdcBuffer[channelIndex][sampleIndex]);
//					UART3_SendToHost((uint8_t *)strtmp);
//				}
				if(sampleIndex == 0){
					AdcBufferPeakMax[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
					AdcBufferPeakMin[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
				} else {
					if(AdcBufferPeakMax[channelIndex] < AdcBuffer[channelIndex][sampleIndex]){
						AdcBufferPeakMax[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
					}
					if(AdcBufferPeakMin[channelIndex] > AdcBuffer[channelIndex][sampleIndex]){
						AdcBufferPeakMin[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
					}
				}
				int32_t tempRealADCValue = AdcBuffer[channelIndex][sampleIndex];
				array_Of_Vrms_ADC_Values[channelIndex] += tempRealADCValue * tempRealADCValue;

				if(sampleIndex == AdcDmaBufferIndexFilter - 1){
					int32_t tempPeakPeak = AdcBufferPeakMax[channelIndex] - AdcBufferPeakMin[channelIndex];

					if(AdcBufferPeakMax[channelIndex] <= 10 || AdcBufferPeakMin[channelIndex] >= -10){
						tempPeakPeak = 0;
					} else if(tempPeakPeak < 60){
						tempPeakPeak = 0;
					}
					if(cycleCounter == 0){
						AdcBufferPeakPeak[channelIndex] = tempPeakPeak;
					} else {
						AdcBufferPeakPeak[channelIndex] = AdcBufferPeakPeak[channelIndex] + tempPeakPeak;
					}
					array_Of_Vrms_ADC_Values[channelIndex] = (array_Of_Vrms_ADC_Values[channelIndex])/AdcDmaBufferIndexFilter;
					if(cycleCounter == 0){
						array_Of_Average_Vrms_ADC_Values[channelIndex] = sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
					} else {
						array_Of_Average_Vrms_ADC_Values[channelIndex] = array_Of_Average_Vrms_ADC_Values[channelIndex] + sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
					}
				}

			}
		}
		cycleCounter++;
		if(cycleCounter >= NUMBER_OF_SAMPLES_PER_AVERAGE){
			cycleCounter = 0;
			for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++){
				AdcBufferPeakPeak[i] = AdcBufferPeakPeak[i] >> SAMPLE_STEPS;
				array_Of_Average_Vrms_ADC_Values[i] = array_Of_Average_Vrms_ADC_Values[i] >> SAMPLE_STEPS;
//				PowerFactor[i] = (array_Of_Average_Vrms_ADC_Values[i]*1000 * 100 * 2) / (AdcBufferPeakPeak[i] * 707);
				PowerFactor[i] = (array_Of_Average_Vrms_ADC_Values[i]*283) / (AdcBufferPeakPeak[i]);
				if(PowerFactor[i] > 98){
					PowerFactor[i] = 100;
				}
				Node_Update(i+1, array_Of_Average_Vrms_ADC_Values[i] * 237, 225, PowerFactor[i], 1);
			}

			adcState = ADC_REPORT_POWER_DATA;
			HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, RESET);
		} else {
			externalInterruptCounter = 0;
			adcState = ADC_FIND_ZERO_VOLTAGE_POINT;
		}
		break;

	case ADC_REPORT_POWER_DATA:
		if(is_Adc_Reading_Timeout()){
			HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, SET);
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex++) {
				array_Of_Vrms_ADC_Values[channelIndex]  = 0;
				array_Of_Average_Vrms_ADC_Values[channelIndex] = 0;
				AdcBufferPeakMax[channelIndex] = 0;
				AdcBufferPeakMin[channelIndex] = 0;
			}
			adcState = ADC_SETUP_TIMER_ONE_SECOND;
		}
		break;

	default:
		adcState = ADC_SETUP_TIMER_ONE_SECOND;
		break;
	}
}


/*
void PowerConsumption_FSM_1(void){
	static uint8_t externalInterruptCounter = 0;
	static uint8_t cycleCounter = 0;

	switch(adcState){
	case SETUP_TIMER_ONE_SECOND:
		SCH_Delete_Task(adc_Timeout_Task_Index);
		Adc_Clear_Timeout_Flag();
		adc_Timeout_Task_Index = SCH_Add_Task(Adc_Reading_Timeout, 100, 0);
		is_Ready_To_Find_Min_Max_Voltage = RESET;
		externalInterruptCounter = 0;
		AdcDmaBufferIndexFilter = 0;
		cycleCounter = 0;
		adcState = FIND_ZERO_VOLTAGE_POINT;
		break;
	case FIND_ZERO_VOLTAGE_POINT:
		if(is_Ready_To_Find_Min_Max_Voltage){
			is_Ready_To_Find_Min_Max_Voltage = RESET;
			externalInterruptCounter++;
			AdcDmaBufferIndexFilter = 0;
			adcState = START_GETTING_ADC;
		}
		break;

	case START_GETTING_ADC:
		if(is_Ready_To_Find_Min_Max_Voltage){
			is_Ready_To_Find_Min_Max_Voltage = RESET;
			externalInterruptCounter++;
		} else {
			if(externalInterruptCounter < 3){
				ADC_Start_Getting_Values();
				adcState = WAIT_FOR_DATA_COMPLETE_TRANSMIT;

			} else if(externalInterruptCounter == 3) {
				adcState = COMPUTE_PEAK_TO_PEAK_VOLTAGE;
			}
		}
		break;

	case WAIT_FOR_DATA_COMPLETE_TRANSMIT:
		if(AdcDmaStoreFlag){
			ADC_Stop_Getting_Values();
			AdcDmaStoreFlag = 0;
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex++) {
				AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] = AdcDmaBuffer[channelIndex] - AdcDmaBuffer[REFERENCE_1V8_VOLTAGE_INDEX];
//				if(AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] < 10 && AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] > -10){
//					AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] = 0;
//				}

//					if(AdcDmaBufferIndexFilter == 0){
//						AdcBufferPeakMax[channelIndex] = AdcBuffer[channelIndex][AdcDmaBufferIndexFilter];
//						AdcBufferPeakMin[channelIndex] = AdcBuffer[channelIndex][AdcDmaBufferIndexFilter];
//					} else {
//						if(AdcBufferPeakMax[channelIndex] < AdcBuffer[channelIndex][AdcDmaBufferIndexFilter]){
//							AdcBufferPeakMax[channelIndex] = AdcBuffer[channelIndex][AdcDmaBufferIndexFilter];
//						}
//						if(AdcBufferPeakMin[channelIndex] > AdcBuffer[channelIndex][AdcDmaBufferIndexFilter]){
//							AdcBufferPeakMin[channelIndex] = AdcBuffer[channelIndex][AdcDmaBufferIndexFilter];
//						}
//					}
			}
			AdcDmaBufferIndexFilter++;
			if(AdcDmaBufferIndexFilter % NUMBER_OF_SAMPLES_PER_SECOND == 0){
				AdcDmaBufferIndexFilter = 0;
			}

			adcState = START_GETTING_ADC;
		}
		break;
	case COMPUTE_PEAK_TO_PEAK_VOLTAGE:
//		for(uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex ++){
//			for(uint8_t sampleIndex = 0; sampleIndex < AdcDmaBufferIndexFilter; sampleIndex ++){
//
//				sample1[channelIndex] = sample2[channelIndex];
//				sample2[channelIndex] = sample3[channelIndex];
//				sample3[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
//				if(sampleIndex >= 3){
//					if((sample1[channelIndex] < sample2[channelIndex] && sample3[channelIndex] < sample2[channelIndex])
//							|| (sample1[channelIndex] > sample2[channelIndex] && sample3[channelIndex] > sample2[channelIndex])){
//						AdcBuffer[channelIndex][sampleIndex - 1] = (sample1[channelIndex] + sample3[channelIndex])/2;
//					}
//				}
//			}
//		}
		UART3_SendToHost((uint8_t *)"n3\r\n");
		for(uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex ++){
			for(uint8_t sampleIndex = 0; sampleIndex < AdcDmaBufferIndexFilter; sampleIndex ++){

				if(channelIndex == 3){
					sprintf((char*) strtmp, "%d\r\n", (int) AdcBuffer[channelIndex][sampleIndex]);
					UART3_SendToHost((uint8_t *)strtmp);
				}
				if(sampleIndex == 0){
					AdcBufferPeakMax[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
					AdcBufferPeakMin[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
				} else {
					if(AdcBufferPeakMax[channelIndex] < AdcBuffer[channelIndex][sampleIndex]){
						AdcBufferPeakMax[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
					}
					if(AdcBufferPeakMin[channelIndex] > AdcBuffer[channelIndex][sampleIndex]){
						AdcBufferPeakMin[channelIndex] = AdcBuffer[channelIndex][sampleIndex];
					}
				}
				int32_t tempRealADCValue = AdcBuffer[channelIndex][sampleIndex];
				array_Of_Vrms_ADC_Values[channelIndex] += tempRealADCValue * tempRealADCValue;

				if(sampleIndex == AdcDmaBufferIndexFilter - 1){
					int32_t tempPeakPeak = AdcBufferPeakMax[channelIndex] - AdcBufferPeakMin[channelIndex];

					if(AdcBufferPeakMax[channelIndex] <= 10 || AdcBufferPeakMin[channelIndex] >= -10){
						tempPeakPeak = 0;
					} else if(tempPeakPeak < 60){
						tempPeakPeak = 0;
					}

					if(cycleCounter == 0){
						AdcBufferPeakPeak[channelIndex] = tempPeakPeak;
					} else {
						AdcBufferPeakPeak[channelIndex] = AdcBufferPeakPeak[channelIndex] + tempPeakPeak;
					}

					array_Of_Vrms_ADC_Values[channelIndex] = (array_Of_Vrms_ADC_Values[channelIndex])/AdcDmaBufferIndexFilter;
					if(cycleCounter == 0){
						array_Of_Average_Vrms_ADC_Values[channelIndex] = sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
					} else {
						array_Of_Average_Vrms_ADC_Values[channelIndex] = array_Of_Average_Vrms_ADC_Values[channelIndex] + sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
					}
				}

			}
		}
		UART3_SendToHost((uint8_t *)"n1\r\n");
		cycleCounter++;
		if(cycleCounter == NUMBER_OF_SAMPLES_PER_AVERAGE){
			cycleCounter = 0;
			for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++){
				AdcBufferPeakPeak[i] = AdcBufferPeakPeak[i] >> SAMPLE_STEPS;
				array_Of_Average_Vrms_ADC_Values[i] = array_Of_Average_Vrms_ADC_Values[i] >> SAMPLE_STEPS;
				PowerFactor[i] = (array_Of_Average_Vrms_ADC_Values[i]*1000 * 100 * 2) / (AdcBufferPeakPeak[i] * 707);
			}
			sprintf((char*) strtmp, "%d\t", (int) PowerFactor[3]);
			UART3_SendToHost((uint8_t *)strtmp);
			sprintf((char*) strtmp, "%d\t", (int) array_Of_Average_Vrms_ADC_Values[3] * 2371);
			UART3_SendToHost((uint8_t *)strtmp);
			sprintf((char*) strtmp, "%d\t", (int) AdcBufferPeakPeak[3]);
			UART3_SendToHost((uint8_t *)strtmp);
			UART3_SendToHost((uint8_t *)"\r\n");
			sprintf((char*) strtmp, "%d\t", (int) PowerFactor[8]);
			UART3_SendToHost((uint8_t *)strtmp);
			sprintf((char*) strtmp, "%d\t", (int) array_Of_Average_Vrms_ADC_Values[8] * 493);
			UART3_SendToHost((uint8_t *)strtmp);
			sprintf((char*) strtmp, "%d\t", (int) AdcBufferPeakPeak[8]);
			UART3_SendToHost((uint8_t *)strtmp);
			UART3_SendToHost((uint8_t *)"\r\n");

			UART3_SendToHost((uint8_t *)"\r\n");
			adcState = REPORT_POWER_DATA;
			test2();
		} else {
			externalInterruptCounter = 0;
			adcState = FIND_ZERO_VOLTAGE_POINT;
		}
		break;

	case REPORT_POWER_DATA:
		if(is_Adc_Reading_Timeout()){
			test2();
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex++) {
				array_Of_Vrms_ADC_Values[channelIndex]  = 0;
				array_Of_Average_Vrms_ADC_Values[channelIndex] = 0;
				AdcBufferPeakMax[channelIndex] = 0;
				AdcBufferPeakMin[channelIndex] = 0;
				AdcBufferSUM[channelIndex] = 0;
			}
			adcState = SETUP_TIMER_ONE_SECOND;
		}
		break;

	default:
		break;
	}
}


void PowerConsumption_FSM_SVA(void){
	static uint8_t externalInterruptCounter = 0;
	static uint8_t cycleCounter = 0;

	switch(adcState){
	case SETUP_TIMER_ONE_SECOND:
		SCH_Delete_Task(adc_Timeout_Task_Index);
		Adc_Clear_Timeout_Flag();
		adc_Timeout_Task_Index = SCH_Add_Task(Adc_Reading_Timeout, 100, 0);
		is_Ready_To_Find_Min_Max_Voltage = RESET;
		externalInterruptCounter = 0;
		AdcDmaBufferIndexFilter = 0;
		cycleCounter = 0;
		adcState = FIND_ZERO_VOLTAGE_POINT;
		break;
	case FIND_ZERO_VOLTAGE_POINT:
		if(is_Ready_To_Find_Min_Max_Voltage){
			is_Ready_To_Find_Min_Max_Voltage = RESET;
			externalInterruptCounter++;
			AdcDmaBufferIndexFilter = 0;
			adcState = START_GETTING_ADC;
		}
		break;

	case START_GETTING_ADC:
		if(is_Ready_To_Find_Min_Max_Voltage){
			is_Ready_To_Find_Min_Max_Voltage = RESET;
			externalInterruptCounter++;
		} else {
			if(externalInterruptCounter < 3){
				adcState = WAIT_FOR_DATA_COMPLETE_TRANSMIT;
				ADC_Start_Getting_Values();
			} else if(externalInterruptCounter == 3) {
				adcState = COMPUTE_PEAK_TO_PEAK_VOLTAGE;
			}
		}
		break;

	case WAIT_FOR_DATA_COMPLETE_TRANSMIT:
		if(AdcDmaStoreFlag){
			ADC_Stop_Getting_Values();
			AdcDmaStoreFlag = 0;
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex++) {
				AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] = AdcDmaBuffer[channelIndex] - AdcDmaBuffer[REFERENCE_1V8_VOLTAGE_INDEX];
				if(AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] < 10 && AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] > -10){
					AdcBuffer[channelIndex][AdcDmaBufferIndexFilter] = 0;
				}

//					if(AdcDmaBufferIndexFilter == 0){
//						AdcBufferPeakMax[channelIndex] = AdcBuffer[channelIndex][AdcDmaBufferIndexFilter];
//						AdcBufferPeakMin[channelIndex] = AdcBuffer[channelIndex][AdcDmaBufferIndexFilter];
//					} else {
//						if(AdcBufferPeakMax[channelIndex] < AdcBuffer[channelIndex][AdcDmaBufferIndexFilter]){
//							AdcBufferPeakMax[channelIndex] = AdcBuffer[channelIndex][AdcDmaBufferIndexFilter];
//						}
//						if(AdcBufferPeakMin[channelIndex] > AdcBuffer[channelIndex][AdcDmaBufferIndexFilter]){
//							AdcBufferPeakMin[channelIndex] = AdcBuffer[channelIndex][AdcDmaBufferIndexFilter];
//						}
//					}
			}
			AdcDmaBufferIndexFilter++;
			if(AdcDmaBufferIndexFilter % NUMBER_OF_SAMPLES_PER_SECOND == 0){
				AdcDmaBufferIndexFilter = 0;
			}

			adcState = START_GETTING_ADC;
		}
		break;
	case COMPUTE_PEAK_TO_PEAK_VOLTAGE:

		for(uint8_t sampleIndex = 0; sampleIndex < AdcDmaBufferIndexFilter; sampleIndex ++){
			for(uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex ++){
				if(sampleIndex > NUMBER_OF_SAMPLES_FOR_SMA){
					AdcBufferSUM[channelIndex] += AdcBuffer[channelIndex][sampleIndex] - AdcBuffer[channelIndex][sampleIndex - NUMBER_OF_SAMPLES_FOR_SMA];
					AdcBufferSMA[channelIndex] = AdcBufferSUM[channelIndex]/NUMBER_OF_SAMPLES_FOR_SMA;
				} else {
					AdcBufferSUM[channelIndex] += AdcBuffer[channelIndex][sampleIndex];
					AdcBufferSMA[channelIndex] = AdcBufferSUM[channelIndex]/NUMBER_OF_SAMPLES_FOR_SMA;
				}

				if(sampleIndex == 0){
					AdcBufferPeakMax[channelIndex] = AdcBufferSMA[channelIndex];
					AdcBufferPeakMin[channelIndex] = AdcBufferSMA[channelIndex];
				} else {
					if(AdcBufferPeakMax[channelIndex] < AdcBufferSMA[channelIndex]){
						AdcBufferPeakMax[channelIndex] = AdcBufferSMA[channelIndex];
					}
					if(AdcBufferPeakMin[channelIndex] > AdcBufferSMA[channelIndex]){
						AdcBufferPeakMin[channelIndex] = AdcBufferSMA[channelIndex];
					}
				}
				int32_t tempRealADCValue = AdcBufferSMA[channelIndex];
				array_Of_Vrms_ADC_Values[channelIndex] += tempRealADCValue * tempRealADCValue;

				if(sampleIndex == AdcDmaBufferIndexFilter - 1){
					int32_t tempPeakPeak = AdcBufferPeakMax[channelIndex] - AdcBufferPeakMin[channelIndex];

					if(AdcBufferPeakMax[channelIndex] <= 10 || AdcBufferPeakMin[channelIndex] >= -10){
						tempPeakPeak = 0;
					} else if(tempPeakPeak < 60){
						tempPeakPeak = 0;
					}

					if(cycleCounter == 0){
						AdcBufferPeakPeak[channelIndex] = tempPeakPeak;
					} else {
						AdcBufferPeakPeak[channelIndex] = AdcBufferPeakPeak[channelIndex] + tempPeakPeak;
					}




					array_Of_Vrms_ADC_Values[channelIndex] = (array_Of_Vrms_ADC_Values[channelIndex])/AdcDmaBufferIndexFilter;
					if(cycleCounter == 0){
						array_Of_Average_Vrms_ADC_Values[channelIndex] = sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
					} else {
						array_Of_Average_Vrms_ADC_Values[channelIndex] = array_Of_Average_Vrms_ADC_Values[channelIndex] + sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
					}
				}

			}
		}

		cycleCounter++;
		if(cycleCounter == NUMBER_OF_SAMPLES_PER_AVERAGE){
			cycleCounter = 0;
			for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++){
				AdcBufferPeakPeak[i] = AdcBufferPeakPeak[i] >> SAMPLE_STEPS;
				array_Of_Average_Vrms_ADC_Values[i] = array_Of_Average_Vrms_ADC_Values[i] >> SAMPLE_STEPS;
				PowerFactor[i] = (array_Of_Average_Vrms_ADC_Values[i]*1000 * 100 * 2) / (AdcBufferPeakPeak[i] * 707);
			}
			sprintf((char*) strtmp, "%d\t", (int) PowerFactor[0]);
			UART3_SendToHost((uint8_t *)strtmp);
			sprintf((char*) strtmp, "%d\t", (int) array_Of_Average_Vrms_ADC_Values[0] * 239);
			UART3_SendToHost((uint8_t *)strtmp);
			sprintf((char*) strtmp, "%d\t", (int) AdcBufferPeakPeak[0]);
			UART3_SendToHost((uint8_t *)strtmp);
			UART3_SendToHost((uint8_t *)"\r\n");
			sprintf((char*) strtmp, "%d\t", (int) PowerFactor[8]);
			UART3_SendToHost((uint8_t *)strtmp);
			sprintf((char*) strtmp, "%d\t", (int) array_Of_Average_Vrms_ADC_Values[8] * 493);
			UART3_SendToHost((uint8_t *)strtmp);
			sprintf((char*) strtmp, "%d\t", (int) AdcBufferPeakPeak[8]);
			UART3_SendToHost((uint8_t *)strtmp);
			UART3_SendToHost((uint8_t *)"\r\n");

			UART3_SendToHost((uint8_t *)"\r\n");
			adcState = REPORT_POWER_DATA;
			test2();
		} else {
			externalInterruptCounter = 0;
			adcState = FIND_ZERO_VOLTAGE_POINT;
		}
		break;

	case REPORT_POWER_DATA:
		if(is_Adc_Reading_Timeout()){
			test2();
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex++) {
				array_Of_Vrms_ADC_Values[channelIndex]  = 0;
				array_Of_Average_Vrms_ADC_Values[channelIndex] = 0;
				AdcBufferPeakMax[channelIndex] = 0;
				AdcBufferPeakMin[channelIndex] = 0;
				AdcBufferSUM[channelIndex] = 0;
			}
			adcState = SETUP_TIMER_ONE_SECOND;
		}
		break;

	default:
		break;
	}
}

*/



void Adc_State_Display(void){
	if(pre_adcState != adcState){
		pre_adcState = adcState;
		switch(adcState){
//		case ADC_SETUP_TIMER_ONE_SECOND:
//			DEBUG_ADC(UART3_SendToHost((uint8_t*)"ADC_SETUP_TIMER_ONE_SECOND"););
//
//			break;
//		case ADC_FIND_ZERO_VOLTAGE_POINT:
//			DEBUG_ADC(UART3_SendToHost((uint8_t*)"ADC_FIND_ZERO_VOLTAGE_POINT"););
//			break;
//		case ADC_START_GETTING:
//			DEBUG_ADC(UART3_SendToHost((uint8_t*)"ADC_START_GETTING\r"););
//			break;
//		case ADC_WAIT_FOR_DATA_COMPLETE_TRANSMIT:
//			DEBUG_ADC(UART3_SendToHost((uint8_t*)"ADC_WAIT_FOR_DATA_COMPLETE_TRANSMIT\r"););
//
//			break;
//		case ADC_COMPUTE_PEAK_TO_PEAK_VOLTAGE:
//			DEBUG_ADC(UART3_SendToHost((uint8_t*)"ADC_COMPUTE_PEAK_TO_PEAK_VOLTAGE\r"););
//			break;

		case ADC_REPORT_POWER_DATA:
			DEBUG_ADC(UART3_SendToHost((uint8_t*)"ADC_REPORT_POWER_DATA\r"););
			break;

		case MAX_NUMBER_OF_ADC_STATES:
			DEBUG_ADC(UART3_SendToHost((uint8_t*)"MAX_NUMBER_OF_ADC_STATES\r"););
			break;
		default:
			break;
		}
	}

}

