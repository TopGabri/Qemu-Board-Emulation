#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/register.h"
#include "hw/irq.h"
#include "qapi/error.h"
#include "qemu/bitops.h"
#include "qemu/log.h"
#include "qemu/cutils.h"
#include "hw/qdev-properties.h"
#include "net/can_emu.h"
#include "net/can_host.h"
#include "qemu/event_notifier.h"
#include "qom/object_interfaces.h"
#include "trace.h"
#include "hw/net/can.h"


static bool can_can_receive(CanBusClientState *client){
    printf("Can receive\n");
    client = client;
    return true;
}

static ssize_t can_receive(CanBusClientState *client,
                               const qemu_can_frame *buf, size_t buf_size){
    printf("Receive\n");
    
    CanState *s = container_of(client, CanState,
                                         bus_client);
    s=s;    //avoid make error

    const qemu_can_frame *frame = buf;

    qemu_canid_t can_id = frame->can_id;
    uint8_t can_dlc = frame->can_dlc;
    uint8_t flags = frame->flags;
    uint8_t data[64];
    memcpy(data, frame->data, sizeof(data));

    //DEBUG
    printf("CAN2: Received can_id:%0x can_dlc:%0x flags:%0x\n", can_id, can_dlc, flags);
    for(int i=0; i<can_dlc && i<8; i++){
        printf("data[%d] = %0x\n", i, data[i]);
    }


    //set status bits
     if (s->sr & RBS) {
        //RX buffer was occupied -> Data Overrun Error
        s->sr |= (DOS | ES); 
    }


    //transfer received data to the proper registers

    //RID (Received ID)
    s->rid = (can_id & QEMU_CAN_EFF_MASK);

    //RFI (Received Frame Info)
    
    s->rfi = (can_id & QEMU_CAN_EFF_FLAG)  | (can_id & QEMU_CAN_RTR_FLAG) | ((can_dlc << DLC_POS) & DLC_MASK);


    //RDA and RDB (Received Data A and Received Data B)
    s->rda = 0x00000000;
    s->rdb = 0x00000000;

    for (int i=0; i<can_dlc && i<8;i++) {
        if (i<4) {
            s->rda |= (data[i] << i*8); 
        } else {
            s->rdb |= (data[i] << ((i-4)*8));
        }
    }

    //set status bits

    s->sr |= RBS;  //a message is received by RX buffer and ready to be read
 
    
    //DEBUG
    printf("CAN 2: Receive Registers: RFI: 0x%x\tRID: 0x%x\tRDA: 0x%x\tRDB: 0x%x\tSR: 0x%x\n", s->rfi, s->rid, s->rda, s->rdb, s->sr);


    
    return 0;
}

static uint64_t can_read(void *opaque, hwaddr addr,
                                       unsigned int size){
    CanState *s = (CanState*) opaque;

    uint32_t retvalue = 0;
    static int read = 0;    //variable used to set RBS to 0 when both RDA and RDB are read

    switch (addr) {
        case TFI:
            //write-only
            break;
        case TID:
            //write-only
            break;        
        case TDA:
            //write-only
            break;
        case TDB:   
            //write-only
            break;
        case RFI:
            retvalue = s->rfi;
            printf("Reading Receive Frame Info (RFI) register: 0x%x\tread=%d\n", retvalue,read);
            break;
        case RID:
            retvalue = s->rid;
            printf("Reading Receive ID (RID) register: 0x%x\tread=%d\n", retvalue,read);
            break;
        case RDA:
            retvalue = s->rda;
            printf("Reading Receive Data A (RDA) register: 0x%x\tread=%d\n", retvalue,read);
            break;
        case RDB:
            retvalue = s->rdb;
            printf("Reading Receive Data B (RDB) register: 0x%x\tread=%d\n", retvalue,read);
            break;
        case SR: 
            retvalue = s->sr;
            break;
    }

    return (uint64_t) retvalue; //MemoryRegionOps expects uint64_t
}

static void can_transmit(CanState *s) {


    qemu_canid_t can_id = (qemu_canid_t) 0;
    uint8_t can_dlc = 0;

    can_id |=  s->tfi & RTR;   //RTR flag
    can_id |= s->tfi & FF;    //FF flag

    //the id written in TID is masked according to the frame format (standard or extended)
    if (s->tfi & FF) {
        can_id |= s->tid & IDS;    //standard frame format
    } else {
        can_id |= s->tid & IDE;    //extended frame format
    }

    //get DLC from TFI only if RTR=0 (data frame), otherwise DLC is 0
    if (!(s->tfi & RTR)) 
        can_dlc = (uint8_t) ((s->tfi & DLC_MASK) >> DLC_POS); //DLC   


    //copy data in TDA and TDB to a buffer of same type of frame.data (uint8_t [64])

    qemu_can_frame frame = {
        .can_id = can_id,
        .can_dlc = can_dlc,
        .flags = 0x00,
    };

    
    for (int i=0; i<can_dlc && i<8;i++) {
        if (i<4) {
            frame.data[i] = (s->tda >> i*8) & 0xFF;
        } else {
            frame.data[i] = (s->tdb >> (i-4)*8) & 0xFF;
        }
    }

    printf("CAN 1: Transmitting frame...can_id: 0x%x\tcan_dlc: 0x%x\tdata: ", frame.can_id, frame.can_dlc);
    for (int i=7; i>=0; i--) {
        printf("0x%x  ", frame.data[i]);
    }
    printf("\n");


    can_bus_client_send(&s->bus_client, &frame, 1);

    //update status bits
    s->sr |= TCS;
}


static void can_write(void *opaque, hwaddr addr,
                                  uint64_t val, unsigned int size){
    CanState *s = (CanState*) opaque;

    switch (addr) {
        case TFI:
            printf("Writing Transmit Frame Info (TFI) register with value 0x%lx\n", val);
            s->tfi = (uint32_t) val;
            break;
        case TID:
            printf("Writing Transmit Identifier (TID) register with value 0x%lx\n", val);
            if (s->tfi & FF) {
                s->tid = (uint32_t) (val & IDS);    //standard frame format
            } else {
                s->tid = (uint32_t) (val & IDE);    //extended frame format
            } 
            break;        
        case TDA:
            printf("Writing Transmit Data A (TDA) register with value 0x%lx\n", val);
            s->tda = (uint32_t) val;
            break;
        case TDB:
            printf("Writing Transmit Data B (TDB) register with value 0x%lx\n", val);
            s->tdb = (uint32_t) val;
            break;
        case RFI:
            //read-only
            break;
        case RID:
            //read-only
            break;
        case RDA:
            //read-only
            break;
        case RDB:
            //read-only
            break;
        case SR: 
            //read-only
            break;
        case CMR:
            if (val & RRB) {   //Release Receive Buffer
                printf("Releasing Receive Buffer\n");
                s->sr &= ~RBS;   //clear Receive Buffer Status
                s->cmr &= ~RRB;  //clear Release Receive Buffer
            }
            if (val & CDO) {   //Clear Data Overrun
                printf("Clearing Data Overrun Status\n");
                s->sr &= ~DOS;   //clear Data Overrun Status
                s->cmr &= ~CDO;  //clear Clear Data Overrun
            }
            if (val & TR) {    //Transmission Request
                printf("Sending Transmission Request\n");
                s->cmr &= ~TR;   //clear Transmission Request
                s->sr &= ~TCS;   //Clear Transmit Complete Status
                can_transmit(s);
            }
    }
    
    return;
}

static void can_reset(DeviceState *dev){
    CanState *s = CAN(dev);

    s-> tfi = 0x00000000;
    s-> tid = 0x00000000;
    s-> tda = 0x00000000;
    s-> tdb = 0x00000000;
    s-> rfi = 0x00000000;
    s-> rid = 0x00000000;
    s-> rda = 0x00000000;
    s-> rdb = 0x00000000;
    s-> sr = 0x00000000;
    s-> cmr = 0x00000000;

    qemu_irq_lower(s->irq);


    return;
}

static CanBusClientInfo can_bus_client_info = {
    .can_receive = can_can_receive,
    .receive = can_receive,
};

static int can_can_connect_to_bus(CanState *s,
                                          CanBusState *bus)
{
    s->bus_client.info = &can_bus_client_info;

    if (can_bus_insert_client(bus, &s->bus_client) < 0) {
        return -1;
    }
    return 0;
}

static void can_realize(DeviceState *dev, Error **errp)
{
    //printf("Realize\n");
    CanState *s = CAN(dev);
    
    if (s->canbus) {
        if (can_can_connect_to_bus(s, s->canbus) < 0) {
            //printf("Error connecting CAN to CAN bus\n");
            return;
        }
    }

}

static const MemoryRegionOps can_ops = {
    .read = can_read,
    .write = can_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property can_properties[] = {
    DEFINE_PROP_LINK("canbus", CanState, canbus, TYPE_CAN_BUS,
                     CanBusState *),
    DEFINE_PROP_END_OF_LIST(),
};

static void can_init(Object *obj)
{
    CanState *s = CAN(obj);
    //printf("Init\n");

    // Init interrupts
    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &can_ops, s,
                          TYPE_CAN, NREG*4);


    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void can_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    //printf("Class init\n");

    dc->reset = can_reset;
    device_class_set_props(dc, can_properties);
    dc->realize = can_realize;
}

static const TypeInfo can_info = {
    .name          = TYPE_CAN,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(CanState),
    .instance_init = can_init,
    .class_init    = can_class_init,
};

static void can_register_types(void)
{
    type_register_static(&can_info);
}

type_init(can_register_types)