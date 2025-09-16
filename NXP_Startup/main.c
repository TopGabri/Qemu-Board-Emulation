#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "uart.h"

#define mainTASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

void useUart(void *pvParameters);
void useCan(void *pvParameters);

int main(int argc, char **argv){

	(void) argc;
	(void) argv;

	UART_init();
	
	xTaskCreate(useUart, "Task1", 1000, (void*)20, mainTASK_PRIORITY, NULL);

	xTaskCreate(useCan, "Task2", 1000, NULL, mainTASK_PRIORITY, NULL);
		
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
	//write to CAN
	char str[50];
	char data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
	int can_id = 0x123;
	int can_dlc = 8;
	int is_extended_id = 1;
	int is_remote_frame = 0;

	//transmit
	sprintf(str, "CAN: writing and transmitting a frame...can_id: 0x%x\tcan_dlc: 0x%x\tdata: ", can_id, can_dlc);
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

	data = CAN_read(1);
	CAN_release_receive_buffer(1);
	
	vTaskDelete(NULL);
}


void Uart_Handler( void ){
	UART_clear_interrupt();
	UART_printf("Interrupt received\n");
}



