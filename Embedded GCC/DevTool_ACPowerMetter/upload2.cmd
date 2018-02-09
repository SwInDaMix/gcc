@echo off
PATH=C:\gcc\avrdude;%PATH%

avrdude.exe -v -c pony-stk200 -p m328p -U flash:w:"bin/ACPowerMetter.elf"
