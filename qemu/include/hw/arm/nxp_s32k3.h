#ifndef HW_NXPS32K3_H
#define HW_NXPS32K3_H

#include "qom/object.h"
#include "hw/sysbus.h"
#include "hw/arm/armv7m.h"
#include "hw/arm/boot.h"
#include "qemu/units.h"
#include "hw/qdev-clock.h"
#include "qobject/qlist.h"

#define TYPE_NXPS32K3S_MCU "NXPS32K3S"

#define ITCM_BA 0x00000000
#define DTCM_BA 0x20000000
#define SRAM_0_BA 0x20400000
#define SRAM_1_BA 0x20440000
#define SRAM_2_BA 0x20480000
#define PFLASH_0_BA 0x00400000
#define DFLASH_0_BA 0x10000000
#define PFLASH_1_BA 0x00600000
#define DFLASH_1_BA 0x1000C000
#define PFLASH_2_BA 0x00800000
#define DFLASH_2_BA 0x10016000
#define ITCM_SIZE 0x10000
#define DTCM_SIZE 0x20000
#define SRAM_SIZE 0x40000
#define PFLASH_SIZE 0x200000
#define DFLASH_SIZE 0xA000
#define PERIPHERAL_BA 0x40008000
#define PERIPHERAL_LAST 0X40588000
#define PERIPHERAL_SIZE 0x3FFF
typedef struct NXPS32K3McuState NXPS32K3McuState;
#define TYPE_NXPS32K3_MCU "nxps32k3-mcu"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K3McuState, NXPS32K3_MCU)

struct NXPS32K3McuState{
    SysBusDevice parent_obj;
    //cpu
    ARMv7MState armv7m;
    //clock
    Clock *sysclk;
    Clock *refclk;
    //ram
    MemoryRegion ITCM;
    MemoryRegion DTCM;
    MemoryRegion SRAM_0;
    MemoryRegion SRAM_0_alias;
    MemoryRegion SRAM_1;
    MemoryRegion SRAM_2;
    //flash memory
    MemoryRegion PFLASH_0;
    MemoryRegion DFLASH_0;
    MemoryRegion PFLASH_1;
    MemoryRegion DFLASH_1;
    MemoryRegion PFLASH_2;
    MemoryRegion DFLASH_2;
};

#endif

