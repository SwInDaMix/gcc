@echo off
PATH=C:\gcc\avrdude;%PATH%

avrdude.exe -v -c usbasp -p m328p -U flash:w:"bin/LifepoCharger.elf"
