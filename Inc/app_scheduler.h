/*
 * app_scheduler.h
 *
 *  Created on: Nov 27, 2019
 *      Author: VAIO
 */

#ifndef APP_SCHEDULER_H_
#define APP_SCHEDULER_H_



#define SCH_MAX_TASKS 			40
#define	NO_TASK_ID				0









void Scheduler_Init(void);
void Insert_Task_ID_To_Queue(uint8_t taskID);
uint8_t Get_Task_ID_From_Queue(void);
void Show_Delay_Values(void);
uint8_t Find_Available_Task_Index(void);
void SCH_Update(void);
uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD);
void SCH_Dispatch_Tasks(void);
uint8_t SCH_Delete_Task(uint32_t TASK_ID);
uint8_t isTaskDone(uint8_t taskIndex);


#endif /* APP_SCHEDULER_H_ */
