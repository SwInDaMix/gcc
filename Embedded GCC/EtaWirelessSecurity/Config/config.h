/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config.h
 ***********************************************************************/

#pragma once

#include "Types.h"
#include "config_periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#define YIELD() {  }

#define SECURITY_KEY_REGISTER_BUTTONS eSecurityActionPressButtons_Long234
#define SECURITY_KEY_FULL_RESET_BUTTONS eSecurityActionPressButtons_Long1234

#define SECURITY_FULL_RESET_SEQUENCE 0xFF00FF00

#define SECURITY_PAGE_SHIFT 10
#define SECURITY_PAGE_SIZE (1 << 10)
#define SECURITY_INITIALIZATION_VECTOR 0xf49ab216, 0x3ffdee05, 0x04fa6a8e, 0x595f851f
#define SECURITY_KEY_REPLACE_PROBABILITY (UINT32_MAX / 10 * 2)

#define SEC_TO_PERIODIC(msec) (PERIODIC_FREQ * (msec) / 1000)
#define PERIOD_IWDG_PERIOD SEC_TO_PERIODIC(1000)
#define PERIOD_FULL_PACKAGE_TIMEOUT SEC_TO_PERIODIC(200)
#define PERIOD_BUTTON_MIN_PRESS SEC_TO_PERIODIC(50)
#define PERIOD_BUTTON_LONG_PRESS SEC_TO_PERIODIC(2000)
#define PERIOD_REQUEST_TIMEOUT SEC_TO_PERIODIC(500)
#define PERIOD_FAIL_TIMEOUT SEC_TO_PERIODIC(1000)
#define PERIOD_CHALLENGE_TIMEOUT SEC_TO_PERIODIC(1000)
#define PERIOD_REGISTRATION_TIMEOUT SEC_TO_PERIODIC(60000)
#define PERIOD_REGISTRATION_ALLOW_TIMEOUT SEC_TO_PERIODIC(5000)
#define PERIOD_INACTIVITY SEC_TO_PERIODIC(3000)
#define PERIOD_REGISTER_SEQUENCE_INACTIVITY SEC_TO_PERIODIC(10000)
#define PERIOD_1BIT_LED SEC_TO_PERIODIC(2000 / 32)

//#define RF_SE8R01
#define RF_NRF24
//#define RF_RFM75
#define RF_SECURITY_CHANNEL 82
#define RF_SECURITY_ADDRPREF 0xFB, 0xC1, 0x79, 0x8E
#define RF_SECURITY_ADDR_REQUEST 0xAA
#define RF_SECURITY_ADDR_CLIENT_START 0x70

// Std IO string formatter options
#define STDIO_ALLOWALIGNMENT
#define STDIO_ALLOWFRACTIONS
#define STDIO_ALLOWFILLCHAR
#define STDIO_ALLOWLONG
#define STDIO_ALLOWFLOAT
#define STDIO_ALLOWHEXDUMP
#define STDIO_ALLOWSIGNPADS

#define FRAME_ADDRESS 0x00
#define FRAME_BCELL_ADDRESS 0xB0

#define FRAMES_EXTENDED
//#define USART_DBG 0
#define FRAMES_DBG 0
#define USART_BAUDRATE 115200
#define USART_BCELL_BAUDRATE 2500
#define FRAMES_YIELD() YIELD()

extern void Assert(char const *msg, int n);

#define _ASSERTW(a)
#define _ASSERT(a) if(!(a)) { Assert(__FILE__, __LINE__); }
#define _ASSERT_VALID(a) if(!(a)) { Assert(__FILE__, __LINE__); }

#ifdef __cplusplus
}
#endif
