SUBCROSS=i686-

OPTIM = -Os

BIN_DIR = bin_win86
EXTRN_OBJ_DIR = obj_win86_extrn
OBJ_DIR = obj_win86

VERY_COMMON_FLAGS +=\
	-m32 \

REBUILD_FORCING_MAKEFILES += Makefile_x86.mk

include Makefile.mkinc
