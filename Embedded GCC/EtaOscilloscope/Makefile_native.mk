include Makefile_common.mkinc
CMSIS_DIR = _CMSIS
STDPERIPH_DIR = _STM32F10x_StdPeriph
USBLIB_DIR = _USBLib
ETA32_DIR = _Eta32
include ../$(CMSIS_DIR)/Makefile_cmsis.mkinc
include ../$(STDPERIPH_DIR)/Makefile_stdperiph.mkinc
include ../$(USBLIB_DIR)/Makefile_usblib.mkinc
include ../$(ETA32_DIR)/Makefile_eta32.mkinc
#include ../_Common/Makefile_lto.mkinc

CROSS=arm-none-eabi-
PLATFORM=NATIVE

SLOT = 3
BIN_DIR = bin_arm
EXTRN_OBJ_DIR = obj_arm_extrn
OBJ_DIR = obj_arm
TARGET = $(BIN_DIR)/EtaOscl$(SLOT)
LDSCRIPT = ld_slot$(SLOT)
DEBUG =
OPTIM = -Os

TARGET_LNK = $(TARGET).elf

VERY_COMMON_FLAGS +=\
	-DUSE_STDPERIPH_DRIVER \
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

REBUILD_FORCING_MAKEFILES += Makefile_native.mk

include Platform/Native/Makefile_platform_native.mkinc

.PHONY : all
all : build $(TARGET).bin $(TARGET).hex

$(TARGET).bin : $(TARGET_LNK)
	@$ $(OBJCOPY) $(TARGET_LNK) -O binary $(TARGET).bin

$(TARGET).hex : $(TARGET_LNK)
	@$ $(OBJCOPY) $(TARGET_LNK) -O ihex $(TARGET).hex

include ../_Common/Makefile_build.mkinc
