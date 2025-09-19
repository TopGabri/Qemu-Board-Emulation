#include "uart.h"

void UART_init( void )
{
	NVIC_SetPriority( LPUART0_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY );	
	NVIC_EnableIRQ( LPUART0_IRQn );
	
    UART_CONTROL = IROE;   //IROE = 1 -> Interrupt raised when ROE = 1
    UART_DIVISOR = 1000;   //divisor = 1000
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
	UART_STATUS &= ~(ROE);
}
