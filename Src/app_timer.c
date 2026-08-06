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

#define DEFAULT_TIMER_OFFSET		900

//#define DEFAULT_TIMER_OFFSET		500

static const BoxPeriodConfig_t boxConfig[] = {
	{CITIHOME_39,  		 			822}, //820 850
	{B3_HOAKHANH_116,	 			915}, // 920
	{NETHOME_117,  		 			820},
	{NETHOME_118,  		 			827}, // 830
	{TOPAZ_119,  		 			845}, //835 850
	{TOPAZ_120,  		 			790}, //780
	{KINGDOM_121,  		 			827}, // 824 830
	{N1_HOAKHANH_124,  	 			850}, // 600
	{HDMON_1_126,  		 			667}, //667
	{HDMON_2_127,  		 			667}, //657
	{PHU_DONG_PREMIER_132, 			780}, // 775
	{THUYLOI_134, 					625}, // 6200
	{LINHTAY_139,		 			817}, // 815 820
	{HAUSINCO_142,	 				810}, // 790
	{BRIDGEVIEW_143, 				827}, // 830
	{THUTHIEMGARDEN_148,			550}, //500
	{PETRO_LANDMARK_154, 			645},//640
	{HAI_THANH_159,					940}, //previous 650 , tháng sau nên giảm xuống còn 940
	{HOA_SEN_169,					595}, //590 600
	{ECOXUAN_BLOCK_A_172, 			840}, //750 810
	{PHU_MY_175,					832}, //835
	{HIMLAM_PHU_DONG_176,			775}, //765
 	{EBOOST_OFFICE_177,  			530},
	{PARKVIEW_179,		  			800}, //780
	{THE_ASCENT_184,				650}, // 600
	{DOCKLAND_189,					780}, // 770
	{WESTERN_CAPITAL_193,			819}, // 817 830
	{VAN_DO_196,					850}, // 940
	{CHUONG_DUONG_HOME_197,			855}, // 845
	{HUNG_VUONG_1_198,				920}, // 850
	{N4_HOA_KHANH_204,				835}, // 820 830
	{HIM_LAM_RIVER_SIDE_207, 		905}, //previous 910
	{CHARM_PLAZA_208,				730}, //700
	{MY_PHUC_1_209, 				823},//820
	{RIVIERA_POINT_213,				813}, //810
	{THU_THIEM_GREEN_HOUSE_221,		843}, //840
	{THU_THIEM_DRAGON_232,			862}, //865
	{DAO_DUY_TU_233,				852}, // 850 860
	{DAO_DUY_TU_234,				850}, //840
	{SATRA_VO_VAN_KIET_235,			800}, //840 860 900
	{RICHMOND_236,					660},//previous 650 670
	{RICHMOND_237,					635},//645
	{CITISOHO_240,					865}, //860
	{SON_AN_1_241,					710}, //705
	{PHU_MY_BLOCK_B_244,			650},
	{MY_PHUC_2_255,      			780}, //800
	{S_RIVERSIDE_256,      			780}, // 770
	{TECCO_GREEN_NEST_257,          570}, //560
	{TECCO_GREEN_NEST_258,          570}, //560
	{N1_HOAKHANH_2_260,				812}, //820
	{NGOC_DONG_DUONG_1_275,         460}, // ok đã update
	{NGOC_DONG_DUONG_2_276,         590}, //ok đã update
	{LUXCITY_277,					800}, //810
	{PHU_DONG_SKY_GARDEN_282,		830}, //820 850
	{DIAMOND_LOTUS_RIVERSIDE_285,	805}, //800 810
	{GOLDEN_WESTLAKE_286, 			575}, //565
	{CC_8X_PLUS_287,				615}, //590
	{LOVERA_VISTA_293,				605}, //600
	{MYVIEN_295,					650}, //600
	{THOI_AN_1_299,      			845}, //840
	{THOI_AN_2_300,      			435}, //425
    {VINAPHARM_311,      			815}, //810
	{GIA_PHUC_313,					690},//670
	{VISION_1_315,					588}, //580
	{VISION_2_316,					558}, //550
    {MY_PHUC_3_319,      			617}, //605
    {MY_PHUC_4_320,      			600}, //585
    {EHOME_3_TAY_SAI_GON_322,		845}, //843 845
	{PRECIA_325,					790}, //770
    {TARA_327,           			799}, // 800
    {TARA_328,           			815}, // 835
    {THE_LIGHT_1_333,      			890}, //910
    {THE_LIGHT_1_334,      			905}, //910 930
	{HOANG_ANH_1_336,				830}, //825 850
	{HOANG_ANH_2_337,				830}, //825 850 810
	{VO_DINH_338,					395}, //385 400
    {PHU_MY_THUAN_341,   			800}, //795
    {HOANG_ANH_THANH_BINH_345, 		750}, // 550
    {HOANG_ANH_THANH_BINH_346, 		720}, //740
	{PHUC_YEN_347,					670}, //650
    {HIM_LAM_PHU_DONG_348,   		800}, //850
    {BINH_KHANH_349, 		  		885}, //890
    {TRUONG_DINH_HOI_353, 		  	830}, //820 830
	{THU_THIEM_GREEN_HOUSE_354,		820}, //835
	{PHU_GIA_1_356,					720}, //730
	{PHU_GIA_2_357,					810}, //800
	{LA_ASTORIA_359,				495}, //500
	{LA_ASTORIA_360,				810}, //800 785
	{SAKURA_TOWER_362,				650}, //550
	{ERATOWN_363, 					830}, //845
	{ERATOWN_364, 					730}, //710
	{B2_HOAKHANH_365,				745}, //740
	{B2_HOAKHANH_366,				616}, //610 620
	{LA_ASTORIA_367,				455}, //450 470
	{LANPHUONG_368,					530},
	{PHU_MY_THUAN_369,				820}, //815
	{PHODONG_370, 					520}, //500
	{TECCO_GREEN_NEST_371, 			520}, //500
	{LOVERA_VISTA_372,				822}, //820 825 830
	{BRIDGEVIEW_373,				650}, //600
	{CC_8X_PLUS_374,				490}, //480 500
	{EHOME_3_BLOCK_A0_375,			810}, //810
	{FLORA_ANHDAO_383, 				490}, //490 500
	{Ebox_387,						900},
	{Ebox_388,						900},
	{Ebox_389,						900},
	{Ebox_390,						900},
	{Ebox_392,						1400},
	{4095,							500}, //710
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
