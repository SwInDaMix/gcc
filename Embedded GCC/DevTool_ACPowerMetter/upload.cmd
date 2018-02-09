@echo off
PATH=C:\gcc\avreal;%PATH%

avreal.exe -as -p1 +mega328p -c bin/ACPowerMetter.hex -e -w -v
