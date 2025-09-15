#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

void TaskA(void *pvParameters);
void TaskB(void *pvParameters);
void Timer(void *pvParameters);

int counter = 5;
SemaphoreHandle_t semaphore;
static TimerHandle_t auto_reload_timer = NULL;

int main(int argc, char **argv){
	(void) argc;
	(void) argv;
	
	semaphore = xSemaphoreCreateBinary();
	
	xTaskCreate(TaskA, "Task A", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY +1, NULL);
	xTaskCreate(TaskB, "Task B", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY +1, NULL);
	xTaskCreate(Timer, "Timer",  configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY +3, NULL);
	
	vTaskStartScheduler();
    for( ; ; );
}

void TaskA (void *pvParameters){
	(void) pvParameters;
	char message [100];
	
	const TickType_t xTaskPeriod = pdMS_TO_TICKS(500);
	TickType_t xPreviousWakeTime = xTaskGetTickCount();
	
	while (1){
		vTaskDelayUntil(&xPreviousWakeTime, xTaskPeriod);
		
		int i;
		for (i=0; i<5; i++){
			counter++;
		}
		
		xSemaphoreGive(semaphore);
	}
}

void TaskB (void *pvParameters){
	(void) pvParameters;
	char message [100];
	
	const TickType_t xTaskPeriod = pdMS_TO_TICKS(500);
	TickType_t xPreviousWakeTime = xTaskGetTickCount();
	
	while (1){
		if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE){
			vTaskDelayUntil(&xPreviousWakeTime, xTaskPeriod);
			
			int i;
			for (i=0; i<5; i++){
				counter--;
			}
			
		}
	}
}



void TaskC (TimerHandle_t timer){
	if ((uint32_t) pvTimerGetTimerID(timer) == 0){
		int current_counter = counter;
		long int result;
		
		__asm volatile(
			"MOV %0, #1\n"
			"loop:\n"
			"MUL %0, %0, %1\n"
			"SUB %1, %1, #1\n"
			"CMP %1, #0\n"
			"BGT loop\n"
			"exit:"
			
			: "=r" (result)
			: "r" (current_counter)
		);
		
	}
}

void Timer (void *pvParameters){
	(void) pvParameters;
	
	auto_reload_timer = xTimerCreate("AutoReloadTimer", pdMS_TO_TICKS(700), pdTRUE,  (void *)0, TaskC);

	xTimerStart(auto_reload_timer, portMAX_DELAY);
	
	vTaskDelete(NULL);
}
