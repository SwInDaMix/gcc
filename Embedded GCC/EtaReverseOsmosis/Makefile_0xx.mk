STDPERIPH_DIR = _STM32F0xx_StdPeriph
include ../$(STDPERIPH_DIR)/Makefile_stdperiph.mkinc
include Makefile_common.mkinc
#include ../_Common/Makefile_lto.mkinc

CROSS=arm-none-eabi-
PLATFORM=030k6

BIN_DIR = bin_0xx
EXTRN_OBJ_DIR = obj_0xx_extrn
OBJ_DIR = obj_0xx
TARGET = $(BIN_DIR)/EtaReverseOsmosis_$(PLATFORM)
LDSCRIPT = ld_$(PLATFORM)
DEBUG =
OPTIM = -O1

TARGET_LNK = $(TARGET).elf

VERY_COMMON_FLAGS +=\
	-DSTM32F030 \
	-mcpu=cortex-m0 \
	-mthumb \

C_COMMON_FLAGS +=\
	-msoft-float \

LINKER_FLAGS +=\
	-msoft-float \
	-nostartfiles \
	-lc \
	-lgcc \
	-T $(LDSCRIPT).lds \

LST_FLAGS +=\
	-Mforce-thumb \

WARNINGS +=\
	-U__STRICT_ANSI__ \

REBUILD_FORCING_MAKEFILES += Makefile_0xx.mk

include Platform/0xx/Makefile_platform_0xx.mkinc

.PHONY : all
all : build $(TARGET).bin $(TARGET).hex

$(TARGET).bin : $(TARGET_LNK)
	@$ $(OBJCOPY) $(TARGET_LNK) -O binary $(TARGET).bin

$(TARGET).hex : $(TARGET_LNK)
	@$ $(OBJCOPY) $(TARGET_LNK) -O ihex $(TARGET).hex

include ../_Common/Makefile_build.mkinc
