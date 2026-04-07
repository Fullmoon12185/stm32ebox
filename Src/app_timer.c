/*
 * app_timer.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_scheduler.h"
#include "app_pcf8574.h"
#include "app_uart.h"


TIM_HandleTypeDef    TimHandle;

/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;

typedef struct {
    uint16_t boxID;
    uint16_t offset;
} BoxPeriodConfig_t;

#define DEFAULT_TIMER_OFFSET		450

const BoxPeriodConfig_t boxConfig[] = {
	{CITIHOME_39,  		 			480}, //previous 450
	{TOPAZ_119,  		 			530}, //previous 450
	{TOPAZ_120,  		 			470},
	{KINGDOM_121,  		 			570}, //previous 500
	{BRIDGEVIEW_143, 				500},
	{HAI_THANH_159,					550}, //previous 500
	{PHU_MY_175,					500},
	{EBOOST_OFFICE_177,  			400},
	{CHUONG_DUONG_HOME_197,			430},
	{N4_HOA_KHANH_204,				450},
	{HIM_LAM_RIVER_SIDE_207, 		550},
	{MY_PHUC_1_209, 				500},
	{RIVIERA_POINT_213,				450},
	{THU_THIEM_GREEN_HOUSE_221,		500},
	{N1_HOAKHANH_2_260,				530},
	{PHU_DONG_SKY_GARDEN_282,		500},
	{DIAMOND_LOTUS_RIVERSIDE_285,	500},
	{LOVERA_VISTA_293,				450},
	{THOI_AN_2_300,      			350},
	{MY_PHUC_2_255,      			420}, //previous 450
    {MY_PHUC_3_319,      			400},
    {MY_PHUC_4_320,      			360},
    {TARA_327,           			500},
    {TARA_328,           			570}, // previous 470
	{VO_DINH_338,					350}, //previous 400
    {PHU_MY_THUAN_341,   			570}, //previous 500
    {HOANG_ANH_THANH_BINH_346, 		500}, //previous 350
	{PHUC_YEN_347,					500},
    {HIM_LAM_PHU_DONG_348,   		500}, //previous 350
};

uint16_t GetPeriodOffset(uint16_t boxID);



uint16_t GetPeriodOffset(uint16_t boxID){
    for (int i = 0; i < sizeof(boxConfig)/sizeof(boxConfig[0]); i++){
        if (boxConfig[i].boxID == boxID){
            return boxConfig[i].offset;
        }
    }
    return DEFAULT_TIMER_OFFSET; // default
}

#if(VERSION_EBOX != VERSION_6_WITH_8CT_20A)
void Timer_Init(void){
	/*##-1- Configure the TIM peripheral #######################################*/
	  /* -----------------------------------------------------------------------
	    In this example TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1) x2,
	    since APB1 prescaler is set to 4 (0x100).
	       TIM3CLK = PCLK1*2
	       PCLK1   = HCLK/2
	    => TIM3CLK = PCLK1*2 = (HCLK/2)*2 = HCLK = SystemCoreClock
	    To get TIM3 counter clock at 10 KHz, the Prescaler is computed as following:
	    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	    Prescaler = (SystemCoreClock /10 KHz) - 1

	    Note:
	     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f1xx.c file.
	     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
	     variable value. Otherwise, any configuration based on this variable will be incorrect.
	     This variable is updated in three ways:
	      1) by calling CMSIS function SystemCoreClockUpdate()
	      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
	      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
	  ----------------------------------------------------------------------- */

	  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
	  uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;

	  /* Set TIMx instance */
	  TimHandle.Instance = TIMx;

	  /* Initialize TIMx peripheral as follows:
	       + Period = 10000 - 1
	       + Prescaler = (SystemCoreClock/10000) - 1
	       + ClockDivision = 0
	       + Counter direction = Up
	  */
	  //previous 100-16, clock hơi nhanh, nên chỉnh lên 100-10
	  TimHandle.Init.Period            = 100-16;
	  TimHandle.Init.Prescaler         = uwPrescalerValue;
	  TimHandle.Init.ClockDivision     = 0;
	  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	  TimHandle.Init.RepetitionCounter = 0;
	  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	  {
	    /* Initialization Error */
	    Error_Handler();
	  }

	  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
	  /* Start Channel1 */
	  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	  {
	    /* Starting Error */
	    Error_Handler();
	  }

	  HAL_NVIC_SetPriority((IRQn_Type)(TIM3_IRQn), 0x0, 0);
}

#else
void Timer_Init(void){
	/*##-1- Configure the TIM peripheral #######################################*/
	  /* -----------------------------------------------------------------------
	    In this example TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1) x2,
	    since APB1 prescaler is set to 4 (0x100).
	       TIM3CLK = PCLK1*2
	       PCLK1   = HCLK/2
	    => TIM3CLK = PCLK1*2 = (HCLK/2)*2 = HCLK = SystemCoreClock
	    To get TIM3 counter clock at 10 KHz, the Prescaler is computed as following:
	    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	    Prescaler = (SystemCoreClock /10 KHz) - 1

	    Note:
	     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f1xx.c file.
	     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
	     variable value. Otherwise, any configuration based on this variable will be incorrect.
	     This variable is updated in three ways:
	      1) by calling CMSIS function SystemCoreClockUpdate()
	      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
	      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
	  ----------------------------------------------------------------------- */
		uint8_t boxID = Get_Box_ID();
		uint8_t strpcf[] = "                           ";
		sprintf((char*) strpcf, "boxID in timer = %d\r\n", (int) boxID);
		UART3_SendToHost((uint8_t *)strpcf);
	  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
	  uwPrescalerValue = (uint32_t)(SystemCoreClock / 1000000) - 1;

	  /* Set TIMx instance */
	  TimHandle.Instance = TIMx;

	  /* Initialize TIMx peripheral as follows:
	       + Period = 10000 - 1
	       + Prescaler = (SystemCoreClock/10000) - 1
	       + ClockDivision = 0
	       + Counter direction = Up
	  */
	  //previous 100-16, clock hơi nhanh, nên chỉnh lên 100-10
	  TimHandle.Init.Period = 10000 - GetPeriodOffset(boxID);
	  TimHandle.Init.Prescaler         = uwPrescalerValue;
	  TimHandle.Init.ClockDivision     = 0;
	  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	  TimHandle.Init.RepetitionCounter = 0;
	  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	  {
	    /* Initialization Error */
	    Error_Handler();
	  }

	  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
	  /* Start Channel1 */
	  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	  {
	    /* Starting Error */
	    Error_Handler();
	  }

	  HAL_NVIC_SetPriority((IRQn_Type)(TIM3_IRQn), 0x0, 0);
}

#endif

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	SCH_Update();

}
