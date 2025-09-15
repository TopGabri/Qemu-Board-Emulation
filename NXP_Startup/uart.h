#ifndef __PRINTF__
#define __PRINTF__

#include "FreeRTOS.h"
//#include "stm32f205xx.h"
//#include "core_cm3.h"

#define UART_ADDRESS                         ( 0x40328000UL )
#define UART_RXDATA                          ( *( ( ( volatile uint32_t * ) ( UART_ADDRESS + 0UL ) ) ) )
#define UART_TXDATA                          ( *( ( ( volatile uint32_t * ) ( UART_ADDRESS + 4UL ) ) ) )
#define UART_STATUS                          ( *( ( ( volatile uint32_t * ) ( UART_ADDRESS + 8UL ) ) ) )
#define UART_CONTROL                         ( *( ( ( volatile uint32_t * ) ( UART_ADDRESS + 12UL ) ) ) )
#define UART_DIVISOR                         ( *( ( ( volatile uint32_t * ) ( UART_ADDRESS + 16UL ) ) ) )

//status bits
#define PE (1 << 0)
#define FE (1 << 1)
#define BRK (1 << 2)
#define ROE (1 << 3)
#define TOE (1 << 4)
#define TMT (1 << 5)
#define TRDY (1 << 6)
#define RRDY (1 << 7)
#define E (1 << 8)

//control bits
#define IPE (1 << 0)
#define IFE (1 << 1)
#define IBRK (1 << 2)
#define IROE (1 << 3)
#define ITOE (1 << 4)
#define ITMT (1 << 5)
#define ITRDY (1 << 6)
#define IRRDY (1 << 7)
#define IE (1 << 8)

void UART_init(void);
void UART_printf(const char *s);
char UART_getchar(void);
void UART_clear_interrupt(void);
void Uart_Handler(void);

#endif