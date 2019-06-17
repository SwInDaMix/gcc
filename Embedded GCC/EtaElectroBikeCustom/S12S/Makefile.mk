STDPERIPH_DIR = _STM8S10x_StdPeriph
include ../$(STDPERIPH_DIR)/Makefile_stdperiph.mkinc
include Makefile_common.mkinc

CROSS=sd
ARCH=stm8
PLATFORM=NATIVE

BIN_DIR = bin
EXTRN_OBJ_DIR = obj_extrn
PRE_OBJ_DIR = obj_pre
OBJ_DIR = obj
TARGET = $(BIN_DIR)/EtaElectroBikeS12SCustom
DEBUG = --debug
OPTIM = --opt-code-speed

TARGET_LNK = $(TARGET).hex

VERY_COMMON_FLAGS +=\

C_COMMON_FLAGS +=\

AS_FLAGS +=\

C_FLAGS +=\

CPP_FLAGS +=\

LINKER_FLAGS +=\

LST_FLAGS +=\

WARNINGS +=\

REBUILD_FORCING_MAKEFILES += Makefile.mk

.PHONY : all
all : build

include ../_Common/Makefile_build_sdcc.mkinc
