@echo off
PATH=%PATH%;C:\gcc\sdcc\bin;C:\cygwin64\bin

make.exe %1 -f Makefile.mk -j12
