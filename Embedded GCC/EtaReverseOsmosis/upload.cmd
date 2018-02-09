@echo off
stm32flash.exe -b 115200 -w bin_0xx\EtaReverseOsmosis_030k6.bin -S 0x08000000:16384 COM4
