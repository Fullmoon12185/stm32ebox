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

static const BoxPeriodConfig_t boxConfig[] = {
	{CITIHOME_39,  		 			910}, //previous 920 900
	{B3_HOAKHANH_116,	 			930}, //previous 950
	{NETHOME_117,  		 			820}, //previous 650
	{NETHOME_118,  		 			905}, //previous 920
	{TOPAZ_119,  		 			830}, //previous 850
	{TOPAZ_120,  		 			720}, //640
	{KINGDOM_121,  		 			900}, //previous 910
	{N1_HOAKHANH_124,  	 			850}, //previous 600
	{HDMON_1_126,  		 			550}, //500
	{HDMON_2_127,  		 			550}, //previous 750
	{PHU_DONG_PREMIER_132, 			740}, //previous 710
	{THUYLOI_134, 					620}, //previous 580
	{LINHTAY_139,		 			850}, //previous 880
	{HAUSINCO_142,	 				760}, //prevous 650
	{BRIDGEVIEW_143, 				860}, //previous 880
	{PETRO_LANDMARK_154, 			630},
	{HAI_THANH_159,					940}, //previous 650 , tháng sau nên giảm xuống còn 940
	{HOA_SEN_169,					560},
	{ECOXUAN_BLOCK_A_172, 			800}, //750
	{PHU_MY_175,					860}, //previous 850 , tháng sau nên giảm xuống còn 940
	{HIMLAM_PHU_DONG_176,			750}, //previous 690 700 730
 	{EBOOST_OFFICE_177,  			530},
	{PARKVIEW_179,		  			730}, //previous 700
	{THE_ASCENT_184,				650}, //previous 600
	{DOCKLAND_189,					720}, //previous 750
	{WESTERN_CAPITAL_193,			810}, //previous 750 830
	{VAN_DO_196,					940}, //previous 850
	{CHUONG_DUONG_HOME_197,			750}, //previous 750 720
	{HUNG_VUONG_1_198,				920}, //previous 850
	{N4_HOA_KHANH_204,				830}, //previous 850
	{HIM_LAM_RIVER_SIDE_207, 		920}, //previous 940
	{CHARM_PLAZA_208,				720}, //700
	{MY_PHUC_1_209, 				850},//880
	{RIVIERA_POINT_213,				780}, //750
	{THU_THIEM_GREEN_HOUSE_221,		810}, //tháng sau nên giảm xuống còn930
	{THU_THIEM_DRAGON_232,			905}, //910
	{DAO_DUY_TU_233,				860}, //previous 850 880
	{DAO_DUY_TU_234,				800},
	{SATRA_VO_VAN_KIET_235,			840}, //860 900
	{RICHMOND_236,					670},//previous 650
	{RICHMOND_237,					655},//previous 670
	{CITISOHO_240,					860}, //previous 850, tháng sau nên giảm xuống còn 820
	{SON_AN_1_241,					700},
	{PHU_MY_BLOCK_B_244,			650},
	{MY_PHUC_2_255,      			800}, //previous 450
	{S_RIVERSIDE_256,      			700}, //previous 650
	{N1_HOAKHANH_2_260,				820}, // tháng sau nên giảm xuống còn 850
	{NGOC_DONG_DUONG_1_275,         450}, // ok đã update
	{NGOC_DONG_DUONG_2_276,         590}, //ok đã update
	{LUXCITY_277,					920},
	{PHU_DONG_SKY_GARDEN_282,		890}, //900
	{DIAMOND_LOTUS_RIVERSIDE_285,	800}, //850
	{GOLDEN_WESTLAKE_286, 			500},
	{CC_8X_PLUS_287,				560}, //550 //580
	{LOVERA_VISTA_293,				550}, //530
	{MYVIEN_295,					570}, //550
	{THOI_AN_1_299,      			820},
	{THOI_AN_2_300,      			390},
    {VINAPHARM_311,      			800}, //680
	{GIA_PHUC_313,					650},
	{VISION_1_315,					550},
	{VISION_2_316,					550},
    {MY_PHUC_3_319,      			605},
    {MY_PHUC_4_320,      			530},
    {EHOME_3_TAY_SAI_GON_322,		880}, // 900
	{PRECIA_325,					770}, //750
    {TARA_327,           			820}, // 850
    {TARA_328,           			870}, // 900 ok
    {THE_LIGHT_1_333,      			910}, //920
    {THE_LIGHT_1_334,      			930}, //920
	{HOANG_ANH_1_336,				850}, //previous 880
	{HOANG_ANH_2_337,				810}, //previous 850
	{VO_DINH_338,					350}, //previous 400
    {PHU_MY_THUAN_341,   			795}, //785
    {HOANG_ANH_THANH_BINH_345, 		750}, //previous 550
    {HOANG_ANH_THANH_BINH_346, 		760}, //previous 550
	{PHUC_YEN_347,					670}, //650
    {HIM_LAM_PHU_DONG_348,   		850},
    {BINH_KHANH_349, 		  		920}, //previous 500
    {TRUONG_DINH_HOI_353, 		  	805}, //previous 830
	{THU_THIEM_GREEN_HOUSE_354,		835},
	{PHU_GIA_1_356,					730},
	{PHU_GIA_2_357,					780},
	{LA_ASTORIA_360,				785}, //800 770
	{B2_HOAKHANH_365,				560}, //530
	{B2_HOAKHANH_366,				530},
	{LANPHUONG_368,					530},
	{PHU_MY_THUAN_369,				830},
	{LOVERA_VISTA_372,				830}, //530
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
