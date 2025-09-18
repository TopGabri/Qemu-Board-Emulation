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
typedef struct NXPS32K3McuClass NXPS32K3McuClass;


static void nxps32k3_realize(DeviceState *dev, Error **errp)
{    
    printf("Realizing NXP S32K3 Micro Controller \n");
    
    NXPS32K3McuState *s = NXPS32K3_MCU(dev);
    DeviceState *armv7m;
    SysBusDevice *busdev;
    
    //CLOCK
    clock_set_mul_div(s->refclk, 8, 1);
    clock_set_source(s->refclk, s->sysclk);
    
    //MEMORY AND CPU
    MemoryRegion *sysmem = get_system_memory();
    
    memory_region_init_ram(&s->ITCM, NULL, "NXPS32K3.ITCM", ITCM_SIZE, &error_fatal);
    memory_region_init_ram(&s->DTCM, NULL, "NXPS32K3.DTCM", DTCM_SIZE, &error_fatal);
    
    memory_region_add_subregion(sysmem, ITCM_BA, &s->ITCM);
    memory_region_add_subregion(sysmem, DTCM_BA, &s->DTCM);
    
   	for (int i=0; i<3; i++) {
   		g_autofree char *sysmem_name = g_strdup_printf("NXPS32K3.cpu-%d-memory", i);
        g_autofree char *alias_name = g_strdup_printf("NXPS32K3.sysmem-alias-%d", i);
   		
   		//Define a memory region for each cpu
   		memory_region_init(&s->cpu_sysmem[i], OBJECT(dev), sysmem_name, UINT64_MAX);
   		memory_region_init_alias(&s->sysmem_alias[i], OBJECT(dev), alias_name, sysmem, 0, UINT64_MAX);
   		memory_region_add_subregion_overlap(&s->cpu_sysmem[i], 0, &s->sysmem_alias[i], -1);
   		
   		//Define the cpu
   		armv7m = DEVICE(&s->armv7m[i]);
   		
        qdev_prop_set_uint32(armv7m, "num-irq", 241);
        qdev_prop_set_uint8(armv7m, "num-prio-bits", 4);
        qdev_prop_set_string(armv7m, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m7"));
        qdev_prop_set_bit(armv7m, "enable-bitband", false);
        qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
        qdev_connect_clock_in(armv7m, "refclk", s->refclk);
        
        if (i != 0) qdev_prop_set_bit(armv7m, "start-powered-off", true);
    	
        	//Link cpu to memory
        	object_property_set_link(OBJECT(&s->armv7m[i]), "memory", OBJECT(&s->cpu_sysmem[i]), &error_abort);
        	
        	//realize the sysbus
        if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m[i]), errp)) {
            return;
        }
        
        //Private memory of each cpu
		uint64_t SRAM_BA[3] = {SRAM_0_BA, SRAM_1_BA, SRAM_2_BA};
		uint64_t PFLASH_BA[3] = {PFLASH_0_BA, PFLASH_0_BA, PFLASH_0_BA};
		uint64_t DFLASH_BA[3] = {DFLASH_0_BA, DFLASH_0_BA, DFLASH_0_BA};
        
        g_autofree char *sramname = g_strdup_printf("NXPS32K3.cpu-%d-sram", i);
        g_autofree char *pflashname = g_strdup_printf("NXPS32K3.cpu-%d-pflash", i);
        g_autofree char *dflashname = g_strdup_printf("NXPS32K3.cpu-%d-dflash", i);
        
        memory_region_init_ram(&s->SRAM[i], NULL, sramname, SRAM_SIZE, &error_fatal);
		memory_region_init_rom(&s->PFLASH[i], NULL, pflashname, PFLASH_SIZE, &error_fatal);
		memory_region_init_rom(&s->DFLASH[i], NULL, dflashname, DFLASH_SIZE, &error_fatal);
		
		memory_region_add_subregion(&s->cpu_sysmem[i], SRAM_BA[i], &s->SRAM[i]);
		memory_region_add_subregion(&s->cpu_sysmem[i], PFLASH_BA[i], &s->PFLASH[i]);
		memory_region_add_subregion(&s->cpu_sysmem[i], DFLASH_BA[i], &s->DFLASH[i]);

		if (i == 0) {
			memory_region_init_alias(&s->PFLASH_0_alias, NULL, "NXPS32K3.PFLASH0-alias", &s->PFLASH[i], 0, ITCM_SIZE);
    	        	memory_region_add_subregion(&s->cpu_sysmem[i], 0, &s->PFLASH_0_alias);
		}
   	}
   	
    //PERIPHERALS
    //UART
    dev = DEVICE(&(s->uart0));
    qdev_prop_set_chr(dev, "chardev", serial_hd(0));    
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->uart0), errp)) {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, UART0_BA);
    
    armv7m = DEVICE(&s->armv7m[0]);
    sysbus_connect_irq(busdev,0,qdev_get_gpio_in(armv7m,UART0_IRQn));

    //CAN0
    dev = DEVICE(&(s->can0));
    object_property_set_link(OBJECT(&s->can0), "canbus",OBJECT(s->canbus), &error_fatal);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->can0), errp)) {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, CAN0_BA);

    armv7m = DEVICE(&s->armv7m[0]);
    sysbus_connect_irq(busdev,0,qdev_get_gpio_in(armv7m,CAN0_IRQn));

    //CAN1
    dev = DEVICE(&(s->can1));
    object_property_set_link(OBJECT(&s->can1), "canbus",OBJECT(s->canbus), &error_fatal);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->can1), errp)) {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, CAN1_BA);

    armv7m = DEVICE(&s->armv7m[0]);
    sysbus_connect_irq(busdev,0,qdev_get_gpio_in(armv7m,CAN1_IRQn));

}


static void nxps32k3_class_init(ObjectClass *oc, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);
    
    dc->realize = nxps32k3_realize;
    dc->user_creatable = false;
}


static void nxps32k3s_class_init(Object *oc)
{
    NXPS32K3McuState *nxps32k3= NXPS32K3_MCU(oc);
    
    //initialize cpu
    object_initialize_child(oc, "armv7m_0", &nxps32k3->armv7m[0], TYPE_ARMV7M);
    object_initialize_child(oc, "armv7m_1", &nxps32k3->armv7m[1], TYPE_ARMV7M);
    object_initialize_child(oc, "armv7m_2", &nxps32k3->armv7m[2], TYPE_ARMV7M);
    
    //initialize peripherals
    object_initialize_child(oc, "uart0", &nxps32k3->uart0, TYPE_UART);
    
    nxps32k3->canbus = CAN_BUS(object_new(TYPE_CAN_BUS));
    object_property_add_child(OBJECT(nxps32k3), "canbus", OBJECT(nxps32k3->canbus));
    object_initialize_child(oc, "can0", &nxps32k3->can0, TYPE_CAN);
    object_initialize_child(oc, "can1", &nxps32k3->can1, TYPE_CAN);

    //clock
    nxps32k3->sysclk = qdev_init_clock_in(DEVICE(nxps32k3), "sysclk", NULL, NULL, 0);
    nxps32k3->refclk = qdev_init_clock_in(DEVICE(nxps32k3), "refclk", NULL, NULL, 0);
}


static const TypeInfo nxps32k3_mcu_info = {
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


