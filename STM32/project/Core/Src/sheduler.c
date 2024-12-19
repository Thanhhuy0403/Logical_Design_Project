/*
 * sheduler.c
 *
 *  Created on: Nov 12, 2024
 *      Author: phan thanh huy
 */

#include "sheduler.h"

sTasks SCH_tasks_G[SCH_MAX_TASKS];
uint8_t current_index_task = 0;

void SCH_Init(void){
	current_index_task = 0;
}

void SCH_Add_Task(void (*pFunction)(), uint32_t Delay, uint32_t Period){
	Delay /= 10;
	Period /= 10;
	if(current_index_task < SCH_MAX_TASKS){
		SCH_tasks_G[current_index_task].pTask = pFunction;
		SCH_tasks_G[current_index_task].Delay = Delay;
		SCH_tasks_G[current_index_task].Period = Period;
		SCH_tasks_G[current_index_task].RunMe = 0;
		SCH_tasks_G[current_index_task].TaskID = current_index_task;

		current_index_task++;
	}
}

void SCH_Update(void){
	for(int i = 0; i < current_index_task; i++){
		if(SCH_tasks_G[i].Delay > 0){
			SCH_tasks_G[i].Delay--;
		}else{
			SCH_tasks_G[i].Delay = SCH_tasks_G[i].Period;
			SCH_tasks_G[i].RunMe += 1;
		}
	}
}

void SCH_Dispatch_Tasks(void){
	for(int i = 0; i < current_index_task; i++){
		if(SCH_tasks_G[i].RunMe){
			SCH_tasks_G[i].RunMe = 0;
			(*SCH_tasks_G[i].pTask)();
		}
	}
}

void SCH_Delete(uint32_t ID) {
    if (ID >= current_index_task) return;

    // Reset the task at the given ID
    SCH_tasks_G[ID].Delay = 0;
    SCH_tasks_G[ID].Period = 0;
    SCH_tasks_G[ID].RunMe = 0;

    // Shift all tasks after the deleted task one position up
    for (int j = ID; j < current_index_task - 1; j++) {
        SCH_tasks_G[j] = SCH_tasks_G[j + 1];
    }

    // Decrement the task count after shifting
    current_index_task--;
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
//	if(htim->Instance == TIM3)
//	SCH_Update();
//}
