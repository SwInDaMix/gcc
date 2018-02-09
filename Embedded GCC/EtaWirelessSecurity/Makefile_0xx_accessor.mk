PLATFORM=030k6

TARGET_POSTFIX = Accessor
CFG_POSTFIX = accessor

VERY_COMMON_FLAGS +=\
	-DSTM32F030 \
	-DSECURITY_ACCESSOR -DSECURITY_CLIENT \
	-mcpu=cortex-m0 \

REBUILD_FORCING_MAKEFILES += Makefile_0xx_accessor.mk

include Makefile_0xx_common.mkinc
