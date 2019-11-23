/*
 * app_scheduler.c
 *
 *  Created on: Nov 27, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_scheduler.h"

void SCH_Update(void) {
    uint8_t Index;
    // NOTE: calculations are in *TICKS* (not milliseconds)
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
    // Check if there is a task at this location
        if (SCH_tasks_G[Index].pTask) {
			if (SCH_tasks_G[Index].Delay == 0) {
			// The task is due to run
				SCH_tasks_G[Index].RunMe += 1; // Inc. the 'RunMe' flag
				if (SCH_tasks_G[Index].Period) {
					// Schedule periodic tasks to run again
					SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period;
				}
			}
			SCH_tasks_G[Index].Delay -= 1;
        }
    }
}


uint8_t SCH_Add_Task(void (* pFunction)(), const uint32_t DELAY, const uint32_t PERIOD) {
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

void SCH_Dispatch_Tasks(void) {
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

uint8_t SCH_Delete_Task(uint8_t TASK_INDEX) {
	uint8_t Return_code;
     if (SCH_tasks_G[TASK_INDEX].pTask == 0) {
    	 Return_code = 0;
     } else {
         Return_code = 1;
     }
      SCH_tasks_G[TASK_INDEX].pTask = 0x0000;
      SCH_tasks_G[TASK_INDEX].Delay = 0;
      SCH_tasks_G[TASK_INDEX].Period = 0;
      SCH_tasks_G[TASK_INDEX].RunMe = 0;
      return Return_code; // return status
}

