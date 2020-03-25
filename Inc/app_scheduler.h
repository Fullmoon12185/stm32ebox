/*
 * app_scheduler.h
 *
 *  Created on: Nov 27, 2019
 *      Author: VAIO
 */

#ifndef APP_SCHEDULER_H_
#define APP_SCHEDULER_H_

#define	NO_TASK_ID				0

#define SCH_MAX_TASKS 			10




typedef struct {
// Pointer to the task (must be a 'void (void)' function)
	void ( * pTask)(void);
// Delay (ticks) until the function will (next) be run
	uint32_t Delay;
// Interval (ticks) between subsequent runs.
	uint32_t Period;
// Incremented (by scheduler) when task is due to execute
	uint8_t RunMe;
	uint8_t TaskID;
} sTask;






void Scheduler_Init(void);
void Insert_Task_ID_To_Queue(uint8_t taskID);
uint8_t Get_Task_ID_From_Queue(void);

uint8_t Find_Available_Task_Index(void);
void SCH_Update(void);
uint8_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD);
void SCH_Dispatch_Tasks(void);
uint8_t SCH_Delete_Task(uint8_t TASK_ID);
FlagStatus isTaskDone(uint8_t taskIndex);


#endif /* APP_SCHEDULER_H_ */
