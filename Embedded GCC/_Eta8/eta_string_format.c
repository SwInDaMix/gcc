/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_string_format.c
 ***********************************************************************/

#include <avr/pgmspace.h>
#include "eta_string_format.h"

typedef enum {
	#ifdef STDIO_ALLOWALIGNMENT
	INTERNALFLAG_LEFTADJUSTED = 0x01,
	#endif
	#ifdef STDIO_ALLOWSIGNPADS
	INTERNALFLAG_FORCESIGN    = 0x02,
	INTERNALFLAG_SIGNPAD      = 0x04,
	#endif
	INTERNALFLAG_UNSIGNED     = 0x08,
	INTERNALFLAG_TRIPLE        = 0x10,
	#ifdef STDIO_ALLOWLONG
	INTERNALFLAG_LONG         = 0x20,
	#endif
	#ifdef STDIO_ALLOWFLOAT
	INTERNALFLAG_DOUBLE       = 0x40,
	#endif
	INTERNALFLAG_NEGATIVE     = 0x80,
} sInternalFlags;

typedef enum {
	INTERNALFLAG2_STRINGPGM		= 0x01,
} sInternalFlags2;

#ifndef	__AVR_HAVE_LPMX__
# if  defined(__AVR_ENHANCED__) && __AVR_ENHANCED__
#  define __AVR_HAVE_LPMX__	1
# endif
#endif

#if  defined(__AVR_HAVE_LPMX__) && __AVR_HAVE_LPMX__
# define GETBYTE(output, flag, mask, pnt) \
    __asm ( \
	"sbrc	%2,%3	\n\t" \
	"lpm	%0,Z	\n\t" \
	"sbrs	%2,%3	\n\t" \
	"ld		%0,Z	" \
	: "=&r" (output) \
	: "z" (pnt), "r" (flag), "I" (ntz(mask)));
#else
# define GETBYTE(output, flag, mask, pnt) \
    __asm ( \
	"sbrc	%2,%3	\n\t" \
	"lpm			\n\t" \
	"sbrs	%2,%3	\n\t" \
	"ld		r0,Z	\n\t" \
	"adiw	r30,1	\n\t" \
	"mov	%0,r0	" \
	: "=&r" (output) \
	: "z" (pnt), "r" (flag), "I" (ntz(mask)) \
	: "r0");
#endif

#ifdef STDIO_ALLOWLONG
typedef ldiv_t stdio_div_t;
typedef long stdio_int_t;
# define stdio_div(num, denom) ldiv(num, denom)
#else
typedef div_t stdio_div_t;
typedef int stdio_int_t;
# define stdio_div(num, denom) div(num, denom)
#endif

#define STDIO_TMPBUFLEN ((sizeof(stdio_int_t) * 8) + 4)

static void _putc(sFmtStream *strm, char c) { if(strm->limit) { strm->putc(strm, c); strm->limit--; strm->length++; } }
static char _get_char(sFormatFlags flags, char const *addr) { char _c; GETBYTE(_c, flags, FORMATFLAG_PGM, addr); return _c; }
static char _get_char2(sInternalFlags2 flags, char const *addr) { char _c; GETBYTE(_c, flags, INTERNALFLAG2_STRINGPGM, addr); return _c; }
#ifndef STDIO_ALLOWALIGNMENT
#endif

stdiosize_t formatString(char const *fmt, va_list ap, sFmtStream *strm) {

	strm->length = 0; char _c;

	while((_c = _get_char(strm->flags, fmt++)) && strm->limit) {
		// Starting formating the field
		if(_c == '%') {
			// percentage character
			if(_get_char(strm->flags, fmt) == '%') { fmt++; _putc(strm, '%'); }
			else {
				#ifdef STDIO_ALLOWALIGNMENT
				char _fillchar = ' ';
				stdiosize_t _width = 0, _precision = 0;
				#endif
				#ifdef STDIO_ALLOWFRACTIONS
				uint8_t _fraction = 0;
				#endif
				sInternalFlags _flags = (sInternalFlags)0; sInternalFlags2 _flags2 = (sInternalFlags2)0;
				_c = _get_char(strm->flags, fmt);

				/// nested routines
				// formats and writes to the stream a character sequence with specified length
				__extension__ auto void _write_str(const char *str, stdiosize_t len) {
					#ifdef STDIO_ALLOWALIGNMENT
					if(_width > len) {
						stdiosize_t _tofill = _width - len;
						if(_flags & INTERNALFLAG_LEFTADJUSTED) { while(len--) _putc(strm, _get_char2(_flags2, str++)); while(_tofill--) _putc(strm, _fillchar); }
						else { while(_tofill--) _putc(strm, _fillchar); while(len--) _putc(strm, _get_char2(_flags2, str++)); }
					}
					else while(len--) _putc(strm, _get_char2(_flags2, str++));
					#else
					char _cc;
					while(len-- && (_cc = _get_char2(_flags2, str++))) _putc(strm, _cc);
					#endif
				}
				// formats and writes to the stream a single character
				__extension__ auto void _write_char(char ch) { _write_str(&ch, 1); }
				// formats and writes to the stream an integer number with specified radix
				__extension__ auto void _write_integer(uint8_t radix, bool is_upercase) {
					char _ibuf[STDIO_TMPBUFLEN]; char *_ibuf_ptr = _ibuf + sizeof(_ibuf);
					stdio_int_t _ival;
					#ifdef STDIO_ALLOWLONG
					if((_flags & INTERNALFLAG_UNSIGNED)) _ival = (_flags & INTERNALFLAG_TRIPLE) ? (uint24_t)va_arg(ap, uint24_t) : ((_flags & INTERNALFLAG_LONG) ? va_arg(ap, uint32_t) : va_arg(ap, uint16_t));
					else _ival = (_flags & INTERNALFLAG_TRIPLE) ? (int24_t)va_arg(ap, int24_t) : ((_flags & INTERNALFLAG_LONG) ? va_arg(ap, int32_t) : va_arg(ap, int16_t));
					#else
					if((_flags & INTERNALFLAG_UNSIGNED)) _ival = (_flags & INTERNALFLAG_TRIPLE) ? (uint24_t)va_arg(ap, uint24_t) : va_arg(ap, uint16_t);
					else _ival = (_flags & INTERNALFLAG_TRIPLE) ? (int24_t)va_arg(ap, int24_t) : va_arg(ap, int16_t);
					#endif
					if(_ival < 0) { _flags |= INTERNALFLAG_NEGATIVE; _ival = -_ival; }
					stdiosize_t _ilen = 0;

					#define iput(dig) ({ *(--_ibuf_ptr) = dig; _ilen++; })
					while(_ival) {
						stdio_div_t _idiv = stdio_div(_ival, radix);
						iput(_idiv.rem + ((_idiv.rem > 9) ? (is_upercase ? 'A' : 'a') - 10 : '0'));
						_ival = _idiv.quot;
						#ifdef STDIO_ALLOWFRACTIONS
						if(_fraction) { if(_fraction == _ilen) iput('.'); }
						#endif
					}
					#ifdef STDIO_ALLOWFRACTIONS
					if(_fraction) {
						while(_fraction > _ilen) { iput('0'); };
						if(_fraction == _ilen) iput('.');
						if(!(_ilen - _fraction - 1)) iput('0');
					}
					#endif
					if(!_ilen) iput('0');
					if(_flags & INTERNALFLAG_NEGATIVE) iput('-');
					#ifdef STDIO_ALLOWSIGNPADS
					else if(_flags & INTERNALFLAG_FORCESIGN) iput('+');
					else if(_flags & INTERNALFLAG_SIGNPAD) iput(' ');
					#endif

					_write_str(_ibuf_ptr, _ilen);
				}
				/// end of nested routines

				#ifdef STDIO_ALLOWFILLCHAR
				// Try to parse char filling
				if(_c == 'r' || _c == 'R') {
					bool _is_custom_char = _c == 'R'; _c = _get_char(strm->flags, ++fmt);
					if(_is_custom_char || _c) {
						char _fillwithchar;
						if(_is_custom_char) _fillwithchar = (char)va_arg(ap, int);
						else { _fillwithchar = _c; fmt++; }
						stdiosize_t _cnt = va_arg(ap, unsigned int);
						while((_cnt--)) { _write_char(_fillwithchar); }
					}
				}
				// Parsing alignment
				else
				#endif
				{
					// Parsing the flags
					while(1) {
						if(!(_c = _get_char(strm->flags, fmt))) break;
						#ifdef STDIO_ALLOWALIGNMENT
						else if(_c == '-') {
							_flags |= INTERNALFLAG_LEFTADJUSTED;
						}
						#endif
						#ifdef STDIO_ALLOWSIGNPADS
						else if(_c == '+') { _flags &= ~INTERNALFLAG_SIGNPAD; _flags |= INTERNALFLAG_FORCESIGN; }
						else if(_c == ' ') { _flags |= INTERNALFLAG_SIGNPAD; }
						#endif
						#ifdef STDIO_ALLOWALIGNMENT
						else if(_c == '0') { _fillchar = _c; }
						#endif
						else break;
						fmt++;
					}

					#ifdef STDIO_ALLOWALIGNMENT
					// Parsing the width
					if(_c == '*') { _c = _get_char(strm->flags, ++fmt); sstdiosize_t _w = (sstdiosize_t)va_arg(ap, int); if(_w < 0) { _w = -_w; _flags |= INTERNALFLAG_LEFTADJUSTED; } _width = _w; }
					else while(_c >= '0' && _c <= '9') { _width = _width * 10 + (_c - '0'); _c = _get_char(strm->flags, ++fmt); }

					// Parsing the precision
					if(_c == '.') {
						_c = _get_char(strm->flags, ++fmt);
						if(_c == '*') { _c = _get_char(strm->flags, ++fmt); _precision = (stdiosize_t)va_arg(ap, int); }
						else while(_c >= '0' && _c <= '9') { _precision = _precision * 10 + (_c - '0'); _c = _get_char(strm->flags, ++fmt); }
					}
					#endif

					// Parsing fractions
					#ifdef STDIO_ALLOWFRACTIONS
					if(_c == 'w' || _c == 'W') {
						_fraction = _get_char(strm->flags, ++fmt);
						if(_fraction >= '0' && _fraction <= '9') _fraction -= '0'; else _fraction = 0;
						_c = _get_char(strm->flags, ++fmt);
					}
					#endif

					// Parsing the size and "signed" flag
					if(_c == 't') { _c = _get_char(strm->flags, ++fmt); _flags |= INTERNALFLAG_TRIPLE; }
					#ifdef STDIO_ALLOWLONG
					else if(_c == 'l') { _c = _get_char(strm->flags, ++fmt); _flags |= INTERNALFLAG_LONG; }
					#endif
					#ifdef STDIO_ALLOWFLOAT
					else if(_c == 'L') { _c = _get_char(strm->flags, ++fmt); _flags |= INTERNALFLAG_DOUBLE; }
					#endif
					if(_c == 'u') { _c = _get_char(strm->flags, ++fmt); _flags |= INTERNALFLAG_UNSIGNED; }

					// Parsing type
					// ============

					// single character
					if(_c == 'c' || _c == 'C') { fmt++; _write_char((char)va_arg(ap, int)); }
					// null-terminated string
					else if(_c == 's' || _c == 'S') {
						fmt++; char const *_str = va_arg(ap, char const *);
						#ifdef STDIO_ALLOWALIGNMENT
						stdiosize_t _len;
						if(_c == 'S') { _flags2 |= INTERNALFLAG2_STRINGPGM; _len = strlen_P(_str); } else { _len = strlen(_str); }
						_write_str(_str, _precision ? min(_precision, _len) : _len);
						#else
						if(_c == 'S') _flags2 |= INTERNALFLAG2_STRINGPGM;
						_write_str(_str, -1);
						#endif
					}
					// floating point number
					#ifdef STDIO_ALLOWFLOAT
					///
					/// !!! not implemented yet :(
					/// else if(_c == 'f') { fmt++;  }
					/// else if(_c == 'e') { fmt++;  }
					/// else if(_c == 'E') { fmt++;  }
					/// else if(_c == 'G') { fmt++;  }
					/// else if(_c == 'g') { fmt++;  }
					///
					#endif
					// integer number
					else if(_c == 'B' || _c == 'b') { fmt++; _write_integer(2, false); }								// binary
					else if(_c == 'O' || _c == 'o') { fmt++; _write_integer(8, false); }								// octal
					else if(_c == 'X' || _c == 'x') { fmt++; _write_integer(16, _c == 'X'); }							// hexadecimal
					else if(_c == 'P' || _c == 'p') { fmt++; _write_str("0x", 2); _write_integer(16, true); }			// pointer as 0x...
					else if(_c == 'D' || _c == 'd' || _c == 'I' || _c == 'i') { fmt++; _write_integer(10, false); }		// decimal
					else _write_integer(10, false);																		// decimal (as the default type)
				}
			}
		}
		// Normal character
		else _putc(strm, _c);
	}
	return strm->length;
}
