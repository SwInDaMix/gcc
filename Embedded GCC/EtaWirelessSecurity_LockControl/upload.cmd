@echo off
PATH=C:\gcc\avreal;%PATH%

avreal.exe -as -p1 +tiny13a -c bin/EtaWirelessSecurity_LockControl.hex -e -w -v
