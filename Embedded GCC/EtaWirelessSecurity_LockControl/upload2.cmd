@echo off
PATH=C:\gcc\avrdude;%PATH%

avrdude.exe -v -c usbasp -p t13 -U flash:w:"bin/EtaWirelessSecurity_LockControl.elf" -U lfuse:w:0x6A:m
