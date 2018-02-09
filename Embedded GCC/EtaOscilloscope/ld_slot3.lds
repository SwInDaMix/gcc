/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : ld_slot3.lds
 ***********************************************************************/

MEMORY
{
  rom (rx) : ORIGIN = 0x0801C000, LENGTH = 64K
  ram (rwx) : ORIGIN = 0x20003000, LENGTH = 36K
}

INCLUDE ld_main.lds
