export PATH="$PATH:/opt/gcc-arm-none-eabi-7-2017-q4-major/bin:/opt/misctools"

make $1 -f Makefile_native.mk -j12
