/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : ld_main.lds
 ***********************************************************************/

_estack = ORIGIN(ram) + LENGTH(ram);
_estack_length = 4K;

ENTRY(Reset_Handler)

SECTIONS {
	.bios 0x08004200 (NOLOAD) : {
		obj_arm/Platform/Native/IntBIOS.o
	}

	.text : {
		. = ALIGN(4);
		_vectors = .;
		KEEP(**(.isr_vectors))
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
	} >ram AT >rom
	
	_sheap = .;
	_eheap = _estack - _estack_length;
}  
