@echo off
PATH=C:\gcc\x86_64-w64-mingw32\bin;C:\gcc\msys\1.0\bin;C:\gcc\misc-tools;%PATH%

make.exe %1 -f Makefile_windows_emulator_x64.mk
