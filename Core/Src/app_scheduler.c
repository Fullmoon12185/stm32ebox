//#include "main.h"
//#include "app_scheduler.h"
//#include "app_uart.h"
//
//
//typedef struct {
//	void ( * pTask)(void);
//	uint32_t Delay;
//	uint32_t Period;
//	uint8_t RunMe;
//	uint32_t TaskID;
//} sTask;
//
//struct sTask_Node{
//	sTask Task;
//	struct sTask_Node *next;
//} ;
//
//
//
//// The array of tasks
//sTask SCH_tasks_G[SCH_MAX_TASKS];
//static uint32_t newTaskID = 0;
//uint32_t rearQueue = 0;
//
//uint32_t count_SCH_Update = 0;
//
//struct sTask_Node *head = NULL;
//
//
//uint32_t Get_New_Task_ID(void){
//	newTaskID++;
//	if(newTaskID == NO_TASK_ID){
//		newTaskID++;
//	}
//	return newTaskID;
//}
//
//
//void SCH_Update(void){
//	if(head->Task.pTask&&head->Task.RunMe==0){
//		if(head->Task.Delay>0){
//			head->Task.Delay-=1;
//		}
//		if(head->Task.Delay==0){
//			head->Task.RunMe = 1;
//		}
//	}
//}
//uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD){
//	uint8_t newTaskIndex = 0;
//	uint32_t sumDelay = 0;
//	uint32_t newDelay = 0;
//	struct sTask_Node *prevNode;
//	struct sTask_Node *currNode;
//	struct sTask_Node *newNode = (struct sTask_Node*)malloc(sizeof(struct sTask_Node));
//	newNode->Task.pTask = pFunction;
//	newNode->Task.Delay = DELAY;
//	newNode->Task.Period = PERIOD;
//	newNode->Task.RunMe = 0;
//	newNode->Task.TaskID = Get_New_Task_ID();
//	newNode->next = NULL;
//	if(head == NULL){
//		head = newNode;
//	}
//	else{
//		prevNode = head;
//		currNode = head;
//		while(currNode!=NULL){
//			sumDelay+= currNode->Task.Delay;
//			if(sumDelay>DELAY){
//				newDelay = DELAY - (sumDelay - currNode->Task.Delay);
//				//Update Delay
//				newNode->Task.Delay = newDelay;
//				//Insert newNode into LinkedList
//				prevNode->next = newNode;
//				newNode->next = currNode;
//				return newNode->Task.TaskID;
//			}
//			//Move to new Node
//			prevNode = currNode;
//			currNode = currNode->next;
//		}
//		newDelay = DELAY - sumDelay;
//		newNode->Task.Delay = newDelay;
//		prevNode->next = newNode;
//		return newNode->Task.TaskID;
//	}
//}
//
//
//uint8_t SCH_Delete_Task(uint32_t taskID){
//	uint8_t Return_code  = 0;
//	struct sTask_Node *prevNode = head;
//	struct sTask_Node *currNode = head;
//	while(currNode!=NULL){
//		if(currNode->Task.TaskID==taskID){
//			Return_code = 1;
//			// prevNode = currNode = head
//			if(prevNode==currNode){
//				head = head->next;
//				free(currNode);
//			}
//			else{
//				prevNode-> next = currNode ->next;
//				free(currNode);
//			}
//
//			return Return_code;
//		}
//		prevNode = currNode;
//		currNode = currNode->next;
//	}
//	return Return_code;
//}
//
//
//void SCH_Dispatch_Tasks(void){
//	sTask temp_Task;
//	if(head->Task.RunMe>0 && head != NULL){
//		(* head->Task.pTask)();	//Run the task
//		temp_Task = head->Task;
//		SCH_Delete_Task(temp_Task.TaskID);
//		if(temp_Task.Period!=0){
//			SCH_Add_Task(temp_Task.pTask, temp_Task.Delay, temp_Task.Period);
//		}
//	}
//}
//
//
//
//
//
//
//
/*
 * @file app_scheduler.c
 * @author thodo
 */
#include <Peripheral/app_uart.h>
#include "main.h"
#include "app_scheduler.h"

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

uint32_t Get_New_Task_ID(void){
	newTaskID++;
	if(newTaskID == NO_TASK_ID){
		newTaskID++;
	}
	return newTaskID;
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


