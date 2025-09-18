__STACK_SIZE = 0x00001000;
__HEAP_SIZE  = 0x0000F000;

MEMORY
{
  ITCM            (RWX) : ORIGIN = 0x00000000, LENGTH = 0x10000
  DTCM             (RW) : ORIGIN = 0x20000000, LENGTH = 0x20000

  M7_0_PFLASH (RX)      : ORIGIN = 0x00400000, LENGTH = 0x200000
  M7_0_DFLASH (RX)      : ORIGIN = 0x10000000, LENGTH = 0xA000
  M7_0_SRAM   (RW)      : ORIGIN = 0x20400000, LENGTH = 0x40000

  M7_1_PFLASH (RX)      : ORIGIN = 0x00600000, LENGTH = 0x200000
  M7_1_DFLASH (RX)      : ORIGIN = 0x1000C000, LENGTH = 0xA000
  M7_1_SRAM   (RW)      : ORIGIN = 0x20440000, LENGTH = 0x40000
  
  M7_2_PFLASH (RX)      : ORIGIN = 0x00800000, LENGTH = 0x400000
  M7_2_DFLASH (RX)      : ORIGIN = 0x10016000, LENGTH = 0xA000
  M7_2_SRAM   (RW)      : ORIGIN = 0x20480000, LENGTH = 0x40000
}
 
ENTRY(Reset_Handler)

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors))
    *(.text*)
    
    KEEP(*(.init))
    KEEP(*(.fini))
    *(.rodata*)
  } > M7_0_PFLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > M7_0_PFLASH

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > M7_0_PFLASH
  __exidx_end = .;

  .ecc.table :
  {
    . = ALIGN(4);
    __ecc_table_start__ = .;

    QUAD (__data_start__)
    QUAD ((__data_end__ - __data_start__) / 8)

    QUAD (__bss_start__)
    QUAD ((__bss_end__ - __bss_start__) / 8)

    QUAD (__HeapTop)
    QUAD ((__HeapLimit - __HeapTop) / 8)

    QUAD (__StackLimit)
    QUAD ((__StackTop - __StackLimit) / 8)

    QUAD (ORIGIN(ITCM))
    QUAD (LENGTH(ITCM) / 8)

    QUAD (ORIGIN(DTCM))
    QUAD (LENGTH(DTCM) / 8)

    __ecc_table_end__ = .;
  } > M7_0_PFLASH

  .copy.table :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;

    LONG (__etext)
    LONG (__data_start__)
    LONG ((__data_end__ - __data_start__) / 4)

    /* Add each additional data section here */
    /*
      LONG (__etext2)
      LONG (__data2_start__)
      LONG ((__data2_end__ - __data2_start__) / 4)
    */
    __copy_table_end__ = .;
  } > M7_0_PFLASH

  .zero.table :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;
    /* Add each additional bss section here */
    /*
      LONG (__bss2_start__)
      LONG ((__bss2_end__ - __bss2_start__) / 4)
    */
    __zero_table_end__ = .;
  } > M7_0_PFLASH

  __etext = ALIGN(8);

  .boot_header :
  {
    KEEP(*(.boot_header))
  } > M7_0_DFLASH

  /* Due ECC initialization sequence __data_start__ and __data_end__ should be aligned on 8 bytes */
  .data : AT (__etext)
  {
    . = ALIGN(8);
    __data_start__ = .;
    *(vtable)
    *(.data)
    *(.data.*)

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))
    . = ALIGN(8);
    /* All data end */
    __data_end__ = .;

  } > M7_0_SRAM

  /* Due ECC initialization sequence __bss_start__ and __bss_end__ should be aligned on 8 bytes */
  .bss :
  {
    . = ALIGN(8);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(8);
    __bss_end__ = .;
  } > M7_0_SRAM

  /* Due ECC initialization sequence __HeapTop and __HeapLimit should be aligned on 8 bytes */
  .heap (COPY):
  {
    . = ALIGN(8);
    __HeapTop = .;
    __end__ = .;
    _end = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > M7_0_SRAM

  /* Due ECC initialization sequence __StackLimit and __StackTop should be aligned on 8 bytes */
  .stack (ORIGIN(M7_0_SRAM) + LENGTH(M7_0_SRAM) - __STACK_SIZE) (COPY) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > M7_0_SRAM
  PROVIDE(__stack = __StackTop);
}
