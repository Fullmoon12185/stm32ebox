 /*
 * app_adc.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */

#include "main.h"
#include "app_adc.h"
#include "app_scheduler.h"
#include "app_uart.h"
#include "app_fsm.h"
#include "app_test.h"
#include "math.h"


#define		REFERENCE_1V8_VOLTAGE_INDEX		12
#define		DIFFERENCE_ADC_VALUE_THRESHOLD				10

ADC_HandleTypeDef ADC1Handle;
DMA_HandleTypeDef Hdma_adc1Handle;


//uint32_t ADCValues[NUMBER_OF_ADC_CHANNELS];

int32_t AdcDmaBuffer[NUMBER_OF_ADC_CHANNELS];

int32_t PowerFactor[NUMBER_OF_RELAYS];

int32_t AdcBuffer[NUMBER_OF_RELAYS][NUMBER_OF_SAMPLES_PER_SECOND];
//int32_t AdcBufferFilter[NUM_OF_FILTER][NUMBER_OF_ADC_CHANNELS];
int32_t AdcBufferPeakMax[NUMBER_OF_RELAYS],
		AdcBufferPeakMin[NUMBER_OF_RELAYS],
		AdcBufferPeakPeak[NUMBER_OF_RELAYS];
int32_t AdcDmaBufferIndex = 0, AdcDmaBufferIndexFilter = 0;
int32_t AdcBufferCurrent[NUMBER_OF_RELAYS];

uint8_t strtmp[] = "Begin read ADcs \r\n                                                                      ";
//uint32_t array_Of_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//uint32_t arrayOfAverageADCValues[NUMBER_OF_ADC_CHANNELS][NUMBER_OF_SAMPLES_PER_SECOND];
//
//uint32_t array_Of_Max_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//uint32_t array_Of_Min_ADC_Values[NUMBER_OF_ADC_CHANNELS];
//
//uint32_t array_Of_Vpp_ADC_Values[NUMBER_OF_ADC_CHANNELS];
uint32_t array_Of_Vrms_ADC_Values[NUMBER_OF_RELAYS];
uint32_t array_Of_Average_Vrms_ADC_Values[NUMBER_OF_RELAYS];
//
uint32_t array_Of_Irms_ADC_Values[NUMBER_OF_RELAYS];
uint32_t array_Of_Power_Consumption[NUMBER_OF_RELAYS];
uint32_t array_Of_Power_Consumption_In_WattHour[NUMBER_OF_RELAYS];
//FlagStatus array_Of_Outlet_Status[NUMBER_OF_ADC_CHANNELS];



uint8_t AdcDmaFlag = 0, AdcDmaStoreFlag = 0;
#define ADC_READING_TIME_OUT	95


uint8_t adc_TimeoutFlag = 0;
uint8_t adc_Timeout_Task_Index = SCH_MAX_TASKS;
FlagStatus is_Ready_To_Find_Min_Max_Voltage = RESET;
ADC_STATE adcState = SETUP_TIMER_ONE_SECOND;


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
//	is_Ready_To_Find_Min_Max_Voltage = SET;
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

uint8_t get_PowerConsumption_FSM_State(){
	return adcState;
}
void PowerConsumption_FSM(void){
	static uint8_t externalInterruptCounter = 0;
	static uint8_t cycleCounter = 0;
//	static int32_t tempAdcValue[NUMBER_OF_ADC_CHANNELS];
//	static int16_t preTempAdcValue[NUMBER_OF_ADC_CHANNELS];

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
			} else if(externalInterruptCounter == 3) {
				adcState = COMPUTE_PEAK_TO_PEAK_VOLTAGE;
			}
		}
		break;

	case WAIT_FOR_DATA_COMPLETE_TRANSMIT:
		if(AdcDmaStoreFlag){
			ADC_Stop_Getting_Values();
			AdcDmaStoreFlag = 0;
			for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++) {
				//filter noise
//				if(AdcDmaBufferIndexFilter == 0){
					AdcBuffer[i][AdcDmaBufferIndexFilter] = AdcDmaBuffer[i] - AdcDmaBuffer[REFERENCE_1V8_VOLTAGE_INDEX];
					if(AdcBuffer[i][AdcDmaBufferIndexFilter] < 10 && AdcBuffer[i][AdcDmaBufferIndexFilter] > -10){
						AdcBuffer[i][AdcDmaBufferIndexFilter] = 0;
					}

//				} else {
//					if(AdcDmaBuffer[i] > AdcBuffer[i][AdcDmaBufferIndexFilter - 1] + 20 ||
//						AdcDmaBuffer[i] < AdcBuffer[i][AdcDmaBufferIndexFilter - 1] - 20)
//						AdcBuffer[i][AdcDmaBufferIndexFilter] = AdcBuffer[i][AdcDmaBufferIndexFilter - 1];
//					else {
//						AdcBuffer[i][AdcDmaBufferIndexFilter] = AdcDmaBuffer[i];
//					}
//				}
			}

//			UART3_SendToHost((uint8_t *)"0\t");
//			sprintf((char*) strtmp, "%d\t", (int) AdcBuffer[REFERENCE_1V8_VOLTAGE_INDEX][AdcDmaBufferIndexFilter]);
//			UART3_SendToHost((uint8_t *)strtmp);
//			sprintf((char*) strtmp, "%d\r\n", (int) AdcBuffer[0][AdcDmaBufferIndexFilter]);
//			UART3_SendToHost((uint8_t *)strtmp);
			AdcDmaBufferIndexFilter++;
			if(AdcDmaBufferIndexFilter % NUMBER_OF_SAMPLES_PER_SECOND == 0){
				AdcDmaBufferIndexFilter = 0;
			}
			ADC_Start_Getting_Values();
			adcState = START_GETTING_ADC;
		}
		break;
	case COMPUTE_ROOT_MEAN_SQUARE:
		UART3_SendToHost((uint8_t *)"\r\n");
		for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++) {
			array_Of_Vrms_ADC_Values[i] = array_Of_Vrms_ADC_Values[i]*10000/AdcDmaBufferIndexFilter;
			array_Of_Vrms_ADC_Values[i] = sqrt(array_Of_Vrms_ADC_Values[i]);

			sprintf((char*) strtmp, "%d\t", (int) array_Of_Vrms_ADC_Values[i]);
			UART3_SendToHost((uint8_t *)strtmp);
		}

		adcState = COMPUTE_POWER_CONSUMPTION;
		break;
	case COMPUTE_PEAK_TO_PEAK_VOLTAGE:
		for(uint8_t sampleIndex = 0; sampleIndex < AdcDmaBufferIndexFilter; sampleIndex ++){
			for(uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex ++){
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
			}

		}

		for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex++) {
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
			for(uint8_t sampleIndex = 0; sampleIndex < AdcDmaBufferIndexFilter; sampleIndex ++){
				int32_t tempRealADCValue = AdcBuffer[channelIndex][sampleIndex];
//				if(tempRealADCValue < 50 && tempRealADCValue > - 50){
//					tempRealADCValue = 0;
//				}
				array_Of_Vrms_ADC_Values[channelIndex] += tempRealADCValue * tempRealADCValue;
			}
			array_Of_Vrms_ADC_Values[channelIndex] = (array_Of_Vrms_ADC_Values[channelIndex])/AdcDmaBufferIndexFilter;
			if(cycleCounter == 0){
				array_Of_Average_Vrms_ADC_Values[channelIndex] = sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
			} else {
				array_Of_Average_Vrms_ADC_Values[channelIndex] = array_Of_Average_Vrms_ADC_Values[channelIndex] + sqrt(array_Of_Vrms_ADC_Values[channelIndex]);
			}
		}


		cycleCounter++;
		if(cycleCounter == NUMBER_OF_SAMPLES_PER_AVERAGE){
			cycleCounter = 0;
			for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++){
				AdcBufferPeakPeak[i] = AdcBufferPeakPeak[i] >> SAMPLE_STEPS;
				array_Of_Average_Vrms_ADC_Values[i] = array_Of_Average_Vrms_ADC_Values[i] >> SAMPLE_STEPS;
				if(AdcBufferPeakPeak[i] == 0){
					PowerFactor[i] = 0;
				} else {
					PowerFactor[i] = (array_Of_Average_Vrms_ADC_Values[i]*1000 * 100 * 2) / (AdcBufferPeakPeak[i] * 707);
				}

				Main.nodes[i].powerFactor = PowerFactor[i];
				Main.nodes[i].current = array_Of_Average_Vrms_ADC_Values[i] * 237;
				if(Main.nodes[i].current < 1000){
					Main.nodes[i].current = 0;
				}
//				Main.nodes[i].power = Main.nodes[i].current * 220 * Main.nodes[i].powerFactor/100;

			}
//			sprintf((char*) strtmp, "%d\t\t\t\t %d\t\t\t %d\t\t\t %d  \r\n", (int)Main.nodes[0].current/100, (int) array_Of_Average_Vrms_ADC_Values[0],(int) Main.nodes[0].power,(int) Main.nodes[0].energy);
//			UART3_SendToHost((uint8_t *)strtmp);



//			sprintf((char*) strtmp, "i\t\t\t s\t\t\t I\t\t\t PF\t\t\t P\t\t\t E \r\n");
//										UART3_SendToHost((uint8_t *)strtmp);

//										for(uint8_t i =0; i< 1/*NUMBER_OF_RELAYS*/; i++){
//											sprintf((char*) strtmp, "%d\t\t\t %d\t\t\t %d\t\t\t\t %d\t\t\t %d\t\t\t %d  \r\n", (int) i, (int)Main.nodes[i].nodeStatus, (int)Main.nodes[i].current/100, (int) Main.nodes[i].powerFactor,(int) Main.nodes[i].power,(int) Main.nodes[i].energy);
//											UART3_SendToHost((uint8_t *)strtmp);
//
//										}


//			sprintf((char*) strtmp, "%d\t", (int) PowerFactor[0]);
//			UART3_SendToHost((uint8_t *)strtmp);
//			sprintf((char*) strtmp, "%d\t", (int) array_Of_Average_Vrms_ADC_Values[0] * 237);
//			UART3_SendToHost((uint8_t *)strtmp);
//			sprintf((char*) strtmp, "%d\t", (int) AdcBufferPeakPeak[0]);
//			UART3_SendToHost((uint8_t *)strtmp);
//			UART3_SendToHost((uint8_t *)"\r\n");


//			sprintf((char*) strtmp, "i\t I\t P\t PF\r\n");
//			UART3_SendToHost((uint8_t *)strtmp);
//
//			for(uint8_t i =0; i< NUMBER_OF_RELAYS; i++){
//				sprintf((char*) strtmp, "%d\t\t %d\t\t %d\t\t %d \r\n", (int) i, (int)Main.nodes[i].current, (int) Main.nodes[i].power,(int) Main.nodes[i].powerFactor);
//				UART3_SendToHost((uint8_t *)strtmp);
//
//			}


			adcState = COMPUTE_POWER_CONSUMPTION;
		} else {
			externalInterruptCounter = 0;
			adcState = FIND_ZERO_VOLTAGE_POINT;
		}

		break;
	case COMPUTE_POWER_CONSUMPTION:
		for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++) {
			if(AdcBufferPeakPeak[i] < 20){
				array_Of_Irms_ADC_Values[i] = 0;
				array_Of_Power_Consumption[i] = 0;
//				array_Of_Outlet_Status[i] = RESET;
			}
			else{
				//convert adc value to voltage
				// Votage = (PP/2)*3.3V/4096
				// current = Votage/680(om) * 1000 (mA)
				// Current(rms) = current*0.707
				// Real input current = Current(rms) * 2000 (ratio)
//				array_Of_Outlet_Status[i] = SET;
				array_Of_Irms_ADC_Values[i] = AdcBufferPeakPeak[i]*84;// -  AdcBufferPeakPeak[REFERENCE_1V8_VOLTAGE_INDEX];

				array_Of_Power_Consumption[i] = array_Of_Irms_ADC_Values[i];
			}
			array_Of_Power_Consumption_In_WattHour[i] +=  array_Of_Power_Consumption[i]; //wattsecond
		}
//		sprintf((char*) strtmp, "%d\t", (int) array_Of_Irms_ADC_Values[0]);
//		UART3_SendToHost((uint8_t *)strtmp);
		test2();
		adcState = REPORT_POWER_DATA;
		break;
	case REPORT_POWER_DATA:
		if(is_Adc_Reading_Timeout()){
			test2();
			for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_RELAYS; channelIndex++) {
				array_Of_Vrms_ADC_Values[channelIndex]  = 0;
				array_Of_Average_Vrms_ADC_Values[channelIndex] = 0;
				PowerFactor[channelIndex] = 0;
				AdcBufferPeakPeak[channelIndex] = 0;


			}
//			sprintf((char*) strtmp, "%d\t", (int) array_Of_Irms_ADC_Values[0]);
//			UART3_SendToHost((uint8_t *)strtmp);
//			sprintf((char*) strtmp, "%d\r\n", (int) array_Of_Power_Consumption[0]);
//			UART3_SendToHost((uint8_t *)strtmp);
			adcState = SETUP_TIMER_ONE_SECOND;
		}
		break;

	default:
		break;
	}
}





