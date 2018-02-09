@echo off
PATH=C:\gcc\avreal;%PATH%

avreal.exe -as -p1 +90s2313 -c bin/UartLogger.hex -e -w -v

