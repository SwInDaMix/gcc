SUBCROSS=x86_64-

OPTIM = -Os

BIN_DIR = bin_win64
EXTRN_OBJ_DIR = obj_win64_extrn
OBJ_DIR = obj_win64

VERY_COMMON_FLAGS +=\
	-m64 \

REBUILD_FORCING_MAKEFILES += Makefile_x64.mk

include Makefile.mkinc
