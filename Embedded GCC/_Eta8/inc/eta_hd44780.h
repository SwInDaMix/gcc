/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_hd44780.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include <avr/io.h>

#ifdef ALCD_DATAPORT

// Commands
typedef enum {
	ALCD_Cmd_Clear = 0x1,
	ALCD_Cmd_Home = 0x2,
	ALCD_Cmd_Mode = 0x4,
	ALCD_Cmd_Visible = 0x8,
	ALCD_Cmd_Shift = 0x10,
	ALCD_Cmd_Func = 0x20,
	ALCD_Cmd_SetCGAddr = 0x40,
	ALCD_Cmd_SetDDAddr = 0x80,

	// Function command flags
	ALCD_Cmd_Func_5x8 = 0,
	ALCD_Cmd_Func_5x10 = 0x4,
	ALCD_Cmd_Func_1line = 0,
	ALCD_Cmd_Func_2line = 0x8,
	ALCD_Cmd_Func_4bits = 0,
	ALCD_Cmd_Func_8bits = 0x10,

	// Visible command flags
	ALCD_Cmd_Visible_Blinking = 0x1,
	ALCD_Cmd_Visible_Cursor = 0x2,
	ALCD_Cmd_Visible_Display = 0x4
} eALCD_Cmd;

// Clear display
void ALCD_Clear();
// Set the LCD display position  x=0..39 y=0..3
void ALCD_GoToXY(uint8_t x, uint8_t y);
// Write single char to the LCD
void ALCD_PutChar(char c);
// write single char multiple time to the LCD
void ALCD_PutCharN(char c, size_t n);
// Write the string str with specified maximum length located in SRAM to the LCD
void ALCD_PutStringN(char const *str, size_t len);
// Write the string str with specified maximum length located in FLASH to the LCD
void ALCD_PutStringN_P(char const *str, size_t len);
// Write the string str located in SRAM to the LCD
void ALCD_PutString(char const *str);
// Write the string str located in FLASH to the LCD
void ALCD_PutString_P(char const *str);
// Write formated string fmt located in SRAM to the LCD
void ALCD_PutFormat(char const *fmt, ...);
// Write formated string fmt located in FLASH to the LCD
void ALCD_PutFormat_P(char const *fmt, ...);
// Initialize the LCD controller
void ALCD_Init();

#endif

