#include "uart.h"

void UART_init( void )
{
	NVIC_SetPriority( 141, configMAX_SYSCALL_INTERRUPT_PRIORITY );	
	NVIC_EnableIRQ( 141 );
	
    UART_CONTROL = 0x100;       //IE = 1 -> Interrupt raised when any error bit is set
    UART_DIVISOR = 10000000;   //divisor = 10000000
}

void UART_printf(const char *s) {
    while(*s != '\0') { /* Loop until end of string */
        while(!(UART_STATUS & TRDY));     //if TRDY = 0, wait
        UART_TXDATA = (unsigned int)(*s); /* uart txdata */
        s++; /* Next char */
    }
}

char UART_getchar(void){
	while(!(UART_STATUS & RRDY));     //if RRDY = 0, wait
    return UART_RXDATA;
}

void UART_clear_interrupt(void){
	UART_STATUS &= ~(E | TOE | ROE | FE | PE);	//error bits = 0 -> clear interrupt
}