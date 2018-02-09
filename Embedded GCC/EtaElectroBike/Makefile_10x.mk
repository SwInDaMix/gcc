STDPERIPH_DIR = _STM32F10x_StdPeriph
include ../$(STDPERIPH_DIR)/Makefile_stdperiph.mkinc
include Makefile_common.mkinc
#include ../_Common/Makefile_lto.mkinc

CROSS=arm-none-eabi-
PLATFORM=103re

BIN_DIR = bin_10x
EXTRN_OBJ_DIR = obj_10x_extrn
OBJ_DIR = obj_10x
TARGET = $(BIN_DIR)/EtaElectroBike_$(PLATFORM)
LDSCRIPT = ld_$(PLATFORM)
DEBUG =
OPTIM = -Os

TARGET_LNK = $(TARGET).elf

VERY_COMMON_FLAGS +=\
	-DSTM32F10X_HD \
	-mcpu=cortex-m3 \
	-mthumb \

C_COMMON_FLAGS +=\
	-msoft-float \

LINKER_FLAGS +=\
	-mfix-cortex-m3-ldrd \
	-msoft-float \
	-nostartfiles \
	-lc \
	-lgcc \
	-T $(LDSCRIPT).lds \

LST_FLAGS +=\
	-Mforce-thumb \

WARNINGS +=\
	-U__STRICT_ANSI__ \

REBUILD_FORCING_MAKEFILES += Makefile_10x.mk

include Platform/10x/Makefile_platform_10x.mkinc

.PHONY : all
all : build $(TARGET).bin $(TARGET).hex

$(TARGET).bin : $(TARGET_LNK)
	@$ $(OBJCOPY) $(TARGET_LNK) -O binary $(TARGET).bin

$(TARGET).hex : $(TARGET_LNK)
	@$ $(OBJCOPY) $(TARGET_LNK) -O ihex $(TARGET).hex

include ../_Common/Makefile_build.mkinc
