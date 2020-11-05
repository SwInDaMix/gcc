/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_fonts.c
 ***********************************************************************/

#include "eta_stgui_fonts.h"

#include "fonts/small.hfont"
STGUI_FONT_DECLARE(small, SMALL);
#include "fonts/large_regular.hfont"
STGUI_FONT_DECLARE(large_regular, LARGE_REGULAR);
#include "fonts/large_bold.hfont"
STGUI_FONT_DECLARE(large_bold, LARGE_BOLD);

#define STGUI_FONT_STEP_DIVIDER 6

void StGUI_StringDrawMaxWidth(sStGUI_Font const *font, uint16_t x, uint16_t y, uint16_t max_len, uint16_t len, eStGUI_Alignment alignment, eStGUI_DrawModeFlags draw_mode, uint32_t ms10ticks, char const *fmt, ...) { va_list argp; va_start(argp, fmt); StGUI_StringDrawMaxWidthVA(font, x, y, max_len, len, alignment, draw_mode, ms10ticks, fmt, argp); va_end(argp); }
void StGUI_StringDrawMaxWidthVA(sStGUI_Font const *font, uint16_t x, uint16_t y, uint16_t max_len, uint16_t len, eStGUI_Alignment alignment, eStGUI_DrawModeFlags draw_mode, uint32_t ms10ticks, char const *fmt, va_list ap) {
    if (!len) len = StGUI_StringMeasureVA(font, fmt, ap);

    if (len <= max_len) {
        if (alignment == ALIGNMENT_CENTER) x += ((max_len - len) >> 1);
        else if (alignment == ALIGNMENT_END) x += (max_len - len);
        StGUI_StringDrawVA(font, x, y, 0, len, draw_mode, fmt, ap);
    }
    else {
        uint16_t _diff = len - max_len;
        uint16_t _steps = (_diff + (STGUI_FONT_RUNNING_TEXT_STEP - 1)) / STGUI_FONT_RUNNING_TEXT_STEP;

        uint32_t _step = (ms10ticks >> STGUI_FONT_STEP_DIVIDER) % ((_steps + STGUI_FONT_RUNNING_HOLD_STEPS) << 1);

        uint16_t _xoff;
        if (_step < _steps) _xoff = _diff * _step / _steps;
        else {
            _step -= _steps;
            if (_step < STGUI_FONT_RUNNING_HOLD_STEPS) _xoff = _diff;
            else {
                _step -= STGUI_FONT_RUNNING_HOLD_STEPS;
                if (_step < _steps) _xoff = _diff * (_steps - _step) / _steps;
                else _xoff = 0;
            }
        }
        StGUI_StringDrawVA(font, x, y, _xoff, max_len, draw_mode, fmt, ap);
    }
}

uint16_t StGUI_StringDraw(sStGUI_Font const *font, uint16_t x, uint16_t y, uint16_t xoff, uint16_t xlen, eStGUI_DrawModeFlags draw_mode, char const *fmt, ...) { va_list argp; va_start(argp, fmt); uint16_t _sz = StGUI_StringDrawVA(font, x, y, xoff, xlen, draw_mode, fmt, argp); va_end(argp); return _sz; }
uint16_t StGUI_StringDrawVA(sStGUI_Font const *font, uint16_t x, uint16_t y, uint16_t xoff, uint16_t xlen, eStGUI_DrawModeFlags draw_mode, char const *fmt, va_list ap) {
    if (x >= GLCD_WIDTH || y >= GLCD_HEIGHT) return 0;

    uint16_t _len = 0;
    bool _is_first = true;
    uint8_t _font_height_bytes = (font->height + 7) >> 3;

    __extension__ void putc(sFmtStream *strm, char c) {
        if (c >= font->char_start) {
            c -= font->char_start;
            if (c < font->char_count) {
                uint8_t _symbol_width = font->fwidth[(uint8_t)c];
                if (_is_first) _is_first = false;
                else {
                    if (xoff == 0 && xlen > 0) {
                        if (!StGUI_ClipDraw_EraseColumn(x, y, font->height, draw_mode)) return;
                        x++; xlen--;
                    }
                    if (xoff) xoff--;
                    _len++;
                }

                uint8_t const *_src = font->fbmp + ((uint8_t)c * font->width * _font_height_bytes);
                while (_symbol_width--) {
                    if (xoff == 0 && xlen > 0) {
                        if (!StGUI_ClipDraw_PutColumn(x, y, font->height, draw_mode, _src, NULL)) return;
                        x++;  xlen--;
                    }
                    if (xoff) xoff--;
                    _src += _font_height_bytes;
                    _len++;
                }
            }
        }
    };

    sFmtStream _strm = { .putc = &putc, .limit = -1, .flags = (sFormatFlags)0 };
    formatString(fmt, ap, &_strm);
    return _len;
}

uint16_t StGUI_StringMeasure(sStGUI_Font const *font, char const *fmt, ...) { va_list argp; va_start(argp, fmt); uint16_t _sz = StGUI_StringMeasureVA(font, fmt, argp); va_end(argp); return _sz; }
uint16_t StGUI_StringMeasureVA(sStGUI_Font const *font, char const *fmt, va_list ap) {
    uint16_t _len = 0;
    bool _is_first = true;

    __extension__ void putc(sFmtStream *strm, char c) {
        if (c >= font->char_start) {
            c -= font->char_start;
            if (c < font->char_count) {
                _len += font->fwidth[(uint8_t)c];
                if (_is_first) _is_first = false;
                else _len++;
            }
        }
    };

    sFmtStream _strm = { .putc = &putc, .limit = -1, .flags = (sFormatFlags)0 };
    formatString(fmt, ap, &_strm);
    return _len;
}
