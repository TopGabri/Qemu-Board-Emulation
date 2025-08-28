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

#define FLASH_BASE_ADDRESS 0x00000000
#define FLASH_SIZE (1024 * 512)
#define RAM_BASE_ADDRESS 0x20000000
#define RAM_SIZE (1024 * 512)

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
    MemoryRegion ram;
    //flash memory
    MemoryRegion flash;
};

#endif

