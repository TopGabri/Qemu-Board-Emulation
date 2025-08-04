#ifndef HW_NXPS32K3_H
#define HW_NXPS32K3_H

//#include "target/avr32/cpu.h"
#include "qom/object.h"
#include "hw/sysbus.h"
#include "hw/arm/armv7m.h"
#include "hw/arm/boot.h"
#include "qemu/units.h"
#include "hw/qdev-clock.h"
#include "qobject/qlist.h"

#define TYPE_NXPS32K3_MCU "NXPS32K3"
#define TYPE_NXPS32K3S_MCU "NXPS32K3S"

typedef struct NXPS32K3McuState NXPS32K3McuState;
DECLARE_INSTANCE_CHECKER(NXPS32K3McuState, NXPS32K3_MCU, TYPE_NXPS32K3_MCU)

struct NXPS32K3McuState{
    SysBusDevice parent_obj;
    //cpu
    ARMv7MState armv7m;
    Clock *sysclk;
    Clock *refclk;
    //flash memory
    MemoryRegion flash;
};

#endif

