@echo off
PATH=C:\gcc\avr\bin;C:\gcc\msys\1.0\bin;%PATH%

make.exe %1 -f Makefile.mk
