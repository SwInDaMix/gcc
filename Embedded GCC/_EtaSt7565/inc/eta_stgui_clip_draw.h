/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_common.h
 ***********************************************************************/

#pragma once

#include "eta_st7565.h"

typedef enum {
    DRAW_MODE_OR,
    DRAW_MODE_AND,
    DRAW_MODE_XOR,
    DRAW_MODE_FLAG_INVERTED = 0x80,
} eStGUI_DrawModeFlags;

bool StGUI_ClipDraw_PutColumn(uint16_t x, uint16_t y, uint16_t height, eStGUI_DrawModeFlags draw_mode, uint8_t const *bmp, uint8_t const *mask);
bool StGUI_ClipDraw_EraseColumn(uint16_t x, uint16_t y, uint16_t height, eStGUI_DrawModeFlags draw_mode);
