/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_bitmaps.c
 ***********************************************************************/

#include "eta_stgui_bitmaps.h"

void StGUI_BitmapDraw(sStGUI_Bitmap const *bitmap, uint16_t x, uint16_t y, eStGUI_DrawModeFlags draw_mode, uint32_t ms10ticks) {
    uint8_t _bitmap_height_bytes = (bitmap->height + 7) >> 3;

    uint8_t const *_src = bitmap->bmp;
    uint8_t _width = bitmap->width;
    while (_width--) {
        if (!StGUI_ClipDraw_PutColumn(x, y, bitmap->height, draw_mode, _src, NULL)) return;
        _src += _bitmap_height_bytes;
        x++;
    }
}
