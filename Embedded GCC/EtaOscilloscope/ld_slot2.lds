/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : ld_slot2.lds
 ***********************************************************************/

MEMORY
{
  rom (rx) : ORIGIN = 0x08014000, LENGTH = 96K
  ram (rwx) : ORIGIN = 0x20003000, LENGTH = 36K
}

INCLUDE ld_main.lds
