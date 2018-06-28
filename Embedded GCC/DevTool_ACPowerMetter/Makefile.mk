include Makefile_common.mkinc
include ../_Common/Makefile_lto.mkinc

CROSS=avr-
PLATFORM=NATIVE

BIN_DIR = bin
EXTRN_OBJ_DIR = obj_extrn
PRE_OBJ_DIR = obj_pre
OBJ_DIR = obj
TARGET = $(BIN_DIR)/ACPowerMetter
DEBUG =
OPTIM = -Os

TARGET_LNK = $(TARGET).elf

VERY_COMMON_FLAGS +=\
	-mmcu=atmega328p \

C_COMMON_FLAGS +=\
	-fpack-struct \
	-fshort-enums \

AS_FLAGS +=\

C_FLAGS +=\

CPP_FLAGS +=\

LINKER_FLAGS +=\

LST_FLAGS +=\

WARNINGS +=\
	-U__STRICT_ANSI__ \

REBUILD_FORCING_MAKEFILES += Makefile.mk

.PHONY : all
all : build $(TARGET).bin $(TARGET).hex

$(TARGET).bin : $(TARGET_LNK)
	@$ $(OBJCOPY) $(TARGET_LNK) -j .text -j .data -O binary $(TARGET).bin

$(TARGET).hex : $(TARGET_LNK)
	@$ $(OBJCOPY) $(TARGET_LNK) -j .text -j .data -O ihex $(TARGET).hex

include ../_Common/Makefile_build.mkinc
