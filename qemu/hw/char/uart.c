#include "qemu/osdep.h"
#include "qapi/error.h" // provides error_fatal() handler
#include "hw/sysbus.h" // provides all sysbus registering func
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/module.h"
#include "hw/char/uart.h"
#include "qemu/log.h"




char useless = '0';   //just a placeholder for future code (make does not build qemu if some code portions are unused)

// ALTERA NIOS II UART PERIPHERAL

static void check_and_set_interrupt(void *opaque){
    UartState *s = (UartState *) opaque;

    if(s->control & s->status){     //if any status bit and its corresponding control bit are set
        //raise interrupt
        qemu_irq_raise(s->irq);
        //DEBUG
        qemu_log_mask(LOG_GUEST_ERROR, "\n---Interrupt raised---        ");
    }

}

static void check_and_clear_interrupt(void *opaque){
    UartState *s = (UartState *) opaque;
    
    if(!(s->control & s->status)){     //if any status bit and its corresponding control bit are set
        //clear interrupt
        qemu_irq_lower(s->irq);
        //DEBUG
        qemu_log_mask(LOG_GUEST_ERROR, "\n---Interrupt cleared---        ");
    }
}

static void set_status_bit(void *opaque, uint32_t status_bit){
    UartState *s = (UartState *) opaque;

    s->status |= status_bit;
    
    if(s->status & (PE | FE | BRK | ROE | TOE)){
        //E status bit is set if any error status bit is set
        s->status |= E;
    }

    //DEBUG
    qemu_log_mask(LOG_GUEST_ERROR, "\n---Status register: %#x---        ", s->status);

    check_and_set_interrupt(s);
}
static void reset_status_bit(void *opaque, uint32_t status_bit){
    UartState *s = (UartState *) opaque;

    s->status &= ~status_bit;

    //no check on error bits as they have to be cleared by the processor

    //DEBUG
    qemu_log_mask(LOG_GUEST_ERROR, "\n---Status register: %#x---        ", s->status);

    check_and_clear_interrupt(s);
}

static void update_params(void *opaque){
    UartState *s = (UartState *) opaque;
    QEMUSerialSetParams ssp;
    
    s->baudrate = s->clk_freq / (s->divisor+1);     //compute baudrate (if divisor=0, br=clk_freq)
    ssp.speed = s->baudrate;    //set serial device speed
    
    //UART 8E1 with configurable baudrate
    ssp.parity = 'E';
    ssp.data_bits = 8;
    ssp.stop_bits = 1;

    s->frame_size = 1 + ssp.data_bits + 1 + ssp.stop_bits;  // start, data, parity, stop bits

    //set new char transmission time
    s->char_tx_time = (NANOSECONDS_PER_SECOND / s->baudrate) * s->frame_size;
    qemu_chr_fe_ioctl(&s->chr, CHR_IOCTL_SERIAL_SET_PARAMS, &ssp);
}

static void wait_for_ch_tx_time(void *opaque, QEMUTimer *timer){
    UartState *s = (UartState *)opaque;

    // start the timer again
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    timer_mod(timer, now + s->char_tx_time);
}

// action to perform when the timer runs out
static void timeout_rx(void *opaque){
    UartState *s = (UartState*)opaque;

    //printf("timeout_rx\n");

    s->rxdata = s->rx_shift_register;

    //notify that frontend is ready to receive new char
    s->can_receive=1;

    if(s->status & RRDY){
        set_status_bit(s, ROE);
    }
    else{
        set_status_bit(s, RRDY);  //new data available in RXDATA register
    }
}

static void timeout_tx(void *opaque){
    UartState *s = (UartState*)opaque;

    //printf("timeout_tx\n");

    uint8_t ch = s->tx_shift_register;
    qemu_chr_fe_write_all(&s->chr, &ch, 1); //shift register has completed the shifting operation

    if(!(s->status & TRDY)){    // TRDY = 0?
        //fill tx_shift_reg with new data
        reset_status_bit(s, TMT);   //TMT = 0
        set_status_bit(s, TRDY);    //TRDY = 1

        s->tx_shift_register = (uint8_t)s->txdata;
        
        wait_for_ch_tx_time(s, s->tx_timer);
    }
    else{   // TRDY = 1
        set_status_bit(s, TMT); // TMT = 1
    }

}

static void uart_reset(DeviceState *dev)
{
    //printf("uart_reset\n");

    //reset invoked after init and realize
    UartState *s = UART(dev);

    //reset all registers
    s->rxdata = 0x00000000;
    s->txdata = 0x00000000;
    s->status = 0x00000000 | TRDY | TMT; 
    s->control = 0x00000000;
    s->divisor = 0x00000001;

    s->can_receive = 1;
    qemu_irq_lower(s->irq);

    qemu_chr_fe_accept_input(&s->chr);  //notify that frontend is ready to receive new char
    update_params(s);
}

static int uart_can_receive(void *opaque)
{
    return 1;
}


static void uart_receive(void *opaque, const uint8_t *buf, int size)
{   
    //printf("uart_receive\n");
    UartState *s = opaque;

    if(s->can_receive){
        s->rx_shift_register = *buf;
        s->can_receive = 0;
        
        wait_for_ch_tx_time(s, s->rx_timer);    //start timer
        qemu_chr_fe_accept_input(&s->chr);  //notify that frontend is ready to receive new char
    }
    
}

static uint64_t uart_read(void *opaque, hwaddr addr,
                                       unsigned int size)
{
    UartState *s = (UartState *) opaque;
    uint32_t retvalue = 0;

    switch (addr) {
        case RXDATA:
            retvalue = (s->rxdata) & 0xFF;
            reset_status_bit(s, RRDY);

            //DEBUG
            qemu_log_mask(LOG_GUEST_ERROR, "\n---RX_DATA read---        ");
            break;
        case TXDATA:
            //write-only
            break;
        case STATUS:
            retvalue = s->status;
            break;
        case CONTROL:
            retvalue = s->control;
            break;
        case DIVISOR: 
            retvalue = s->divisor;
            break;
    }

    return (uint64_t) retvalue; //MemoryRegionOps expects uint64_t
}

static void uart_write(void *opaque, hwaddr addr,
                                  uint64_t val, unsigned int size)
{
    //printf("uart_write\n");
    UartState *s = (UartState *) opaque;

    switch (addr) {
        case RXDATA:
            //read-only
            break;
        case TXDATA:
            s->txdata = (uint32_t) val; //MemoryRegionOps expects uint64_t, but registers are 32bits wide

            qemu_log_mask(LOG_GUEST_ERROR, "\n---TXDATA register was written: %#x---        ", s->txdata);

            if(s->status & TMT){    //is shift register empty? (TMT = 1?)
                //write char inside tx_shift_register and wait for its transmission to end
                reset_status_bit(s, TMT);   //shift register filled (TMT = 0)
                set_status_bit(s, TRDY);    //TRDY = 1

                s->tx_shift_register = (uint8_t) s->txdata;

                wait_for_ch_tx_time(s, s->tx_timer);
            }
            else{
                if(!(s->status & TRDY)){    //TRDY = 0?
                    set_status_bit(s, TOE);     //TOE = 1
                }
                else{
                    reset_status_bit(s, TRDY);  //TRDY = 0
                }
            }

            break;
        case STATUS:
            s->status = val;

            qemu_log_mask(LOG_GUEST_ERROR, "\n---STATUS register was written: %#x---        ", s->status);

            check_and_set_interrupt(s);
            check_and_clear_interrupt(s);

            break;
        case CONTROL:
            s->control = val;

            qemu_log_mask(LOG_GUEST_ERROR, "\n---CONTROL register was written: %#x---        ", s->control);

            check_and_set_interrupt(s);
            check_and_clear_interrupt(s);

            break;
        case DIVISOR: 
            s->divisor = val;

            qemu_log_mask(LOG_GUEST_ERROR, "\n---DIVISOR register was written: %#x---        ", s->status);

            update_params(s);
            break;
    }

    return;
}

static void uart_realize(DeviceState *dev, Error **errp)
{
    UartState *s = UART(dev);
    //printf("uart_realize\n");
    qemu_chr_fe_set_handlers(
        &s->chr,                        //CharBackend
        uart_can_receive,               //IOCanReadHandler
        uart_receive,                   //IOReadHandler
        NULL,                           //IOEventHandler
        NULL,                           //BackendChangeHandler
        s,                              //opaque
        NULL,                           //context
        true                            //set_open
    );
    uart_reset(dev);
}

static const MemoryRegionOps uart_ops = {
    .read = uart_read,
    .write = uart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const Property uart_properties[] = {
    DEFINE_PROP_CHR("chardev", UartState, chr),
};

static void uart_init(Object *obj)
{
    UartState *s = UART(obj);

    // Init interrupts
    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &uart_ops, s,
                          TYPE_UART, 20);
    // 5 regs * 4 bytes = 20 bytes

    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);

    s->rx_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, timeout_rx, s);   //initialize the timer
    s->tx_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, timeout_tx, s);   //initialize the timer
    s->clk_freq = CLK_FREQ;
    
}

static void uart_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, uart_properties);
    device_class_set_legacy_reset(dc, uart_reset);
    dc->realize = uart_realize;
}

static const TypeInfo uart_info = {
    .name          = TYPE_UART,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(UartState),
    .instance_init = uart_init,
    .class_init    = uart_class_init,
};

static void uart_register_types(void)
{
    type_register_static(&uart_info);
}

type_init(uart_register_types)
