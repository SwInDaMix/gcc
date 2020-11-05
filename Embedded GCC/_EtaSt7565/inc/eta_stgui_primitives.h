/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_primitives.h
 ***********************************************************************/

#pragma once

#include "eta_stgui_fonts.h"

#define STGUI_PRIMITIVE_ARROW_MARK_RECOMMENDED_SEPARATOR 2
#define STGUI_PRIMITIVE_CHECKBOX_RECOMMENDED_SEPARATOR 2

void StGUI_Primitive_Clear(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

typedef struct {
    uint16_t top;
    uint16_t left_right_bottom;
} sStGUI_Primitive_FrameMeasure;
void StGUI_Primitive_FrameMeasure(sStGUI_Font const *font_caption, char const *caption, sStGUI_Primitive_FrameMeasure *frame_measure);
void StGUI_Primitive_FrameDraw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t ms10ticks, sStGUI_Font const *font_caption, char const *caption, ...);
void StGUI_Primitive_FrameDrawVA(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t ms10ticks, sStGUI_Font const *font_caption, char const *caption, va_list ap);

uint16_t StGUI_Primitive_ArrowMarkMeasureWidth(uint16_t height);
void StGUI_Primitive_ArrowMarkDraw(uint16_t x, uint16_t y, uint16_t height);

uint16_t StGUI_Primitive_CheckboxMarkMeasureWidth(uint16_t height);
void StGUI_Primitive_CheckboxMarkDraw(uint16_t x, uint16_t y, uint16_t height, bool is_checked);

typedef struct {
    uint16_t left_right;
    uint16_t top_bottom;
} sStGUI_Primitive_HighlightIndents;
typedef void (*d_stgui_primitive_highlight_draw_content)(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
sStGUI_Primitive_HighlightIndents const *StGUI_Primitive_HighlightIndents();
void StGUI_Primitive_HighlightDraw(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void StGUI_Primitive_HighlightDrawContent(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool is_highlight, d_stgui_primitive_highlight_draw_content draw_content);

uint16_t StGUI_Primitive_HighlightWithStringMeasureHeight(sStGUI_Font const *font);
uint16_t StGUI_Primitive_HighlightWithStringMeasureWidth(sStGUI_Font const *font, char const *fmt, ...);
uint16_t StGUI_Primitive_HighlightWithStringMeasureWidthVA(sStGUI_Font const *font, char const *fmt, va_list ap);
void StGUI_Primitive_HighlightWithStringDraw(uint16_t x, uint16_t y, uint16_t max_width, eStGUI_Alignment alignment, bool is_highlight, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, ...);
void StGUI_Primitive_HighlightWithStringDrawVA(uint16_t x, uint16_t y, uint16_t max_width, eStGUI_Alignment alignment, bool is_highlight, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, va_list ap);

uint16_t StGUI_Primitive_HighlightWithStringAndArrowMarkMeasureWidth(sStGUI_Font const *font, char const *fmt, ...);
uint16_t StGUI_Primitive_HighlightWithStringAndArrowMarkMeasureWidthVA(sStGUI_Font const *font, char const *fmt, va_list ap);
void StGUI_Primitive_HighlightWithStringAndArrowMarkDraw(uint16_t x, uint16_t y, uint16_t max_width, bool is_highlight, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, ...);
void StGUI_Primitive_HighlightWithStringAndArrowMarkDrawVA(uint16_t x, uint16_t y, uint16_t max_width, bool is_highlight, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, va_list ap);

uint16_t StGUI_Primitive_HighlightWithStringAndCheckboxMarkMeasureWidth(sStGUI_Font const *font, char const *fmt, ...);
uint16_t StGUI_Primitive_HighlightWithStringAndCheckboxMarkMeasureWidthVA(sStGUI_Font const *font, char const *fmt, va_list ap);
void StGUI_Primitive_HighlightWithStringAndCheckboxMarkDraw(uint16_t x, uint16_t y, uint16_t max_width, bool is_highlight, bool is_checked, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, ...);
void StGUI_Primitive_HighlightWithStringAndCheckboxMarkDrawVA(uint16_t x, uint16_t y, uint16_t max_width, bool is_highlight, bool is_checked, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, va_list ap);

typedef struct {
    uint16_t title_width;
    uint16_t separator_width;
    uint16_t choices_width;
} sStGUI_Primitive_ChoicesMeasure;
typedef enum {
    PRIMITIVE_CHOICES_HIGHLIGHT_NONE,
    PRIMITIVE_CHOICES_HIGHLIGHT_FULL,
    PRIMITIVE_CHOICES_HIGHLIGHT_CHOICE_ONLY,
} sStGUI_Primitive_ChoicesHighlight;
uint16_t StGUI_Primitive_HighlightWithChoicesMeasureWidth(sStGUI_Font const *font, char const *title, char const *const *choices, uint8_t choices_count, sStGUI_Primitive_ChoicesMeasure *choices_measure);
void StGUI_Primitive_HighlightWithChoicesDraw(uint16_t x, uint16_t y, uint16_t max_width, sStGUI_Primitive_ChoicesHighlight choices_highlight, uint8_t choice_selected, uint32_t ms10ticks, sStGUI_Font const *font, char const *title, char const *const *choices, sStGUI_Primitive_ChoicesMeasure const *choices_measure);

void StGUI_Primitive_ProgressDraw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t progress, eStGUI_DrawModeFlags draw_mode, uint32_t ms10ticks);

typedef struct {
    uint16_t width;
    uint16_t height;
} sStGUI_Primitive_ProgressWithStringMeasure;
void StGUI_Primitive_ProgressWithStringMeasure(sStGUI_Font const *font, char const *title, sStGUI_Primitive_ProgressWithStringMeasure *progress_with_string_measure);
void StGUI_Primitive_ProgressWithStringDraw(uint16_t x, uint16_t y, uint16_t max_width, uint16_t progress, uint32_t ms10ticks, sStGUI_Font const *font, char const *title);

typedef struct {
    uint16_t width;
    uint16_t height;
} sStGUI_Primitive_ButtonMeasure;
void StGUI_Primitive_ButtonMeasure(sStGUI_Font const *font, sStGUI_Primitive_ButtonMeasure *button_measure, char const *fmt, ...);
void StGUI_Primitive_ButtonMeasureVA(sStGUI_Font const *font, sStGUI_Primitive_ButtonMeasure *button_measure, char const *fmt, va_list ap);

void StGUI_Primitive_ButtonDraw(uint16_t x, uint16_t y, uint16_t max_width, bool is_selected, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, ...);
void StGUI_Primitive_ButtonDrawVA(uint16_t x, uint16_t y, uint16_t max_width, bool is_selected, uint32_t ms10ticks, sStGUI_Font const *font, char const *fmt, va_list ap);
