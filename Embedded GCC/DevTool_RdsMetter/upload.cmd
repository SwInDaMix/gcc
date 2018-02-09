@echo off
PATH=C:\gcc\avreal;%PATH%

avreal.exe -as -p1 +mega48pa -c bin/RdsMetter.hex -e -w -v

