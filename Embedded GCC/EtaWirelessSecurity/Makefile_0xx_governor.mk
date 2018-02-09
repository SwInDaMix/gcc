PLATFORM=030c8

TARGET_POSTFIX = Governor
CFG_POSTFIX = governor

VERY_COMMON_FLAGS +=\
	-DSTM32F030 \
	-DSECURITY_GOVERNOR -DSECURITY_SERVER \
	-mcpu=cortex-m0 \

REBUILD_FORCING_MAKEFILES += Makefile_0xx_governor.mk

include Makefile_0xx_common.mkinc
