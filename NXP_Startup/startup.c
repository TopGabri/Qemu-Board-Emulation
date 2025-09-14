#include <stdint.h>

typedef void( *pFunc )( void );

extern int main(void);
static void Default_Handler(void) __attribute__((naked));
void Reset_Handler(void) __attribute__((naked));

extern uint32_t _estack;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* ARM CORE Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
const uint32_t* isr_vector[241] __attribute__((section(".isr_vector"),used)) =
{
  (uint32_t *) &_estack,       /*     Initial Stack Pointer */
  (uint32_t *) Reset_Handler,                  /*     Initial Program Counter: Reset Handler */
  (uint32_t *) NMI_Handler,                    /* -14 NMI Handler */
  (uint32_t *) HardFault_Handler,              /* -13 Hard Fault Handler */
  (uint32_t *) MemManage_Handler,              /* -12 MPU Fault Handler */
  (uint32_t *) BusFault_Handler,               /* -11 Bus Fault Handler */
  (uint32_t *) UsageFault_Handler,             /* -10 Usage Fault Handler */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  (uint32_t *) SVC_Handler,                    /*  -5 SVCall Handler */
  (uint32_t *) DebugMon_Handler,               /*  -4 Debug Monitor Handler */
  0,                                        /*     Reserved */
  (uint32_t *) PendSV_Handler,                 /*  -2 PendSV Handler */
  (uint32_t *) SysTick_Handler,                /*  -1 SysTick Handler */
};

/*----------------------------------------------------------------------------
  Reset Handler
*----------------------------------------------------------------------------*/
void Reset_Handler(void)
{	
	//__asm volatile ("LDR SP, =_estack\n");
	main();
	__asm volatile (
		"MOV r5, #0\n"
		"MOV r6, #3\n"
		"ADD r7, r5, r6\n"
	);
	
	while (1) {}
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
	while (1) {}
}
