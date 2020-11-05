/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : ld_030k6.lds
 ***********************************************************************/

_persistent_storage_length = 4K;
_estack_length = 1K;

MEMORY {
  rom (rx) : ORIGIN = 0x08000000, LENGTH = 32K - _persistent_storage_length
  persistent_storage (rx) : ORIGIN = ORIGIN(rom) + LENGTH(rom), LENGTH = _persistent_storage_length
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 4K - _estack_length
}

PersistentStorage = ORIGIN(persistent_storage);
PersistentStorage_End = ORIGIN(persistent_storage) + LENGTH(persistent_storage);

_estack = ORIGIN(ram) + LENGTH(ram) + _estack_length;

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
}  
