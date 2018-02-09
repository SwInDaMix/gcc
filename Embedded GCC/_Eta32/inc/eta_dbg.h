/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_dbg.h
 ***********************************************************************/

#pragma once

#include "eta_usart.h"
#include "eta_frames.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined USART_DBG && defined USART_HAS_TRANSMITTER) || (defined FRAMES_DBG && defined FRAMES_TRANSMITTER)
// Put debug char
void DBG_PutChar(char c);
// Put debug string
void DBG_PutString(char const *str);
// Put debug string with specified maximum length
void DBG_PutStringN(char const *str, size_t len);
// Put debug formatted string
void DBG_PutFormat(char const *fmt, ...);
// Ensures all debug data flushed to hardware
void DBG_Flush();
// Put memory hex dump to debug
void DBG_DumpHex(char const *title, void const *buf, size_t buf_len);
#else
#define DBG_PutChar(c)
#define DBG_PutString(str)
#define DBG_PutStringN(str, len)
#define DBG_PutFormat(fmt, ...)
#define DBG_Flush()
#define DBG_DumpHex(title, buf, buf_len);
#endif

#ifdef __cplusplus
}
#endif
