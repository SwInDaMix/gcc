PATH = %PATH%;c:\gcc\misc-tools

STVP_CmdLine -BoardName=ST-LINK -ProgMode=SWIM -Port=USB -Device=STM8S105x6 -FileProg=bin\EtaElectroBikeKTLCD3Custom.hex -verbose -no_loop
