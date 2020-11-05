@echo off
PATH=C:\gcc\arm-none-eabi\bin;C:\cygwin64\bin;C:\gcc\misc-tools;%PATH%

make.exe %1 -f Makefile_native.mk
