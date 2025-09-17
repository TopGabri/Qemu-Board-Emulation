#include "can.h"
#include <stdint.h>


    void CAN_init(void) {
        
    }


    void CAN_write(int can_number, int can_id, const char *data, int can_dlc, int is_extended_id, int is_remote_frame){

        switch (can_number) {
            case 0:
                CAN0_TID = (uint32_t) can_id;
                CAN0_TFI = (uint32_t) (((is_extended_id & 0x1) << 31) | ((is_remote_frame & 0x1) << 30) | ((can_dlc & 0xF) << DLC_POS));
                CAN0_TDA = (uint32_t) (data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
                CAN0_TDB = (uint32_t) (data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24));
                break;
            case 1:
                CAN1_TID = (uint32_t) can_id;
                CAN1_TFI = (uint32_t) ((can_dlc & 0xF) << DLC_POS);
                CAN1_TDA = (uint32_t) (data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
                CAN1_TDB = (uint32_t) (data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24));
                break;
            default:
                //invalid can_number
                break;
        }
    }

    void CAN_transmit(int can_number){
        switch (can_number) {
            case 0:
                CAN0_CMR |= TR; //set TR bit to request transmission
                break;
            case 1:
                CAN1_CMR |= TR; //set TR bit to request transmission
                break;
            default:
                //invalid can_number
                break;
        }
    }

    int CAN_has_received(int can_number){
        switch (can_number) {
            case 0:
                if (CAN0_SR & RBS) { //if RBS = 1, a frame has been received
                    return 1;
                }
                break;
            case 1:
                if (CAN1_SR & RBS) { //if RBS = 1, a frame has been received
                    return 1;
                }
                break;
            default:
                //invalid can_number
                break;
        }
        return 0;
    }

    int CAN_read_DLC(int can_number){
        int dlc = 0;
        switch (can_number) {
            case 0:
                dlc = (CAN0_RFI & DLC_MASK) >> DLC_POS;
                break;
            case 1:
                dlc = (CAN1_RFI & DLC_MASK) >> DLC_POS;
                break;
            default:
                //invalid can_number
                break;
        }
        return dlc;
    }


    int CAN_read_ID(int can_number){
        int id = 0;
        switch (can_number) {
            case 0:
                if (CAN0_RFI & FF) { //standard frame format
                    id = CAN0_RID & IDS;
                } else {          //extended frame format
                    id = CAN0_RID & IDE;
                }
                break;
            case 1:
                if (CAN1_RFI & FF) { //standard frame format
                    id = CAN1_RID & IDS;
                } else {          //extended frame format
                    id = CAN1_RID & IDE;
                }
                break;
            default:
                //invalid can_number
                break;
        }
        return id;
    }


    int CAN_read_data(int can_number, char *data){
        int dlc = 0;
        switch (can_number) {
            case 0:
                dlc = (CAN0_RFI & DLC_MASK) >> DLC_POS;
                for (int i=0; i<dlc && i<8; i++) {
                    if (i<4) {
                        data[i] = (CAN0_RDA >> (i*8)) & 0xFF;
                    } else {
                        data[i] = (CAN0_RDB >> ((i-4)*8)) & 0xFF;
                    }
                }
                break;
            case 1:
                dlc = (CAN1_RFI & DLC_MASK) >> DLC_POS;
                for (int i=0; i<dlc && i<8; i++) {
                    if (i<4) {
                        data[i] = (CAN1_RDA >> (i*8)) & 0xFF;
                    } else {
                        data[i] = (CAN1_RDB >> ((i-4)*8)) & 0xFF;
                    }
                }
                break;
            default:
                //invalid can_number
                return -1;

        }
        return dlc;
    }

    int CAN_read_status_bit(int can_number, int status_bit){
        int status = 0;
        switch (can_number) {
            case 0:
                status = (CAN0_SR & status_bit) ? 1 : 0;
                break;
            case 1:
                status = (CAN1_SR & status_bit) ? 1 : 0;
                break;
            default:
                //invalid can_number
                break;
        }
        return status;
    }


    void CAN_release_receive_buffer(int can_number){
        switch (can_number) {
            case 0:
                CAN0_CMR |= RRB; //set RRB bit to release receive buffer
                break;
            case 1:
                CAN1_CMR |= RRB; //set RRB bit to release receive buffer
                break;
            default:
                //invalid can_number
                break;
        }
    }
    void CAN_clear_data_overrun(int can_number){
        switch (can_number) {
            case 0:
                CAN0_CMR |= CDO; //set CDO bit to clear data overrun
                break;
            case 1:
                CAN1_CMR |= CDO; //set CDO bit to clear data overrun
                break;
            default:
                //invalid can_number
                break;
        }
    }



    void CAN_clear_interrupt(void);
