#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "uart.h"

#define mainTASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

void useUart(void *pvParameters);

int main(int argc, char **argv){

	(void) argc;
	(void) argv;

	UART_init();
	
	xTaskCreate(useUart, "Task1", 1000, (void*)20, mainTASK_PRIORITY, NULL);
		
	// Give control to the scheduler
	vTaskStartScheduler();

	// If everything ok should never reach here
	for( ; ; );
}

void useUart(void *pvParameters){
	//write to UART
	char str[50];
	char c;
	long n = (long)pvParameters;
	
	sprintf(str, "Hi, I have this argument: %ld\n", n);
	UART_printf(str);
	
	UART_printf("Reading 7 chars...\n");
	//read from UART
	for(int i=0; i<7; i++){
		c = UART_getchar();
		sprintf(str, "%c\n", c);
		UART_printf(str);
	}
	
	//check if uart interrupt works
	while(1){
		vTaskDelay(pdMS_TO_TICKS(1000));
		sprintf(str, "%0lx\n", UART_STATUS);
		UART_printf(str);
	}
	
	vTaskDelete(NULL);
}


void Uart_Handler( void ){
	UART_clear_interrupt();
	UART_printf("Interrupt received\n");
}



