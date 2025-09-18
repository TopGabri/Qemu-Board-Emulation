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

_estack = ORIGIN(M7_0_SRAM) + LENGTH(M7_0_SRAM);

SECTIONS
{
	.isr_vector :
	{	
		__vector_table = .;
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} > M7_0_PFLASH
	
	.text :
	{
		*(.text)
		*(.rodata*)
		*(.constdata*)
		_etext = .;
	} > M7_0_PFLASH
	
	.data :
	{
		. = ALIGN(8);
		_data = .;
		_sdata = .;
		*(vtable)
		*(.data)
		_edata = .;
	} > M7_0_SRAM
	
	.bss :
	{
		. = ALIGN(8);
		_bss = .;
		_sbss = .;
		*(.bss)
		_ebss = .;
	} > M7_0_SRAM
	
	.heap :
	{
		. = ALIGN(8);
		PROVIDE ( end = .);
		PROVIDE ( _end = .);
		_heap_bottom = .;
		. = . + __HEAP_SIZE;
		_heap_top = .;
		. = . + __STACK_SIZE;
		. =  ALIGN(8);
	} > M7_0_SRAM
	
	__StackTop = ORIGIN(M7_0_SRAM) + LENGTH(M7_0_SRAM);
	__StackLimit = __StackTop - __STACK_SIZE;
	PROVIDE (__stack = __StackTop);

}
