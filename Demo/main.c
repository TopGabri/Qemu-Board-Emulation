#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "can.h"

#define mainTASK_PRIORITY    ( tskIDLE_PRIORITY + 4 )

void useUart(void *pvParameters);
void useCan(void *pvParameters);

void read_from_UART(void *pvParameters);
void write_to_UART(void *pvParameters);
void write_to_CAN_and_transmit(void *pvParameters);
void read_from_CAN(void *pvParameters);


SemaphoreHandle_t xSem1, xSem2, xSem3, xSem4;

//global variables
char characters[8] = {0};
char characters2[9] = {0};	//only the first 8 chars will be changed, the last one remains '\0'

int main(int argc, char **argv){

	(void) argc;
	(void) argv;

	//initialize semaphores
	if((xSem1 = xSemaphoreCreateBinary())==NULL){
		UART_printf("Semaphore creation failed\n");
	}
	if((xSem2 = xSemaphoreCreateBinary())==NULL){
		UART_printf("Semaphore creation failed\n");
	}
	if((xSem3 = xSemaphoreCreateBinary())==NULL){
		UART_printf("Semaphore creation failed\n");
	}
	if((xSem4 = xSemaphoreCreateBinary())==NULL){
		UART_printf("Semaphore creation failed\n");
	}
	//xSem1 is free to be taken
	xSemaphoreGive(xSem1);

	UART_init();
	
	xTaskCreate(read_from_UART, "Task1", 1000, NULL, mainTASK_PRIORITY, NULL);
	xTaskCreate(write_to_CAN_and_transmit, "Task2", 1000, NULL, mainTASK_PRIORITY, NULL);
	xTaskCreate(read_from_CAN, "Task3", 1000, NULL, mainTASK_PRIORITY, NULL);
	xTaskCreate(write_to_UART, "Task4", 1000, NULL, mainTASK_PRIORITY, NULL);
		
	// Give control to the scheduler
	vTaskStartScheduler();

	// If everything ok should never reach here
	for( ; ; );
}

void read_from_UART(void *pvParameters){
	
	(void) pvParameters;

	for(;;){
		xSemaphoreTake(xSem1, portMAX_DELAY);

		UART_printf("(read_from_UART)\n");

		UART_printf("Reading 8 chars...\n");

		//read from UART
		for(int i=0; i<8; i++){
			characters[i] = UART_getchar();
		}

		UART_printf("(Bye)\n");

		xSemaphoreGive(xSem2);
	}
	
}


void write_to_CAN_and_transmit(void *pvParameters){

	(void) pvParameters;

	for(;;){
		xSemaphoreTake(xSem2, portMAX_DELAY);

		UART_printf("(write_to_CAN_and_transmit)\n");

		//write to CAN
		int can_id = 0x123;
		int can_dlc = 8;
		int is_extended_id = 1;
		int is_remote_frame = 0;

		//can_data = characters
		CAN_write(0, can_id, characters, can_dlc, is_extended_id, is_remote_frame);

		CAN_transmit(0);	//Use CAN0 to transmit

		UART_printf("(Bye)\n");
		
		xSemaphoreGive(xSem3);
	}

}

void read_from_CAN(void *pvParameters){

	(void) pvParameters;

	for(;;){	
		xSemaphoreTake(xSem3, portMAX_DELAY);

		UART_printf("(read_from_CAN)\n");

		while(!CAN_has_received(1)){}
		
		CAN_read_data(1, characters2);	//read data from CAN1
		CAN_release_receive_buffer(1);

		UART_printf("(Bye)\n");

		xSemaphoreGive(xSem4);
	}


}


void write_to_UART(void *pvParameters){

	(void) pvParameters;

	for(;;){
		xSemaphoreTake(xSem4, portMAX_DELAY);

		UART_printf("(write_to_UART)\n");

		char str[50];

		sprintf(str, "%s\n", characters2);
		UART_printf(str);

		UART_printf("(Bye)\n");

		xSemaphoreGive(xSem1);
	}
}

// Interrupt handlers
void Uart_Handler( void ){
	UART_clear_interrupt();
	UART_printf("UART: Interrupt received\n");
}


void CAN0_Handler(void){
	CAN_clear_interrupt(0);
	UART_printf("CAN0: Interrupt received\n");
}

void CAN1_Handler(void){
	CAN_clear_interrupt(1);
	UART_printf("CAN1: Interrupt received\n");
}




