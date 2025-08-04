#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qemu/cutils.h"
#include "qapi/error.h"
#include "qemu/error-report.h"
#include "hw/arm/boot.h"
#include "hw/arm/armv7m.h"
#include "hw/or-irq.h"
#include "hw/boards.h"
#include "system/address-spaces.h"
#include "system/system.h"
#include "hw/qdev-properties.h"
#include "hw/misc/unimp.h"
#include "hw/char/cmsdk-apb-uart.h"
#include "hw/timer/cmsdk-apb-timer.h"
#include "hw/timer/cmsdk-apb-dualtimer.h"
#include "hw/misc/mps2-scc.h"
#include "hw/misc/mps2-fpgaio.h"
#include "hw/ssi/pl022.h"
#include "hw/i2c/arm_sbcon_i2c.h"
#include "hw/net/lan9118.h"
#include "net/net.h"
#include "hw/watchdog/cmsdk-apb-watchdog.h"
#include "hw/qdev-clock.h"
#include "qobject/qlist.h"
#include "qom/object.h"
#include "hw/arm/nxp_s32k3.h"

struct NXPS32K3McuClass {
    SysBusDeviceClass parent_class;
    
    const char *cpu_type;
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
    
}

static void nxps32k3_class_init(ObjectClass *oc, void *data){
    DeviceClass *dc = DEVICE_CLASS(oc);
    
    dc->realize = nxps32k3_realize;
    dc->user_creatable = false;
}

static void nxps32k3s_class_init(ObjectClass *oc, void *data){
    NXPS32K3McuClass *nxps32k3= NXPS32K3_MCU_CLASS(oc);
    
    /*mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m7");*/
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

