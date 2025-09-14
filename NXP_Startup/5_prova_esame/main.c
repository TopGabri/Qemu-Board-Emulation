#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "uart.h"
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
    UART_init();
	
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
		
		sprintf(message, "TaskA: %d\n", counter);
		UART_printf(message);
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
			
			sprintf(message, "TaskB: %d\n", counter);
			UART_printf(message);
		}
	}
}



void TaskC (TimerHandle_t timer){
	if ((uint32_t) pvTimerGetTimerID(timer) == 0){
		int current_counter = counter;
		long int result;
		char message [100];
		sprintf(message, "TaskC: counter: %d\n", current_counter);
		UART_printf(message);
		
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
		
		sprintf(message, "TaskC: result: %ld\n", result);
		UART_printf(message);
	}
}

void Timer (void *pvParameters){
	(void) pvParameters;
	
	auto_reload_timer = xTimerCreate("AutoReloadTimer", pdMS_TO_TICKS(700), pdTRUE,  (void *)0, TaskC);
	if (auto_reload_timer == NULL) UART_printf("ERROR\n");

	xTimerStart(auto_reload_timer, portMAX_DELAY);
	
	vTaskDelete(NULL);
}
