/*
 * app_scheduler.c
 *
 *  Created on: Nov 27, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_scheduler.h"
#include "app_uart.h"

typedef struct {
	void ( * pTask)(void);
	uint32_t Delay;
	uint32_t Period;
	uint8_t RunMe;
	uint32_t TaskID;
} sTask;

// The array of tasks
sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t array_Of_Task_ID[SCH_MAX_TASKS];
static uint32_t newTaskID = 0;
uint32_t rearQueue = 0;

uint32_t count_SCH_Update = 0;
uint8_t testFlag = 0;
uint8_t strScheduler[] = "                                                                    ";


void SCH_Dispatch_Tasks_in_Timer(void);

uint32_t Get_New_Task_ID(void);

uint32_t Get_New_Task_ID(void){
	newTaskID++;
	if(newTaskID == NO_TASK_ID){
		newTaskID++;
	}
	return newTaskID;
}

void SCH_Update_with_dispatch(void){
	// Check if there is a task at this location
	count_SCH_Update ++;
	if (SCH_tasks_G[0].pTask) {
		if (SCH_tasks_G[0].Delay == 0) {
			SCH_tasks_G[0].RunMe = 1;
			SCH_Dispatch_Tasks_in_Timer();
			testFlag = 1;
		}
		SCH_tasks_G[0].Delay = SCH_tasks_G[0].Delay - 1;
	}
}

void SCH_Update(void){
	// Check if there is a task at this location
	count_SCH_Update ++;
	if (SCH_tasks_G[0].pTask && SCH_tasks_G[0].RunMe == 0) {
		if(SCH_tasks_G[0].Delay > 0){
			SCH_tasks_G[0].Delay = SCH_tasks_G[0].Delay - 1;
		}
		if (SCH_tasks_G[0].Delay == 0) {
			SCH_tasks_G[0].RunMe = 1;
		}
	}
}
uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	uint8_t newTaskIndex = 0;
	uint32_t sumDelay = 0;
	uint32_t newDelay = 0;

	for(newTaskIndex = 0; newTaskIndex < SCH_MAX_TASKS; newTaskIndex ++){
		sumDelay = sumDelay + SCH_tasks_G[newTaskIndex].Delay;
		if(sumDelay > DELAY){
			newDelay = DELAY - (sumDelay - SCH_tasks_G[newTaskIndex].Delay);
			SCH_tasks_G[newTaskIndex].Delay = sumDelay - DELAY;
			for(uint8_t i = SCH_MAX_TASKS - 1; i > newTaskIndex; i --){
//				if(SCH_tasks_G[i - 1].pTask != 0)
				{
					SCH_tasks_G[i].pTask = SCH_tasks_G[i - 1].pTask;
					SCH_tasks_G[i].Period = SCH_tasks_G[i - 1].Period;
					SCH_tasks_G[i].Delay = SCH_tasks_G[i - 1].Delay;
//					SCH_tasks_G[i].RunMe = SCH_tasks_G[i - 1].RunMe;
					SCH_tasks_G[i].TaskID = SCH_tasks_G[i - 1].TaskID;
				}
			}
			SCH_tasks_G[newTaskIndex].pTask = pFunction;
			SCH_tasks_G[newTaskIndex].Delay = newDelay;
			SCH_tasks_G[newTaskIndex].Period = PERIOD;
			if(SCH_tasks_G[newTaskIndex].Delay == 0){
				SCH_tasks_G[newTaskIndex].RunMe = 1;
			} else {
				SCH_tasks_G[newTaskIndex].RunMe = 0;
			}
			SCH_tasks_G[newTaskIndex].TaskID = Get_New_Task_ID();
			return SCH_tasks_G[newTaskIndex].TaskID;
		} else {
			if(SCH_tasks_G[newTaskIndex].pTask == 0x0000){
				SCH_tasks_G[newTaskIndex].pTask = pFunction;
				SCH_tasks_G[newTaskIndex].Delay = DELAY - sumDelay;
				SCH_tasks_G[newTaskIndex].Period = PERIOD;
				if(SCH_tasks_G[newTaskIndex].Delay == 0){
					SCH_tasks_G[newTaskIndex].RunMe = 1;
				} else {
					SCH_tasks_G[newTaskIndex].RunMe = 0;
				}
				SCH_tasks_G[newTaskIndex].TaskID = Get_New_Task_ID();
				return SCH_tasks_G[newTaskIndex].TaskID;
			}
		}
	}
	return SCH_tasks_G[newTaskIndex].TaskID;
}


uint8_t SCH_Delete_Task(uint32_t taskID){
	uint8_t Return_code  = 0;
	uint8_t taskIndex;
	uint8_t j;
	if(taskID != NO_TASK_ID){
		for(taskIndex = 0; taskIndex < SCH_MAX_TASKS; taskIndex ++){
			if(SCH_tasks_G[taskIndex].TaskID == taskID){
				Return_code = 1;
				if(taskIndex != 0 && taskIndex < SCH_MAX_TASKS - 1){
					if(SCH_tasks_G[taskIndex+1].pTask != 0x0000){
						SCH_tasks_G[taskIndex+1].Delay += SCH_tasks_G[taskIndex].Delay;
					}
				}

				for( j = taskIndex; j < SCH_MAX_TASKS - 1; j ++){
					SCH_tasks_G[j].pTask = SCH_tasks_G[j+1].pTask;
					SCH_tasks_G[j].Period = SCH_tasks_G[j+1].Period;
					SCH_tasks_G[j].Delay = SCH_tasks_G[j+1].Delay;
					SCH_tasks_G[j].RunMe = SCH_tasks_G[j+1].RunMe;
					SCH_tasks_G[j].TaskID = SCH_tasks_G[j+1].TaskID;
				}
				SCH_tasks_G[j].pTask = 0;
				SCH_tasks_G[j].Period = 0;
				SCH_tasks_G[j].Delay = 0;
				SCH_tasks_G[j].RunMe = 0;
				SCH_tasks_G[j].TaskID = 0;
				return Return_code;
			}
		}
	}
	return Return_code; // return status
}

void Show_Delay_Values(void){
	if(testFlag){
		testFlag = 0;
		UART3_SendToHost((uint8_t *)"start\r\n");
		sprintf((char*) strScheduler, "count=%d\r\n", (int) count_SCH_Update);
		UART3_SendToHost((uint8_t *)strScheduler);
		for (uint8_t i = 0; i < SCH_MAX_TASKS; i ++){
			if(SCH_tasks_G[i].pTask != 0){
				sprintf((char*) strScheduler, "i = %d, id = %d, d = %d\r\n", (int)i, (int) SCH_tasks_G[i].TaskID, (int) SCH_tasks_G[i].Delay);
				UART3_SendToHost((uint8_t *)strScheduler);
			}
		}
		UART3_SendToHost((uint8_t *)"end\r\n");
	}

}

void SCH_Dispatch_Tasks(void){
	if(SCH_tasks_G[0].RunMe > 0) {
//		sprintf((char*) strScheduler, "count=%d\r\n", (int) count_SCH_Update);
//		UART3_SendToHost((uint8_t *)strScheduler);
		(*SCH_tasks_G[0].pTask)(); // Run the task
		SCH_tasks_G[0].RunMe = 0; // Reset / reduce RunMe flag
		sTask temtask = SCH_tasks_G[0];
		SCH_Delete_Task(temtask.TaskID);
		if (temtask.Period != 0) {
			SCH_Add_Task(temtask.pTask, temtask.Period, temtask.Period);
		}
	}
}




void SCH_Dispatch_Tasks_in_Timer(void){
	while(SCH_tasks_G[0].Delay == 0){
		(*SCH_tasks_G[0].pTask)(); // Run the task
		SCH_tasks_G[0].RunMe = 0; // Reset / reduce RunMe flag
		sTask temtask = SCH_tasks_G[0];
		SCH_Delete_Task(temtask.TaskID);
		if (temtask.Period != 0) {
			SCH_Add_Task(temtask.pTask, temtask.Period, temtask.Period);
		}
	}
}




//
//uint8_t SCH_Add_TaskOn(void (* pFunction)(), const uint32_t DELAY, const uint32_t PERIOD) {
//    uint8_t Index = 0;
//    // First find a gap in the array (if there is one)
//    while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS)) {
//        Index++;
//    }
//    if (Index == SCH_MAX_TASKS) {// Have we reached the end of the list?
//        // Task list is full, Set the global error variable
//        return SCH_MAX_TASKS; // Also return an error code
//   }
//    // If we're here, there is a space in the task array
//    SCH_tasks_G[Index].pTask = pFunction;
//    SCH_tasks_G[Index].Delay = DELAY;
//    SCH_tasks_G[Index].Period = PERIOD;
//    SCH_tasks_G[Index].RunMe = 0;
//    return Index; // return position of task (to allow later deletion)
//}
//void SCH_Update_On(void) {
//    uint8_t Index;
//    // NOTE: calculations are in *TICKS* (not milliseconds)
//    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
//    // Check if there is a task at this location
//        if (SCH_tasks_G[Index].pTask) {
//        	SCH_tasks_G[Index].Delay -= 1;
//        	if (SCH_tasks_G[Index].Delay == 0) {
//			// The task is due to run
//				SCH_tasks_G[Index].RunMe = 1; // Inc. the 'RunMe' flag
//				if (SCH_tasks_G[Index].Period) {
//					// Schedule periodic tasks to run again
//					SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period;
//				}
//			}
//
//        }
//    }
//}
//void SCH_Dispatch_Tasks_On(void) {
//    uint8_t Index;
//    // Dispatches (runs) the next task (if one is ready)
//    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
//        if (SCH_tasks_G[Index].RunMe > 0) {
//           (*SCH_tasks_G[Index].pTask)(); // Run the task
//           SCH_tasks_G[Index].RunMe = 0; // Reset / reduce RunMe flag
//          // Periodic tasks will automatically run again
//          // - if this is a 'one shot' task, remove it from the array
//          if (SCH_tasks_G[Index].Period == 0) SCH_Delete_Task(Index);
//      }
//   }
////   SCH_Report_Status(); // Report system status
//}
//
//uint8_t SCH_Delete_Task_On(uint8_t taskIndex) {
//	uint8_t Return_code  = 0;
//	if(taskIndex < SCH_MAX_TASKS){
//		if (SCH_tasks_G[taskIndex].pTask == 0) {
//			Return_code = 0;
//		} else {
//			Return_code = 1;
//		}
//		SCH_tasks_G[taskIndex].pTask = 0x0000;
//		SCH_tasks_G[taskIndex].Delay = 0;
//		SCH_tasks_G[taskIndex].Period = 0;
//		SCH_tasks_G[taskIndex].RunMe = 0;
//	}
//	  return Return_code; // return status
//}
//
//
////FlagStatus isTaskDone(uint8_t taskIndex){
////	return (SCH_tasks_G[taskIndex].pTask == 0x0000);
////}






