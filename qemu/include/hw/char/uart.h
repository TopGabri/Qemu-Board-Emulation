/*

    Emulation of the NXP S32K3X8EVB-Q289  © 2025 
    by Claudio Pio Perricone, Gabriele Arcidiacono, Matteo Ruggeri, Stefano Galati 
    is licensed under CC BY-NC 4.0. To view a copy of this license, 
    visit https://creativecommons.org/licenses/by-nc/4.0/

*/

#ifndef UART_H
#define UART_H

    #include "qom/object.h"
    #include "hw/sysbus.h"
    #include "chardev/char-fe.h" // CharBackend
    #include "chardev/char-serial.h"
    #include "stdint.h"
    #include "qemu/timer.h"
    #include "hw/irq.h"

    #define TYPE_UART "uart"
    OBJECT_DECLARE_SIMPLE_TYPE(UartState, UART)

    // ALTERA NIOS II UART PERIPHERAL

    //registers
    #define RXDATA   0x00   //read-only
    #define TXDATA   0x04   //write-only
    #define STATUS   0x08   //read-write
    #define CONTROL  0x0C   //read-write
    #define DIVISOR  0x10   //read-write

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

    #define CLK_FREQ 115200

    struct UartState {
        SysBusDevice parent_obj;
        QEMUTimer *rx_timer;
        QEMUTimer *tx_timer;
        MemoryRegion mmio;

        uint8_t can_receive;    //internal state variable

        uint64_t clk_freq;
        uint64_t baudrate;
        uint64_t char_tx_time;
        int frame_size;

        uint32_t rxdata;    //read-only
        uint32_t txdata;    //write-only
        uint32_t status;    //read-write
        uint32_t control;   //read-write
        uint32_t divisor;   //read-write

        uint8_t tx_shift_register;
        uint8_t rx_shift_register;

        qemu_irq irq;   //interrupt

        CharBackend chr;
    };

#endif
