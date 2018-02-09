/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_dbg.h
 ***********************************************************************/

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
