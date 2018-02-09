/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_dbg.h
 ***********************************************************************/

#pragma once

#include <avr/pgmspace.h>
#include "eta_usart.h"
#include "eta_frames.h"

#if (defined USART_DBG && defined USART_TRANSMITTER) || (defined FRAMES_DBG && defined FRAMES_TRANSMITTER)
// Put debug char
void DBG_PutChar(char c);
// Put debug string
void DBG_PutString(char const *str);
void DBG_PutString_P(char const *str);
// Put debug string with specified maximum length
void DBG_PutStringN(char const *str, size_t len);
void DBG_PutStringN_P(char const *str, size_t len);
// Put debug formatted string
void DBG_PutFormat(char const *fmt, ...);
void DBG_PutFormat_P(char const *fmt, ...);
// Ensures all debug data flushed to hardware
void DBG_Flush();
#else
#define DBG_PutChar(c) {}
#define DBG_PutString(str) {}
#define DBG_PutString_P(str) {}
#define DBG_PutStringN(str, len) {}
#define DBG_PutStringN_P(str, len) {}
#define DBG_PutFormat(fmt, ...) {}
#define DBG_PutFormat_P(fmt, ...) {}
#define DBG_Flush() {}
#endif
