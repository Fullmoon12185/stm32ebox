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

#define DEFAULT_TIMER_OFFSET		650

const BoxPeriodConfig_t boxConfig[] = {
	{CITIHOME_39,  		 			650}, //previous 500
	{B3_HOAKHANH_116,	 			650}, //previous 550
	{NETHOME_117,  		 			650}, //previous 550
	{NETHOME_118,  		 			650}, //previous 550
	{TOPAZ_119,  		 			620}, //previous 450
	{TOPAZ_120,  		 			420},
	{KINGDOM_121,  		 			650}, //previous 600
	{N1_HOAKHANH_124,  	 			600}, //previous 600
	{HDMON_1_126,  		 			600},
	{HDMON_2_127,  		 			650},
	{PHU_DONG_PREMIER_132, 			650}, //previous 450
	{LINHTAY_139,		 			650}, //previous 450
	{HAUSINCO_142,	 				600},
	{BRIDGEVIEW_143, 				650},
	{PETRO_LANDMARK_154, 			600},
	{HAI_THANH_159,					650}, //previous 550
	{PHU_MY_175,					670},
	{HIMLAM_PHU_DONG_176,			600},
	{EBOOST_OFFICE_177,  			450},
	{PARKVIEW_179,		  			600},
	{WESTERN_CAPITAL_193,			650},
	{VAN_DO_196,					650},
	{CHUONG_DUONG_HOME_197,			580},
	{HUNG_VUONG_1_198,				660},
	{N4_HOA_KHANH_204,				600},
	{HIM_LAM_RIVER_SIDE_207, 		650},
	{CHARM_PLAZA_208,				600},
	{MY_PHUC_1_209, 				650},
	{RIVIERA_POINT_213,				500},
	{THU_THIEM_GREEN_HOUSE_221,		660},
	{RICHMOND_236,					600},
	{RICHMOND_237,					450},
	{CITISOHO_240,					650},
	{N1_HOAKHANH_2_260,				650},
	{LUXCITY_277,					650},
	{PHU_DONG_SKY_GARDEN_282,		650},
	{DIAMOND_LOTUS_RIVERSIDE_285,	600},
	{LOVERA_VISTA_293,				450},
	{THOI_AN_1_299,      			650},
	{THOI_AN_2_300,      			350},
	{MY_PHUC_2_255,      			450}, //previous 450
    {VINAPHARM_311,      			650},
    {MY_PHUC_3_319,      			520},
    {MY_PHUC_4_320,      			400},
    {EHOME_3_TAY_SAI_GON_322,		600},
	{PRECIA_325,					550},
    {TARA_327,           			650},
    {TARA_328,           			650}, // previous 570
    {THE_LIGHT_1_333,      			600},
	{HOANG_ANH_2_336,				550}, //previous 400
	{VO_DINH_338,					550}, //previous 400
    {PHU_MY_THUAN_341,   			660}, //previous 570
    {HOANG_ANH_THANH_BINH_346, 		550}, //previous 350
	{PHUC_YEN_347,					600},
    {HIM_LAM_PHU_DONG_348,   		650}, //previous 500
    {BINH_KHANH_349, 		  		600}, //previous 500
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
