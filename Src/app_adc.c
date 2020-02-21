 /*
 * app_adc.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */

#include "main.h"

#include "app_scheduler.h"
#include "app_uart.h"
#include "app_fsm.h"


#define NUM_OF_SAMPLE	50
#define NUM_OF_FILTER	100



ADC_HandleTypeDef ADC1Handle;
DMA_HandleTypeDef Hdma_adc1Handle;


//uint32_t ADCValues[NUMBER_OF_ADC_CHANNELS];

uint16_t AdcDmaBuffer[NUMBER_OF_ADC_CHANNELS];
uint16_t AdcBuffer[NUM_OF_SAMPLE][NUMBER_OF_ADC_CHANNELS];
uint32_t AdcBufferFilter[NUM_OF_FILTER][NUMBER_OF_ADC_CHANNELS];
uint16_t AdcBufferPeakMax[NUMBER_OF_ADC_CHANNELS],
		AdcBufferPeakMin[NUMBER_OF_ADC_CHANNELS],
		AdcBufferPeakPeak[NUMBER_OF_ADC_CHANNELS];
uint16_t AdcDmaBufferIndex = 0, AdcDmaBufferIndexFilter = 0;
uint16_t AdcBufferCurrent[NUMBER_OF_ADC_CHANNELS];
uint8_t strtmp[] = "Begin read ADcs \r\n";
//uint32_t array_Of_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//uint32_t arrayOfAverageADCValues[NUMBER_OF_ADC_CHANNELS][NUMBER_OF_SAMPLES_PER_SECOND];
//
//uint32_t array_Of_Max_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//uint32_t array_Of_Min_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//
//uint32_t array_Of_Vpp_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//uint32_t array_Of_Vrms_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//
//uint32_t array_Of_Irms_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//uint32_t array_Of_Power_Consumption[NUMBER_OF_ADC_CHANNELS];
//uint32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_ADC_CHANNELS];

uint8_t AdcDmaFlag = 0, AdcDmaStoreFlag = 0;
#define ADC_READING_TIME_OUT	95

typedef enum {
	START_GETTING_ADC = 0,
	WAIT_FOR_DATA_COMPLETE_TRANSMIT,
	STOP_GETTING_ADC,
	FIND_MIN_MAX,
	COMPUTE_PEAK_TO_PEAK_VOLTAGE,
	COMPUTE_POWER_CONSUMPTION,
	MAX_NUMBER_OF_ADC_STATE
}ADC_STATE;

uint8_t adc_TimeoutFlag = 0;
uint8_t adc_Timeout_Task_Index = 0;
ADC_STATE adcState = START_GETTING_ADC;

void ClearAllBuffers(void);

void ADC_Setup_Min_Max_Value_Buffers(void);
void ADC_Get_Voltage_PeakToPeak(void);
void ADC_Get_Voltage_MinMax(void);

void ClearAllBuffers(void){


}




/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  AdcHandle : AdcHandle handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
{
	AdcDmaStoreFlag = 1;
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
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
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
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */
  ClearAllBuffers();
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


void ADC_Setup_Min_Max_Value_Buffers(void){
	uint8_t i;
 	for (i = 0; i < NUMBER_OF_ADC_CHANNELS; i ++){
 		AdcBufferPeakMax[i] = 0;
 		AdcBufferPeakMin[i] = 4096;
	}
}

//void AverageADCValueCalculation(void){
//	uint8_t i, j;
//	uint32_t tempAverage;
//	static uint8_t circularIndexOfArrayOfAverageADCValues = 0;
//	if(AdcDmaStoreFlag == 1){
//		ADC_Stop_Getting_Values();
//		circularIndexOfArrayOfAverageADCValues = (circularIndexOfArrayOfAverageADCValues + 1) % NUMBER_OF_SAMPLES_PER_SECOND;
//		for(i = 0; i < NUMBER_OF_ADC_CHANNELS; i ++){
//			tempAverage = 0;
//			for(j = 0; j < NUMBER_OF_SAMPLES_PER_AVERAGE; j++){
//				tempAverage = tempAverage + array_Of_ADC_Values[i + j*NUMBER_OF_ADC_CHANNELS];
//			}
//			arrayOfAverageADCValues[i][circularIndexOfArrayOfAverageADCValues] = tempAverage >> SAMPLE_STEPS;
//		}
//
//		ADC_Start_Getting_Values();
//		adcCompleteTransfer = 0;
//
//		UpdatePublishMessage(0, arrayOfAverageADCValues[i][circularIndexOfArrayOfAverageADCValues]);
//		UART3_SendToHost((uint8_t*)"\r");
//		HAL_Delay(1);
//	}
//}
//
//
//void ADC_Get_Voltage_MinMax(void){
//
//	uint8_t adcChannelIndex = 0;
//	uint8_t j = 0;
//	ADC_Setup_Min_Max_Value_Buffers();
//	for(adcChannelIndex = 0; adcChannelIndex < NUMBER_OF_ADC_CHANNELS; adcChannelIndex ++){
//		for(j = 0; j < NUMBER_OF_SAMPLES_PER_SECOND; j ++){
//			if(arrayOfAverageADCValues[adcChannelIndex][j] > array_Of_Max_ADC_Values[adcChannelIndex]){
//				array_Of_Max_ADC_Values[adcChannelIndex] = arrayOfAverageADCValues[adcChannelIndex][j];
//			}
//			if(arrayOfAverageADCValues[adcChannelIndex][j] < array_Of_Min_ADC_Values[adcChannelIndex]){
//				array_Of_Min_ADC_Values[adcChannelIndex] = arrayOfAverageADCValues[adcChannelIndex][j];
//			}
//		}
//	}
//
//}
//
//void ADC_Get_Voltage_PeakToPeak(void){
//	uint8_t adcChannelIndex = 0;
////	ADC_Stop_Getting_Values();
//	for(adcChannelIndex = 0; adcChannelIndex < NUMBER_OF_ADC_CHANNELS; adcChannelIndex ++){
//		array_Of_Vpp_ADC_Values[adcChannelIndex] =  array_Of_Max_ADC_Values[adcChannelIndex] -	array_Of_Min_ADC_Values[adcChannelIndex];
//	}
//}
//
//void ADC_ComputePowerConsumption(void){
//	uint8_t adcChannelIndex = 0;
//	for(adcChannelIndex = 0; adcChannelIndex < NUMBER_OF_ADC_CHANNELS; adcChannelIndex ++){
////		array_Of_Vrms_ADC_Values[adcChannelIndex] = array_Of_Vpp_ADC_Values[adcChannelIndex]*707/2;
////		array_Of_Vrms_ADC_Values[adcChannelIndex] = array_Of_Vpp_ADC_Values[adcChannelIndex] - CALIBRATION/10000;
////		array_Of_Irms_ADC_Values[adcChannelIndex] = array_Of_Vrms_ADC_Values[adcChannelIndex]*1000/SENSITIVITY;
//
//		array_Of_Irms_ADC_Values[adcChannelIndex] = array_Of_Vpp_ADC_Values[adcChannelIndex]*237;
////		if(array_Of_Irms_ADC_Values[adcChannelIndex] > 100 && array_Of_Irms_ADC_Values[adcChannelIndex] < 1000){
////			array_Of_Irms_ADC_Values[adcChannelIndex] = 0;
////		}
//		array_Of_Power_Consumption[adcChannelIndex] = SUPPLY_VOLTAGE * array_Of_Irms_ADC_Values[adcChannelIndex] /100;
//		array_Of_Power_Consumption_In_WattHour[adcChannelIndex] += array_Of_Power_Consumption[adcChannelIndex];
//	}
//
//
//}



void Adc_Clear_Timeout_Flag(void){
	adc_TimeoutFlag = 0;
}
void Adc_Reading_Timeout(void){
	adc_TimeoutFlag = 1;
}
uint8_t is_Adc_Reading_Timeout(void){
	return adc_TimeoutFlag;
}
void PowerConsumption_FSM(void){
	switch(adcState){
	case START_GETTING_ADC:
		ADC_Setup_Min_Max_Value_Buffers();
		ADC_Start_Getting_Values();
		adcState = WAIT_FOR_DATA_COMPLETE_TRANSMIT;
		break;
	case WAIT_FOR_DATA_COMPLETE_TRANSMIT:
		if(AdcDmaStoreFlag){
			AdcDmaStoreFlag = 0;
			for (uint8_t i = 0; i < NUMBER_OF_ADC_CHANNELS; i++) {
				AdcBufferFilter[AdcDmaBufferIndexFilter][i] = AdcDmaBuffer[i];
			}
			AdcDmaBufferIndexFilter++;
			if (AdcDmaBufferIndexFilter >= NUM_OF_FILTER) {
				AdcDmaBufferIndexFilter = 0;
				uint32_t sumFilter = 0;
				for(uint16_t itmp = 0; itmp< NUM_OF_FILTER; itmp ++){
					sumFilter += AdcBufferFilter[itmp][0] ;
				}
				AdcBuffer[AdcDmaBufferIndex][0] = sumFilter/NUM_OF_FILTER;
//					HAL_UART_Transmit(&huart3, (uint8_t*) strtmp, sprintf(strtmp, "%d\t",AdcBuffer[AdcDmaBufferIndex][0]), 500);
//					HAL_UART_Transmit(&huart3, (uint8_t*) strtmp, sprintf(strtmp, "%d\r\n", AdcBufferPeakPeak[0] + 2170), 500);
				AdcDmaBufferIndex++;
				if (AdcDmaBufferIndex >= NUM_OF_SAMPLE) {
					AdcDmaBufferIndex = 0;
					AdcDmaFlag = 1;
					ADC_Stop_Getting_Values();
					AdcDmaFlag = 0;
					//					mode = FSM_MODE_TRANSMIT_UART;
					adcState = COMPUTE_PEAK_TO_PEAK_VOLTAGE;
				} else {
					//					HAL_ADC_Start_DMA(&hadc1, AdcDmaBuffer, NUM_OF_ADC_CHANNEL);
				}

			}

		}
		break;
	case COMPUTE_PEAK_TO_PEAK_VOLTAGE:
		for (uint8_t i = 0; i < NUMBER_OF_ADC_CHANNELS; i++) {
			for (uint8_t j = 0; j < NUMBER_OF_ADC_CHANNELS; j++) {
				if (AdcBufferPeakMax[i] < AdcBuffer[j][i])
					AdcBufferPeakMax[i] = AdcBuffer[j][i];
				if (AdcBufferPeakMin[i] > AdcBuffer[j][i])
					AdcBufferPeakMin[i] = AdcBuffer[j][i];
			}
			AdcBufferPeakPeak[i] = (8* AdcBufferPeakPeak[i] + 2 * (AdcBufferPeakMax[i]- AdcBufferPeakMin[i]))/10;
		}
//			HAL_UART_Transmit(&huart3, "\t\t\t", 3, 100);
//			I = adcPeak/2  / 4096 * 3v3 /680  * 2000   * 1000 = adc * 2.3696 / 2
//			I = 3.3 *
		// P = 220 * adc * 2.3696 / 1.4
		snprintf((char *)strtmp, sizeof(strtmp), "\r\n");
		UART3_SendToHost(strtmp);
//			I = AdcBufferPeakPeak[0] * 2.3696;
//			P = AdcBufferPeakPeak[0] * 521.312;
		uint32_t current = AdcBufferPeakPeak[0] * 1184;//1185;	//2370;
		uint32_t power = AdcBufferPeakPeak[0]  *185;//186;//260;	//521312;
		if(current < 2370) current = 0 ;
		else current = current / 1000;
		sprintf((char *)strtmp, "I: %ld\t", current);
		UART3_SendToHost(strtmp);
		sprintf((char *)strtmp, "P: %ld\t", power);
		UART3_SendToHost(strtmp);
//		HAL_UART_Transmit(&huart3, (uint8_t*)strtmp, sprintf(strtmp, "I: %d\t", current), 500);
//		HAL_UART_Transmit(&huart3, (uint8_t*)strtmp, sprintf(strtmp, "P: %d\t", power), 500);

		//transmit or do something here
//			mode = FSM_MODE_BEGIN;
		adcState = START_GETTING_ADC;
		break;

	default:
		break;
	}
}





