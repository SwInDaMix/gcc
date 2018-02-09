/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_hd44780.c
 ***********************************************************************/

#include "config.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "eta_string_format.h"
#include "eta_hd44780.h"
#include "eta_usart.h"

#ifdef ALCD_DATAPORT

#define ALCD_DIRPORT (*((&ALCD_DATAPORT) - 1))
#define ALCD_PINPORT (*((&ALCD_DATAPORT) - 2))
#define ALCD_DATAPORT_BUSYFLAG 7

#define ALCD_CBI(port, pin) port &= (~(1 << pin))
#define ALCD_SBI(port, pin) port |= (1 << pin)

#ifdef ALCD_HALFDUPLEX
#define ALCD_MASK (0xF << ALCD_HALFDUPLEX)
#endif

typedef enum {
	ealcd_flag_single_duplex = 0x01,
	ealcd_flag_wait_for_ready = 0x02,
	ealcd_flag_cmd = 0x04,
} ealcd_flags;

static uint8_t _alcd_read_byte() {
	uint8_t _data;
	ALCD_SBI(ALCD_CTL_E_PORT, ALCD_CTL_E_PIN);
	ALCD_DELAY();
	_data = ALCD_PINPORT;
	ALCD_CBI(ALCD_CTL_E_PORT, ALCD_CTL_E_PIN);
	ALCD_DELAY();
	#ifdef ALCD_HALFDUPLEX
	_data = (_data << (4 - ALCD_HALFDUPLEX));
	ALCD_SBI(ALCD_CTL_E_PORT, ALCD_CTL_E_PIN);
	ALCD_DELAY();
	uint8_t _data2 = ALCD_PINPORT;
	ALCD_CBI(ALCD_CTL_E_PORT, ALCD_CTL_E_PIN);
	ALCD_DELAY();
	_data |= (_data2 >> ALCD_HALFDUPLEX);
	#endif
	return _data;
}

static void _alcd_write_byte(ealcd_flags flags, uint8_t data) {
	#ifdef ALCD_HALFDUPLEX
	ALCD_DATAPORT = (ALCD_DATAPORT & (~ALCD_MASK)) | ((data >> (4 - ALCD_HALFDUPLEX)) & ALCD_MASK);
	if(!(flags & ealcd_flag_single_duplex)) {
		ALCD_SBI(ALCD_CTL_E_PORT, ALCD_CTL_E_PIN);
		ALCD_DELAY();
		ALCD_CBI(ALCD_CTL_E_PORT, ALCD_CTL_E_PIN);
		ALCD_DELAY();
		ALCD_DATAPORT = (ALCD_DATAPORT & (~ALCD_MASK)) | ((data << ALCD_HALFDUPLEX) & ALCD_MASK);
	}
	#else
	ALCD_DATAPORT = data;
	#endif
	ALCD_SBI(ALCD_CTL_E_PORT, ALCD_CTL_E_PIN);
	ALCD_DELAY();
	ALCD_CBI(ALCD_CTL_E_PORT, ALCD_CTL_E_PIN);
	ALCD_DELAY();
}

// Wait for LCD get ready
static void _alcd_ready() {
	#ifdef ALCD_HALFDUPLEX
	ALCD_DIRPORT &= (~ALCD_MASK);
	ALCD_DATAPORT &= (~ALCD_MASK);
	#else
	ALCD_DIRPORT = 0;
	ALCD_DATAPORT = 0;
	#endif
	ALCD_CBI(ALCD_CTL_RS_PORT, ALCD_CTL_RS_PIN);
	ALCD_SBI(ALCD_CTL_RW_PORT, ALCD_CTL_RW_PIN);
	while(_alcd_read_byte() & (1 << ALCD_DATAPORT_BUSYFLAG)); // { uint8_t _ddd = _alcd_read_byte(); DBG_PutFormat_P(PSTR("!!%02X"NL), _ddd); DBG_Flush(); }
	ALCD_CBI(ALCD_CTL_RW_PORT, ALCD_CTL_RW_PIN);
}

// Write data to LCD
static void _alcd_write(ealcd_flags flags, uint8_t data) {
    if(flags & ealcd_flag_wait_for_ready) _alcd_ready();
	#ifdef ALCD_HALFDUPLEX
	ALCD_DIRPORT |= ALCD_MASK;
	#else
	ALCD_DIRPORT = 0xFF;
	#endif
	if(flags & ealcd_flag_cmd) ALCD_CBI(ALCD_CTL_RS_PORT, ALCD_CTL_RS_PIN); else ALCD_SBI(ALCD_CTL_RS_PORT, ALCD_CTL_RS_PIN);
	ALCD_CBI(ALCD_CTL_RW_PORT, ALCD_CTL_RW_PIN);
	_alcd_write_byte(flags, data);
}

// Clear display
void ALCD_Clear() { _alcd_write(ealcd_flag_wait_for_ready | ealcd_flag_cmd, ALCD_Cmd_Clear); }

// Set the LCD display position  x=0..39 y=0..3
void ALCD_GoToXY(uint8_t x, uint8_t y) {
	uint8_t _addr = x;
    if(y & 0x2) { _addr += 40; }
    if(y & 0x1) { _addr |= 0x40; }
    _alcd_write(ealcd_flag_wait_for_ready | ealcd_flag_cmd, ALCD_Cmd_SetDDAddr | _addr);
}

// Write single char to the LCD
void ALCD_PutChar(char c) { _alcd_write(ealcd_flag_wait_for_ready, c); }
// write single char multiple time to the LCD
void ALCD_PutCharN(char c, size_t n) { while(n--) _alcd_write(ealcd_flag_wait_for_ready, c); }
// Write the string str with specified maximum length located in SRAM to the LCD
void ALCD_PutStringN(char const *str, size_t len) { char _c; while(len-- && (_c = *str++)) { _alcd_write(ealcd_flag_wait_for_ready, _c); } }
// Write the string str with specified maximum length located in FLASH to the LCD
void ALCD_PutStringN_P(char const *str, size_t len) { char _c; while(len-- && (_c = pgm_read_byte(str++))) { _alcd_write(ealcd_flag_wait_for_ready, _c); } }
// Write the string str located in SRAM to the LCD
void ALCD_PutString(char const *str) { ALCD_PutStringN(str, -1); }
// Write the string str located in FLASH to the LCD
void ALCD_PutString_P(char const *str) { ALCD_PutStringN_P(str, -1); }

static void _alcd_putfmtc(sFmtStream *strm, char c) { _alcd_write(ealcd_flag_wait_for_ready, c); }

// Write formated string fmtstr located in SRAM to the LCD
void ALCD_PutFormat(char const *fmt, ...) { va_list _ap; va_start(_ap, fmt); sFmtStream _strm = { .putc = _alcd_putfmtc, .limit = -1, .flags = (sFormatFlags)0 }; formatString(fmt, _ap, &_strm); va_end(_ap); }
// Write formated string fmtstr located in FLASH to the LCD
void ALCD_PutFormat_P(char const *fmt, ...) { va_list _ap; va_start(_ap, fmt); sFmtStream _strm = { .putc = _alcd_putfmtc, .limit = -1, .flags = (sFormatFlags)FORMATFLAG_PGM }; formatString(fmt, _ap, &_strm); va_end(_ap); }

// Initialize the LCD controller
void ALCD_Init() {
	_delay_ms(400);
	_alcd_write(ealcd_flag_cmd | ealcd_flag_single_duplex, ALCD_Cmd_Func | ALCD_Cmd_Func_5x8 | ALCD_Cmd_Func_2line | ALCD_Cmd_Func_8bits);
	_delay_ms(200);
	_alcd_write(ealcd_flag_cmd | ealcd_flag_single_duplex, ALCD_Cmd_Func | ALCD_Cmd_Func_5x8 | ALCD_Cmd_Func_2line | ALCD_Cmd_Func_8bits);
	_delay_ms(200);
	_alcd_write(ealcd_flag_cmd | ealcd_flag_single_duplex, ALCD_Cmd_Func | ALCD_Cmd_Func_5x8 | ALCD_Cmd_Func_2line | ALCD_Cmd_Func_8bits);
	#ifdef ALCD_HALFDUPLEX
	_delay_ms(200);
	_alcd_write(ealcd_flag_cmd | ealcd_flag_single_duplex, ALCD_Cmd_Func | ALCD_Cmd_Func_5x8 | ALCD_Cmd_Func_2line | ALCD_Cmd_Func_4bits);
	_delay_ms(200);
	_alcd_write(ealcd_flag_cmd, ALCD_Cmd_Func | ALCD_Cmd_Func_5x8 | ALCD_Cmd_Func_2line | ALCD_Cmd_Func_4bits);
	#endif
	_alcd_write(ealcd_flag_wait_for_ready | ealcd_flag_cmd, ALCD_Cmd_Visible | ALCD_Cmd_Visible_Display);
}

#endif

