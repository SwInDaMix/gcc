@echo off
PATH=C:\gcc\avreal;%PATH%

avreal.exe -as -p1 +mega48pa -c bin/SafeCodeLock.hex -e -w -v

