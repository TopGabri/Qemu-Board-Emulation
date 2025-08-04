#include "qemu/osdep.h"
#include "qemu/module.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "sysemu/sysemu.h"
#include "hw/qdev-properties.h"
#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/misc/unimp.h"
#include "nxp_s32k3.h"

struct NXPS32K3McuClass {
    SysBusDeviceClass parent_class;
    
    const char *cpu_type;
    
     size_t flash_size;
};

typedef struct NXPS32K3McuClass NXPS32K3McuClass;
DECLARE_CLASS_CHECKERS(NXPS32K3McuClass,NXPS32K3_MCU, TYPE_NXPS32K3_MCU)

static void nxps32k3_realize(DeviceState *dev, Error **errp){
    printf("Realizing NXP S32K3 Micro Controller \n");
    
    NXPS32K3McuState *s = NXPS32K3_MCU(dev);
    const NXPS32K3McuClass *mc = NXPS32K3_MCU_GET_CLASS(dev);
    
    //cpu
    /*object_initialize_child(OBJECT(mms), "armv7m", &mms->armv7m, TYPE_ARMV7M);
    object_property_set_bool(OBJECT(&s->cpu),"realized",true,&error_abort);*/
    
    //flash
    memory_region_init_rom(&s->flash, OBJECT(dev),"flash",mc->flash_size,&error_fatal);
    memory_region_add_subregion(get_system_memory(),0xd0000000, &s->flash);
}

static void nxps32k3_class_init(ObjectClass *oc, void *data){
    DeviceClass *dc = DEVICE_CLASS(oc);
    
    dc->realize = nxps32k3_realize;
    dc->user_creatable = false;
}

static void nxps32k3s_class_init(ObjectClass *oc, void *data){
    NXPS32K3McuClass *nxps32k3= NXPS32K3_MCU_CLASS(oc);
    
    /*mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m7");*/
    nxps32k3->flash_size=1024 * Kib;
}

static const TypeInfo nxps32k3_mcu_types[]={
    {
        .name= TYPE_NXPS32K3S_MCU,
        .parent=TYPE_NXPS32K3_MCU,
        .class_init=nxps32k3_class_init,
    },{
        .name=TYPE_NXPS32K3_MCU,
        .parent=TYPE_SYS_BUS_DEVICE,
        .instance_size=sizeof(NXPS32K3McuState),
        .class_size=sizeof(NXPS32K3McuClass),
        .class_init=nxps32k3s_class_init,
        .abstract = true,
    }
};

DEFINE_TYPES(nxps32k3_mcu_types)

