/**
 * @file  app_scheduler.h
 * @author huunguyengnuyentran
 * @brief Header file for app_scheduler.c. This file contains declare some interface for external process can be scheduled.
 */

#ifndef APP_SCHEDULER_H_
#define APP_SCHEDULER_H_

#include <Peripheral/app_uart.h>
#include "main.h"


#define SCH_MAX_TASKS 			40
#define	NO_TASK_ID				0
// The array of tasks


void Scheduler_Init(void);
void Insert_Task_ID_To_Queue(uint8_t taskID);
uint8_t Get_Task_ID_From_Queue(void);
void Show_Delay_Values(void);
uint8_t Find_Available_Task_Index(void);
void SCH_Update(void);
uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD);
void SCH_Dispatch_Tasks(void);
uint8_t SCH_Delete_Task(uint32_t TASK_ID);
FlagStatus isTaskDone(uint8_t taskIndex);

void Set_Got_Message();

#endif /* APP_SCHEDULER_H_ */
