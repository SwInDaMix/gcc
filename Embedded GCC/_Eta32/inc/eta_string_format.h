/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_string_format.h
 ***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	None
} sFormatFlags;

#define FMTCFG_MAX_STRING_SIZE -1			// limits the ever possible length of the formated string (-1 means unlimited)

typedef struct sFmtStream sFmtStream;
struct sFmtStream {
	void (*putc)(sFmtStream *strm, char c);
	uint32_t limit;
	uint16_t length;
	sFormatFlags flags;
};

uint32_t formatString(char const *fmt, va_list ap, sFmtStream *strm);

#ifdef __cplusplus
}
#endif
