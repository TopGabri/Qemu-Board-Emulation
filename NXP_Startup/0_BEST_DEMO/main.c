#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "uart.h"
#include <stdlib.h>
#include <limits.h>

void TaskA(void *pvParameters);

int main(int argc, char **argv){
	(void) argc;
	(void) argv;
    UART_init();
	 
	xTaskCreate(TaskA, "Task A", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY +2, NULL);
	
	vTaskStartScheduler();
    for( ; ; );
}

void TaskA (void *pvParameters){
	(void) pvParameters;
	
}
