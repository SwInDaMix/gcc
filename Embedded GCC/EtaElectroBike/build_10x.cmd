@echo off
PATH=C:\gcc\arm-none-eabi\bin;C:\gcc\msys\1.0\bin;C:\gcc\misc-tools;%PATH%

make.exe %1 -f Makefile_10x.mk
