/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <app_init.h>
#include "main.h"
#include "app_fsm.h"
#include "app_uart.h"
#include "app_adc.h"
#include "app_scheduler.h"
#include "app_relay.h"
#include "app_Led_Display.h"
#include "app_sim3g.h"
#include "app_test.h"
#include "app_pcf8574.h"

//Vinh add begin

uint32_t lastTimeErr, lastReport;
uint8_t strtmpMain[] = "                                                               ";
powerSystem Main;

void node_setup(uint8_t position) {
	if (position == 0) {	//setup for Main node
		Main.currentTotal = 0;
		Main.status = SYSTEMNORMAL;
		Main.refTotalCurrent = 0;
		Main.valueRef = 0;
		Main.valueTotal = 0;
		Main.ambientTemp = 25;
		Main.internalTemp = 25;
		Led_Display_Clear_All();
	} else if (position <= No_CHANNEL) {
		position -= 1;
//		Main.nodes[position].nodeID = position;
//		Main.nodes[position].voltage = 220;
//		Main.nodes[position].frequency = 50;
		Main.nodes[position].powerFactor = 100;
		Main.nodes[position].power = 0;
		Main.nodes[position].limitEnergy = 0;
//		Main.nodes[position].nodeValue = 0;
		Main.nodes[position].current = 0;
//		Main.nodes[position].workingTime = 0;
		Main.nodes[position].energy = 0;
		Main.nodes[position].nodeStatus = NODENORMAL;

		//for testing, enable first channel
//		if(position == 0) Main.nodes[0].limitEnergy = 300000;
//		Led_Display_Color(position, GREEN);
//		Led_Display_Show();
//		HAL_Delay(1000);
	}
}

uint8_t node_check(uint8_t position) {
	if (position == 0) {
		if (Main.valueTotal > 1800 || Main.valueRef > 1800) Main.status = SYSTEMOVERCURRENT;
		else if ((Main.valueTotal - Main.valueRef) > 100) Main.status = NODEOVERTOTAL;
		else if (Main.valueRef - Main.valueTotal > 100) Main.status = NODEUNDERTOTAL;
		else if (Main.ambientTemp > 80) Main.status = SYSTEMOVERTEMP;
		else if (Main.internalTemp > 80) Main.status = SYSTEMOVERTEMP;
		else {
			for (uint8_t i = 0; i < No_CHANNEL; i++) {
				if (Main.nodes[i].nodeStatus == NODEOVERCURRENT) {
					Main.status = SYSTEMOVERCURRENT;
					return Main.status;
				}
			}
			Main.status = SYSTEMNORMAL;
		}
		return Main.status;
	} else if (position <= No_CHANNEL) {
		position -= 1;
		PowerNode currentNode = Main.nodes[position];
		uint8_t PCFValue = PCF_read(position);
//		PCFValue = 0;
		uint8_t currentStatus = currentNode.nodeStatus;
		if ((PCFValue & 0x02) > 0) {	//return NOFUSE
			if (position < 4) {
				Main.nodes[position].nodeStatus = NOFUSE;
			}
		}
		else if ((PCFValue & 0x01) > 0 && (Get_Relay_Status(position) == SET)) {	//relay not working MUST and is Working
			if (position < 4) {
				Main.nodes[position].nodeStatus = NORELAY;
			}
		}
		else if (currentNode.current > 300000) {	// nodeValue from 0 to 1860
			Main.nodes[position].nodeStatus = NODEOVERCURRENT;
		}
		else if (currentNode.limitEnergy > 0 && currentNode.energy >= currentNode.limitEnergy) {
			Main.nodes[position].nodeStatus = NODEOVERMONEY;
		}
		else if (currentNode.limitEnergy == 0) {
			Main.nodes[position].nodeStatus = NODENORMAL;
		}
		else if (currentNode.current >= 1000) {
			Main.nodes[position].nodeStatus = CHARGING;
		}
		else if (currentStatus == CHARGING) {
			if (currentNode.lastPower - currentNode.power > 20) {
				Main.nodes[position].nodeStatus = UNPLUG;
			} else {
				Main.nodes[position].nodeStatus = CHARGEFULL;
			}
			Main.nodes[position].lastPower = currentNode.power;
		}
		else {
			Main.nodes[position].nodeStatus = NODENORMAL;
		}
		return Main.nodes[position].nodeStatus;
	}
	return 0;
}

uint8_t node_update(uint8_t position) {	//update and return energy ???
	if (position == 0) {	//setup for Main node
//		Main.ambientTemp = AdcDmaBuffer[10] * 0.08056641;	//chua tinh, LM35 10mV/C => C = V/10mV = ADC*3.3/4096 *100 = adc*0.08056641
//		Main.internalTemp = ((1.43 - (AdcDmaBuffer[12] * 3.3 / 4096)) / 4.3) + 25 ;//AdcDmaBuffer[12];	//chua lay 1v8//Temp = (v25 - Vsense)/AVGSlope + 25 = (1.43 - Vsense)/4.3 + 25 = (1.43 - (ADC*3.3/4096) / 4.3 ) + 25
//		Main.valueRef =
	} else if (position <= No_CHANNEL) {
		position -= 1;
//		Main.nodes[position].current = Main.nodes[position].nodeValue * CONSTANT_TO_CURRENT;	//comment if using rms
//		Main.nodes[position].power = Main.nodes[position].current * Main.nodes[position].voltage * Main.nodes[position].powerFactor;//Main.nodes[position].nodeValue * CONSTANT_TO_POWER;
//		if(Main.nodes[position].powerFactor >98 ) Main.nodes[position].powerFactor = 100;
		Main.nodes[position].power = 220 * Main.nodes[position].current * Main.nodes[position].powerFactor / 100000;	//in mA	// /100
//		Main.nodes[position].energy = Main.nodes[position].workingTime * Main.nodes[position].power;
		if (Main.nodes[position].limitEnergy > 0) Main.nodes[position].energy += Main.nodes[position].power;
		else Main.nodes[position].energy = 0;
		return Main.nodes[position].nodeStatus;
	}
	return 0;
}

void node_update_task(void) {
	for (uint8_t i = 0; i <= No_CHANNEL; i++) {
		node_update(i);
	}
}

uint8_t channeli;
uint8_t mode = 0;
void power_setup() {
	//setup FSM state
	mode = FSM_Main_BEGIN;
	channeli = 0;
	//setup system and nodes
	for (uint8_t i = 0; i <= No_CHANNEL; i++) {
		node_setup(i);
	}
	Led_Display_Clear_All();
//setup read from PCF8574
//	uint8_t initData[1] = { 0xff };
//	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) PCF_WADR1, (uint8_t*) initData, 1, 10);
//	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) PCF_WADR2, (uint8_t*) initData, 1, 10);
//	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) PCF_WADR3, (uint8_t*) initData, 1, 10);
}

uint32_t lastTimeUpdate;
uint8_t displayFlag = 0;
void power_loop3() {
	switch (mode) {
	case FSM_Main_FINISH:
		mode = FSM_Main_BEGIN;
		break;
	case FSM_Main_BEGIN:
		mode = FSM_Main_CHECK_STATUS;
		channeli = 0;
		break;
	case FSM_Main_CHECK_STATUS:
		channeli++;		//check channel from 1 to 10
		node_check(channeli);
		if (channeli > No_CHANNEL) {
			channeli = 0;
			mode = FSM_Main_CHECK_SYSTEM;
		}
		break;
	case FSM_Main_CHECK_SYSTEM:
		node_check(0);
		mode = FSM_Main_CHECK_COMMAND;
		break;
	case FSM_Main_CHECK_COMMAND:
		if (Main.status == SYSTEMOVERCURRENT) {
//			report or do somethign
			//Led_Display_Set_All();
			UART3_SendToHost((uint8_t*) "System over current \r\n");
			mode = FSM_Main_ERR_HANDLE;
		} else if (Main.status == SYSTEMOVERTEMP) {
//			report or do some thing
//			for (uint8_t i = 0; i < No_CHANNEL; i++)
//				Reset_Relay(i);
//			//Led_Display_Set_All();
			UART3_SendToHost((uint8_t*) "System over temp \r\n");
			mode = FSM_Main_WAR_HANDLE;
		} else if (Main.status == NODEOVERTOTAL || Main.status == NODEUNDERTOTAL) {
//			//Led_Display_Set_All();
			UART3_SendToHost((uint8_t*) "System Node != total \r\n");
			mode = FSM_Main_WAR_HANDLE;
			//report difference
			Main.status = SYSTEMNORMAL;
		} else if (Main.status == SYSTEMNORMAL) {
			if (Main.nodes[channeli].nodeStatus == NODENORMAL) {
				//if command then set_node_energy(1000);
				if (Main.nodes[channeli].limitEnergy > Main.nodes[channeli].energy) {
//					if (Get_Relay_Status(channeli) == RESET)
						Set_Relay(channeli);
					Led_Display_Color(channeli, GREEN);
//					UART3_SendToHost((uint8_t*) "Het tien\r\n");
				} else {
					Led_Display_Color(lastReport, GREEN);
//					if (Get_Relay_Status(channeli) == SET)
						Reset_Relay(channeli);
				}
			} else if (Main.nodes[channeli].nodeStatus == CHARGING) {
				//if( receive command from server then close
				Led_Display_Color(channeli, YELLOW);
//				sprintf((char*) strtmpMain, "Charging %d\r\n", channeli);
//								UART3_SendToHost((uint8_t*) strtmpMain);
			} else if (Main.nodes[channeli].nodeStatus == CHARGEFULL) {
				if (HAL_GetTick() - lastReport > 5000) {
					lastReport = HAL_GetTick();
					//report chargefull
				}
				Led_Display_Color(channeli, BLINK_GREEN_FAST);
//				sprintf((char*) strtmpMain, "Full Charge %d\r\n", channeli);
//								UART3_SendToHost((uint8_t*) strtmpMain);
				//if first time then rport to server
			} else if (Main.nodes[channeli].nodeStatus == UNPLUG) {
				//set time out then turn off relay
				Led_Display_Color(channeli, BLINK_RED_FAST);
//				UART3_SendToHost((uint8_t*) "Rut phich cam \r\n");
//				sprintf((char*) strtmpMain, "Unplug %d\r\n", channeli);
//				UART3_SendToHost((uint8_t*) strtmpMain);

				//if(first time then report to server
			} else if (Main.nodes[channeli].nodeStatus == NOFUSE) {
				Led_Display_Color(channeli, BLINK_RED_FAST);
//				sprintf((char*) strtmpMain, "No Fuse %d\r\n", channeli);
//				UART3_SendToHost((uint8_t*) strtmpMain);
//				Reset_Relay(channeli);
				//if this is first time then report to server
			} else if (Main.nodes[channeli].nodeStatus == NORELAY) {
				Led_Display_Color(channeli, BLINK_RED_SLOW);
//				UART3_SendToHost((uint8_t*) "Relay khong hoat dong \r\n");
				//if( this is first time then report to server
//				Reset_Relay(channeli);
			} else if (Main.nodes[channeli].nodeStatus == NODEOVERMONEY) {
//				if (Get_Relay_Status(channeli) == SET)
				if (Get_Relay_Status(channeli) == SET) Reset_Relay(channeli);
				Main.nodes[channeli].limitEnergy = 0;
//				Main.nodes[channeli].nodeStatus = NODENORMAL;
				Led_Display_Color(channeli, BLINK_GREEN_FAST);
//				for(uint8_t i = 0; i< 4; i++){ //blink
//					//Led_Display_Color(channeli, RED);
//					HAL_Delay(300);
//					//Led_Display_Color(channeli, NONE);
//					HAL_Delay(500);
//				}

//				sprintf((char*) strtmpMain, "Het tien, tat relay so %d \r\n", channeli);
//				UART3_SendToHost((uint8_t*) strtmpMain);
//				UART3_SendToHost((uint8_t*) "Het tien, tat relay \r\n");
				//report to server
			} else if (Main.nodes[channeli].nodeStatus == NODEOVERTIME) {
				//report to server
				if (Get_Relay_Status(channeli) == 1) Reset_Relay(channeli);
				Main.nodes[channeli].nodeStatus = NODENORMAL;
//				//Led_Display_Color(channeli, RED);
			}
//			}

//			if(displayFlag > 0 ){
//				sprintf((char*) strtmpMain, "%d\t\t %d\t\t %d\t\t %d\t\t %d \r\n", (int) channeli, (int)Main.nodes[channeli].nodeStatus, (int)Main.nodes[channeli].power, (int)Main.nodes[channeli].energy, (int)Main.nodes[channeli].limitEnergy);
//				UART3_SendToHost((uint8_t*) strtmpMain);
//			}
			channeli++;
			if (channeli >= No_CHANNEL) {
				channeli = 0;
				mode = FSM_Main_FINISH;

//				Led_Display_Show();

//				sprintf((char*) strtmpMain, "i\t\t\t s\t\t\t I\t\t\t PF\t\t\t P\t\t\t E \r\n");
//				UART3_SendToHost((uint8_t*) strtmpMain);
//
//				for (uint8_t i = 0; i < 4; i++) {
//					sprintf((char*) strtmpMain, "%d\t\t\t %d\t\t\t %d\t\t\t\t %d\t\t\t %d\t\t\t %d  \r\n", (int) i, (int) Main.nodes[i].nodeStatus, (int) Main.nodes[i].current,
//							(int) Main.nodes[i].powerFactor, (int) Main.nodes[i].power, (int) Main.nodes[i].energy);
//					UART3_SendToHost((uint8_t*) strtmpMain);
//				}
//
//				UART3_SendToHost((uint8_t*) "\r\n");
			}
		}

		break;
	case FSM_Main_ERR_HANDLE:

		//report server and recheck after time
		if (HAL_GetTick() - lastTimeErr > 5000) {
			lastTimeErr = HAL_GetTick();
			//check and check
			for (uint8_t i = 0; i < No_CHANNEL; i++)
				Reset_Relay(i);

			mode = FSM_Main_FINISH;
		}
		break;
	case FSM_Main_WAR_HANDLE:
		//report to server
		mode = FSM_Main_FINISH;
		break;
	default:
		break;
	}
}

uint32_t lastTimeUpdateServer;
uint32_t lastTimeUpdateLedDisplay;
//Vinh add end

int main(void) {
	System_Initialization();
	UART3_SendToHost((uint8_t*) "Start program \r\n");
//	power_setup();
//	SCH_Add_Task(node_update_task, 0, 100);
//	Set_Input_PCF_Pins();
	while (1) {
		SCH_Dispatch_Tasks();
		Zero_Point_Detection();
		PowerConsumption_FSM();

		if (get_PowerConsumption_FSM_State() == REPORT_POWER_DATA) {
//			power_loop3();
			main_fsm();
			FSM_Process_Data_Received_From_Sim3g();
//			if (HAL_GetTick() - lastTimeUpdateServer > 10000 || 0) {				//hoac co bat cu su thay doi status nao quan trong
//				lastTimeUpdateServer = HAL_GetTick();
//				//update to server here
//			}

//			if(HAL_GetTick() - lastTimeUpdateLedDisplay > 200){
//				lastTimeUpdateLedDisplay = HAL_GetTick();
//				LED_Display_fsm() ;
//			}

		}
//		//Led_Display();
//		power_loop3();
	}
	return 0;
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	while (1) {
//		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
		HAL_Delay(100);
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
