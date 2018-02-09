/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_dbg.c
 ***********************************************************************/

#include "eta_string_format.h"
#include "eta_dbg.h"

//#include "intstdio.h"
#include <stdio.h>

// Put debug char
void DBG_PutChar(char c) { fputc(c, stdout); }
// Put debug string
void DBG_PutString(char const *str) { DBG_PutStringN(str, -1); }
// Put debug string with specified maximum length
void DBG_PutStringN(char const *str, size_t len) { char _c; while(len-- && (_c = *str++)) { DBG_PutChar(_c); } }
// Put debug formatted string
static void dbg_put_putfmtc(sFmtStream *strm, char c) { DBG_PutChar(c); }
void DBG_PutFormat(char const *fmt, ...) { va_list _ap; va_start(_ap, fmt); sFmtStream _strm = { .putc = dbg_put_putfmtc, .limit = -1, .flags = (sFormatFlags)0 }; formatString(fmt, _ap, &_strm); va_end(_ap); }
// Ensures all debug data flushed to hardware
void DBG_Flush() { }
// Put memory hex dump to debug
void DBG_DumpHex(char const *title, void const *buf, size_t buf_len) {
	char _c;
	while((_c = *title++)) DBG_PutChar(_c);
	DBG_PutChar(':');
	uint8_t const *_buf_ptr = (uint8_t const *)buf;
	uint8_t _rowcnt = 0;
	while((buf_len--)) {
		DBG_PutChar((_rowcnt & 0xF) ? ' ' : '\n');
		uint8_t _byte = *_buf_ptr++;
		_c = (_byte >> 4); DBG_PutChar(_c + ((_c > 9) ? 'A' - 10 : '0'));
		_c = (_byte & 0xF); DBG_PutChar(_c + ((_c > 9) ? 'A' - 10 : '0'));
		_rowcnt++;
	}
}
