/*
 * app_flash_internal.h
 *
 *  Created on: Jul 29, 2021
 *      Author: Nguyet
 */

#ifndef APP_FLASH_INTERNAL_H_
#define APP_FLASH_INTERNAL_H_

#ifdef __cplusplus
 extern "C" {
#endif

/**
  ******************************************************************************
  * @file    combros_peripheral_flash.h
  * @author  Combros
  * @version 1.0
  * @date    9/4/2019 11:15:54 AM
  * @brief   $brief$
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include "boards.h"
//#include "application_sdk_config.h"
//#include "application_config.h"

#ifdef MCU_STM32L0
#include "stm32l0xx_hal.h"
#endif

#ifdef MCU_STM32L1
#include "stm32l1xx_hal.h"
#endif

#ifdef MCU_STM32L4
#include "stm32l4xx_hal.h"
#endif

#ifdef MCU_STM32F1
#include "stm32f1xx_hal.h"
#endif

#ifdef MCU_STM32F2
#include "stm32f2xx_hal.h"
#endif

#ifdef MCU_STM32F3
#include "stm32f3xx_hal.h"
#endif

#ifdef MCU_STM32F4
#include "stm32f4xx_hal.h"
#endif

/* Exported typedef ----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/*!< Base address of the Flash pages */
#if defined (MCU_STM32F3)
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08002800) /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08003000) /* Base @ of Page 6, 2 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08003800) /* Base @ of Page 7, 2 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08004000) /* Base @ of Page 8, 2 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08004800) /* Base @ of Page 9, 2 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08005000) /* Base @ of Page 10, 2 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08005800) /* Base @ of Page 11, 2 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08006000) /* Base @ of Page 12, 2 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08006800) /* Base @ of Page 13, 2 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08007000) /* Base @ of Page 14, 2 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08007800) /* Base @ of Page 15, 2 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08008000) /* Base @ of Page 16, 2 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08008800) /* Base @ of Page 17, 2 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08009000) /* Base @ of Page 18, 2 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08009800) /* Base @ of Page 19, 2 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x0800A000) /* Base @ of Page 20, 2 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x0800A800) /* Base @ of Page 21, 2 Kbytes  */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x0800B000) /* Base @ of Page 22, 2 Kbytes  */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x0800B800) /* Base @ of Page 23, 2 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x0800C000) /* Base @ of Page 24, 2 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x0800C800) /* Base @ of Page 25, 2 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0800D000) /* Base @ of Page 26, 2 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0800D800) /* Base @ of Page 27, 2 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0800E000) /* Base @ of Page 28, 2 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0800E800) /* Base @ of Page 29, 2 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0800F000) /* Base @ of Page 30, 2 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0800F800) /* Base @ of Page 31, 2 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x08010000) /* Base @ of Page 32, 2 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x08010800) /* Base @ of Page 33, 2 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x08011000) /* Base @ of Page 34, 2 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x08011800) /* Base @ of Page 35, 2 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x08012000) /* Base @ of Page 36, 2 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x08012800) /* Base @ of Page 37, 2 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x08013000) /* Base @ of Page 38, 2 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x08013800) /* Base @ of Page 39, 2 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x08014000) /* Base @ of Page 40, 2 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x08014800) /* Base @ of Page 41, 2 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x08015000) /* Base @ of Page 42, 2 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x08015800) /* Base @ of Page 43, 2 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x08016000) /* Base @ of Page 44, 2 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x08016800) /* Base @ of Page 45, 2 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x08017000) /* Base @ of Page 46, 2 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x08017800) /* Base @ of Page 47, 2 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x08018000) /* Base @ of Page 48, 2 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x08018800) /* Base @ of Page 49, 2 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x08019000) /* Base @ of Page 50, 2 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x08019800) /* Base @ of Page 51, 2 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0801A000) /* Base @ of Page 52, 2 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0801A800) /* Base @ of Page 53, 2 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0801B000) /* Base @ of Page 54, 2 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0801B800) /* Base @ of Page 55, 2 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0801C000) /* Base @ of Page 56, 2 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0801C800) /* Base @ of Page 57, 2 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0801D000) /* Base @ of Page 58, 2 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0801D800) /* Base @ of Page 59, 2 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0801E000) /* Base @ of Page 60, 2 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0801E800) /* Base @ of Page 61, 2 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0801F000) /* Base @ of Page 62, 2 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0801F800) /* Base @ of Page 63, 2 Kbytes */
#define ADDR_FLASH_PAGE_64    ((uint32_t)0x08020000) /* Base @ of Page 64, 2 Kbytes */
#define ADDR_FLASH_PAGE_65    ((uint32_t)0x08020800) /* Base @ of Page 65, 2 Kbytes */
#define ADDR_FLASH_PAGE_66    ((uint32_t)0x08021000) /* Base @ of Page 66, 2 Kbytes */
#define ADDR_FLASH_PAGE_67    ((uint32_t)0x08021800) /* Base @ of Page 67, 2 Kbytes */
#define ADDR_FLASH_PAGE_68    ((uint32_t)0x08022000) /* Base @ of Page 68, 2 Kbytes */
#define ADDR_FLASH_PAGE_69    ((uint32_t)0x08022800) /* Base @ of Page 69, 2 Kbytes */
#define ADDR_FLASH_PAGE_70    ((uint32_t)0x08023000) /* Base @ of Page 70, 2 Kbytes */
#define ADDR_FLASH_PAGE_71    ((uint32_t)0x08023800) /* Base @ of Page 71, 2 Kbytes */
#define ADDR_FLASH_PAGE_72    ((uint32_t)0x08024000) /* Base @ of Page 72, 2 Kbytes */
#define ADDR_FLASH_PAGE_73    ((uint32_t)0x08024800) /* Base @ of Page 73, 2 Kbytes */
#define ADDR_FLASH_PAGE_74    ((uint32_t)0x08025000) /* Base @ of Page 74, 2 Kbytes */
#define ADDR_FLASH_PAGE_75    ((uint32_t)0x08025800) /* Base @ of Page 75, 2 Kbytes */
#define ADDR_FLASH_PAGE_76    ((uint32_t)0x08026000) /* Base @ of Page 76, 2 Kbytes */
#define ADDR_FLASH_PAGE_77    ((uint32_t)0x08026800) /* Base @ of Page 77, 2 Kbytes */
#define ADDR_FLASH_PAGE_78    ((uint32_t)0x08027000) /* Base @ of Page 78, 2 Kbytes */
#define ADDR_FLASH_PAGE_79    ((uint32_t)0x08027800) /* Base @ of Page 79, 2 Kbytes */
#define ADDR_FLASH_PAGE_80    ((uint32_t)0x08028000) /* Base @ of Page 80, 2 Kbytes */
#define ADDR_FLASH_PAGE_81    ((uint32_t)0x08028800) /* Base @ of Page 81, 2 Kbytes */
#define ADDR_FLASH_PAGE_82    ((uint32_t)0x08029000) /* Base @ of Page 82, 2 Kbytes */
#define ADDR_FLASH_PAGE_83    ((uint32_t)0x08029800) /* Base @ of Page 83, 2 Kbytes */
#define ADDR_FLASH_PAGE_84    ((uint32_t)0x0802A000) /* Base @ of Page 84, 2 Kbytes */
#define ADDR_FLASH_PAGE_85    ((uint32_t)0x0802A800) /* Base @ of Page 85, 2 Kbytes */
#define ADDR_FLASH_PAGE_86    ((uint32_t)0x0802B000) /* Base @ of Page 86, 2 Kbytes */
#define ADDR_FLASH_PAGE_87    ((uint32_t)0x0802B800) /* Base @ of Page 87, 2 Kbytes */
#define ADDR_FLASH_PAGE_88    ((uint32_t)0x0802C000) /* Base @ of Page 88, 2 Kbytes */
#define ADDR_FLASH_PAGE_89    ((uint32_t)0x0802C800) /* Base @ of Page 89, 2 Kbytes */
#define ADDR_FLASH_PAGE_90    ((uint32_t)0x0802D000) /* Base @ of Page 90, 2 Kbytes */
#define ADDR_FLASH_PAGE_91    ((uint32_t)0x0802D800) /* Base @ of Page 91, 2 Kbytes */
#define ADDR_FLASH_PAGE_92    ((uint32_t)0x0802E000) /* Base @ of Page 92, 2 Kbytes */
#define ADDR_FLASH_PAGE_93    ((uint32_t)0x0802E800) /* Base @ of Page 93, 2 Kbytes */
#define ADDR_FLASH_PAGE_94    ((uint32_t)0x0802F000) /* Base @ of Page 94, 2 Kbytes */
#define ADDR_FLASH_PAGE_95    ((uint32_t)0x0802F800) /* Base @ of Page 95, 2 Kbytes */
#define ADDR_FLASH_PAGE_96    ((uint32_t)0x08030000) /* Base @ of Page 96, 2 Kbytes */
#define ADDR_FLASH_PAGE_97    ((uint32_t)0x08030800) /* Base @ of Page 97, 2 Kbytes */
#define ADDR_FLASH_PAGE_98    ((uint32_t)0x08031000) /* Base @ of Page 98, 2 Kbytes */
#define ADDR_FLASH_PAGE_99    ((uint32_t)0x08031800) /* Base @ of Page 99, 2 Kbytes */
#define ADDR_FLASH_PAGE_100   ((uint32_t)0x08032000) /* Base @ of Page 100, 2 Kbytes */
#define ADDR_FLASH_PAGE_101   ((uint32_t)0x08032800) /* Base @ of Page 101, 2 Kbytes */
#define ADDR_FLASH_PAGE_102   ((uint32_t)0x08033000) /* Base @ of Page 102, 2 Kbytes */
#define ADDR_FLASH_PAGE_103   ((uint32_t)0x08033800) /* Base @ of Page 103, 2 Kbytes */
#define ADDR_FLASH_PAGE_104   ((uint32_t)0x08034000) /* Base @ of Page 104, 2 Kbytes */
#define ADDR_FLASH_PAGE_105   ((uint32_t)0x08034800) /* Base @ of Page 105, 2 Kbytes */
#define ADDR_FLASH_PAGE_106   ((uint32_t)0x08035000) /* Base @ of Page 106, 2 Kbytes */
#define ADDR_FLASH_PAGE_107   ((uint32_t)0x08035800) /* Base @ of Page 107, 2 Kbytes */
#define ADDR_FLASH_PAGE_108   ((uint32_t)0x08036000) /* Base @ of Page 108, 2 Kbytes */
#define ADDR_FLASH_PAGE_109   ((uint32_t)0x08036800) /* Base @ of Page 109, 2 Kbytes */
#define ADDR_FLASH_PAGE_110   ((uint32_t)0x08037000) /* Base @ of Page 110, 2 Kbytes */
#define ADDR_FLASH_PAGE_111   ((uint32_t)0x08037800) /* Base @ of Page 111, 2 Kbytes */
#define ADDR_FLASH_PAGE_112   ((uint32_t)0x08038000) /* Base @ of Page 112, 2 Kbytes */
#define ADDR_FLASH_PAGE_113   ((uint32_t)0x08038800) /* Base @ of Page 113, 2 Kbytes */
#define ADDR_FLASH_PAGE_114   ((uint32_t)0x08039000) /* Base @ of Page 114, 2 Kbytes */
#define ADDR_FLASH_PAGE_115   ((uint32_t)0x08039800) /* Base @ of Page 115, 2 Kbytes */
#define ADDR_FLASH_PAGE_116   ((uint32_t)0x0803A000) /* Base @ of Page 116, 2 Kbytes */
#define ADDR_FLASH_PAGE_117   ((uint32_t)0x0803A800) /* Base @ of Page 117, 2 Kbytes  */
#define ADDR_FLASH_PAGE_118   ((uint32_t)0x0803B000) /* Base @ of Page 118, 2 Kbytes  */
#define ADDR_FLASH_PAGE_119   ((uint32_t)0x0803B800) /* Base @ of Page 119, 2 Kbytes */
#define ADDR_FLASH_PAGE_120   ((uint32_t)0x0803C000) /* Base @ of Page 120, 2 Kbytes */
#define ADDR_FLASH_PAGE_121   ((uint32_t)0x0803C800) /* Base @ of Page 121, 2 Kbytes */
#define ADDR_FLASH_PAGE_122   ((uint32_t)0x0803D000) /* Base @ of Page 122, 2 Kbytes */
#define ADDR_FLASH_PAGE_123   ((uint32_t)0x0803D800) /* Base @ of Page 123, 2 Kbytes */
#define ADDR_FLASH_PAGE_124   ((uint32_t)0x0803E000) /* Base @ of Page 124, 2 Kbytes */
#define ADDR_FLASH_PAGE_125   ((uint32_t)0x0803E800) /* Base @ of Page 125, 2 Kbytes */
#define ADDR_FLASH_PAGE_126   ((uint32_t)0x0803F000) /* Base @ of Page 126, 2 Kbytes */
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0803F800) /* Base @ of Page 127, 2 Kbytes */

#elif (defined(STM32F101x6) || defined(STM32F102x6) || defined(STM32F103x6) || defined(STM32F100xB) || defined(STM32F101xB) || defined(STM32F102xB) || defined(STM32F103xB))
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 1 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000400) /* Base @ of Page 1, 1 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08000800) /* Base @ of Page 2, 1 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08000C00) /* Base @ of Page 3, 1 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08001000) /* Base @ of Page 4, 1 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08001400) /* Base @ of Page 5, 1 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08001800) /* Base @ of Page 6, 1 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08001C00) /* Base @ of Page 7, 1 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08002000) /* Base @ of Page 8, 1 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08002400) /* Base @ of Page 9, 1 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08002800) /* Base @ of Page 10, 1 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08002C00) /* Base @ of Page 11, 1 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08003000) /* Base @ of Page 12, 1 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08003400) /* Base @ of Page 13, 1 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08003800) /* Base @ of Page 14, 1 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08003C00) /* Base @ of Page 15, 1 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08004000) /* Base @ of Page 16, 1 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08004400) /* Base @ of Page 17, 1 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08004800) /* Base @ of Page 18, 1 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08004C00) /* Base @ of Page 19, 1 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x08005000) /* Base @ of Page 20, 1 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x08005400) /* Base @ of Page 21, 1 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x08005800) /* Base @ of Page 22, 1 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x08005C00) /* Base @ of Page 23, 1 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x08006000) /* Base @ of Page 24, 1 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x08006400) /* Base @ of Page 25, 1 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x08006800) /* Base @ of Page 26, 1 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x08006C00) /* Base @ of Page 27, 1 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x08007000) /* Base @ of Page 28, 1 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x08007400) /* Base @ of Page 29, 1 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x08007800) /* Base @ of Page 30, 1 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x08007C00) /* Base @ of Page 31, 1 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x08008000) /* Base @ of Page 32, 1 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x08008400) /* Base @ of Page 33, 1 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x08008800) /* Base @ of Page 34, 1 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x08008C00) /* Base @ of Page 35, 1 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x08009000) /* Base @ of Page 36, 1 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x08009400) /* Base @ of Page 37, 1 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x08009800) /* Base @ of Page 38, 1 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x08009C00) /* Base @ of Page 39, 1 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x0800A000) /* Base @ of Page 40, 1 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x0800A400) /* Base @ of Page 41, 1 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x0800A800) /* Base @ of Page 42, 1 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x0800AC00) /* Base @ of Page 43, 1 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x0800B000) /* Base @ of Page 44, 1 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x0800B400) /* Base @ of Page 45, 1 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x0800B800) /* Base @ of Page 46, 1 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x0800BC00) /* Base @ of Page 47, 1 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x0800C000) /* Base @ of Page 48, 1 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x0800C400) /* Base @ of Page 49, 1 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x0800C800) /* Base @ of Page 50, 1 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x0800CC00) /* Base @ of Page 51, 1 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0800D000) /* Base @ of Page 52, 1 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0800D400) /* Base @ of Page 53, 1 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0800D800) /* Base @ of Page 54, 1 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0800DC00) /* Base @ of Page 55, 1 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0800E000) /* Base @ of Page 56, 1 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0800E400) /* Base @ of Page 57, 1 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0800E800) /* Base @ of Page 58, 1 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0800EC00) /* Base @ of Page 59, 1 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0800F000) /* Base @ of Page 60, 1 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0800F400) /* Base @ of Page 61, 1 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0800F800) /* Base @ of Page 62, 1 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0800FC00) /* Base @ of Page 63, 1 Kbytes */
#define ADDR_FLASH_PAGE_64    ((uint32_t)0x08010000) /* Base @ of Page 64, 1 Kbytes */
#define ADDR_FLASH_PAGE_65    ((uint32_t)0x08010400) /* Base @ of Page 65, 1 Kbytes */
#define ADDR_FLASH_PAGE_66    ((uint32_t)0x08010800) /* Base @ of Page 66, 1 Kbytes */
#define ADDR_FLASH_PAGE_67    ((uint32_t)0x08010C00) /* Base @ of Page 67, 1 Kbytes */
#define ADDR_FLASH_PAGE_68    ((uint32_t)0x08011000) /* Base @ of Page 68, 1 Kbytes */
#define ADDR_FLASH_PAGE_69    ((uint32_t)0x08011400) /* Base @ of Page 69, 1 Kbytes */
#define ADDR_FLASH_PAGE_70    ((uint32_t)0x08011800) /* Base @ of Page 70, 1 Kbytes */
#define ADDR_FLASH_PAGE_71    ((uint32_t)0x08011C00) /* Base @ of Page 71, 1 Kbytes */
#define ADDR_FLASH_PAGE_72    ((uint32_t)0x08012000) /* Base @ of Page 72, 1 Kbytes */
#define ADDR_FLASH_PAGE_73    ((uint32_t)0x08012400) /* Base @ of Page 73, 1 Kbytes */
#define ADDR_FLASH_PAGE_74    ((uint32_t)0x08012800) /* Base @ of Page 74, 1 Kbytes */
#define ADDR_FLASH_PAGE_75    ((uint32_t)0x08012C00) /* Base @ of Page 75, 1 Kbytes */
#define ADDR_FLASH_PAGE_76    ((uint32_t)0x08013000) /* Base @ of Page 76, 1 Kbytes */
#define ADDR_FLASH_PAGE_77    ((uint32_t)0x08013400) /* Base @ of Page 77, 1 Kbytes */
#define ADDR_FLASH_PAGE_78    ((uint32_t)0x08013800) /* Base @ of Page 78, 1 Kbytes */
#define ADDR_FLASH_PAGE_79    ((uint32_t)0x08013C00) /* Base @ of Page 79, 1 Kbytes */
#define ADDR_FLASH_PAGE_80    ((uint32_t)0x08014000) /* Base @ of Page 80, 1 Kbytes */
#define ADDR_FLASH_PAGE_81    ((uint32_t)0x08014400) /* Base @ of Page 81, 1 Kbytes */
#define ADDR_FLASH_PAGE_82    ((uint32_t)0x08014800) /* Base @ of Page 82, 1 Kbytes */
#define ADDR_FLASH_PAGE_83    ((uint32_t)0x08014C00) /* Base @ of Page 83, 1 Kbytes */
#define ADDR_FLASH_PAGE_84    ((uint32_t)0x08015000) /* Base @ of Page 84, 1 Kbytes */
#define ADDR_FLASH_PAGE_85    ((uint32_t)0x08015400) /* Base @ of Page 85, 1 Kbytes */
#define ADDR_FLASH_PAGE_86    ((uint32_t)0x08015800) /* Base @ of Page 86, 1 Kbytes */
#define ADDR_FLASH_PAGE_87    ((uint32_t)0x08015C00) /* Base @ of Page 87, 1 Kbytes */
#define ADDR_FLASH_PAGE_88    ((uint32_t)0x08016000) /* Base @ of Page 88, 1 Kbytes */
#define ADDR_FLASH_PAGE_89    ((uint32_t)0x08016400) /* Base @ of Page 89, 1 Kbytes */
#define ADDR_FLASH_PAGE_90    ((uint32_t)0x08016800) /* Base @ of Page 90, 1 Kbytes */
#define ADDR_FLASH_PAGE_91    ((uint32_t)0x08016C00) /* Base @ of Page 91, 1 Kbytes */
#define ADDR_FLASH_PAGE_92    ((uint32_t)0x08017000) /* Base @ of Page 92, 1 Kbytes */
#define ADDR_FLASH_PAGE_93    ((uint32_t)0x08017400) /* Base @ of Page 93, 1 Kbytes */
#define ADDR_FLASH_PAGE_94    ((uint32_t)0x08017800) /* Base @ of Page 94, 1 Kbytes */
#define ADDR_FLASH_PAGE_95    ((uint32_t)0x08017C00) /* Base @ of Page 95, 1 Kbytes */
#define ADDR_FLASH_PAGE_96    ((uint32_t)0x08018000) /* Base @ of Page 96, 1 Kbytes */
#define ADDR_FLASH_PAGE_97    ((uint32_t)0x08018400) /* Base @ of Page 97, 1 Kbytes */
#define ADDR_FLASH_PAGE_98    ((uint32_t)0x08018800) /* Base @ of Page 98, 1 Kbytes */
#define ADDR_FLASH_PAGE_99    ((uint32_t)0x08018C00) /* Base @ of Page 99, 1 Kbytes */
#define ADDR_FLASH_PAGE_100   ((uint32_t)0x08019000) /* Base @ of Page 100, 1 Kbytes */
#define ADDR_FLASH_PAGE_101   ((uint32_t)0x08019400) /* Base @ of Page 101, 1 Kbytes */
#define ADDR_FLASH_PAGE_102   ((uint32_t)0x08019800) /* Base @ of Page 102, 1 Kbytes */
#define ADDR_FLASH_PAGE_103   ((uint32_t)0x08019C00) /* Base @ of Page 103, 1 Kbytes */
#define ADDR_FLASH_PAGE_104   ((uint32_t)0x0801A000) /* Base @ of Page 104, 1 Kbytes */
#define ADDR_FLASH_PAGE_105   ((uint32_t)0x0801A400) /* Base @ of Page 105, 1 Kbytes */
#define ADDR_FLASH_PAGE_106   ((uint32_t)0x0801A800) /* Base @ of Page 106, 1 Kbytes */
#define ADDR_FLASH_PAGE_107   ((uint32_t)0x0801AC00) /* Base @ of Page 107, 1 Kbytes */
#define ADDR_FLASH_PAGE_108   ((uint32_t)0x0801B000) /* Base @ of Page 108, 1 Kbytes */
#define ADDR_FLASH_PAGE_109   ((uint32_t)0x0801B400) /* Base @ of Page 109, 1 Kbytes */
#define ADDR_FLASH_PAGE_110   ((uint32_t)0x0801B800) /* Base @ of Page 110, 1 Kbytes */
#define ADDR_FLASH_PAGE_111   ((uint32_t)0x0801BC00) /* Base @ of Page 111, 1 Kbytes */
#define ADDR_FLASH_PAGE_112   ((uint32_t)0x0801C000) /* Base @ of Page 112, 1 Kbytes */
#define ADDR_FLASH_PAGE_113   ((uint32_t)0x0801C400) /* Base @ of Page 113, 1 Kbytes */
#define ADDR_FLASH_PAGE_114   ((uint32_t)0x0801C800) /* Base @ of Page 114, 1 Kbytes */
#define ADDR_FLASH_PAGE_115   ((uint32_t)0x0801CC00) /* Base @ of Page 115, 1 Kbytes */
#define ADDR_FLASH_PAGE_116   ((uint32_t)0x0801D000) /* Base @ of Page 116, 1 Kbytes */
#define ADDR_FLASH_PAGE_117   ((uint32_t)0x0801D400) /* Base @ of Page 117, 1 Kbytes */
#define ADDR_FLASH_PAGE_118   ((uint32_t)0x0801D800) /* Base @ of Page 118, 1 Kbytes */
#define ADDR_FLASH_PAGE_119   ((uint32_t)0x0801DC00) /* Base @ of Page 119, 1 Kbytes */
#define ADDR_FLASH_PAGE_120   ((uint32_t)0x0801E000) /* Base @ of Page 120, 1 Kbytes */
#define ADDR_FLASH_PAGE_121   ((uint32_t)0x0801E400) /* Base @ of Page 121, 1 Kbytes */
#define ADDR_FLASH_PAGE_122   ((uint32_t)0x0801E800) /* Base @ of Page 122, 1 Kbytes */
#define ADDR_FLASH_PAGE_123   ((uint32_t)0x0801EC00) /* Base @ of Page 123, 1 Kbytes */
#define ADDR_FLASH_PAGE_124   ((uint32_t)0x0801F000) /* Base @ of Page 124, 1 Kbytes */
#define ADDR_FLASH_PAGE_125   ((uint32_t)0x0801F400) /* Base @ of Page 125, 1 Kbytes */
#define ADDR_FLASH_PAGE_126   ((uint32_t)0x0801F800) /* Base @ of Page 126, 1 Kbytes */
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0801FC00) /* Base @ of Page 127, 1 Kbytes */

#elif (defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F103xE) || defined(STM32F101xG) || defined(STM32F103xG) || defined(STM32F105xC) || defined(STM32F107xC))
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08002800) /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08003000) /* Base @ of Page 6, 2 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08003800) /* Base @ of Page 7, 2 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08004000) /* Base @ of Page 8, 2 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08004800) /* Base @ of Page 9, 2 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08005000) /* Base @ of Page 10, 2 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08005800) /* Base @ of Page 11, 2 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08006000) /* Base @ of Page 12, 2 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08006800) /* Base @ of Page 13, 2 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08007000) /* Base @ of Page 14, 2 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08007800) /* Base @ of Page 15, 2 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08008000) /* Base @ of Page 16, 2 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08008800) /* Base @ of Page 17, 2 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08009000) /* Base @ of Page 18, 2 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08009800) /* Base @ of Page 19, 2 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x0800A000) /* Base @ of Page 20, 2 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x0800A800) /* Base @ of Page 21, 2 Kbytes  */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x0800B000) /* Base @ of Page 22, 2 Kbytes  */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x0800B800) /* Base @ of Page 23, 2 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x0800C000) /* Base @ of Page 24, 2 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x0800C800) /* Base @ of Page 25, 2 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0800D000) /* Base @ of Page 26, 2 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0800D800) /* Base @ of Page 27, 2 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0800E000) /* Base @ of Page 28, 2 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0800E800) /* Base @ of Page 29, 2 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0800F000) /* Base @ of Page 30, 2 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0800F800) /* Base @ of Page 31, 2 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x08010000) /* Base @ of Page 32, 2 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x08010800) /* Base @ of Page 33, 2 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x08011000) /* Base @ of Page 34, 2 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x08011800) /* Base @ of Page 35, 2 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x08012000) /* Base @ of Page 36, 2 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x08012800) /* Base @ of Page 37, 2 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x08013000) /* Base @ of Page 38, 2 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x08013800) /* Base @ of Page 39, 2 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x08014000) /* Base @ of Page 40, 2 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x08014800) /* Base @ of Page 41, 2 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x08015000) /* Base @ of Page 42, 2 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x08015800) /* Base @ of Page 43, 2 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x08016000) /* Base @ of Page 44, 2 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x08016800) /* Base @ of Page 45, 2 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x08017000) /* Base @ of Page 46, 2 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x08017800) /* Base @ of Page 47, 2 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x08018000) /* Base @ of Page 48, 2 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x08018800) /* Base @ of Page 49, 2 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x08019000) /* Base @ of Page 50, 2 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x08019800) /* Base @ of Page 51, 2 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0801A000) /* Base @ of Page 52, 2 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0801A800) /* Base @ of Page 53, 2 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0801B000) /* Base @ of Page 54, 2 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0801B800) /* Base @ of Page 55, 2 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0801C000) /* Base @ of Page 56, 2 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0801C800) /* Base @ of Page 57, 2 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0801D000) /* Base @ of Page 58, 2 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0801D800) /* Base @ of Page 59, 2 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0801E000) /* Base @ of Page 60, 2 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0801E800) /* Base @ of Page 61, 2 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0801F000) /* Base @ of Page 62, 2 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0801F800) /* Base @ of Page 63, 2 Kbytes */
#define ADDR_FLASH_PAGE_64    ((uint32_t)0x08020000) /* Base @ of Page 64, 2 Kbytes */
#define ADDR_FLASH_PAGE_65    ((uint32_t)0x08020800) /* Base @ of Page 65, 2 Kbytes */
#define ADDR_FLASH_PAGE_66    ((uint32_t)0x08021000) /* Base @ of Page 66, 2 Kbytes */
#define ADDR_FLASH_PAGE_67    ((uint32_t)0x08021800) /* Base @ of Page 67, 2 Kbytes */
#define ADDR_FLASH_PAGE_68    ((uint32_t)0x08022000) /* Base @ of Page 68, 2 Kbytes */
#define ADDR_FLASH_PAGE_69    ((uint32_t)0x08022800) /* Base @ of Page 69, 2 Kbytes */
#define ADDR_FLASH_PAGE_70    ((uint32_t)0x08023000) /* Base @ of Page 70, 2 Kbytes */
#define ADDR_FLASH_PAGE_71    ((uint32_t)0x08023800) /* Base @ of Page 71, 2 Kbytes */
#define ADDR_FLASH_PAGE_72    ((uint32_t)0x08024000) /* Base @ of Page 72, 2 Kbytes */
#define ADDR_FLASH_PAGE_73    ((uint32_t)0x08024800) /* Base @ of Page 73, 2 Kbytes */
#define ADDR_FLASH_PAGE_74    ((uint32_t)0x08025000) /* Base @ of Page 74, 2 Kbytes */
#define ADDR_FLASH_PAGE_75    ((uint32_t)0x08025800) /* Base @ of Page 75, 2 Kbytes */
#define ADDR_FLASH_PAGE_76    ((uint32_t)0x08026000) /* Base @ of Page 76, 2 Kbytes */
#define ADDR_FLASH_PAGE_77    ((uint32_t)0x08026800) /* Base @ of Page 77, 2 Kbytes */
#define ADDR_FLASH_PAGE_78    ((uint32_t)0x08027000) /* Base @ of Page 78, 2 Kbytes */
#define ADDR_FLASH_PAGE_79    ((uint32_t)0x08027800) /* Base @ of Page 79, 2 Kbytes */
#define ADDR_FLASH_PAGE_80    ((uint32_t)0x08028000) /* Base @ of Page 80, 2 Kbytes */
#define ADDR_FLASH_PAGE_81    ((uint32_t)0x08028800) /* Base @ of Page 81, 2 Kbytes */
#define ADDR_FLASH_PAGE_82    ((uint32_t)0x08029000) /* Base @ of Page 82, 2 Kbytes */
#define ADDR_FLASH_PAGE_83    ((uint32_t)0x08029800) /* Base @ of Page 83, 2 Kbytes */
#define ADDR_FLASH_PAGE_84    ((uint32_t)0x0802A000) /* Base @ of Page 84, 2 Kbytes */
#define ADDR_FLASH_PAGE_85    ((uint32_t)0x0802A800) /* Base @ of Page 85, 2 Kbytes */
#define ADDR_FLASH_PAGE_86    ((uint32_t)0x0802B000) /* Base @ of Page 86, 2 Kbytes */
#define ADDR_FLASH_PAGE_87    ((uint32_t)0x0802B800) /* Base @ of Page 87, 2 Kbytes */
#define ADDR_FLASH_PAGE_88    ((uint32_t)0x0802C000) /* Base @ of Page 88, 2 Kbytes */
#define ADDR_FLASH_PAGE_89    ((uint32_t)0x0802C800) /* Base @ of Page 89, 2 Kbytes */
#define ADDR_FLASH_PAGE_90    ((uint32_t)0x0802D000) /* Base @ of Page 90, 2 Kbytes */
#define ADDR_FLASH_PAGE_91    ((uint32_t)0x0802D800) /* Base @ of Page 91, 2 Kbytes */
#define ADDR_FLASH_PAGE_92    ((uint32_t)0x0802E000) /* Base @ of Page 92, 2 Kbytes */
#define ADDR_FLASH_PAGE_93    ((uint32_t)0x0802E800) /* Base @ of Page 93, 2 Kbytes */
#define ADDR_FLASH_PAGE_94    ((uint32_t)0x0802F000) /* Base @ of Page 94, 2 Kbytes */
#define ADDR_FLASH_PAGE_95    ((uint32_t)0x0802F800) /* Base @ of Page 95, 2 Kbytes */
#define ADDR_FLASH_PAGE_96    ((uint32_t)0x08030000) /* Base @ of Page 96, 2 Kbytes */
#define ADDR_FLASH_PAGE_97    ((uint32_t)0x08030800) /* Base @ of Page 97, 2 Kbytes */
#define ADDR_FLASH_PAGE_98    ((uint32_t)0x08031000) /* Base @ of Page 98, 2 Kbytes */
#define ADDR_FLASH_PAGE_99    ((uint32_t)0x08031800) /* Base @ of Page 99, 2 Kbytes */
#define ADDR_FLASH_PAGE_100   ((uint32_t)0x08032000) /* Base @ of Page 100, 2 Kbytes */
#define ADDR_FLASH_PAGE_101   ((uint32_t)0x08032800) /* Base @ of Page 101, 2 Kbytes */
#define ADDR_FLASH_PAGE_102   ((uint32_t)0x08033000) /* Base @ of Page 102, 2 Kbytes */
#define ADDR_FLASH_PAGE_103   ((uint32_t)0x08033800) /* Base @ of Page 103, 2 Kbytes */
#define ADDR_FLASH_PAGE_104   ((uint32_t)0x08034000) /* Base @ of Page 104, 2 Kbytes */
#define ADDR_FLASH_PAGE_105   ((uint32_t)0x08034800) /* Base @ of Page 105, 2 Kbytes */
#define ADDR_FLASH_PAGE_106   ((uint32_t)0x08035000) /* Base @ of Page 106, 2 Kbytes */
#define ADDR_FLASH_PAGE_107   ((uint32_t)0x08035800) /* Base @ of Page 107, 2 Kbytes */
#define ADDR_FLASH_PAGE_108   ((uint32_t)0x08036000) /* Base @ of Page 108, 2 Kbytes */
#define ADDR_FLASH_PAGE_109   ((uint32_t)0x08036800) /* Base @ of Page 109, 2 Kbytes */
#define ADDR_FLASH_PAGE_110   ((uint32_t)0x08037000) /* Base @ of Page 110, 2 Kbytes */
#define ADDR_FLASH_PAGE_111   ((uint32_t)0x08037800) /* Base @ of Page 111, 2 Kbytes */
#define ADDR_FLASH_PAGE_112   ((uint32_t)0x08038000) /* Base @ of Page 112, 2 Kbytes */
#define ADDR_FLASH_PAGE_113   ((uint32_t)0x08038800) /* Base @ of Page 113, 2 Kbytes */
#define ADDR_FLASH_PAGE_114   ((uint32_t)0x08039000) /* Base @ of Page 114, 2 Kbytes */
#define ADDR_FLASH_PAGE_115   ((uint32_t)0x08039800) /* Base @ of Page 115, 2 Kbytes */
#define ADDR_FLASH_PAGE_116   ((uint32_t)0x0803A000) /* Base @ of Page 116, 2 Kbytes */
#define ADDR_FLASH_PAGE_117   ((uint32_t)0x0803A800) /* Base @ of Page 117, 2 Kbytes  */
#define ADDR_FLASH_PAGE_118   ((uint32_t)0x0803B000) /* Base @ of Page 118, 2 Kbytes  */
#define ADDR_FLASH_PAGE_119   ((uint32_t)0x0803B800) /* Base @ of Page 119, 2 Kbytes */
#define ADDR_FLASH_PAGE_120   ((uint32_t)0x0803C000) /* Base @ of Page 120, 2 Kbytes */
#define ADDR_FLASH_PAGE_121   ((uint32_t)0x0803C800) /* Base @ of Page 121, 2 Kbytes */
#define ADDR_FLASH_PAGE_122   ((uint32_t)0x0803D000) /* Base @ of Page 122, 2 Kbytes */
#define ADDR_FLASH_PAGE_123   ((uint32_t)0x0803D800) /* Base @ of Page 123, 2 Kbytes */
#define ADDR_FLASH_PAGE_124   ((uint32_t)0x0803E000) /* Base @ of Page 124, 2 Kbytes */
#define ADDR_FLASH_PAGE_125   ((uint32_t)0x0803E800) /* Base @ of Page 125, 2 Kbytes */
#define ADDR_FLASH_PAGE_126   ((uint32_t)0x0803F000) /* Base @ of Page 126, 2 Kbytes */
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0803F800) /* Base @ of Page 127, 2 Kbytes */

#elif defined (MCU_STM32L1)
#define ADDR_FLASH_PAGE_0   ((uint32_t)0x08000000) /* Base @ of Page 0, 256 bytes */
#define ADDR_FLASH_PAGE_1   ((uint32_t)0x08000100) /* Base @ of Page 1, 256 bytes */
#define ADDR_FLASH_PAGE_2   ((uint32_t)0x08000200) /* Base @ of Page 2, 256 bytes */
#define ADDR_FLASH_PAGE_3   ((uint32_t)0x08000300) /* Base @ of Page 3, 256 bytes */
#define ADDR_FLASH_PAGE_4   ((uint32_t)0x08000400) /* Base @ of Page 4, 256 bytes */
#define ADDR_FLASH_PAGE_5   ((uint32_t)0x08000500) /* Base @ of Page 5, 256 bytes */
#define ADDR_FLASH_PAGE_6   ((uint32_t)0x08000600) /* Base @ of Page 6, 256 bytes */
#define ADDR_FLASH_PAGE_7   ((uint32_t)0x08000700) /* Base @ of Page 7, 256 bytes */
#define ADDR_FLASH_PAGE_8   ((uint32_t)0x08000800) /* Base @ of Page 8, 256 bytes */
#define ADDR_FLASH_PAGE_9   ((uint32_t)0x08000900) /* Base @ of Page 9, 256 bytes */
#define ADDR_FLASH_PAGE_10  ((uint32_t)0x08000A00) /* Base @ of Page 10, 256 bytes */
#define ADDR_FLASH_PAGE_11  ((uint32_t)0x08000B00) /* Base @ of Page 11, 256 bytes */
#define ADDR_FLASH_PAGE_12  ((uint32_t)0x08000C00) /* Base @ of Page 12, 256 bytes */
#define ADDR_FLASH_PAGE_13  ((uint32_t)0x08000D00) /* Base @ of Page 13, 256 bytes */
#define ADDR_FLASH_PAGE_14  ((uint32_t)0x08000E00) /* Base @ of Page 14, 256 bytes */
#define ADDR_FLASH_PAGE_15  ((uint32_t)0x08000F00) /* Base @ of Page 15, 256 bytes */
#define ADDR_FLASH_PAGE_16  ((uint32_t)0x08001000) /* Base @ of Page 16, 256 bytes */
#define ADDR_FLASH_PAGE_17  ((uint32_t)0x08001100) /* Base @ of Page 17, 256 bytes */
#define ADDR_FLASH_PAGE_18  ((uint32_t)0x08001200) /* Base @ of Page 18, 256 bytes */
#define ADDR_FLASH_PAGE_19  ((uint32_t)0x08001300) /* Base @ of Page 19, 256 bytes */
#define ADDR_FLASH_PAGE_20  ((uint32_t)0x08001400) /* Base @ of Page 20, 256 bytes */
#define ADDR_FLASH_PAGE_21  ((uint32_t)0x08001500) /* Base @ of Page 21, 256 bytes */
#define ADDR_FLASH_PAGE_22  ((uint32_t)0x08001600) /* Base @ of Page 22, 256 bytes */
#define ADDR_FLASH_PAGE_23  ((uint32_t)0x08001700) /* Base @ of Page 23, 256 bytes */
#define ADDR_FLASH_PAGE_24  ((uint32_t)0x08001800) /* Base @ of Page 24, 256 bytes */
#define ADDR_FLASH_PAGE_25  ((uint32_t)0x08001900) /* Base @ of Page 25, 256 bytes */
#define ADDR_FLASH_PAGE_26  ((uint32_t)0x08001A00) /* Base @ of Page 26, 256 bytes */
#define ADDR_FLASH_PAGE_27  ((uint32_t)0x08001B00) /* Base @ of Page 27, 256 bytes */
#define ADDR_FLASH_PAGE_28  ((uint32_t)0x08001C00) /* Base @ of Page 28, 256 bytes */
#define ADDR_FLASH_PAGE_29  ((uint32_t)0x08001D00) /* Base @ of Page 29, 256 bytes */
#define ADDR_FLASH_PAGE_30  ((uint32_t)0x08001E00) /* Base @ of Page 30, 256 bytes */
#define ADDR_FLASH_PAGE_31  ((uint32_t)0x08001F00) /* Base @ of Page 31, 256 bytes */
#define ADDR_FLASH_PAGE_32  ((uint32_t)0x08002000) /* Base @ of Page 32, 256 bytes */
#define ADDR_FLASH_PAGE_33  ((uint32_t)0x08002100) /* Base @ of Page 33, 256 bytes */
#define ADDR_FLASH_PAGE_34  ((uint32_t)0x08002200) /* Base @ of Page 34, 256 bytes */
#define ADDR_FLASH_PAGE_35  ((uint32_t)0x08002300) /* Base @ of Page 35, 256 bytes */
#define ADDR_FLASH_PAGE_36  ((uint32_t)0x08002400) /* Base @ of Page 36, 256 bytes */
#define ADDR_FLASH_PAGE_37  ((uint32_t)0x08002500) /* Base @ of Page 37, 256 bytes */
#define ADDR_FLASH_PAGE_38  ((uint32_t)0x08002600) /* Base @ of Page 38, 256 bytes */
#define ADDR_FLASH_PAGE_39  ((uint32_t)0x08002700) /* Base @ of Page 39, 256 bytes */
#define ADDR_FLASH_PAGE_40  ((uint32_t)0x08002800) /* Base @ of Page 40, 256 bytes */
#define ADDR_FLASH_PAGE_41  ((uint32_t)0x08002900) /* Base @ of Page 41, 256 bytes */
#define ADDR_FLASH_PAGE_42  ((uint32_t)0x08002A00) /* Base @ of Page 42, 256 bytes */
#define ADDR_FLASH_PAGE_43  ((uint32_t)0x08002B00) /* Base @ of Page 43, 256 bytes */
#define ADDR_FLASH_PAGE_44  ((uint32_t)0x08002C00) /* Base @ of Page 44, 256 bytes */
#define ADDR_FLASH_PAGE_45  ((uint32_t)0x08002D00) /* Base @ of Page 45, 256 bytes */
#define ADDR_FLASH_PAGE_46  ((uint32_t)0x08002E00) /* Base @ of Page 46, 256 bytes */
#define ADDR_FLASH_PAGE_47  ((uint32_t)0x08002F00) /* Base @ of Page 47, 256 bytes */
#define ADDR_FLASH_PAGE_48  ((uint32_t)0x08003000) /* Base @ of Page 48, 256 bytes */
#define ADDR_FLASH_PAGE_49  ((uint32_t)0x08003100) /* Base @ of Page 49, 256 bytes */
#define ADDR_FLASH_PAGE_50  ((uint32_t)0x08003200) /* Base @ of Page 50, 256 bytes */
#define ADDR_FLASH_PAGE_51  ((uint32_t)0x08003300) /* Base @ of Page 51, 256 bytes */
#define ADDR_FLASH_PAGE_52  ((uint32_t)0x08003400) /* Base @ of Page 52, 256 bytes */
#define ADDR_FLASH_PAGE_53  ((uint32_t)0x08003500) /* Base @ of Page 53, 256 bytes */
#define ADDR_FLASH_PAGE_54  ((uint32_t)0x08003600) /* Base @ of Page 54, 256 bytes */
#define ADDR_FLASH_PAGE_55  ((uint32_t)0x08003700) /* Base @ of Page 55, 256 bytes */
#define ADDR_FLASH_PAGE_56  ((uint32_t)0x08003800) /* Base @ of Page 56, 256 bytes */
#define ADDR_FLASH_PAGE_57  ((uint32_t)0x08003900) /* Base @ of Page 57, 256 bytes */
#define ADDR_FLASH_PAGE_58  ((uint32_t)0x08003A00) /* Base @ of Page 58, 256 bytes */
#define ADDR_FLASH_PAGE_59  ((uint32_t)0x08003B00) /* Base @ of Page 59, 256 bytes */
#define ADDR_FLASH_PAGE_60  ((uint32_t)0x08003C00) /* Base @ of Page 60, 256 bytes */
#define ADDR_FLASH_PAGE_61  ((uint32_t)0x08003D00) /* Base @ of Page 61, 256 bytes */
#define ADDR_FLASH_PAGE_62  ((uint32_t)0x08003E00) /* Base @ of Page 62, 256 bytes */
#define ADDR_FLASH_PAGE_63  ((uint32_t)0x08003F00) /* Base @ of Page 63, 256 bytes */
#define ADDR_FLASH_PAGE_64  ((uint32_t)0x08004000) /* Base @ of Page 64, 256 bytes */
#define ADDR_FLASH_PAGE_65  ((uint32_t)0x08004100) /* Base @ of Page 65, 256 bytes */
#define ADDR_FLASH_PAGE_66  ((uint32_t)0x08004200) /* Base @ of Page 66, 256 bytes */
#define ADDR_FLASH_PAGE_67  ((uint32_t)0x08004300) /* Base @ of Page 67, 256 bytes */
#define ADDR_FLASH_PAGE_68  ((uint32_t)0x08004400) /* Base @ of Page 68, 256 bytes */
#define ADDR_FLASH_PAGE_69  ((uint32_t)0x08004500) /* Base @ of Page 69, 256 bytes */
#define ADDR_FLASH_PAGE_70  ((uint32_t)0x08004600) /* Base @ of Page 70, 256 bytes */
#define ADDR_FLASH_PAGE_71  ((uint32_t)0x08004700) /* Base @ of Page 71, 256 bytes */
#define ADDR_FLASH_PAGE_72  ((uint32_t)0x08004800) /* Base @ of Page 72, 256 bytes */
#define ADDR_FLASH_PAGE_73  ((uint32_t)0x08004900) /* Base @ of Page 73, 256 bytes */
#define ADDR_FLASH_PAGE_74  ((uint32_t)0x08004A00) /* Base @ of Page 74, 256 bytes */
#define ADDR_FLASH_PAGE_75  ((uint32_t)0x08004B00) /* Base @ of Page 75, 256 bytes */
#define ADDR_FLASH_PAGE_76  ((uint32_t)0x08004C00) /* Base @ of Page 76, 256 bytes */
#define ADDR_FLASH_PAGE_77  ((uint32_t)0x08004D00) /* Base @ of Page 77, 256 bytes */
#define ADDR_FLASH_PAGE_78  ((uint32_t)0x08004E00) /* Base @ of Page 78, 256 bytes */
#define ADDR_FLASH_PAGE_79  ((uint32_t)0x08004F00) /* Base @ of Page 79, 256 bytes */
#define ADDR_FLASH_PAGE_80  ((uint32_t)0x08005000) /* Base @ of Page 80, 256 bytes */
#define ADDR_FLASH_PAGE_81  ((uint32_t)0x08005100) /* Base @ of Page 81, 256 bytes */
#define ADDR_FLASH_PAGE_82  ((uint32_t)0x08005200) /* Base @ of Page 82, 256 bytes */
#define ADDR_FLASH_PAGE_83  ((uint32_t)0x08005300) /* Base @ of Page 83, 256 bytes */
#define ADDR_FLASH_PAGE_84  ((uint32_t)0x08005400) /* Base @ of Page 84, 256 bytes */
#define ADDR_FLASH_PAGE_85  ((uint32_t)0x08005500) /* Base @ of Page 85, 256 bytes */
#define ADDR_FLASH_PAGE_86  ((uint32_t)0x08005600) /* Base @ of Page 86, 256 bytes */
#define ADDR_FLASH_PAGE_87  ((uint32_t)0x08005700) /* Base @ of Page 87, 256 bytes */
#define ADDR_FLASH_PAGE_88  ((uint32_t)0x08005800) /* Base @ of Page 88, 256 bytes */
#define ADDR_FLASH_PAGE_89  ((uint32_t)0x08005900) /* Base @ of Page 89, 256 bytes */
#define ADDR_FLASH_PAGE_90  ((uint32_t)0x08005A00) /* Base @ of Page 90, 256 bytes */
#define ADDR_FLASH_PAGE_91  ((uint32_t)0x08005B00) /* Base @ of Page 91, 256 bytes */
#define ADDR_FLASH_PAGE_92  ((uint32_t)0x08005C00) /* Base @ of Page 92, 256 bytes */
#define ADDR_FLASH_PAGE_93  ((uint32_t)0x08005D00) /* Base @ of Page 93, 256 bytes */
#define ADDR_FLASH_PAGE_94  ((uint32_t)0x08005E00) /* Base @ of Page 94, 256 bytes */
#define ADDR_FLASH_PAGE_95  ((uint32_t)0x08005F00) /* Base @ of Page 95, 256 bytes */
#define ADDR_FLASH_PAGE_96  ((uint32_t)0x08006000) /* Base @ of Page 96, 256 bytes */
#define ADDR_FLASH_PAGE_97  ((uint32_t)0x08006100) /* Base @ of Page 97, 256 bytes */
#define ADDR_FLASH_PAGE_98  ((uint32_t)0x08006200) /* Base @ of Page 98, 256 bytes */
#define ADDR_FLASH_PAGE_99  ((uint32_t)0x08006300) /* Base @ of Page 99, 256 bytes */
#define ADDR_FLASH_PAGE_100 ((uint32_t)0x08006400) /* Base @ of Page 100, 256 bytes */
#define ADDR_FLASH_PAGE_101 ((uint32_t)0x08006500) /* Base @ of Page 101, 256 bytes */
#define ADDR_FLASH_PAGE_102 ((uint32_t)0x08006600) /* Base @ of Page 102, 256 bytes */
#define ADDR_FLASH_PAGE_103 ((uint32_t)0x08006700) /* Base @ of Page 103, 256 bytes */
#define ADDR_FLASH_PAGE_104 ((uint32_t)0x08006800) /* Base @ of Page 104, 256 bytes */
#define ADDR_FLASH_PAGE_105 ((uint32_t)0x08006900) /* Base @ of Page 105, 256 bytes */
#define ADDR_FLASH_PAGE_106 ((uint32_t)0x08006A00) /* Base @ of Page 106, 256 bytes */
#define ADDR_FLASH_PAGE_107 ((uint32_t)0x08006B00) /* Base @ of Page 107, 256 bytes */
#define ADDR_FLASH_PAGE_108 ((uint32_t)0x08006C00) /* Base @ of Page 108, 256 bytes */
#define ADDR_FLASH_PAGE_109 ((uint32_t)0x08006D00) /* Base @ of Page 109, 256 bytes */
#define ADDR_FLASH_PAGE_110 ((uint32_t)0x08006E00) /* Base @ of Page 110, 256 bytes */
#define ADDR_FLASH_PAGE_111 ((uint32_t)0x08006F00) /* Base @ of Page 111, 256 bytes */
#define ADDR_FLASH_PAGE_112 ((uint32_t)0x08007000) /* Base @ of Page 112, 256 bytes */
#define ADDR_FLASH_PAGE_113 ((uint32_t)0x08007100) /* Base @ of Page 113, 256 bytes */
#define ADDR_FLASH_PAGE_114 ((uint32_t)0x08007200) /* Base @ of Page 114, 256 bytes */
#define ADDR_FLASH_PAGE_115 ((uint32_t)0x08007300) /* Base @ of Page 115, 256 bytes */
#define ADDR_FLASH_PAGE_116 ((uint32_t)0x08007400) /* Base @ of Page 116, 256 bytes */
#define ADDR_FLASH_PAGE_117 ((uint32_t)0x08007500) /* Base @ of Page 117, 256 bytes */
#define ADDR_FLASH_PAGE_118 ((uint32_t)0x08007600) /* Base @ of Page 118, 256 bytes */
#define ADDR_FLASH_PAGE_119 ((uint32_t)0x08007700) /* Base @ of Page 119, 256 bytes */
#define ADDR_FLASH_PAGE_120 ((uint32_t)0x08007800) /* Base @ of Page 120, 256 bytes */
#define ADDR_FLASH_PAGE_121 ((uint32_t)0x08007900) /* Base @ of Page 121, 256 bytes */
#define ADDR_FLASH_PAGE_122 ((uint32_t)0x08007A00) /* Base @ of Page 122, 256 bytes */
#define ADDR_FLASH_PAGE_123 ((uint32_t)0x08007B00) /* Base @ of Page 123, 256 bytes */
#define ADDR_FLASH_PAGE_124 ((uint32_t)0x08007C00) /* Base @ of Page 124, 256 bytes */
#define ADDR_FLASH_PAGE_125 ((uint32_t)0x08007D00) /* Base @ of Page 125, 256 bytes */
#define ADDR_FLASH_PAGE_126 ((uint32_t)0x08007E00) /* Base @ of Page 126, 256 bytes */
#define ADDR_FLASH_PAGE_127 ((uint32_t)0x08007F00) /* Base @ of Page 127, 256 bytes */

#define ADDR_FLASH_PAGE_128 ((uint32_t)0x08008000) /* Base @ of Page 128, 256 bytes */
#define ADDR_FLASH_PAGE_129 ((uint32_t)0x08008100) /* Base @ of Page 129, 256 bytes */
#define ADDR_FLASH_PAGE_130 ((uint32_t)0x08008200) /* Base @ of Page 130, 256 bytes */
#define ADDR_FLASH_PAGE_131 ((uint32_t)0x08008300) /* Base @ of Page 131, 256 bytes */
#define ADDR_FLASH_PAGE_132 ((uint32_t)0x08008400) /* Base @ of Page 132, 256 bytes */
#define ADDR_FLASH_PAGE_133 ((uint32_t)0x08008500) /* Base @ of Page 133, 256 bytes */
#define ADDR_FLASH_PAGE_134 ((uint32_t)0x08008600) /* Base @ of Page 134, 256 bytes */
#define ADDR_FLASH_PAGE_135 ((uint32_t)0x08008700) /* Base @ of Page 135, 256 bytes */
#define ADDR_FLASH_PAGE_136 ((uint32_t)0x08008800) /* Base @ of Page 136, 256 bytes */
#define ADDR_FLASH_PAGE_137 ((uint32_t)0x08008900) /* Base @ of Page 137, 256 bytes */
#define ADDR_FLASH_PAGE_138 ((uint32_t)0x08008A00) /* Base @ of Page 138, 256 bytes */
#define ADDR_FLASH_PAGE_139 ((uint32_t)0x08008B00) /* Base @ of Page 139, 256 bytes */
#define ADDR_FLASH_PAGE_140 ((uint32_t)0x08008C00) /* Base @ of Page 140, 256 bytes */
#define ADDR_FLASH_PAGE_141 ((uint32_t)0x08008D00) /* Base @ of Page 141, 256 bytes */
#define ADDR_FLASH_PAGE_142 ((uint32_t)0x08008E00) /* Base @ of Page 142, 256 bytes */
#define ADDR_FLASH_PAGE_143 ((uint32_t)0x08008F00) /* Base @ of Page 143, 256 bytes */
#define ADDR_FLASH_PAGE_144 ((uint32_t)0x08009000) /* Base @ of Page 144, 256 bytes */
#define ADDR_FLASH_PAGE_145 ((uint32_t)0x08009100) /* Base @ of Page 145, 256 bytes */
#define ADDR_FLASH_PAGE_146 ((uint32_t)0x08009200) /* Base @ of Page 146, 256 bytes */
#define ADDR_FLASH_PAGE_147 ((uint32_t)0x08009300) /* Base @ of Page 147, 256 bytes */
#define ADDR_FLASH_PAGE_148 ((uint32_t)0x08009400) /* Base @ of Page 148, 256 bytes */
#define ADDR_FLASH_PAGE_149 ((uint32_t)0x08009500) /* Base @ of Page 149, 256 bytes */
#define ADDR_FLASH_PAGE_150 ((uint32_t)0x08009600) /* Base @ of Page 150, 256 bytes */
#define ADDR_FLASH_PAGE_151 ((uint32_t)0x08009700) /* Base @ of Page 151, 256 bytes */
#define ADDR_FLASH_PAGE_152 ((uint32_t)0x08009800) /* Base @ of Page 152, 256 bytes */
#define ADDR_FLASH_PAGE_153 ((uint32_t)0x08009900) /* Base @ of Page 153, 256 bytes */
#define ADDR_FLASH_PAGE_154 ((uint32_t)0x08009A00) /* Base @ of Page 154, 256 bytes */
#define ADDR_FLASH_PAGE_155 ((uint32_t)0x08009B00) /* Base @ of Page 155, 256 bytes */
#define ADDR_FLASH_PAGE_156 ((uint32_t)0x08009C00) /* Base @ of Page 156, 256 bytes */
#define ADDR_FLASH_PAGE_157 ((uint32_t)0x08009D00) /* Base @ of Page 157, 256 bytes */
#define ADDR_FLASH_PAGE_158 ((uint32_t)0x08009E00) /* Base @ of Page 158, 256 bytes */
#define ADDR_FLASH_PAGE_159 ((uint32_t)0x08009F00) /* Base @ of Page 159, 256 bytes */
#define ADDR_FLASH_PAGE_160 ((uint32_t)0x0800A000) /* Base @ of Page 160, 256 bytes */
#define ADDR_FLASH_PAGE_161 ((uint32_t)0x0800A100) /* Base @ of Page 161, 256 bytes */
#define ADDR_FLASH_PAGE_162 ((uint32_t)0x0800A200) /* Base @ of Page 162, 256 bytes */
#define ADDR_FLASH_PAGE_163 ((uint32_t)0x0800A300) /* Base @ of Page 163, 256 bytes */
#define ADDR_FLASH_PAGE_164 ((uint32_t)0x0800A400) /* Base @ of Page 164, 256 bytes */
#define ADDR_FLASH_PAGE_165 ((uint32_t)0x0800A500) /* Base @ of Page 165, 256 bytes */
#define ADDR_FLASH_PAGE_166 ((uint32_t)0x0800A600) /* Base @ of Page 166, 256 bytes */
#define ADDR_FLASH_PAGE_167 ((uint32_t)0x0800A700) /* Base @ of Page 167, 256 bytes */
#define ADDR_FLASH_PAGE_168 ((uint32_t)0x0800A800) /* Base @ of Page 168, 256 bytes */
#define ADDR_FLASH_PAGE_169 ((uint32_t)0x0800A900) /* Base @ of Page 169, 256 bytes */
#define ADDR_FLASH_PAGE_170 ((uint32_t)0x0800AA00) /* Base @ of Page 170, 256 bytes */
#define ADDR_FLASH_PAGE_171 ((uint32_t)0x0800AB00) /* Base @ of Page 171, 256 bytes */
#define ADDR_FLASH_PAGE_172 ((uint32_t)0x0800AC00) /* Base @ of Page 172, 256 bytes */
#define ADDR_FLASH_PAGE_173 ((uint32_t)0x0800AD00) /* Base @ of Page 173, 256 bytes */
#define ADDR_FLASH_PAGE_174 ((uint32_t)0x0800AE00) /* Base @ of Page 174, 256 bytes */
#define ADDR_FLASH_PAGE_175 ((uint32_t)0x0800AF00) /* Base @ of Page 175, 256 bytes */
#define ADDR_FLASH_PAGE_176 ((uint32_t)0x0800B000) /* Base @ of Page 176, 256 bytes */
#define ADDR_FLASH_PAGE_177 ((uint32_t)0x0800B100) /* Base @ of Page 177, 256 bytes */
#define ADDR_FLASH_PAGE_178 ((uint32_t)0x0800B200) /* Base @ of Page 178, 256 bytes */
#define ADDR_FLASH_PAGE_179 ((uint32_t)0x0800B300) /* Base @ of Page 179, 256 bytes */
#define ADDR_FLASH_PAGE_180 ((uint32_t)0x0800B400) /* Base @ of Page 180, 256 bytes */
#define ADDR_FLASH_PAGE_181 ((uint32_t)0x0800B500) /* Base @ of Page 181, 256 bytes */
#define ADDR_FLASH_PAGE_182 ((uint32_t)0x0800B600) /* Base @ of Page 182, 256 bytes */
#define ADDR_FLASH_PAGE_183 ((uint32_t)0x0800B700) /* Base @ of Page 183, 256 bytes */
#define ADDR_FLASH_PAGE_184 ((uint32_t)0x0800B800) /* Base @ of Page 184, 256 bytes */
#define ADDR_FLASH_PAGE_185 ((uint32_t)0x0800B900) /* Base @ of Page 185, 256 bytes */
#define ADDR_FLASH_PAGE_186 ((uint32_t)0x0800BA00) /* Base @ of Page 186, 256 bytes */
#define ADDR_FLASH_PAGE_187 ((uint32_t)0x0800BB00) /* Base @ of Page 187, 256 bytes */
#define ADDR_FLASH_PAGE_188 ((uint32_t)0x0800BC00) /* Base @ of Page 188, 256 bytes */
#define ADDR_FLASH_PAGE_189 ((uint32_t)0x0800BD00) /* Base @ of Page 189, 256 bytes */
#define ADDR_FLASH_PAGE_190 ((uint32_t)0x0800BE00) /* Base @ of Page 190, 256 bytes */
#define ADDR_FLASH_PAGE_191 ((uint32_t)0x0800BF00) /* Base @ of Page 191, 256 bytes */
#define ADDR_FLASH_PAGE_192 ((uint32_t)0x0800C000) /* Base @ of Page 192, 256 bytes */
#define ADDR_FLASH_PAGE_193 ((uint32_t)0x0800C100) /* Base @ of Page 193, 256 bytes */
#define ADDR_FLASH_PAGE_194 ((uint32_t)0x0800C200) /* Base @ of Page 194, 256 bytes */
#define ADDR_FLASH_PAGE_195 ((uint32_t)0x0800C300) /* Base @ of Page 195, 256 bytes */
#define ADDR_FLASH_PAGE_196 ((uint32_t)0x0800C400) /* Base @ of Page 196, 256 bytes */
#define ADDR_FLASH_PAGE_197 ((uint32_t)0x0800C500) /* Base @ of Page 197, 256 bytes */
#define ADDR_FLASH_PAGE_198 ((uint32_t)0x0800C600) /* Base @ of Page 198, 256 bytes */
#define ADDR_FLASH_PAGE_199 ((uint32_t)0x0800C700) /* Base @ of Page 199, 256 bytes */
#define ADDR_FLASH_PAGE_200 ((uint32_t)0x0800C800) /* Base @ of Page 200, 256 bytes */
#define ADDR_FLASH_PAGE_201 ((uint32_t)0x0800C900) /* Base @ of Page 201, 256 bytes */
#define ADDR_FLASH_PAGE_202 ((uint32_t)0x0800CA00) /* Base @ of Page 202, 256 bytes */
#define ADDR_FLASH_PAGE_203 ((uint32_t)0x0800CB00) /* Base @ of Page 203, 256 bytes */
#define ADDR_FLASH_PAGE_204 ((uint32_t)0x0800CC00) /* Base @ of Page 204, 256 bytes */
#define ADDR_FLASH_PAGE_205 ((uint32_t)0x0800CD00) /* Base @ of Page 205, 256 bytes */
#define ADDR_FLASH_PAGE_206 ((uint32_t)0x0800CE00) /* Base @ of Page 206, 256 bytes */
#define ADDR_FLASH_PAGE_207 ((uint32_t)0x0800CF00) /* Base @ of Page 207, 256 bytes */
#define ADDR_FLASH_PAGE_208 ((uint32_t)0x0800D000) /* Base @ of Page 208, 256 bytes */
#define ADDR_FLASH_PAGE_209 ((uint32_t)0x0800D100) /* Base @ of Page 209, 256 bytes */
#define ADDR_FLASH_PAGE_210 ((uint32_t)0x0800D200) /* Base @ of Page 210, 256 bytes */
#define ADDR_FLASH_PAGE_211 ((uint32_t)0x0800D300) /* Base @ of Page 211, 256 bytes */
#define ADDR_FLASH_PAGE_212 ((uint32_t)0x0800D400) /* Base @ of Page 212, 256 bytes */
#define ADDR_FLASH_PAGE_213 ((uint32_t)0x0800D500) /* Base @ of Page 213, 256 bytes */
#define ADDR_FLASH_PAGE_214 ((uint32_t)0x0800D600) /* Base @ of Page 214, 256 bytes */
#define ADDR_FLASH_PAGE_215 ((uint32_t)0x0800D700) /* Base @ of Page 215, 256 bytes */
#define ADDR_FLASH_PAGE_216 ((uint32_t)0x0800D800) /* Base @ of Page 216, 256 bytes */
#define ADDR_FLASH_PAGE_217 ((uint32_t)0x0800D900) /* Base @ of Page 217, 256 bytes */
#define ADDR_FLASH_PAGE_218 ((uint32_t)0x0800DA00) /* Base @ of Page 218, 256 bytes */
#define ADDR_FLASH_PAGE_219 ((uint32_t)0x0800DB00) /* Base @ of Page 219, 256 bytes */
#define ADDR_FLASH_PAGE_220 ((uint32_t)0x0800DC00) /* Base @ of Page 220, 256 bytes */
#define ADDR_FLASH_PAGE_221 ((uint32_t)0x0800DD00) /* Base @ of Page 221, 256 bytes */
#define ADDR_FLASH_PAGE_222 ((uint32_t)0x0800DE00) /* Base @ of Page 222, 256 bytes */
#define ADDR_FLASH_PAGE_223 ((uint32_t)0x0800DF00) /* Base @ of Page 223, 256 bytes */
#define ADDR_FLASH_PAGE_224 ((uint32_t)0x0800E000) /* Base @ of Page 224, 256 bytes */
#define ADDR_FLASH_PAGE_225 ((uint32_t)0x0800E100) /* Base @ of Page 225, 256 bytes */
#define ADDR_FLASH_PAGE_226 ((uint32_t)0x0800E200) /* Base @ of Page 226, 256 bytes */
#define ADDR_FLASH_PAGE_227 ((uint32_t)0x0800E300) /* Base @ of Page 227, 256 bytes */
#define ADDR_FLASH_PAGE_228 ((uint32_t)0x0800E400) /* Base @ of Page 228, 256 bytes */
#define ADDR_FLASH_PAGE_229 ((uint32_t)0x0800E500) /* Base @ of Page 229, 256 bytes */
#define ADDR_FLASH_PAGE_230 ((uint32_t)0x0800E600) /* Base @ of Page 230, 256 bytes */
#define ADDR_FLASH_PAGE_231 ((uint32_t)0x0800E700) /* Base @ of Page 231, 256 bytes */
#define ADDR_FLASH_PAGE_232 ((uint32_t)0x0800E800) /* Base @ of Page 232, 256 bytes */
#define ADDR_FLASH_PAGE_233 ((uint32_t)0x0800E900) /* Base @ of Page 233, 256 bytes */
#define ADDR_FLASH_PAGE_234 ((uint32_t)0x0800EA00) /* Base @ of Page 234, 256 bytes */
#define ADDR_FLASH_PAGE_235 ((uint32_t)0x0800EB00) /* Base @ of Page 235, 256 bytes */
#define ADDR_FLASH_PAGE_236 ((uint32_t)0x0800EC00) /* Base @ of Page 236, 256 bytes */
#define ADDR_FLASH_PAGE_237 ((uint32_t)0x0800ED00) /* Base @ of Page 237, 256 bytes */
#define ADDR_FLASH_PAGE_238 ((uint32_t)0x0800EE00) /* Base @ of Page 238, 256 bytes */
#define ADDR_FLASH_PAGE_239 ((uint32_t)0x0800EF00) /* Base @ of Page 239, 256 bytes */
#define ADDR_FLASH_PAGE_240 ((uint32_t)0x0800F000) /* Base @ of Page 240, 256 bytes */
#define ADDR_FLASH_PAGE_241 ((uint32_t)0x0800F100) /* Base @ of Page 241, 256 bytes */
#define ADDR_FLASH_PAGE_242 ((uint32_t)0x0800F200) /* Base @ of Page 242, 256 bytes */
#define ADDR_FLASH_PAGE_243 ((uint32_t)0x0800F300) /* Base @ of Page 243, 256 bytes */
#define ADDR_FLASH_PAGE_244 ((uint32_t)0x0800F400) /* Base @ of Page 244, 256 bytes */
#define ADDR_FLASH_PAGE_245 ((uint32_t)0x0800F500) /* Base @ of Page 245, 256 bytes */
#define ADDR_FLASH_PAGE_246 ((uint32_t)0x0800F600) /* Base @ of Page 246, 256 bytes */
#define ADDR_FLASH_PAGE_247 ((uint32_t)0x0800F700) /* Base @ of Page 247, 256 bytes */
#define ADDR_FLASH_PAGE_248 ((uint32_t)0x0800F800) /* Base @ of Page 248, 256 bytes */
#define ADDR_FLASH_PAGE_249 ((uint32_t)0x0800F900) /* Base @ of Page 249, 256 bytes */
#define ADDR_FLASH_PAGE_250 ((uint32_t)0x0800FA00) /* Base @ of Page 250, 256 bytes */
#define ADDR_FLASH_PAGE_251 ((uint32_t)0x0800FB00) /* Base @ of Page 251, 256 bytes */
#define ADDR_FLASH_PAGE_252 ((uint32_t)0x0800FC00) /* Base @ of Page 252, 256 bytes */
#define ADDR_FLASH_PAGE_253 ((uint32_t)0x0800FD00) /* Base @ of Page 253, 256 bytes */
#define ADDR_FLASH_PAGE_254 ((uint32_t)0x0800FE00) /* Base @ of Page 254, 256 bytes */
#define ADDR_FLASH_PAGE_255 ((uint32_t)0x0800FF00) /* Base @ of Page 255, 256 bytes */

#elif defined (MCU_STM32L4)
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08002800) /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08003000) /* Base @ of Page 6, 2 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08003800) /* Base @ of Page 7, 2 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08004000) /* Base @ of Page 8, 2 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08004800) /* Base @ of Page 9, 2 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08005000) /* Base @ of Page 10, 2 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08005800) /* Base @ of Page 11, 2 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08006000) /* Base @ of Page 12, 2 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08006800) /* Base @ of Page 13, 2 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08007000) /* Base @ of Page 14, 2 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08007800) /* Base @ of Page 15, 2 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08008000) /* Base @ of Page 16, 2 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08008800) /* Base @ of Page 17, 2 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08009000) /* Base @ of Page 18, 2 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08009800) /* Base @ of Page 19, 2 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x0800A000) /* Base @ of Page 20, 2 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x0800A800) /* Base @ of Page 21, 2 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x0800B000) /* Base @ of Page 22, 2 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x0800B800) /* Base @ of Page 23, 2 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x0800C000) /* Base @ of Page 24, 2 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x0800C800) /* Base @ of Page 25, 2 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0800D000) /* Base @ of Page 26, 2 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0800D800) /* Base @ of Page 27, 2 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0800E000) /* Base @ of Page 28, 2 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0800E800) /* Base @ of Page 29, 2 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0800F000) /* Base @ of Page 30, 2 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0800F800) /* Base @ of Page 31, 2 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x08010000) /* Base @ of Page 32, 2 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x08010800) /* Base @ of Page 33, 2 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x08011000) /* Base @ of Page 34, 2 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x08011800) /* Base @ of Page 35, 2 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x08012000) /* Base @ of Page 36, 2 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x08012800) /* Base @ of Page 37, 2 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x08013000) /* Base @ of Page 38, 2 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x08013800) /* Base @ of Page 39, 2 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x08014000) /* Base @ of Page 40, 2 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x08014800) /* Base @ of Page 41, 2 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x08015000) /* Base @ of Page 42, 2 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x08015800) /* Base @ of Page 43, 2 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x08016000) /* Base @ of Page 44, 2 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x08016800) /* Base @ of Page 45, 2 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x08017000) /* Base @ of Page 46, 2 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x08017800) /* Base @ of Page 47, 2 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x08018000) /* Base @ of Page 48, 2 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x08018800) /* Base @ of Page 49, 2 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x08019000) /* Base @ of Page 50, 2 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x08019800) /* Base @ of Page 51, 2 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0801A000) /* Base @ of Page 52, 2 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0801A800) /* Base @ of Page 53, 2 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0801B000) /* Base @ of Page 54, 2 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0801B800) /* Base @ of Page 55, 2 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0801C000) /* Base @ of Page 56, 2 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0801C800) /* Base @ of Page 57, 2 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0801D000) /* Base @ of Page 58, 2 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0801D800) /* Base @ of Page 59, 2 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0801E000) /* Base @ of Page 60, 2 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0801E800) /* Base @ of Page 61, 2 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0801F000) /* Base @ of Page 62, 2 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0801F800) /* Base @ of Page 63, 2 Kbytes */
#endif

/*!< Define the address from where user application will be loaded.
   Note: this area is reserved for the IAP code                  */
#define FLASH_PAGE_STEP         			FLASH_PAGE_SIZE           /* Size of page : 2 Kbytes */

/*!< Notable Flash addresses */
#define USER_FLASH_END_ADDRESS        0x08020000

/*!< Define bitmap representing user flash area that could be write protected */
#define FLASH_PAGE_TO_BE_PROTECTED    (OB_WRP_PAGES20TO21 | OB_WRP_PAGES22TO23 | OB_WRP_PAGES24TO25 | \
                                       OB_WRP_PAGES26TO27 | OB_WRP_PAGES28TO29 | OB_WRP_PAGES30TO31 | \
                                       OB_WRP_PAGES32TO33)

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint8_t	Flash_PageSize_Buffer[];

/* Exported functions --------------------------------------------------------*/
void			cb_periph_flash_init(void);
uint8_t 	cb_periph_flash_write(uint32_t Addr, uint8_t* pBuffer, uint16_t Length);
uint8_t 	cb_periph_flash_write_string(uint32_t Addr, uint8_t* pData, uint16_t Length);
uint8_t 	cb_periph_flash_write_replace(uint32_t Addr, uint8_t* pData, uint16_t Length);
uint8_t*	cb_periph_flash_read(uint32_t Addr, uint8_t* pData, uint16_t Length);
uint32_t	cb_periph_flash_erase(uint32_t start, uint8_t num_page);

uint8_t 	cb_periph_flash_write_firmware(uint32_t Addr, uint8_t* pData, uint16_t Length);

#define		FLASH_WRITE(addr,data,len)						cb_periph_flash_write(addr,data,len)
#define		FLASH_WRITE_STR(addr,data,len)				    cb_periph_flash_write_string(addr,data,len)
#define		FLASH_WRITE_FIRMWARE(addr,data,len)		        cb_periph_flash_write_firmware(addr,data,len)
#define		FLASH_WRITE_REPLACE(addr,data,len)		        cb_periph_flash_write_replace(addr,data,len)
#define		FLASH_READ(addr,data,len)						cb_periph_flash_read(addr,data,len)
#define  	FLASH_ERASE(start_page,num_page)			    cb_periph_flash_erase(start_page,num_page)

#ifdef __cplusplus
}
#endif



#endif /* APP_FLASH_INTERNAL_H_ */
