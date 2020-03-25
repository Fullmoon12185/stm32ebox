/*
 * app_scheduler.c
 *
 *  Created on: Nov 27, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_scheduler.h"
#include "app_uart.h"

uint32_t count_SCH_Update = 0;
uint8_t strScheduler[] = "                                ";
// The array of tasks
sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t array_Of_Task_ID[SCH_MAX_TASKS];
uint8_t headQueue = 0;
uint8_t rearQueue = 0;


void Scheduler_Init(void){
	for(uint8_t i = 0; i < SCH_MAX_TASKS; i ++){
		array_Of_Task_ID[i] = i + 1;
	}
}

uint8_t Was_TaskID_Deleted(uint8_t task_ID){
	for(uint8_t i = 0; i < SCH_MAX_TASKS; i ++){
		if(task_ID == array_Of_Task_ID[i]){
			return 0;
		}
	}
	return 1;
}
void Insert_Task_ID_To_Queue(uint8_t taskID){
	if(rearQueue != headQueue){
		array_Of_Task_ID[rearQueue] = taskID;
		rearQueue = (rearQueue + 1) % SCH_MAX_TASKS;
	}

}
uint8_t Get_Task_ID_From_Queue(void){
	uint8_t tempTaskID;
	tempTaskID = array_Of_Task_ID[headQueue];
	array_Of_Task_ID[headQueue] = 0;
	headQueue = (headQueue + 1)%SCH_MAX_TASKS;
	return tempTaskID;
}

void SCH_Update(void){
	// NOTE: calculations are in *TICKS* (not milliseconds)
	// Check if there is a task at this location
	count_SCH_Update ++;
	if (SCH_tasks_G[0].pTask && SCH_tasks_G[0].RunMe == 0) {
		SCH_tasks_G[0].Delay -= 1;
		if (SCH_tasks_G[0].Delay == 0) {
		// The task is due to run
			SCH_tasks_G[0].RunMe = 1; // Inc. the 'RunMe' flag
//			if (SCH_tasks_G[0].Period) {
//				// Schedule periodic tasks to run again
//				SCH_tasks_G[0].Delay = SCH_tasks_G[0].Period;
//			}
		}
	}
}
void SCH_Update_On(void) {
    uint8_t Index;
    // NOTE: calculations are in *TICKS* (not milliseconds)
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
    // Check if there is a task at this location
        if (SCH_tasks_G[Index].pTask) {
			if (SCH_tasks_G[Index].Delay == 0) {
			// The task is due to run
				SCH_tasks_G[Index].RunMe += 1; // Inc. the 'RunMe' flag
			}
			SCH_tasks_G[Index].Delay -= 1;
        }
    }
}
uint8_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	uint8_t newTaskIndex = 0;
	uint32_t sumDelay = 0;
	uint32_t newDelay = 0;
//	if(DELAY != 0){
//		DELAY = DELAY - 1;
//	}
	if(SCH_tasks_G[SCH_MAX_TASKS - 1].pTask == 0){
		for(newTaskIndex = 0; newTaskIndex < SCH_MAX_TASKS - 1; newTaskIndex ++){
			if(SCH_tasks_G[newTaskIndex].pTask != 0){
				sumDelay = sumDelay + SCH_tasks_G[newTaskIndex].Delay;
				if(sumDelay > DELAY){
					newDelay = DELAY - (sumDelay - SCH_tasks_G[newTaskIndex].Delay);
					SCH_tasks_G[newTaskIndex].Delay = sumDelay - DELAY;

					for(uint8_t i = SCH_MAX_TASKS - 1; i > newTaskIndex; i --){
						if(SCH_tasks_G[i - 1].pTask != 0){
							SCH_tasks_G[i].pTask = SCH_tasks_G[i - 1].pTask;
							SCH_tasks_G[i].Period = SCH_tasks_G[i - 1].Period;
							SCH_tasks_G[i].Delay = SCH_tasks_G[i - 1].Delay;
							SCH_tasks_G[i].RunMe = SCH_tasks_G[i - 1].RunMe;
							SCH_tasks_G[i].TaskID = SCH_tasks_G[i - 1].TaskID;
						}

					}
					// If we're here, there is a space in the task array
					SCH_tasks_G[newTaskIndex].pTask = pFunction;
					SCH_tasks_G[newTaskIndex].Delay = newDelay;
					SCH_tasks_G[newTaskIndex].Period = PERIOD;
					if(SCH_tasks_G[newTaskIndex].Delay == 0){
						SCH_tasks_G[newTaskIndex].RunMe = 1;
					} else {
						SCH_tasks_G[newTaskIndex].RunMe = 0;
					}
					SCH_tasks_G[newTaskIndex].TaskID = Find_Available_Task_Index();
					break;
				}
			} else {
				// If we're here, there is a space in the task array
				SCH_tasks_G[newTaskIndex].pTask = pFunction;
				SCH_tasks_G[newTaskIndex].Delay = DELAY - sumDelay;
				SCH_tasks_G[newTaskIndex].Period = PERIOD;
				if(SCH_tasks_G[newTaskIndex].Delay == 0){
					SCH_tasks_G[newTaskIndex].RunMe = 1;
				} else {
					SCH_tasks_G[newTaskIndex].RunMe = 0;
				}
				SCH_tasks_G[newTaskIndex].TaskID = Find_Available_Task_Index();
				break;
			}
		}
		return SCH_tasks_G[newTaskIndex].TaskID;
	} else {
		return 0;
	}

}

uint8_t SCH_Add_Task_On(void (* pFunction)(), const uint32_t DELAY, const uint32_t PERIOD) {
    uint8_t Index = 0;
    // First find a gap in the array (if there is one)
    while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS)) {
        Index++;
    }
    if (Index == SCH_MAX_TASKS) {// Have we reached the end of the list?
        // Task list is full, Set the global error variable
        return SCH_MAX_TASKS; // Also return an error code
   }
    // If we're here, there is a space in the task array
    SCH_tasks_G[Index].pTask = pFunction;
    SCH_tasks_G[Index].Delay = DELAY;
    SCH_tasks_G[Index].Period = PERIOD;
    SCH_tasks_G[Index].RunMe = 0;
    return Index; // return position of task (to allow later deletion)
}


uint8_t Find_Available_Task_Index(void){
	uint8_t taskID;
	while(1){
		taskID = Get_Task_ID_From_Queue();
		for(uint8_t i = 0; i < SCH_MAX_TASKS; i++){
			if(SCH_tasks_G[i].pTask != 0){
				if(taskID == SCH_tasks_G[i].TaskID){
					break;
				}
			} else {
				return taskID;
			}
		}
		return taskID;
	}
}


uint8_t SCH_Delete_Task(uint8_t taskID){
	uint8_t Return_code  = 0;
	uint8_t i;
	uint8_t firstMet = 0;
	if(taskID != NO_TASK_ID){
		if(Was_TaskID_Deleted(taskID)){
			Insert_Task_ID_To_Queue(taskID);
			for(i = 0; i < SCH_MAX_TASKS; i ++){
				if(SCH_tasks_G[i].pTask != 0x0000) {
					if(SCH_tasks_G[i].TaskID == taskID){
						SCH_tasks_G[i].pTask = 0;
						SCH_tasks_G[i].Period = 0;
						if(i != 0 && i < SCH_MAX_TASKS - 1){
							if(SCH_tasks_G[i+1].pTask != 0x0000){
								SCH_tasks_G[i+1].Delay += SCH_tasks_G[i].Delay;
							}
						}
						SCH_tasks_G[i].Delay = 0;
						SCH_tasks_G[i].RunMe = 0;
						SCH_tasks_G[i].TaskID = 0;
						firstMet = 1;
					} else {
						if(firstMet == 1){
							SCH_tasks_G[i-1].pTask = SCH_tasks_G[i].pTask;
							SCH_tasks_G[i-1].Period = SCH_tasks_G[i].Period;
							SCH_tasks_G[i-1].Delay = SCH_tasks_G[i].Delay;
							SCH_tasks_G[i-1].RunMe = SCH_tasks_G[i].RunMe;
							SCH_tasks_G[i-1].TaskID = SCH_tasks_G[i].TaskID;
						}
					}
				} else {
					if(firstMet == 1){
						SCH_tasks_G[i - 1].pTask = 0;
						SCH_tasks_G[i - 1].Period = 0;
						SCH_tasks_G[i - 1].Delay = 0;
						SCH_tasks_G[i - 1].RunMe = 0;
						SCH_tasks_G[i - 1].TaskID = 0;
					}
					break;
				}
			}
		}


	}
	 return Return_code; // return status
}
uint8_t SCH_Delete_Task_On(uint8_t taskIndex) {
	uint8_t Return_code  = 0;
	if(taskIndex < SCH_MAX_TASKS){
		if (SCH_tasks_G[taskIndex].pTask == 0) {
			Return_code = 0;
		} else {
			Return_code = 1;
		}
		SCH_tasks_G[taskIndex].pTask = 0x0000;
		SCH_tasks_G[taskIndex].Delay = 0;
		SCH_tasks_G[taskIndex].Period = 0;
		SCH_tasks_G[taskIndex].RunMe = 0;
	}
	  return Return_code; // return status
}

void SCH_Dispatch_Tasks(void){
	// Dispatches (runs) the next task (if one is ready)
	if (SCH_tasks_G[0].RunMe > 0) {
//		sprintf((char*) strScheduler, "count_SCH_Update =  %d\r\n", (int) count_SCH_Update);
//		UART3_SendToHost((uint8_t *)strScheduler);
		(*SCH_tasks_G[0].pTask)(); // Run the task
		SCH_tasks_G[0].RunMe = 0; // Reset / reduce RunMe flag
		// Periodic tasks will automatically run again
		// - if this is a 'one shot' task, remove it from the array
		sTask temtask = SCH_tasks_G[0];
		SCH_Delete_Task(temtask.TaskID);
		if (temtask.Period != 0) {
			SCH_Add_Task(temtask.pTask, temtask.Period, temtask.Period);
		}
	}
}
void SCH_Dispatch_Tasks_On(void) {
    uint8_t Index;
    // Dispatches (runs) the next task (if one is ready)
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        if (SCH_tasks_G[Index].RunMe > 0) {
           (*SCH_tasks_G[Index].pTask)(); // Run the task
           SCH_tasks_G[Index].RunMe -= 1; // Reset / reduce RunMe flag
          // Periodic tasks will automatically run again
          // - if this is a 'one shot' task, remove it from the array
          if (SCH_tasks_G[Index].Period == 0) SCH_Delete_Task(Index);
      }
   }
//   SCH_Report_Status(); // Report system status
}

FlagStatus isTaskDone(uint8_t taskIndex){
	return (SCH_tasks_G[taskIndex].pTask == 0x0000);
}






