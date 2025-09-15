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
#include "hw/misc/unimp.h"
  

#define SYSCLK_FRQ 120000000ULL
#define debug 1

struct NXPS32K3McuClass {
    SysBusDeviceClass parent_class;
    
    const char *cpu_type;
};

static const uint32_t uart_addr=0x40328000;
static const uint32_t uart_irq=141;

typedef struct NXPS32K3McuClass NXPS32K3McuClass;
//DECLARE_CLASS_CHECKERS(NXPS32K3McuClass,NXPS32K3_MCU, TYPE_NXPS32K3_MCU)

static void nxps32k3_realize(DeviceState *dev, Error **errp){
    printf("Realizing NXP S32K3 Micro Controller \n");
    
    NXPS32K3McuState *s = NXPS32K3_MCU(dev);
    DeviceState *armv7m;
    SysBusDevice *busdev;
    MemoryRegion *system_memory = get_system_memory();
    
    //memory
    memory_region_init_ram(&s->SRAM_0, OBJECT(dev), "NXPS32K3.SRAM0",SRAM_SIZE,&error_fatal);
    memory_region_add_subregion(system_memory,SRAM_0_BA,&s->SRAM_0);
    
    memory_region_init_ram(&s->ITCM, OBJECT(dev), "NXPS32K3.ITCM",ITCM_SIZE,&error_fatal);
    memory_region_init_ram(&s->DTCM, OBJECT(dev), "NXPS32K3.DTCM",DTCM_SIZE,&error_fatal);
    memory_region_init_ram(&s->SRAM_1, OBJECT(dev), "NXPS32K3.SRAM1",SRAM_SIZE,&error_fatal);
    memory_region_init_ram(&s->SRAM_2, OBJECT(dev), "NXPS32K3.SRAM2",SRAM_SIZE,&error_fatal);
    memory_region_init_rom(&s->PFLASH_0,OBJECT(dev),"NXPS32K3.PFLASH0", PFLASH_SIZE, &error_fatal);
    memory_region_init_rom(&s->DFLASH_0,OBJECT(dev),"NXPS32K3.DFLASH0", DFLASH_SIZE, &error_fatal);
    memory_region_init_rom(&s->PFLASH_1,OBJECT(dev),"NXPS32K3.PFLASH1", PFLASH_SIZE, &error_fatal);
    memory_region_init_rom(&s->DFLASH_1,OBJECT(dev),"NXPS32K3.DFLASH1", DFLASH_SIZE, &error_fatal);
    memory_region_init_rom(&s->PFLASH_2,OBJECT(dev),"NXPS32K3.PFLASH2", PFLASH_SIZE, &error_fatal);
    memory_region_init_rom(&s->DFLASH_2,OBJECT(dev),"NXPS32K3.DFLASH2", DFLASH_SIZE, &error_fatal);
    
    memory_region_add_subregion(system_memory,ITCM_BA,&s->ITCM);
    memory_region_add_subregion(system_memory,DTCM_BA,&s->DTCM);
    memory_region_add_subregion(system_memory,SRAM_1_BA,&s->SRAM_1);
    memory_region_add_subregion(system_memory,SRAM_2_BA,&s->SRAM_2);
    memory_region_add_subregion(system_memory,PFLASH_0_BA,&s->PFLASH_0);
    memory_region_add_subregion(system_memory,DFLASH_0_BA,&s->DFLASH_0);
    memory_region_add_subregion(system_memory,PFLASH_1_BA,&s->PFLASH_1);
    memory_region_add_subregion(system_memory,DFLASH_1_BA,&s->DFLASH_1);
    memory_region_add_subregion(system_memory,PFLASH_2_BA,&s->PFLASH_2);
    memory_region_add_subregion(system_memory,DFLASH_2_BA,&s->DFLASH_2);
    
    memory_region_init_alias(&s->PFLASH_0_alias, OBJECT(dev), "NXPS32K3.PFLASH0-alias", &s->PFLASH_0, 0, ITCM_SIZE);
    memory_region_add_subregion(system_memory, 0, &s->PFLASH_0_alias);

    /*uint32_t current_peripheral_address = PERIPHERAL_BA;
    while(current_peripheral_address != PERIPHERAL_LAST){
        create_unimplemented_device("general peripheral",  current_peripheral_address, PERIPHERAL_SIZE);
        current_peripheral_address += PERIPHERAL_SIZE;
        printf ("peripheral creata: %X\n" , current_peripheral_address);
    }
    printf ("fuori");*/
    
 
    armv7m = DEVICE(&s->armv7m);
    
    //clock
    clock_set_mul_div(s->refclk, 8, 1);
    clock_set_source(s->refclk, s->sysclk);
    //CPU, initialize
    qdev_prop_set_uint32(armv7m, "num-irq", 241);
    qdev_prop_set_uint8(armv7m, "num-prio-bits", 4);
    qdev_prop_set_string(armv7m, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m7"));
    qdev_prop_set_bit(armv7m, "enable-bitband", false);
    qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
    qdev_connect_clock_in(armv7m, "refclk", s->refclk);
    
    object_property_set_link(OBJECT(&s->armv7m), "memory",OBJECT(get_system_memory()), &error_abort);
    
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp)) {
        return;
    }
    if(debug == 1){
        printf ("Dentro debug MCU\n");
    }
    printf("qui ci sono\n");
    
    //peripherals
    dev = DEVICE(&(s->uart));
    qdev_prop_set_chr(dev, "chardev", serial_hd(0));    
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, uart_addr);
    sysbus_connect_irq(busdev,0,qdev_get_gpio_in(armv7m,uart_irq));
        
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
    
    //initialize peripherals
    object_initialize_child(oc, "uart", &nxps32k3->uart, TYPE_UART);
    
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
