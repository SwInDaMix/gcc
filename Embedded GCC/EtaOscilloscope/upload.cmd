@echo off
stm32flash.exe -b 115200 -w bin_arm\EtaOscl3.bin -S 0x0801C000:49152 COM7
