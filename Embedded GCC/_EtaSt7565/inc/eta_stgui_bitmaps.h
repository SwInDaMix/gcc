/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_bitmaps.h
 ***********************************************************************/

#pragma once

#include "eta_st7565.h"
#include "eta_stgui_clip_draw.h"

#define STGUI_BITMAP_DECLARE_IN_HEADER(lower) extern StGUI_Bitmap const g_stgui_bitmap_##lower
#define STGUI_BITMAP_DECLARE(lower, upper) \
sStGUI_Bitmap const g_stgui_bitmap_##lower = { \
    .width = BITMAP_##upper##_WIDTH, \
    .height = BITMAP_##upper##_HEIGHT, \
    .bmp = g_bitmap_##lower, \
}

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t const *bmp;
    uint8_t bmp_count;
    uint32_t animation_speed_ms10;
} sStGUI_Bitmap;

void StGUI_BitmapDraw(sStGUI_Bitmap const *bitmap, uint16_t x, uint16_t y, eStGUI_DrawModeFlags draw_mode, uint32_t ms10ticks);
