@echo off
PATH=C:\gcc\avreal;%PATH%

avreal.exe -as -p1 +tiny13a -c bin/EtaElectroBike_BatteryCellController.hex -e -w -v
