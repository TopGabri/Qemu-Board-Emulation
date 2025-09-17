#include "FreeRTOS.h"
#include "task.h"
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

//global variables
char characters[8], characters2[8];

int main(int argc, char **argv){

	(void) argc;
	(void) argv;

	UART_init();
	
	xTaskCreate(read_from_UART, "Task1", 1000, NULL, mainTASK_PRIORITY, NULL);
	xTaskCreate(write_to_CAN_and_transmit, "Task2", 1000, NULL, mainTASK_PRIORITY-1, NULL);
	xTaskCreate(read_from_CAN, "Task3", 1000, NULL, mainTASK_PRIORITY-2, NULL);
	xTaskCreate(write_to_UART, "Task4", 1000, NULL, mainTASK_PRIORITY-3, NULL);
		
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

void useCan(void *pvParameters){

	(void) pvParameters;

	//write to CAN
	char str[200];
	char tmp[9];
	char data[8] = {0x73, 0x6F, 0x6E, 0x6F, 0x20, 0x67, 0x61, 0x79};
	int can_id = 0x123;
	int can_dlc = 8;
	int is_extended_id = 1;
	int is_remote_frame = 0;

	//transmit
	sprintf(str, "CAN0: writing and transmitting a frame...can_id: 0x%x\tcan_dlc: 0x%x\tRTR: %d\tFF: %d\tdata: ", can_id, can_dlc,is_remote_frame, is_extended_id);
	for(int i=7; i>=0; i--){
		sprintf(str + strlen(str), "0x%x  ", data[i]);
	}
	sprintf(str + strlen(str), "\n");
	UART_printf(str);

	CAN_write(0, can_id, data, can_dlc, is_extended_id, is_remote_frame);
	CAN_transmit(0);


	//receive
	while(!CAN_has_received(1)){
	}

	CAN_read_data(1,tmp);
	can_id = CAN_read_ID(1);
	CAN_release_receive_buffer(1);
	can_dlc = CAN_read_DLC(1);
	tmp[can_dlc] = '\0';
	 


	sprintf(str, "CAN1: Received frame: can_id: 0x%x\tdata: %s \n", can_id, tmp);
	UART_printf(str);
	
	vTaskDelete(NULL);
}

void read_from_UART(void *pvParameters){
	
	(void) pvParameters;

	UART_printf("read_from_UART\n");

	UART_printf("Reading 8 chars...\n");

	//read from UART
	for(int i=0; i<7; i++){
		characters[i] = UART_getchar();
	}

	UART_printf("Bye\n");
	vTaskDelete(NULL);
}

void write_to_UART(void *pvParameters){

	(void) pvParameters;

	UART_printf("write_to_UART\n");

	char str[50];

	sprintf(str, "%s\n", characters2);
	UART_printf(str);

	UART_printf("Bye\n");
	vTaskDelete(NULL);
}

void write_to_CAN_and_transmit(void *pvParameters){

	(void) pvParameters;
	UART_printf("write_to_CAN_and_transmit\n");

	//write to CAN
	int can_id = 0x123;
	int can_dlc = 8;
	int is_extended_id = 1;
	int is_remote_frame = 0;

	//can_data = characters
	CAN_write(0, can_id, characters, can_dlc, is_extended_id, is_remote_frame);
	UART_printf("Ok\n");

	CAN_transmit(0);	//Use CAN1 to transmit
	UART_printf("Ok\n");

	UART_printf("Bye\n");
	vTaskDelete(NULL);
}

void read_from_CAN(void *pvParameters){

	(void) pvParameters;

	UART_printf("read_from_CAN\n");

	if(CAN_has_received(1)){	//Use CAN2 to receive
		CAN_read_data(1, characters2);
		CAN_release_receive_buffer(1);
	}

	UART_printf("Bye\n");
	vTaskDelete(NULL);
}

// Interrupt handlers
void Uart_Handler( void ){
	UART_clear_interrupt();
	UART_printf("Interrupt received\n");
}



