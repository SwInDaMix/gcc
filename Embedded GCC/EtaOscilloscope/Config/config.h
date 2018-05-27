/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config.h
 ***********************************************************************/

#pragma once

#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEMMAN_FIBONACHI

#define YIELD() {  }

// Std IO string formatter options
#define STDIO_ALLOWALIGNMENT
#define STDIO_ALLOWFILLCHAR
#define STDIO_ALLOWLONG
#define STDIO_ALLOWFLOAT
#define STDIO_ALLOWSIGNPADS

// Simple USART options
#define USART0_PERIPH USART2
#define USART0_IRQ USART2_IRQn
#define USART0_RX_BUFFER_SIZESHIFT 8
#define USART0_TX_BUFFER_SIZESHIFT 6

// Framelike USART options
#define FRAMES_SYNC_SIGNATURE_SIZESHIFT 3
#define FRAMES_SYNC_SIGNATURE 0x72, 0xC5, 0xD9, 0xAC, 0x86, 0xE3, 0xBA, 0x9E
#define FRAMES_STUBBYTE 0xAA

#define FRAMES0_PERIPH USART_DBG
#define FRAMES0_IRQ USART_DBG_IRQn
#define FRAMES0_RX_BUFFER_SIZESHIFT 8
#define FRAMES0_TX_BUFFER_SIZESHIFT 8

//#define USART_DBG 0
#define FRAMES_DBG 0
#define DBG_USART_BAUDRATE 115200
#define USART_YIELD() YIELD()

extern void Assert(char const *msg, int n);

#define _ASSERTW(a)
#define _ASSERT(a) if(!(a)) { Assert(__FILE__, __LINE__); }
#define _ASSERT_VALID(a) if(!(a)) { Assert(__FILE__, __LINE__); }
#ifdef __cplusplus
}
#endif
