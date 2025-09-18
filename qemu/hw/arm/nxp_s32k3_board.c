#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "hw/arm/boot.h"
#include "qom/object.h"
#include "hw/boards.h"
#include "hw/arm/nxp_s32k3.h"

typedef struct {
    MachineState parent_obj;
    NXPS32K3McuState mcu;
} NXPS32K3BoardMachineState;


typedef struct {
    MachineClass parent_class;
} NXPS32K3BoardMachineClass;

#define TYPE_NXPS32K3_BOARD_BASE_MACHINE MACHINE_TYPE_NAME("nxps32k3-board-base")
#define TYPE_NXPS32K3_BOARD_MACHINE MACHINE_TYPE_NAME ("nxps32k3-board")

DECLARE_OBJ_CHECKERS(NXPS32K3BoardMachineState, NXPS32K3BoardMachineClass, NXPS32K3_BOARD_MACHINE, TYPE_NXPS32K3_BOARD_MACHINE)

static void nxps32k3_board_init(MachineState *machine){
    DeviceState *dev;
    dev = qdev_new(TYPE_NXPS32K3_MCU);
    object_property_add_child(OBJECT(machine), "mcu", OBJECT(dev));
    
    // printf ("setting up board...\n");
    
    //clock setup
    Clock *sysclk;
    sysclk= clock_new(OBJECT(machine), "SYSCLK");
    clock_set_hz(sysclk, SYSCLK_FRQ);
    qdev_connect_clock_in(dev, "sysclk", sysclk);
    
    //CPU SETUP
    sysbus_realize(SYS_BUS_DEVICE(dev),&error_abort);
    
    //LOAD KERNEL HERE
    armv7m_load_kernel(NXPS32K3_MCU(dev)->armv7m[0].cpu, machine->kernel_filename, 0x00400000, PFLASH_SIZE);
                       
    // printf("Board setup complete\n");
}

static void nxps32k3_board_class_init(ObjectClass *oc,const void* data){

    static const char * const valid_cpu_types[] = {
        ARM_CPU_TYPE_NAME("cortex-m7"),
        NULL
    };

    MachineClass *mc = MACHINE_CLASS(oc);
    
    mc->desc = "NXP S32K3 Board";
    mc->init = nxps32k3_board_init;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_cpus = mc->min_cpus = mc->max_cpus = 3;
    mc->no_floppy = 1;
    mc->no_cdrom = 1;
    mc->no_parallel = 1;
}

static const TypeInfo nxps32k3_board_machine_types[] = {
    {
        .name            = TYPE_NXPS32K3_BOARD_MACHINE,
        .parent          = TYPE_MACHINE,
        .instance_size   = sizeof(NXPS32K3BoardMachineState),
        .class_size      = sizeof(NXPS32K3BoardMachineClass),
        .class_init      = nxps32k3_board_class_init
    
    }
};

DEFINE_TYPES(nxps32k3_board_machine_types)
