#ifndef CAN_H
#define CAN_H

    #include "qom/object.h"
    #include "hw/sysbus.h"
    #include "hw/irq.h"
    #include "net/can_emu.h"
    #include "net/can_host.h"
    #include "stdint.h"

    #define TYPE_CAN "can"
    OBJECT_DECLARE_SIMPLE_TYPE(CanState, CAN)

    #define CAN_DEFAULT_CLOCK 1000000000

    //CAN controller registers

    #define NREG 11

    #define TFI 0x00    //Transmit Frame Info 
    #define TID 0x04    //Transmit ID
    #define TDA 0x08    //Transmit Data A
    #define TDB 0x0C    //Transmit Data B
    #define RFI 0x10    //Receive Frame Info
    #define RID 0x14    //Receive ID
    #define RDA 0x18    //Receive Data A   
    #define RDB 0x1C    //Receive Data B
    #define SR  0x20    //Status Register
    #define CMR 0x24    //Command Register   
    #define IER 0x28    //Interrupt Enable Register 

    //TFI/RFI bits
    #define DLC_POS 16                  //Data Length Code Position
    #define DLC_MASK (0xF << DLC_POS)   //Data Length Code Mask 
    #define RTR (1 << 30)
    #define FF (1 << 31)


    //TID/RID bits
    #define IDS 0x7FF       //ID Standard (11-bits)
    #define IDE 0x1FFFFFFF //ID Extended (29-bits)

    //SR bits
    #define RBS (1 << 0)    //Receive Buffer Status
    #define DOS (1 << 1)    //Data Overrun Status
    #define TBS (1 << 2)    //Transmit Buffer Status
    #define TCS (1 << 3)    //Transmit Complete Status

    //CMR bits
    #define TR (1 << 0)     //Transmission Request
    #define RRB (1 << 2)    //Release Receive Buffer    
    #define CDO (1 << 3)    //Clear Data Overrun

    //IER bits
    #define RIE (1 << 0)    //Receive Interrupt Enable
    #define TIE (1 << 1)    //Transmit Interrupt Enable
    #define DOIE (1 << 3)   //Data Overrun Interrupt Enable

    struct CanState {
        SysBusDevice parent_obj;

        MemoryRegion mmio;

        uint32_t ext_clk_freq;

        //controller registers
        uint32_t tfi;
        uint32_t tid;
        uint32_t tda;
        uint32_t tdb;
        uint32_t rfi;
        uint32_t rid;
        uint32_t rda;
        uint32_t rdb;
        uint32_t sr;
        uint32_t cmr;
        uint32_t ier;

        qemu_irq irq;   //interrupt

        CanBusClientState bus_client;
        CanBusState *canbus;
    };


#endif
