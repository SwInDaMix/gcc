/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_fonts.h
 ***********************************************************************/

#pragma once

#include "eta_st7565.h"
#include "eta_stgui_common.h"
#include "eta_stgui_clip_draw.h"
#include "eta_string_format.h"

#define STGUI_FONT_RUNNING_TEXT_STEP 10
#define STGUI_FONT_RUNNING_HOLD_STEPS 3

#define STGUI_FONT_DECLARE_IN_HEADER(lower) extern sStGUI_Font const g_stgui_font_##lower
#define STGUI_FONT_DECLARE(lower, upper) \
sStGUI_Font const g_stgui_font_##lower = { \
    .fbmp = g_font_bmp_##lower, \
    .fwidth = g_font_width_##lower, \
    .width = FONT_##upper##_WIDTH, \
    .height = FONT_##upper##_HEIGHT, \
    .char_start = FONT_##upper##_START_CODE, \
    .char_count = FONT_##upper##_CHARS \
}

typedef struct {
    uint8_t const *fbmp;
    uint8_t const *fwidth;
    uint16_t width;
    uint16_t height;
    uint8_t char_start;
    uint8_t char_count;
} sStGUI_Font;

STGUI_FONT_DECLARE_IN_HEADER(small);
STGUI_FONT_DECLARE_IN_HEADER(large_regular);
STGUI_FONT_DECLARE_IN_HEADER(large_bold);

void StGUI_StringDrawMaxWidth(sStGUI_Font const *font, uint16_t x, uint16_t y, uint16_t max_len, uint16_t len, eStGUI_Alignment alignment, eStGUI_DrawModeFlags draw_mode, uint32_t ms10ticks, char const *fmt, ...);
void StGUI_StringDrawMaxWidthVA(sStGUI_Font const *font, uint16_t x, uint16_t y, uint16_t max_len, uint16_t len, eStGUI_Alignment alignment, eStGUI_DrawModeFlags draw_mode, uint32_t ms10ticks, char const *fmt, va_list ap);

uint16_t StGUI_StringDraw(sStGUI_Font const *font, uint16_t x, uint16_t y, uint16_t xoff, uint16_t xlen, eStGUI_DrawModeFlags draw_mode, char const *fmt, ...);
uint16_t StGUI_StringDrawVA(sStGUI_Font const *font, uint16_t x, uint16_t y, uint16_t xoff, uint16_t xlen, eStGUI_DrawModeFlags draw_mode, char const *fmt, va_list ap);

uint16_t StGUI_StringMeasure(sStGUI_Font const *font, char const *fmt, ...);
uint16_t StGUI_StringMeasureVA(sStGUI_Font const *font, char const *fmt, va_list ap);
