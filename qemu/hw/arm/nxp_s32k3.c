#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qemu/error-report.h"
#include "hw/arm/boot.h"
#include "hw/arm/armv7m.h"
#include "hw/boards.h"
#include "system/address-spaces.h"
#include "system/system.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "qobject/qlist.h"
#include "qom/object.h"
#include "hw/arm/nxp_s32k3.h"
#include "qapi/error.h"

#define SYSCLK_FRQ 120000000ULL

struct NXPS32K3McuClass {
    SysBusDeviceClass parent_class;
    
    const char *cpu_type;
};

typedef struct NXPS32K3McuClass NXPS32K3McuClass;
//DECLARE_CLASS_CHECKERS(NXPS32K3McuClass,NXPS32K3_MCU, TYPE_NXPS32K3_MCU)

static void nxps32k3_realize(DeviceState *dev, Error **errp){
    printf("Realizing NXP S32K3 Micro Controller \n");
    
    NXPS32K3McuState *s = NXPS32K3_MCU(dev);
    DeviceState *armv7m;
    MemoryRegion *system_memory = get_system_memory();
    
    //memory
    memory_region_init_rom(&s->flash, OBJECT(dev), "NXPS32K3.flash",FLASH_SIZE, &error_fatal);
    memory_region_init_ram(&s->ram, NULL, "NXPS32K3.ram", RAM_SIZE, &error_fatal);
    memory_region_add_subregion(system_memory, FLASH_BASE_ADDRESS, &s->flash);
    memory_region_add_subregion(system_memory, RAM_BASE_ADDRESS, &s->ram);
    armv7m = DEVICE(&s->armv7m);
    
    //clock
    clock_set_mul_div(s->refclk, 8, 1);
    clock_set_source(s->refclk, s->sysclk);
    //CPU, initialize
    qdev_prop_set_uint32(armv7m, "num-irq", 96);
    qdev_prop_set_uint8(armv7m, "num-prio-bits", 4);
    qdev_prop_set_string(armv7m, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m7"));
    qdev_prop_set_bit(armv7m, "enable-bitband", false);
    qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
    qdev_connect_clock_in(armv7m, "refclk", s->refclk);
    
    object_property_set_link(OBJECT(&s->armv7m), "memory",OBJECT(get_system_memory()), &error_abort);
    
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp)) {
        return;
    }
    printf("qui ci sono\n");
        
}

static void nxps32k3_class_init(ObjectClass *oc, const void *data){
    DeviceClass *dc = DEVICE_CLASS(oc);
    
    dc->realize = nxps32k3_realize;
    dc->user_creatable = false;
}

static void nxps32k3s_class_init(Object *oc){
    NXPS32K3McuState *nxps32k3= NXPS32K3_MCU(oc);
    
    //initialize cpu
    object_initialize_child(oc, "armv7m", &nxps32k3->armv7m, TYPE_ARMV7M);
    
    //clock
    nxps32k3->sysclk = qdev_init_clock_in(DEVICE(nxps32k3), "sysclk", NULL, NULL, 0);
    nxps32k3->refclk = qdev_init_clock_in(DEVICE(nxps32k3), "refclk", NULL, NULL, 0);
    
  
}

static const TypeInfo nxps32k3_mcu_info={
        .name=TYPE_NXPS32K3_MCU,
        .parent=TYPE_SYS_BUS_DEVICE,
        .instance_size=sizeof(NXPS32K3McuState),
        .instance_init=nxps32k3s_class_init,
        .class_init=nxps32k3_class_init,
};

static void nxps32k3_mcu_types(void)
{
    type_register_static(&nxps32k3_mcu_info);
}

type_init(nxps32k3_mcu_types)
