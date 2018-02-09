/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : ld_slot4.lds
 ***********************************************************************/

MEMORY
{
  rom (rx) : ORIGIN = 0x08024000, LENGTH = 32K
  ram (rwx) : ORIGIN = 0x20003000, LENGTH = 36K
}

INCLUDE ld_main.lds
