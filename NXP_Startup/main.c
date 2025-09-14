#include "FreeRTOS.h"
#include "task.h"

#define mainTASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

void vTaskFunction(void *pvParameters);

int main(int argc, char **argv){

	(void) argc;
	(void) argv;

	xTaskCreate(
		// Function which implements the task
		vTaskFunction,
		// Name of the task (debug purposes, not used by the kernel)
		"Task1",
		// Stack to allocate to the task
		configMINIMAL_STACK_SIZE,
		// Parameter passed to the task. Not needed for Hello World example
		NULL,
		// Priority assigned to the task
		mainTASK_PRIORITY,
		// Task handle. Not required
		NULL
	);
	vTaskStartScheduler();
    for( ; ; );
}

/* Task Function */
void vTaskFunction(void *pvParameters) {
	(void) pvParameters;
	volatile int a=100;
	volatile int b=10;
	volatile int c=a+b;
	a=c;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
