@echo off
stm32flash.exe -b 115200 -r firmfull.bin -S 0x08000000:262144 COM7
