/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_dbg.c
 ***********************************************************************/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "eta_string_format.h"
#include "eta_dbg.h"

#if (defined USART_DBG && defined USART_TRANSMITTER)


// Put debug char
void DBG_PutChar(char c) { usart_put_char(c); }
// Put debug string
void DBG_PutString(char const *str) { DBG_PutStringN(str, -1); }
void DBG_PutString_P(char const *str) { DBG_PutStringN_P(str, -1); }
// Put debug string with specified maximum length
void DBG_PutStringN(char const *str, size_t len) { char _c; while(len-- && (_c = *str++)) { DBG_PutChar(_c); } }
void DBG_PutStringN_P(char const *str, size_t len) { char _c; while(len-- && (_c = pgm_read_byte(str++))) { DBG_PutChar(_c); } }
// Put debug formatted string
static void dbg_put_putfmtc(sFmtStream *strm, char c) { DBG_PutChar(c); }
void DBG_PutFormat(char const *fmt, ...) { va_list _ap; va_start(_ap, fmt); sFmtStream _strm = { .putc = dbg_put_putfmtc, .limit = -1, .flags = (sFormatFlags)0 }; formatString(fmt, _ap, &_strm); va_end(_ap); }
void DBG_PutFormat_P(char const *fmt, ...) { va_list _ap; va_start(_ap, fmt); sFmtStream _strm = { .putc = dbg_put_putfmtc, .limit = -1, .flags = (sFormatFlags)FORMATFLAG_PGM }; formatString(fmt, _ap, &_strm); va_end(_ap); }
// Ensures all debug data flushed to hardware
void DBG_Flush() { usart_flush(); }

#elif (defined FRAMES_DBG && defined FRAMES_TRANSMITTER)

#define USART_DBG_BUFFER_SIZESHIFT (FRAMES_TX_BUFFER_SIZESHIFT - 2)
#define USART_DBG_BUFFER_SIZE (1 << USART_DBG_BUFFER_SIZESHIFT)
#define USART_DBG_BUFFER_SIZEMASK (USART_DBG_BUFFER_SIZE - 1)

static char dbg_buffer[USART_DBG_BUFFER_SIZE];
#if USART_DBG_BUFFER_SIZESHIFT <= 8
static uint8_t dbg_wr_index;
#else
static uint16_t dbg_wr_index;
#endif

static void _dbg_flush_buffer() { if(dbg_wr_index) { while(!frames_put_frame(0, 0, dbg_buffer, dbg_wr_index)) { FRAMES_YIELD(); } dbg_wr_index = 0; } }

// Put debug char
void DBG_PutChar(char c) {
	dbg_buffer[dbg_wr_index++] = c;
	if(dbg_wr_index >= USART_DBG_BUFFER_SIZE) _dbg_flush_buffer();
}
// Put debug string
void DBG_PutString(char const *str) { DBG_PutStringN(str, -1); }
void DBG_PutString_P(char const *str) { DBG_PutStringN_P(str, -1); }
// Put debug string with specified maximum length
void DBG_PutStringN(char const *str, size_t len) { char _c; while(len-- && (_c = *str++)) { DBG_PutChar(_c); } _dbg_flush_buffer(); }
void DBG_PutStringN_P(char const *str, size_t len) { char _c; while(len-- && (_c = pgm_read_byte((uint8_t *)str++))) { DBG_PutChar(_c); } _dbg_flush_buffer(); }
// Put debug formatted string
static void dbg_put_putfmtc(sFmtStream *strm, char c) { DBG_PutChar(c); }
void DBG_PutFormat(char const *fmt, ...) { va_list _ap; va_start(_ap, fmt); sFmtStream _strm = { .putc = dbg_put_putfmtc, .limit = -1, .flags = (sFormatFlags)0 }; formatString(fmt, _ap, &_strm); va_end(_ap); _dbg_flush_buffer(); }
void DBG_PutFormat_P(char const *fmt, ...) { va_list _ap; va_start(_ap, fmt); sFmtStream _strm = { .putc = dbg_put_putfmtc, .limit = -1, .flags = (sFormatFlags)FORMATFLAG_PGM }; formatString(fmt, _ap, &_strm); va_end(_ap); _dbg_flush_buffer(); }
// Ensures all debug data flushed to hardware
void DBG_Flush() { _dbg_flush_buffer(); frames_flush(); }

#endif
