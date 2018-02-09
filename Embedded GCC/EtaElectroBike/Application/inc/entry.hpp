/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : entry.hpp
 ***********************************************************************/

#include "config.h"

extern "C" {

	typedef enum {
		FRAME_CMD_DEBUG_STRING,
		FRAME_CMD_STATUS,
		FRAME_CMD_NEW_PARAMS,
		FRAME_CMD_TEST_COMMAND
	} sFrameCmd;

	typedef struct __attribute__((packed)) {
		uint8_t HallPosition;
		uint8_t HallPrescaler;
		uint16_t HallPeriod;
		uint16_t PWMPower;
	} sStatus;
	typedef struct __attribute__((packed)) {
		uint8_t HallPrescaler;
		uint16_t HallPeriod;
		uint16_t PWMPower;
	} sNewParams;

}
