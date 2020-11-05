rem PATH = %PATH%;c:\Program Files (x86)\STMicroelectronics\st_toolset\stvp
rem STVP_CmdLine -BoardName=ST-LINK -ProgMode=SWIM -Port=USB -Device=STM32S030x6 -FileProg=bin_0xx\EtaReverseOsmosis_030k6.hex -verbose -no_loop

PATH = %PATH%;c:\gcc\OpenOCD\bin;c:\gcc\OpenOCD\share\openocd
openocd -f interface/stlink-v2.cfg -f target/stm32f0x.cfg -c "init" -c "reset init" -c "flash write_image erase bin_0xx/EtaReverseOsmosis_030k6.hex" -c "reset" -c "shutdown"
