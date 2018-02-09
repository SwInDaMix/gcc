/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : ld_030c8_governor.lds
 ***********************************************************************/

_serverdisk_length = 8K;
_stack_length = 3K;

MEMORY {
  rom (rx) : ORIGIN = 0x08000000, LENGTH = 64K - _serverdisk_length
  serverdisk (rx) : ORIGIN = ORIGIN(rom) + LENGTH(rom), LENGTH = _serverdisk_length
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 8K - _stack_length
} 

SecurityServerDisk = ORIGIN(serverdisk);
SecurityServerDisk_End = ORIGIN(serverdisk) + LENGTH(serverdisk);

_estack = ORIGIN(ram) + LENGTH(ram) + _stack_length;

ENTRY(Reset_Handler)

SECTIONS {
	.text : {
		. = ALIGN(4);
		KEEP(*(.isr_vectors))
		*(.text*)
		*(.rodata*)
		. = ALIGN(4);
	} >rom
	.ARM.exidx : { 
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} >rom

	.preinit_array : {
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} >rom
	.init_array : {
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} >rom

	_sidata = .;
 	.data : {
		. = ALIGN(4);
		_sdata = .;
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} >ram AT >rom

	.bss(NOLOAD) : {
		. = ALIGN(4);
		_sbss = .;
		*(.bss*)
		. = ALIGN(4);
		_ebss = .;
	} >ram

	_sheap = .;
	_eheap = _estack - _stack_length;
}  
