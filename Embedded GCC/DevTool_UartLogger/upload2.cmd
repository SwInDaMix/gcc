@echo off
PATH=C:\gcc\avrdude;%PATH%

avrdude.exe -v -c usbasp -p 2313 -U flash:w:"bin/UartLogger.elf"
