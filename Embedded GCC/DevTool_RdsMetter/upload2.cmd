@echo off
PATH=C:\gcc\avrdude;%PATH%

avrdude.exe -v -c usbasp -p m48p -U flash:w:"bin/RdsMetter.elf"
