/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : ld_slot1.lds
 ***********************************************************************/

MEMORY
{
  rom (rx) : ORIGIN = 0x0800C000, LENGTH = 128K
  ram (rwx) : ORIGIN = 0x20003000, LENGTH = 36K
}

INCLUDE ld_main.lds
