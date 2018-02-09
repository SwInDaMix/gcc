/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_string_format.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include <avr/io.h>

typedef enum {
	FORMATFLAG_PGM			= 0x01,
} sFormatFlags;

#ifdef STDIO_MICROSIZE
typedef uint8_t stdiosize_t;
typedef int8_t sstdiosize_t;
#else
typedef uint16_t stdiosize_t;
typedef int16_t sstdiosize_t;
#endif

#define FMTCFG_MAX_STRING_SIZE -1			// limits the ever possible length of the formated string (-1 means unlimited)

typedef struct _sFmtStream sFmtStream;
struct _sFmtStream {
	void (*putc)(sFmtStream *strm, char c);
	stdiosize_t limit;
	stdiosize_t length;
	sFormatFlags flags;
	void *ptr;
};

stdiosize_t formatString(char const *fmt, va_list ap, sFmtStream *strm);

