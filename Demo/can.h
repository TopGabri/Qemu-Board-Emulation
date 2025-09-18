#ifndef CAN_H
    #define CAN_H

    #include "FreeRTOS.h"
    #include "ARMCM7.h"

    #define CAN0_ADDRESS    ( 0x40304000UL )
    #define CAN1_ADDRESS    ( 0x40308000UL )

    #define CAN0_TFI    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 0UL ) ) ) )
    #define CAN0_TID    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 4UL ) ) ) )    
    #define CAN0_TDA    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 8UL ) ) ) )
    #define CAN0_TDB    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 12UL ) ) ) )
    #define CAN0_RFI    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 16UL ) ) ) )
    #define CAN0_RID    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 20UL ) ) ) )
    #define CAN0_RDA    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 24UL ) ) ) )
    #define CAN0_RDB    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 28UL ) ) ) )
    #define CAN0_SR     ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 32UL ) ) ) )
    #define CAN0_CMR    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 36UL ) ) ) )
    #define CAN0_IER    ( *( ( ( volatile uint32_t * ) ( CAN0_ADDRESS + 40UL ) ) ) )


    #define CAN1_TFI    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 0UL ) ) ) )
    #define CAN1_TID    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 4UL ) ) ) )    
    #define CAN1_TDA    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 8UL ) ) ) )
    #define CAN1_TDB    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 12UL ) ) ) )
    #define CAN1_RFI    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 16UL ) ) ) )
    #define CAN1_RID    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 20UL ) ) ) )
    #define CAN1_RDA    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 24UL ) ) ) )
    #define CAN1_RDB    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 28UL ) ) ) )
    #define CAN1_SR     ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 32UL ) ) ) )
    #define CAN1_CMR    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 36UL ) ) ) )
    #define CAN1_IER    ( *( ( ( volatile uint32_t * ) ( CAN1_ADDRESS + 40UL ) ) ) )


    #define TFI 0x00    //Transmit Frame Info 
    #define TID 0x04    //Transmit ID
    #define TDA 0x08    //Transmit Data A
    #define TDB 0x0C    //Transmit Data B
    #define RFI 0x10    //Receive Frame Info
    #define RID 0x14    //Receive ID
    #define RDA 0x18    //Receive Data A   
    #define RDB 0x1C    //Receive Data B
    #define SR  0x20    //Status Register
    #define CMR 0x24    //Command Register   
    #define IER 0x28    //Interrupt Enable Register

    //TFI/RFI bits
    #define DLC_POS 16                  //Data Length Code Position
    #define DLC_MASK (0xF << DLC_POS)   //Data Length Code Mask 
    #define RTR (1 << 30)
    #define FF (1 << 31)


    //TID/RID bits
    #define IDS 0x7FF       //ID Standard (11-bits)
    #define IDE 0x1FFFFFFFF //ID Extended (28-bits)

    //SR bits
    #define RBS (1 << 0)    //Receive Buffer Status
    #define DOS (1 << 1)    //Data Overrun Status
    #define TBS (1 << 2)    //Transmit Buffer Status
    #define TCS (1 << 3)    //Transmit Complete Status
    #define RS  (1 << 4)    //Receive Status
    #define TS  (1 << 5)    //Transmit Status
    #define ES  (1 << 6)    //Error Status

    //CMR bits
    #define TR (1 << 0)     //Transmission Request
    #define RRB (1 << 2)    //Release Receive Buffer    
    #define CDO (1 << 3)    //Clear Data Overrun

    //IER bits
    #define RIE (1 << 0)    //Receive Interrupt Enable
    #define TIE (1 << 1)    //Transmit Interrupt Enable
    #define DOIE (1 << 3)   //Data Overrun Interrupt Enable


    void CAN_init(int can_number);
    void CAN_write(int can_number, int can_id, char *data, int can_dlc, int is_extended_id, int is_remote_frame);
    void CAN_transmit(int can_number);
    int CAN_has_received(int can_number);
    int CAN_read_DLC(int can_number);
    int CAN_read_ID(int can_number);
    int CAN_read_data(int can_number, char *data);
    int CAN_read_status_bit(int can_number, int status_bit);
    void CAN_release_receive_buffer(int can_number);
    void CAN_clear_data_overrun(int can_number);
    void CAN_clear_interrupt(int can_number);
    void CAN0_Handler(void);
    void CAN1_Handler(void);

#endif
