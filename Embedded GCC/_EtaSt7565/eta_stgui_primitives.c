/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_primitives.c
 ***********************************************************************/

#include "eta_stgui_clip_draw.h"
#include "eta_stgui_primitives.h"

/// ******************************************************************************************************
/// Frame
/// ******************************************************************************************************

static void _generate_frame_border_column(uint8_t *column, uint16_t height, bool is_space_before, bool is_space_after) {
    if (is_space_before) height++;
    uint16_t _l_border = height - 1;
    uint8_t _nb = is_space_before ? 0xFC : 0xFE;
    if (is_space_after) height++;

    while (true) {
        uint8_t _b = _nb;
        if (_l_border >= 8) _l_border -= 8;
        else { _b &= ((1 << _l_border) - 1); _l_border = 0; }

        *column++ = _b;
        _nb = 0xFF;

        if (height > 8) height -= 8;
        else break;
    }
}

static void _generate_frame_main_column(uint8_t *column, uint16_t height, uint16_t height_caption, bool is_space_before, bool is_space_after) {
    uint16_t _l_caption = height_caption + 1;
    if (is_space_before) { height++; _l_caption++; }
    uint16_t _l_bottom_border = height - 1;
    uint8_t _nb = is_space_before ? 0xFE : 0xFF;
    if (is_space_after) height++;

    while (true) {
        uint8_t _b = _nb;
        if (_l_caption >= 8) _l_caption -= 8;
        else { _b &= ((1 << _l_caption) - 1); _l_caption = 0; }

        if (_l_bottom_border >= 8) _l_bottom_border -= 8;
        else { _b |= (1 << _l_bottom_border); _l_bottom_border = UINT16_MAX; }

        *column++ = _b;
        _nb = 0xFF;

        if (height > 8) height -= 8;
        else break;
    }
}

/// ******************************************************************************************************
/// Clear area
/// ******************************************************************************************************

void StGUI_Primitive_Clear(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    for (uint16_t _fc = 0; _fc < width; _fc++) {
        if (!StGUI_ClipDraw_EraseColumn(x + _fc, y, height, DRAW_MODE_OR)) break;
    }
}

/// ******************************************************************************************************
/// Frame
/// ******************************************************************************************************

#define STGUI_PRIMITIVE_FRAME_BORDER 1

void StGUI_Primitive_FrameMeasure(sStGUI_Font const *font_caption, char const *caption, sStGUI_Primitive_FrameMeasure *frame_measure) {
    frame_measure->top = caption ? (STGUI_PRIMITIVE_FRAME_BORDER + font_caption->height + 1) : STGUI_PRIMITIVE_FRAME_BORDER;
    frame_measure->left_right_bottom = STGUI_PRIMITIVE_FRAME_BORDER;
}

void StGUI_Primitive_FrameDraw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t ms10ticks, sStGUI_Font const *font_caption, char const *caption, ...) { va_list argp; va_start(argp, caption); StGUI_Primitive_FrameDrawVA(x, y, width, height, ms10ticks, font_caption, caption, argp); va_end(argp); }
void StGUI_Primitive_FrameDrawVA(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t ms10ticks, sStGUI_Font const *font_caption, char const *caption, va_list ap) {
    if (width < 2 || !height) return;

    uint16_t _height_caption = caption ? font_caption->height + 1 : 0;
    uint16_t _height_total = height + (STGUI_PRIMITIVE_FRAME_BORDER << 1) + _height_caption;
    uint16_t _width_total = width + (STGUI_PRIMITIVE_FRAME_BORDER << 1);
    if (_height_total > GLCD_HEIGHT + (STGUI_PRIMITIVE_FRAME_BORDER << 1)) _height_total = GLCD_HEIGHT + (STGUI_PRIMITIVE_FRAME_BORDER << 1);

    if (x && !StGUI_ClipDraw_EraseColumn(x - 1, y, _height_total, DRAW_MODE_OR)) return;

    uint16_t _height_main = _height_total + ((bool)y ? 2 : 1);
    uint16_t _y_main = (bool)y ? y - 1 : y;

    uint8_t _column_border[(GLCD_HEIGHT >> 3) + 1];
    uint8_t _column_main[(GLCD_HEIGHT >> 3) + 1];
    _generate_frame_border_column(_column_border, _height_total, (bool)y, true);
    _generate_frame_main_column(_column_main, _height_total, _height_caption, (bool)y, true);

    for (uint16_t _fc = 0; _fc < _width_total; _fc++) {
        if (!StGUI_ClipDraw_PutColumn(x + _fc, _y_main, _height_main, DRAW_MODE_OR, (_fc == 0 || _fc == (_width_total - 1)) ? _column_border : _column_main, NULL)) break;
    }
    StGUI_ClipDraw_EraseColumn(x + _width_total, y, _height_total, DRAW_MODE_OR);

    if (caption) StGUI_StringDrawMaxWidthVA(font_caption, x + STGUI_PRIMITIVE_FRAME_BORDER + 1, y + STGUI_PRIMITIVE_FRAME_BORDER, width - 2, 0, ALIGNMENT_CENTER, DRAW_MODE_XOR, ms10ticks, caption, ap);
}

/// ******************************************************************************************************
/// Arrow mark
/// ******************************************************************************************************

uint16_t StGUI_Primitive_ArrowMarkMeasureWidth(uint16_t height) { return ((height + 1) >> 1); }

void StGUI_Primitive_ArrowMarkDraw(uint16_t x, uint16_t y, uint16_t height) {
    if (height < 3 || height > (sizeof(uint64_t) * 8)) return;

    uint64_t _value = UINT64_MAX;
    while (height) {
        if (!StGUI_ClipDraw_PutColumn(x, y, height, DRAW_MODE_XOR, (uint8_t *)&_value, NULL)) return;
        x++; y++;
        if (height > 2) height -= 2;
        else break;
    }
}

/// ******************************************************************************************************
/// Checkbox mark
/// ******************************************************************************************************

uint16_t StGUI_Primitive_CheckboxMarkMeasureWidth(uint16_t height) { return height; }

void StGUI_Primitive_CheckboxMarkDraw(uint16_t x, uint16_t y, uint16_t height, bool is_checked) {
    if (height < 5 || height > (sizeof(uint64_t) * 8)) return;

    uint64_t _mask = (((uint64_t)1 << (height - 2)) - 1) << 1;
    uint64_t _not_mask = ~_mask;
    uint64_t _check_mark = (((uint64_t)1 << (height - 4)) - 1) << 2;
    uint64_t _column = ((uint64_t)1 << height) - 1;

    for (uint16_t _ic = 0; _ic < height; _ic++) {
        if (_ic == 0 || _ic == height - 1) { if(!StGUI_ClipDraw_PutColumn(x, y, height, DRAW_MODE_OR, (uint8_t *)&_mask, (uint8_t *)&_not_mask)) return; }
        else {
            uint32_t _col = _mask ^ _column;
            if (is_checked && _ic != 1 && _ic != height - 2) _col |= _check_mark;
            if(!StGUI_ClipDraw_PutColumn(x, y, height, DRAW_MODE_OR, (uint8_t *)&_col, NULL)) return;
        }
        x++;
    }
}

/// ******************************************************************************************************
/// Highlight
/// ******************************************************************************************************

#define STGUI_PRIMITIVE_HIGHLIGHT_WIDTH 2
#define STGUI_PRIMITIVE_HIGHLIGHT_HEIGHT 1

sStGUI_Primitive_HighlightIndents const *StGUI_Primitive_HighlightIndents() {
    static sStGUI_Primitive_HighlightIndents const _s_highlight_indents = {
        .left_right = STGUI_PRIMITIVE_HIGHLIGHT_WIDTH,
        .top_bottom = STGUI_PRIMITIVE_HIGHLIGHT_HEIGHT
    };
    return &_s_highlight_indents;
}

void StGUI_Primitive_HighlightDraw(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    sStGUI_Primitive_HighlightIndents const *_highlight_indents = StGUI_Primitive_HighlightIndents();

    if (width < (_highlight_indents->left_right << 1)) return;
    if (height < ((_highlight_indents->top_bottom << 1) + 1) || height > (sizeof(uint64_t) * 8)) return;

    uint64_t _border = (((uint64_t)1 << (height - 2)) - 1) << 1;
    uint64_t _main = ((uint64_t)1 << height) - 1;

    for (uint16_t _ic = 0; _ic < width; _ic++) {
        if(!StGUI_ClipDraw_PutColumn(x, y, height, DRAW_MODE_OR, (_ic == 0 || _ic == width - 1) ? (uint8_t *)&_border : (uint8_t *)&_main, NULL)) return;
        x++;
    }
}

void StGUI_Primitive_HighlightDrawContent(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool is_highlight, d_stgui_primitive_highlight_draw_content draw_content) {
    sStGUI_Primitive_HighlightIndents const *_highlight_indents = StGUI_Primitive_HighlightIndents();

    if (width < (_highlight_indents->left_right << 1)) return;

    if (is_highlight) StGUI_Primitive_HighlightDraw(x, y, width, height);
    draw_content(x + _highlight_indents->left_right, y + _highlight_indents->top_bottom, width - (_highlight_indents->left_right << 1), height - (_highlight_indents->top_bottom << 1));

}

uint16_t StGUI_Primitive_HighlightWithStringMeasureHeight(sStGUI_Font const *font) {
    sStGUI_Primitive_HighlightIndents const *_highlight_indents = StGUI_Primitive_HighlightIndents();
    return font->height + (_highlight_indents->top_bottom << 1);
}

uint16_t StGUI_Primitive_HighlightWithStringMeasureWidth(sStGUI_Font const *font, char const *fmt, ...) { va_list argp; va_start(argp, fmt); uint16_t _width = StGUI_Primitive_HighlightWithStringMeasureWidthVA(font, fmt, argp); va_end(argp); return _width; }
uint16_t StGUI_Primitive_HighlightWithStringMeasureWidthVA(sStGUI_Font const *font, char const *fmt, va_list ap) {
    sStGUI_Primitive_HighlightIndents const *_highlight_indents = StGUI_Primitive_HighlightIndents();
    return StGUI_StringMeasureVA(font, fmt, ap) + (_highlight_indents->left_right << 1);
}

void StGUI_Primitive_HighlightWithStringDraw(uint16_t x, uint16_t y, uint16_t max_width, eStGUI_Alignment alignment, bool is_highlight, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, ...) { va_list argp; va_start(argp, fmt); StGUI_Primitive_HighlightWithStringDrawVA(x, y, max_width, alignment, is_highlight, ms10ticks, font, fmt, argp); va_end(argp); }
void StGUI_Primitive_HighlightWithStringDrawVA(uint16_t x, uint16_t y, uint16_t max_width, eStGUI_Alignment alignment, bool is_highlight, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, va_list ap) {
    __extension__ void _draw_content(uint16_t h_x, uint16_t h_y, uint16_t h_width, uint16_t h_height) {
        StGUI_StringDrawMaxWidthVA(font, h_x, h_y, h_width, 0, alignment, DRAW_MODE_XOR, ms10ticks, fmt, ap);
    }

    StGUI_Primitive_HighlightDrawContent(x, y, max_width, StGUI_Primitive_HighlightWithStringMeasureHeight(font), is_highlight, _draw_content);
}

/// ******************************************************************************************************
/// Highlight with arrow mark with string
/// ******************************************************************************************************

uint16_t StGUI_Primitive_HighlightWithStringAndArrowMarkMeasureWidth(sStGUI_Font const *font, char const *fmt, ...) { va_list argp; va_start(argp, fmt); uint16_t _width = StGUI_Primitive_HighlightWithStringAndArrowMarkMeasureWidthVA(font, fmt, argp); va_end(argp); return _width; }
uint16_t StGUI_Primitive_HighlightWithStringAndArrowMarkMeasureWidthVA(sStGUI_Font const *font, char const *fmt, va_list ap) {
    sStGUI_Primitive_HighlightIndents const *_highlight_indents = StGUI_Primitive_HighlightIndents();
    return (_highlight_indents->left_right << 1) + StGUI_StringMeasureVA(font, fmt, ap) + STGUI_PRIMITIVE_ARROW_MARK_RECOMMENDED_SEPARATOR + StGUI_Primitive_ArrowMarkMeasureWidth(font->height);
}

void StGUI_Primitive_HighlightWithStringAndArrowMarkDraw(uint16_t x, uint16_t y, uint16_t max_width, bool is_highlight, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, ...) { va_list argp; va_start(argp, fmt); StGUI_Primitive_HighlightWithStringAndArrowMarkDrawVA(x, y, max_width, is_highlight, ms10ticks, font, fmt, argp); va_end(argp); }
void StGUI_Primitive_HighlightWithStringAndArrowMarkDrawVA(uint16_t x, uint16_t y, uint16_t max_width, bool is_highlight, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, va_list ap) {
    __extension__ void _draw_content(uint16_t h_x, uint16_t h_y, uint16_t h_width, uint16_t h_height) {
        uint16_t _mark_width = StGUI_Primitive_ArrowMarkMeasureWidth(font->height);
        if (h_width < (STGUI_PRIMITIVE_ARROW_MARK_RECOMMENDED_SEPARATOR + _mark_width)) return;
        StGUI_StringDrawMaxWidthVA(font, h_x, h_y, h_width - STGUI_PRIMITIVE_ARROW_MARK_RECOMMENDED_SEPARATOR - _mark_width, 0, ALIGNMENT_START, DRAW_MODE_XOR, ms10ticks, fmt, ap);
        StGUI_Primitive_ArrowMarkDraw(h_x + h_width - _mark_width, h_y, font->height);
    }

    StGUI_Primitive_HighlightDrawContent(x, y, max_width, StGUI_Primitive_HighlightWithStringMeasureHeight(font), is_highlight, _draw_content);
}

/// ******************************************************************************************************
/// Highlight with checkbox mark with string
/// ******************************************************************************************************

uint16_t StGUI_Primitive_HighlightWithStringAndCheckboxMarkMeasureWidth(sStGUI_Font const *font, char const *fmt, ...) { va_list argp; va_start(argp, fmt); uint16_t _width = StGUI_Primitive_HighlightWithStringAndCheckboxMarkMeasureWidthVA(font, fmt, argp); va_end(argp); return _width; }
uint16_t StGUI_Primitive_HighlightWithStringAndCheckboxMarkMeasureWidthVA(sStGUI_Font const *font, char const *fmt, va_list ap) {
    sStGUI_Primitive_HighlightIndents const *_highlight_indents = StGUI_Primitive_HighlightIndents();
    return (_highlight_indents->left_right << 1) + StGUI_StringMeasureVA(font, fmt, ap) + STGUI_PRIMITIVE_CHECKBOX_RECOMMENDED_SEPARATOR + StGUI_Primitive_CheckboxMarkMeasureWidth(font->height);
}

void StGUI_Primitive_HighlightWithStringAndCheckboxMarkDraw(uint16_t x, uint16_t y, uint16_t max_width, bool is_highlight, bool is_checked, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, ...) { va_list argp; va_start(argp, fmt); StGUI_Primitive_HighlightWithStringAndCheckboxMarkDrawVA(x, y, max_width, is_highlight, is_checked, ms10ticks, font, fmt, argp); va_end(argp); }
void StGUI_Primitive_HighlightWithStringAndCheckboxMarkDrawVA(uint16_t x, uint16_t y, uint16_t max_width, bool is_highlight, bool is_checked, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, va_list ap) {
    __extension__ void _draw_content(uint16_t h_x, uint16_t h_y, uint16_t h_width, uint16_t h_height) {
        uint16_t _mark_width = StGUI_Primitive_CheckboxMarkMeasureWidth(font->height);
        if (h_width < (STGUI_PRIMITIVE_CHECKBOX_RECOMMENDED_SEPARATOR + _mark_width)) return;
        StGUI_StringDrawMaxWidthVA(font, h_x, h_y, h_width - STGUI_PRIMITIVE_CHECKBOX_RECOMMENDED_SEPARATOR - _mark_width, 0, ALIGNMENT_START, DRAW_MODE_XOR, ms10ticks, fmt, ap);
        StGUI_Primitive_CheckboxMarkDraw(h_x + h_width - _mark_width, h_y, font->height, is_checked);
    }

    StGUI_Primitive_HighlightDrawContent(x, y, max_width, StGUI_Primitive_HighlightWithStringMeasureHeight(font), is_highlight, _draw_content);
}

/// ******************************************************************************************************
/// Choices selection
/// ******************************************************************************************************

#define STGUI_PRIMITIVE_CHOICES_SEPARATOR_TEXT "="

uint16_t StGUI_Primitive_HighlightWithChoicesMeasureWidth(sStGUI_Font const *font, char const *title, char const *const *choices, uint8_t choices_count, sStGUI_Primitive_ChoicesMeasure *choices_measure) {
    sStGUI_Primitive_HighlightIndents const *_highlight_indents = StGUI_Primitive_HighlightIndents();
    choices_measure->title_width = StGUI_StringMeasure(font, title);
    choices_measure->separator_width = _highlight_indents->left_right + StGUI_StringMeasure(font, STGUI_PRIMITIVE_CHOICES_SEPARATOR_TEXT) + 2;
    uint16_t _cw_max = 0;
    for (uint8_t _ci = 0; _ci < choices_count; _ci++) {
        uint16_t _cw = StGUI_StringMeasure(font, choices[_ci]);
        maxwith(_cw_max, _cw);
    }
    choices_measure->choices_width = _cw_max;
    return choices_measure->title_width + choices_measure->separator_width + choices_measure->choices_width + (_highlight_indents->left_right * 3);
}

void StGUI_Primitive_HighlightWithChoicesDraw(uint16_t x, uint16_t y, uint16_t max_width, sStGUI_Primitive_ChoicesHighlight choices_highlight, uint8_t choice_selected, uint32_t ms10ticks, sStGUI_Font const *font, char const *title, char const *const *choices, sStGUI_Primitive_ChoicesMeasure const *choices_measure) {
    sStGUI_Primitive_HighlightIndents const *_highlight_indents = StGUI_Primitive_HighlightIndents();
    uint16_t _extra_width = choices_measure->separator_width + (_highlight_indents->left_right * 3);
    if (max_width < _extra_width) return;

    uint16_t _highlight_height = StGUI_Primitive_HighlightWithStringMeasureHeight(font);
    uint16_t _available_width = max_width - _extra_width;
    uint16_t _title_width = choices_measure->title_width;
    uint16_t _choices_width = choices_measure->choices_width;

    if ((_title_width + _choices_width) > _available_width) {
        uint32_t _diff = (_title_width + _choices_width) - _available_width;
        _title_width -= (_title_width * _diff / (_title_width + _choices_width));
        _choices_width = _available_width - _title_width;
    }

    __extension__ void _draw_content_title(uint16_t h_x, uint16_t h_y, uint16_t h_width, uint16_t h_height) {
        StGUI_StringDrawMaxWidth(font, h_x, h_y, _title_width, choices_measure->title_width, ALIGNMENT_START, DRAW_MODE_XOR, ms10ticks, title);
        StGUI_StringDrawMaxWidth(font, h_x + _title_width + _highlight_indents->left_right + 1, h_y, choices_measure->separator_width, choices_measure->separator_width, ALIGNMENT_START, DRAW_MODE_XOR, ms10ticks, STGUI_PRIMITIVE_CHOICES_SEPARATOR_TEXT);
    }
    __extension__ void _draw_content_choice(uint16_t h_x, uint16_t h_y, uint16_t h_width, uint16_t h_height) {
        StGUI_StringDrawMaxWidth(font, h_x, h_y, _choices_width, 0, ALIGNMENT_START, DRAW_MODE_XOR, ms10ticks, choices[choice_selected]);    }

    StGUI_Primitive_HighlightDrawContent(x, y, max_width, _highlight_height, choices_highlight == PRIMITIVE_CHOICES_HIGHLIGHT_FULL, _draw_content_title);
    StGUI_Primitive_HighlightDrawContent(x + _highlight_indents->left_right + _title_width + choices_measure->separator_width, y, _choices_width + (_highlight_indents->left_right << 1), _highlight_height, choices_highlight == PRIMITIVE_CHOICES_HIGHLIGHT_CHOICE_ONLY, _draw_content_choice);
}

/// ******************************************************************************************************
/// Arbitrary progress bar
/// ******************************************************************************************************

#define STGUI_PRIMITIVE_PROGRESS_WIDTH 1

void StGUI_Primitive_ProgressDraw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t progress, eStGUI_DrawModeFlags draw_mode, uint32_t ms10ticks) {
    if (width <= (STGUI_PRIMITIVE_PROGRESS_WIDTH << 1) || height <= (STGUI_PRIMITIVE_PROGRESS_WIDTH << 1) || height > (sizeof(uint64_t) * 8)) return;

    uint16_t _bar_width = width - (STGUI_PRIMITIVE_PROGRESS_WIDTH << 1);
    uint16_t _ps, _pe;
    bool _pxor = false;
    if (progress <= UINT8_MAX) {
        _ps = 0; _pe = _bar_width * progress / UINT8_MAX;
    } else {
        _ps = (uint8_t)(ms10ticks / 3);
        _pe = (uint8_t)(ms10ticks);
        if (_ps > _pe) { xchg(_ps, _pe); _pxor = true; }
        _ps = _bar_width * _ps / UINT8_MAX;
        _pe = _bar_width * _pe / UINT8_MAX;
        if ((ms10ticks / 384) & 1) _pxor = !_pxor;
    }


    uint64_t _mask = (((uint64_t)1 << (height - 2)) - 1) << 1;
    uint64_t _column = ((uint64_t)1 << height) - 1;

    for (uint16_t _ic = 0; _ic < width; _ic++) {
        if (_ic == 0 || _ic == width - 1) { if(!StGUI_ClipDraw_PutColumn(x, y, height, draw_mode, (uint8_t *)&_mask, NULL)) return; }
        else {
            uint64_t _col = (((_ic - 1) >= _ps && (_ic - 1) < _pe) ^ _pxor) ? _column : _column ^ _mask;
            if(!StGUI_ClipDraw_PutColumn(x, y, height, draw_mode, (uint8_t *)&_col, NULL)) return;
        }
        x++;
    }
}

/// ******************************************************************************************************
/// Progress bar with string
/// ******************************************************************************************************

#define STGUI_PRIMITIVE_PROGRESS_WITH_STRING_INDENT 1
#define STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH (STGUI_PRIMITIVE_PROGRESS_WIDTH + STGUI_PRIMITIVE_PROGRESS_WITH_STRING_INDENT)

void StGUI_Primitive_ProgressWithStringMeasure(sStGUI_Font const *font, char const *title, sStGUI_Primitive_ProgressWithStringMeasure *progress_with_string_measure) {
    progress_with_string_measure->width = StGUI_StringMeasure(font, title ? "s: 100%%" : "100%%", title) + (STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH << 1);
    progress_with_string_measure->height = font->height + (STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH << 1);
}

void StGUI_Primitive_ProgressWithStringDraw(uint16_t x, uint16_t y, uint16_t max_width, uint16_t progress, uint32_t ms10ticks, sStGUI_Font const *font, char const *title) {
    if (max_width < (STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH << 1)) return;

    StGUI_Primitive_ProgressDraw(x, y, max_width, font->height + (STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH << 1), progress, DRAW_MODE_OR, ms10ticks);
    if (progress <= UINT8_MAX) {
        if (title) StGUI_StringDrawMaxWidth(font, x + STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH, y + STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH, max_width - (STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH << 1), 0, ALIGNMENT_CENTER, DRAW_MODE_XOR, ms10ticks, "%s: %u%%", title, 100 * progress / UINT8_MAX);
        else StGUI_StringDrawMaxWidth(font, x + STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH, y + STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH, max_width - (STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH << 1), 0, ALIGNMENT_CENTER, DRAW_MODE_XOR, ms10ticks, "%u%%", 100 * progress / UINT8_MAX);
    }
    else StGUI_StringDrawMaxWidth(font, x + STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH, y + STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH, max_width - (STGUI_PRIMITIVE_PROGRESS_WITH_STRING_WIDTH << 1), 0, ALIGNMENT_CENTER, DRAW_MODE_XOR, ms10ticks, "%s...", title ? title : "");
}

/// ******************************************************************************************************
/// Button
/// ******************************************************************************************************

#define STGUI_PRIMITIVE_BUTTON_INDENT 1
#define STGUI_PRIMITIVE_BUTTON_WIDTH (STGUI_PRIMITIVE_PROGRESS_WIDTH + STGUI_PRIMITIVE_BUTTON_INDENT)

void StGUI_Primitive_ButtonMeasure(sStGUI_Font const *font, sStGUI_Primitive_ButtonMeasure *button_measure, char const *fmt, ...) { va_list argp; va_start(argp, fmt); StGUI_Primitive_ButtonMeasureVA(font, button_measure, fmt, argp); va_end(argp); }
void StGUI_Primitive_ButtonMeasureVA(sStGUI_Font const *font, sStGUI_Primitive_ButtonMeasure *button_measure, char const *fmt, va_list ap) {
    button_measure->width = StGUI_StringMeasureVA(font, fmt, ap) + (STGUI_PRIMITIVE_BUTTON_WIDTH << 1);
    button_measure->height = font->height + (STGUI_PRIMITIVE_BUTTON_WIDTH << 1);
}

void StGUI_Primitive_ButtonDraw(uint16_t x, uint16_t y, uint16_t max_width, bool is_selected, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, ...) { va_list argp; va_start(argp, fmt); StGUI_Primitive_ButtonDrawVA(x, y, max_width, is_selected, ms10ticks, font, fmt, argp); va_end(argp); }
void StGUI_Primitive_ButtonDrawVA(uint16_t x, uint16_t y, uint16_t max_width, bool is_selected, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, va_list ap) {
    if (max_width < (STGUI_PRIMITIVE_BUTTON_WIDTH << 1)) return;

    StGUI_Primitive_ProgressDraw(x, y, max_width, font->height + (STGUI_PRIMITIVE_BUTTON_WIDTH << 1), is_selected ? UINT8_MAX : 0, DRAW_MODE_OR, ms10ticks);
    StGUI_StringDrawMaxWidth(font, x + STGUI_PRIMITIVE_BUTTON_WIDTH, y + STGUI_PRIMITIVE_BUTTON_WIDTH, max_width - (STGUI_PRIMITIVE_BUTTON_WIDTH << 1), 0, ALIGNMENT_CENTER, DRAW_MODE_XOR, ms10ticks, fmt, ap);
}
