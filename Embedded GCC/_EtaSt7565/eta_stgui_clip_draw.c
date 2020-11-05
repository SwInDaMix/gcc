/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_fonts.c
 ***********************************************************************/

#include "eta_stgui_clip_draw.h"

bool StGUI_ClipDraw_PutColumn(uint16_t x, uint16_t y, uint16_t height, eStGUI_DrawModeFlags draw_mode, uint8_t const *bmp, uint8_t const *mask) {
    if (x >= GLCD_WIDTH || y >= GLCD_HEIGHT || !height) return false;

    eStGUI_DrawModeFlags _draw_mode_simple = draw_mode & 0x7F;
    if ((y + height) > GLCD_HEIGHT) height = GLCD_HEIGHT - y;
    uint8_t _shift = (y & 0x7);
    uint8_t *_dst = GLCD_GetScreen() + x + ((y >> 3) * GLCD_WIDTH);

    uint8_t _end_left = (y + height) & 7;
    uint8_t _end_mask = _end_left ? ~((1 << (_end_left)) - 1) : 0;
    uint8_t _left_mask = (1 << _shift) - 1;
    uint8_t _left_bmp = 0;
    uint16_t _len = (height + _shift + 7) >> 3;
    while (_len--) {
        uint8_t _b = *_dst;
        uint16_t _mask_ex = ((mask ? (uint16_t)(*mask++) : (_draw_mode_simple == DRAW_MODE_OR ? 0 : 0xFF)) << _shift) | _left_mask;
        if (!_len) _mask_ex |= _end_mask;
        _b &= _mask_ex;

        uint8_t _bmp = bmp ? *bmp++ : 0;
        if (draw_mode & DRAW_MODE_FLAG_INVERTED) _bmp = ~_bmp;
        uint16_t _bmp_ex = ((uint16_t)_bmp << _shift) | _left_bmp;
        _bmp = (uint8_t)_bmp_ex;
        if (!_len) _bmp &= ~_end_mask;

        if (_draw_mode_simple == DRAW_MODE_OR) _b |= _bmp;
        else if (_draw_mode_simple == DRAW_MODE_AND) _b &= _bmp;
        else if (_draw_mode_simple == DRAW_MODE_XOR) _b ^= _bmp;
        *_dst = _b;
        _dst += GLCD_WIDTH;

        _left_mask = (uint8_t)(_mask_ex >> 8);
        _left_bmp = (uint8_t)(_bmp_ex >> 8);
    }

    return true;
}

bool StGUI_ClipDraw_EraseColumn(uint16_t x, uint16_t y, uint16_t height, eStGUI_DrawModeFlags draw_mode) { return StGUI_ClipDraw_PutColumn(x, y, height, draw_mode, NULL, NULL); }
