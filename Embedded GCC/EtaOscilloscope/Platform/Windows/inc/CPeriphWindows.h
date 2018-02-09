/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CPeriphWindows.h
***********************************************************************/

#pragma once

#define LCD_WINCOLOR24_FROM_COLOR16_correct(clr) ((uint32_t)clr | (((uint32_t)clr & 0xE000E0) >> 5) | (((uint32_t)clr & 0xC000) >> 6))
#define LCD_WINCOLOR24_FROM_COLOR16(clr) (LCD_WINCOLOR24_FROM_COLOR16_correct(((((uint32_t)clr & 0x1F) << 19) | (((uint32_t)clr & 0x7E0) << 5) | (((uint32_t)clr & 0xF800) >> 8))))
#define LCD_COLOR16_FROM_WINCOLOR24(clr) ((((uint32_t)clr & 0xF8) << 8) | (((uint32_t)clr & 0xFC00) >> 5) | (((uint32_t)clr & 0xF80000) >> 19))

#include "config.h"
#include <windows.h>
#include <stdint.h>
#include <CPeriph.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile bool lcd_is_need_draw;

void KBD_Reset();
bool KBD_KeyDown(unsigned char vk);
bool KBD_KeyUp(unsigned char vk);

void LCD_CreateContext(uint32_t x, uint32_t y);
void LCD_DestroyContext();
void LCD_PaintToDC(HDC hDC);

#ifdef __cplusplus
}
#endif
