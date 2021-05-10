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
#define 	NUMBER_OF_SAMPLES_FOR_SMA					12
#define		MAIN_ADC_INDEX								12
#define		REFERENCE_1V8_VOLTAGE_INDEX					12
#define		DIFFERENCE_ADC_VALUE_THRESHOLD				10

//#define		AVCC_OPAMP								5 //5 Volt
#define AVCC_OPAMP								3 //3.3 Volt

#if (AVCC_OPAMP == 3)
#define		COEFFICIENT_0								1100
#define		COEFFICIENT_01								950
#define		COEFFICIENT_1								750
#define		COEFFICIENT_2								390
#define		COEFFICIENT_3								252

#else

#define		COEFFICIENT_1								540
#define		COEFFICIENT_2								390
#define		COEFFICIENT_3								252

#endif
ADC_HandleTypeDef ADC1Handle;
DMA_HandleTypeDef Hdma_adc1Handle;

int32_t AdcDmaBuffer[NUMBER_OF_ADC_CHANNELS];

int32_t PowerFactor[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION];

int32_t AdcBuffer[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION][NUMBER_OF_SAMPLES_PER_SECOND];
int32_t AdcBufferPeakPeak[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION][NUMBER_OF_SAMPLES_FOR_SMA];
int32_t AdcBufferPeakMax[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION],
		AdcBufferPeakMin[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION],
		AdcBufferAveragePeakPeak[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION];
int32_t AdcDmaBufferIndex = 0, AdcDmaBufferIndexFilter = 0;

uint8_t strtmp[] = "Begin read ADcs \r\n";
uint32_t array_Of_Vrms_ADC_Values[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION];
uint32_t array_Of_Average_Vrms_ADC_Values[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION];
//
uint32_t array_Of_Average_Irms_ADC_Values[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION];
uint32_t array_Of_Irms_ADC_Values[NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION][NUMBER_OF_SAMPLES_FOR_SMA];



/* Variable to report ADC analog watchdog status:   */
/*   RESET <=> voltage into AWD window   */
/*   SET   <=> voltage out of AWD window */
FlagStatus         ubAnalogWatchdogStatus = RESET;  /* Set into analog watchdog interrupt callback */
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
uint8_t adc_Timeout_Task_Index = 0;
FlagStatus is_Ready_To_Find_Min_Max_Voltage = RESET;
ADC_STATE adcState = ADC_SETUP_TIMER_ONE_SECOND;
ADC_STATE pre_adcState = MAX_NUMBER_OF_ADC_STATES;



void Adc_State_Display(void);
void Adc_Buffers_Init(void);
void WatchDogAnalogInit(void);

void Adc_Buffers_Init(void){
	uint16_t channelIndex, sampleIndex;
	for(channelIndex = 0; channelIndex < NUMBER_OF_ADC_CHANNELS; channelIndex++){
		AdcDmaBuffer[channelIndex] = 0;
	}

	for(channelIndex = 0; channelIndex < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; channelIndex++){
		PowerFactor[channelIndex] = 0;
		AdcBufferPeakMax[channelIndex] = 0;
		AdcBufferPeakMin[channelIndex] = 0;
		AdcBufferAveragePeakPeak[channelIndex] = 0;
		array_Of_Vrms_ADC_Values[channelIndex] = 0;
		array_Of_Average_Vrms_ADC_Values[channelIndex] = 0;
		array_Of_Average_Irms_ADC_Values[channelIndex] = 0;
	}
	for(channelIndex = 0; channelIndex < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; channelIndex++){
		for(sampleIndex = 0; sampleIndex < NUMBER_OF_SAMPLES_PER_SECOND; sampleIndex ++)
			AdcBuffer[channelIndex][sampleIndex] = 0;
	}
	for(channelIndex = 0; channelIndex < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; channelIndex++){
		for(sampleIndex = 0; sampleIndex < NUMBER_OF_SAMPLES_FOR_SMA; sampleIndex ++)
			AdcBufferPeakPeak[channelIndex][sampleIndex] = 0;
		array_Of_Irms_ADC_Values[channelIndex][sampleIndex] = 0;
	}
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
  sConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;
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

 // WatchDogAnalogInit();

  Adc_Buffers_Init();
}


void WatchDogAnalogInit(void){


	  ADC_AnalogWDGConfTypeDef AnalogWDGConfig;

	/* Analog watchdog 1 configuration */
	  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;
	  AnalogWDGConfig.ITMode = ENABLE;
	  AnalogWDGConfig.HighThreshold = 4000;
	  AnalogWDGConfig.LowThreshold = 100;

	  AnalogWDGConfig.Channel = ADC_CHANNEL_0;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
	    /* Channel Configuration Error */
	    Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_1;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_2;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_3;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_4;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_5;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_6;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_7;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_8;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }

	  AnalogWDGConfig.Channel = ADC_CHANNEL_9;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }

//	  AnalogWDGConfig.Channel = ADC_CHANNEL_10;
//	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
//	  {
//		/* Channel Configuration Error */
//		Error_Handler();
//	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_11;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }
	  AnalogWDGConfig.Channel = ADC_CHANNEL_12;
	  if (HAL_ADC_AnalogWDGConfig(&ADC1Handle, &AnalogWDGConfig) != HAL_OK)
	  {
		/* Channel Configuration Error */
		Error_Handler();
	  }

	  /* NVIC configuration for ADC interrupt */
	/* Priority: high-priority */
	HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
}


/**
  * @brief  Analog watchdog callback in non blocking mode.
  * @param  hadc: ADC handle
  * @retval None
  */
  void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
  /* Set variable to report analog watchdog out of window status to main      */
  /* program.                                                                 */
  ubAnalogWatchdogStatus = SET;
  UART3_SendToHost((uint8_t *)"Level out\r\n");
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
	static uint8_t indexForAverageCurrent = 0;
//	Adc_State_Display();
	switch(adcState){
	case ADC_SETUP_TIMER_ONE_SECOND:
//		SCH_Delete_Task(adc_Timeout_Task_Index);
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
		if(is_Adc_Reading_Timeout()){
			adcState = ADC_REPORT_POWER_DATA;
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
		if(is_Adc_Reading_Timeout()){
			adcState = ADC_REPORT_POWER_DATA;
		}
		break;

	case ADC_WAIT_FOR_DATA_COMPLETE_TRANSMIT:
		if(AdcDmaStoreFlag == SET){
			ADC_Stop_Getting_Values();
			AdcDmaStoreFlag = RESET;
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; channelIndex++) {
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
		if(is_Adc_Reading_Timeout()){
			adcState = ADC_REPORT_POWER_DATA;
		}
		break;
	case ADC_COMPUTE_PEAK_TO_PEAK_VOLTAGE:
		for(uint8_t channelIndex = 0; channelIndex < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; channelIndex ++){
			for(uint16_t sampleIndex = 0; sampleIndex < AdcDmaBufferIndexFilter; sampleIndex ++){

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
//					if(cycleCounter == 0){
//						AdcBufferPeakPeak[channelIndex] = tempPeakPeak;
//					} else {
//						AdcBufferPeakPeak[channelIndex] = AdcBufferPeakPeak[channelIndex] + tempPeakPeak;
//					}
					AdcBufferPeakPeak[channelIndex][indexForAverageCurrent] = tempPeakPeak;
					AdcBufferAveragePeakPeak[channelIndex] += AdcBufferPeakPeak[channelIndex][indexForAverageCurrent]
												- AdcBufferPeakPeak[channelIndex][(indexForAverageCurrent + 1 + NUMBER_OF_SAMPLES_FOR_SMA)%NUMBER_OF_SAMPLES_FOR_SMA];


					array_Of_Vrms_ADC_Values[channelIndex] = (array_Of_Vrms_ADC_Values[channelIndex])/AdcDmaBufferIndexFilter;
					uint32_t tempVrms = sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
					if(cycleCounter == 0){
						array_Of_Average_Vrms_ADC_Values[channelIndex] = tempVrms;
					} else {
						array_Of_Average_Vrms_ADC_Values[channelIndex] = array_Of_Average_Vrms_ADC_Values[channelIndex] + tempVrms;
					}
					array_Of_Irms_ADC_Values[channelIndex][indexForAverageCurrent] = tempVrms;
					array_Of_Average_Irms_ADC_Values[channelIndex] += array_Of_Irms_ADC_Values[channelIndex][indexForAverageCurrent]
														- array_Of_Irms_ADC_Values[channelIndex][(indexForAverageCurrent + 1 + NUMBER_OF_SAMPLES_FOR_SMA)%NUMBER_OF_SAMPLES_FOR_SMA];

				}

			}
		}
		indexForAverageCurrent = (indexForAverageCurrent + 1) % NUMBER_OF_SAMPLES_FOR_SMA;
		cycleCounter++;
		if(cycleCounter >= NUMBER_OF_SAMPLES_PER_AVERAGE){
			cycleCounter = 0;
			for (uint8_t i = 0; i < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; i++){
//				AdcBufferPeakPeak[i] = AdcBufferPeakPeak[i] >> SAMPLE_STEPS;
				array_Of_Average_Vrms_ADC_Values[i] = array_Of_Average_Vrms_ADC_Values[i] >> SAMPLE_STEPS;
				if(array_Of_Average_Vrms_ADC_Values[i] < 5 && i != MAIN_INPUT){
					array_Of_Average_Vrms_ADC_Values[i] = 0;
				}
//				PowerFactor[i] = (array_Of_Average_Vrms_ADC_Values[i]*1000 * 100 * 2) / (AdcBufferPeakPeak[i] * 707);
				PowerFactor[i] = (array_Of_Average_Vrms_ADC_Values[i] * 283) / (AdcBufferAveragePeakPeak[i]/NUMBER_OF_SAMPLES_FOR_SMA);
				if(PowerFactor[i] >= 98){
					PowerFactor[i] = 100;
				}

//				if(i < 10 && i >= 0){
//					sprintf((char*) strtmp, "%d: %d\t", (int) i, (int) PowerFactor[i]);
//					UART3_SendToHost((uint8_t *)strtmp);
//					sprintf((char*) strtmp, "%d\t", (int) array_Of_Average_Irms_ADC_Values[i]/NUMBER_OF_SAMPLES_FOR_SMA);
//					UART3_SendToHost((uint8_t *)strtmp);
//					sprintf((char*) strtmp, "%d\t", (int) array_Of_Average_Irms_ADC_Values[i]/NUMBER_OF_SAMPLES_FOR_SMA);
//					UART3_SendToHost((uint8_t *)strtmp);
//					sprintf((char*) strtmp, "%d\r\n", (int) AdcBufferAveragePeakPeak[i]/NUMBER_OF_SAMPLES_FOR_SMA);
//					UART3_SendToHost((uint8_t *)strtmp);
//				}


#if(VERSION_EBOX == 2)
				uint32_t tempIrmsADCValue = AdcBufferAveragePeakPeak[i];

				if(tempIrmsADCValue > 1250 && (i == 0 || i == 1)){
					Node_Update(i, (array_Of_Average_Irms_ADC_Values[i]* COEFFICIENT_0)/NUMBER_OF_SAMPLES_FOR_SMA , 230, PowerFactor[i], 1);
				}
				else if(tempIrmsADCValue > 1100 && (i == 0 || i == 1)){
					Node_Update(i, (array_Of_Average_Irms_ADC_Values[i]* COEFFICIENT_01)/NUMBER_OF_SAMPLES_FOR_SMA , 230, PowerFactor[i], 1);
				}
				else if(tempIrmsADCValue > 1000){
					Node_Update(i, (array_Of_Average_Irms_ADC_Values[i]* COEFFICIENT_1)/NUMBER_OF_SAMPLES_FOR_SMA , 230, PowerFactor[i], 1);
				} else if (tempIrmsADCValue > 800){
					Node_Update(i, (array_Of_Average_Irms_ADC_Values[i]* COEFFICIENT_2)/NUMBER_OF_SAMPLES_FOR_SMA , 230, PowerFactor[i], 1);
				} else {
					Node_Update(i, (array_Of_Average_Irms_ADC_Values[i]* COEFFICIENT_3)/NUMBER_OF_SAMPLES_FOR_SMA , 230, PowerFactor[i], 1);
				}
#else
				uint32_t tempIrmsADCValue = AdcBufferAveragePeakPeak[i];
				if(tempIrmsADCValue > 1000){
					Node_Update(i, (array_Of_Average_Irms_ADC_Values[i]* COEFFICIENT_1)/NUMBER_OF_SAMPLES_FOR_SMA , 230, PowerFactor[i], 1);
				} else if (tempIrmsADCValue > 800){
					Node_Update(i, (array_Of_Average_Irms_ADC_Values[i]* COEFFICIENT_2)/NUMBER_OF_SAMPLES_FOR_SMA , 230, PowerFactor[i], 1);
				} else {
					Node_Update(i, (array_Of_Average_Irms_ADC_Values[i]* COEFFICIENT_3)/NUMBER_OF_SAMPLES_FOR_SMA , 230, PowerFactor[i], 1);
				}
#endif
			}


			adcState = ADC_REPORT_POWER_DATA;
			HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, RESET);
		} else {
			externalInterruptCounter = 0;
			adcState = ADC_FIND_ZERO_VOLTAGE_POINT;
		}

		if(is_Adc_Reading_Timeout()){
			adcState = ADC_REPORT_POWER_DATA;
		}
		break;

	case ADC_REPORT_POWER_DATA:
		if(is_Adc_Reading_Timeout()){
			HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, SET);
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_ADC_CHANNELS_FOR_POWER_CALCULATION; channelIndex++) {
				array_Of_Vrms_ADC_Values[channelIndex]  = 0;
				array_Of_Average_Vrms_ADC_Values[channelIndex] = 0;
				AdcBufferPeakMax[channelIndex] = 0;
				AdcBufferPeakMin[channelIndex] = 0;
			}
			adcState = ADC_SETUP_TIMER_ONE_SECOND;
		}
		break;

	default:
		adcState = ADC_REPORT_POWER_DATA;
		break;
	}
}



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

