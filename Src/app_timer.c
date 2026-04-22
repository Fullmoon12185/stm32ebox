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

#define DEFAULT_TIMER_OFFSET		800

const BoxPeriodConfig_t boxConfig[] = {
	{CITIHOME_39,  		 			920}, //previous 650
	{B3_HOAKHANH_116,	 			950}, //previous 550
	{NETHOME_117,  		 			850}, //previous 650
	{NETHOME_118,  		 			940}, //previous 1000
	{TOPAZ_119,  		 			900}, //previous 800
	{TOPAZ_120,  		 			550}, //390
	{KINGDOM_121,  		 			950}, //previous 850
	{N1_HOAKHANH_124,  	 			750}, //previous 600
	{HDMON_1_126,  		 			900},
	{HDMON_2_127,  		 			900}, //previous 750
	{PHU_DONG_PREMIER_132, 			900}, //previous 450
	{THUYLOI_134, 					550}, //previous 350
	{LINHTAY_139,		 			900}, //previous 750
	{HAUSINCO_142,	 				750}, //prevous 650
	{BRIDGEVIEW_143, 				900}, //previous 700
	{PETRO_LANDMARK_154, 			630},
	{HAI_THANH_159,					940}, //previous 650 , tháng sau nên giảm xuống còn 940
	{ECOXUAN_BLOCK_A_172, 			600},
	{PHU_MY_175,					940}, //previous 670 , tháng sau nên giảm xuống còn 940
	{HIMLAM_PHU_DONG_176,			700}, //previous 600
 	{EBOOST_OFFICE_177,  			550},
	{PARKVIEW_179,		  			700}, //previous 600
	{THE_ASCENT_184,				650}, //previous 600
	{DOCKLAND_189,					750}, //previous 650
	{WESTERN_CAPITAL_193,			940}, //previous 850
	{VAN_DO_196,					940}, //previous 850
	{CHUONG_DUONG_HOME_197,			940}, //previous 850
	{HUNG_VUONG_1_198,				940}, //previous 850
	{N4_HOA_KHANH_204,				900}, //previous 780
	{HIM_LAM_RIVER_SIDE_207, 		940}, //previous 650 , tháng sau nên giảm xuống còn 940
	{CHARM_PLAZA_208,				700},
	{MY_PHUC_1_209, 				950},
	{RIVIERA_POINT_213,				750},
	{THU_THIEM_GREEN_HOUSE_221,		930}, //tháng sau nên giảm xuống còn930
	{THU_THIEM_DRAGON_232,			930}, //tháng sau nên giảm xuống còn 930
	{DAO_DUY_TU_233,				920}, //previous 850
	{DAO_DUY_TU_234,				750},
	{SATRA_VO_VAN_KIET_235,			900},
	{RICHMOND_236,					670},//previous 650
	{RICHMOND_237,					670},//previous 650
	{CITISOHO_240,					820}, //previous 850, tháng sau nên giảm xuống còn 820
	{SON_AN_1_241,					650},
	{PHU_MY_BLOCK_B_244,			650},
	{MY_PHUC_2_255,      			850}, //previous 450
	{N1_HOAKHANH_2_260,				930}, // tháng sau nên giảm xuống còn 930
	{NGOC_DONG_DUONG_2_276,         650},
	{LUXCITY_277,					950},
	{PHU_DONG_SKY_GARDEN_282,		950}, // tháng sau nên giảm xuống còn 930
	{DIAMOND_LOTUS_RIVERSIDE_285,	850},
	{CC_8X_PLUS_287,				600},
	{LOVERA_VISTA_293,				550},
	{THOI_AN_1_299,      			850},
	{THOI_AN_2_300,      			400},
    {VINAPHARM_311,      			800}, //680
	{GIA_PHUC_313,					650},
	{VISION_1_315,					650},
    {MY_PHUC_3_319,      			600},
    {MY_PHUC_4_320,      			500},
    {EHOME_3_TAY_SAI_GON_322,		930}, // tháng sau nên giảm xuống còn 930
	{PRECIA_325,					750},
    {TARA_327,           			930}, // 930
    {TARA_328,           			930}, // 950 ok
    {THE_LIGHT_1_333,      			950},
	{HOANG_ANH_1_336,				850}, //previous 75
	{HOANG_ANH_2_337,				850}, //previous 750
	{VO_DINH_338,					500}, //previous 400
    {PHU_MY_THUAN_341,   			930}, //ok, nen giam xuong con 930
    {HOANG_ANH_THANH_BINH_345, 		750}, //previous 550
    {HOANG_ANH_THANH_BINH_346, 		750}, //previous 550
	{PHUC_YEN_347,					600},
    {HIM_LAM_PHU_DONG_348,   		850},
    {BINH_KHANH_349, 		  		950}, //previous 500
    {TRUONG_DINH_HOI_353, 		  	800}, //previous 500
	{THU_THIEM_GREEN_HOUSE_354,		800},
	{PHU_GIA_1_356,					700},
	{PHU_GIA_2_357,					700},
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
		uint16_t boxID = Get_Box_ID();
		uint8_t strpcf[] = "                           ";
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
	  sprintf((char*) strpcf, "boxID = %d --> GetPeriodOffset = %d\n", (int) boxID, GetPeriodOffset(boxID));
	  UART3_SendToHost((uint8_t *)strpcf);

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
