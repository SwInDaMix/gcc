export PATH="$PATH:/opt/avr-gcc-8.1.0-x64-linux/bin"

make $1 -f Makefile.mk -j12
