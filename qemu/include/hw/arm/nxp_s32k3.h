#ifndef HW_NXPS32K3_H
#define HW_NXPS32K3_H

#include "qom/object.h"
#include "hw/sysbus.h"
#include "hw/arm/armv7m.h"
#include "hw/arm/boot.h"
#include "qemu/units.h"
#include "hw/qdev-clock.h"
#include "qobject/qlist.h"
#include "hw/char/uart.h"
#include "hw/net/can.h"
#include "net/can_emu.h"

#define TYPE_NXPS32K3S_MCU "NXPS32K3S"

//Memory
#define ITCM_BA 0x00000000
#define DTCM_BA 0x20000000
#define ITCM_SIZE 0x10000
#define DTCM_SIZE 0x20000
#define SRAM_0_BA 0x20400000
#define SRAM_1_BA 0x20440000
#define SRAM_2_BA 0x20480000
#define SRAM_SIZE 0x40000
#define PFLASH_0_BA 0x00400000
#define PFLASH_1_BA 0x00600000
#define PFLASH_2_BA 0x00800000
#define PFLASH_SIZE 0x200000
#define DFLASH_0_BA 0x10000000
#define DFLASH_1_BA 0x1000C000
#define DFLASH_2_BA 0x10016000
#define DFLASH_SIZE 0xA000


//Peripherals 
#define UART0_BA 0x40328000
#define UART0_IRQn 141
#define CAN0_BA 0x40304000
#define CAN0_IRQn 109
#define CAN1_BA 0x40308000
#define CAN1_IRQn 113


typedef struct NXPS32K3McuState NXPS32K3McuState;
#define TYPE_NXPS32K3_MCU "nxps32k3-mcu"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K3McuState, NXPS32K3_MCU)

#define SYSCLK_FRQ 120000000ULL



struct NXPS32K3McuState{
	//Sysbus
    SysBusDevice parent_obj;
    
    //cpu
    ARMv7MState armv7m[3];
    
    //clock
    Clock *sysclk;
    Clock *refclk;
    
    //MEMORY
    MemoryRegion sysmem_alias[3];
    MemoryRegion cpu_sysmem[3];
    //shared
    MemoryRegion ITCM;
    MemoryRegion DTCM;
    //private
    MemoryRegion SRAM[3];
    MemoryRegion PFLASH[3];
    MemoryRegion DFLASH[3];
    //boot
    MemoryRegion PFLASH_0_alias;
    
    //peripheral
    UartState uart0;
    CanState can0;
    CanState can1;
    CanBusState *canbus;
};

#endif

