/*
 * app_scheduler.h
 *
 *  Created on: Nov 27, 2019
 *      Author: VAIO
 */

#ifndef APP_SCHEDULER_H_
#define APP_SCHEDULER_H_
typedef struct {
// Pointer to the task (must be a 'void (void)' function)
	void ( * pTask)(void);
// Delay (ticks) until the function will (next) be run
	uint32_t Delay;
// Interval (ticks) between subsequent runs.
	uint32_t Period;
// Incremented (by scheduler) when task is due to execute
	uint8_t RunMe;
} sTask;

#define SCH_MAX_TASKS (50)
// The array of tasks
sTask SCH_tasks_G[SCH_MAX_TASKS];


void SCH_Update(void);
uint8_t SCH_Add_Task(void (* pFunction)(), const uint32_t DELAY, const uint32_t PERIOD);
void SCH_Dispatch_Tasks(void);
uint8_t SCH_Delete_Task(uint8_t TASK_INDEX);
FlagStatus isTaskDone(uint8_t taskIndex);


#endif /* APP_SCHEDULER_H_ */
