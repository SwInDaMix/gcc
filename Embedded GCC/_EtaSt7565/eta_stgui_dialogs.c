/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_dialogs.c
 ***********************************************************************/

#include "eta_stgui_primitives.h"
#include "eta_stgui_dialogs.h"

#define STGUI_DIALOG_SPACER_WIDTH 2
#define STGUI_DIALOG_POINTER_IS_INDEX(dialog_pointer) (((dialog_pointer)->index & (STGUI_DIALOG_POINTER_MAGIC_MASK)) == (STGUI_DIALOG_POINTER_MAGIC))
#define STGUI_DIALOG_POINTER_GET_INDEX(dialog_pointer) ((dialog_pointer)->index & STGUI_DIALOG_POINTER_INDEX_MASK)

typedef struct {
    sStGUI_DialogItemBase const *item;
    uint16_t min_width;
    uint16_t min_height;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    union {
        struct {
            sStGUI_Primitive_FrameMeasure frame_measure;
        } dialog;
        struct {
            sStGUI_Primitive_ChoicesMeasure measure;
        } choices;
    } helper;
} sStGUI_DialogItemCalcs;

typedef struct {
    sStGUI_DialogController const *dialog_controller;
    sStGUI_DialogItemCalcs *item_calcs;
    sStGUI_Primitive_HighlightIndents const *highlight_indents;
    uint16_t highlight_item_height;
    sStGUI_Font const *font_caption;
    sStGUI_Font const *font_items;
    uint16_t total_items;
    uint16_t selected_item;
} sStGUI_DialogContext;

static sStGUI_DialogValue _get_value(sStGUI_DialogContext const *ctx, sStGUI_DialogPointer const *dialog_pointer) { return STGUI_DIALOG_POINTER_IS_INDEX(dialog_pointer) ? ctx->dialog_controller->get_value(STGUI_DIALOG_POINTER_GET_INDEX(dialog_pointer)) : dialog_pointer->value; }

static sStGUI_DialogItemBase const *_get_selected(sStGUI_DialogContext const *ctx) { return ctx->selected_item != UINT16_MAX ? ctx->item_calcs[ctx->selected_item].item : NULL; }
static bool _is_selected(sStGUI_DialogContext const *ctx, sStGUI_DialogItemBase const *current_item) { return ctx->selected_item != UINT16_MAX && ctx->item_calcs[ctx->selected_item].item == current_item; }
static bool _is_selectable(sStGUI_DialogItemBase const *item) { return item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_BUTTON || item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_CHECKBOX || item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_CHOICES; }

static void _select_next(sStGUI_DialogContext *ctx) {
    uint16_t _start_idx = ctx->selected_item != UINT16_MAX ? ctx->selected_item : ctx->total_items - 1;
    uint16_t _cur_idx = _start_idx;
    do {
        _cur_idx++;
        if (_cur_idx >= ctx->total_items) _cur_idx = 0;

        if (_is_selectable(ctx->item_calcs[_cur_idx].item)) {
            ctx->selected_item = _cur_idx;
            break;
        }
    } while (_cur_idx != _start_idx);
}
static void _select_prev(sStGUI_DialogContext *ctx) {
    uint16_t _start_idx = ctx->selected_item != UINT16_MAX ? ctx->selected_item : 0;
    uint16_t _cur_idx = _start_idx;
    do {
        if (_cur_idx == 0) _cur_idx = ctx->total_items;
        _cur_idx--;

        if (_is_selectable(ctx->item_calcs[_cur_idx].item)) {
            ctx->selected_item = _cur_idx;
            break;
        }
    } while (_cur_idx != _start_idx);
}

static uint16_t _calc_items(sStGUI_DialogItemBase const *current_item) {
    uint16_t _items = 1;
    if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_CONTAINER) {
        sStGUI_DialogItemContainer const *_item_container = (sStGUI_DialogItemContainer const *)current_item;
        for (uint8_t _ci = 0; _ci < _item_container->child_items_count; _ci++) _items += _calc_items(_item_container->child_items[_ci]);
    } else if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_SINGLE) {
        sStGUI_DialogItemSingle const *_item_single = (sStGUI_DialogItemSingle const *)current_item;
        _items += _calc_items(_item_single->child_item);
    }
    return _items;
}

static uint16_t _assing_items(sStGUI_DialogItemBase const *current_item, sStGUI_DialogItemCalcs *item_calcs) {
    uint16_t _items = 1;
    item_calcs->item = current_item;
    if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_CONTAINER) {
        sStGUI_DialogItemContainer const *_item_container = (sStGUI_DialogItemContainer const *)current_item;
        sStGUI_DialogItemCalcs *_child_item_calcs = item_calcs + 1;
        for (uint8_t _ci = 0; _ci < _item_container->child_items_count; _ci++) {
            uint16_t _citems = _assing_items(_item_container->child_items[_ci], _child_item_calcs);
            _child_item_calcs += _citems;
            _items += _citems;
        }
    } else if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_SINGLE) {
        sStGUI_DialogItemSingle const *_item_single = (sStGUI_DialogItemSingle const *)current_item;
        sStGUI_DialogItemCalcs *_child_item_calcs = item_calcs + 1;
        _items += _assing_items(_item_single->child_item, _child_item_calcs);
    }
    return _items;
}

static uint16_t _calc_min_sizes(sStGUI_DialogContext const *ctx, sStGUI_DialogItemBase const *current_item, sStGUI_DialogItemCalcs *item_calcs) {
    uint16_t _items = 1;
    if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_CONTAINER) {
        sStGUI_DialogItemContainer const *_item_container = (sStGUI_DialogItemContainer const *)current_item;
        sStGUI_DialogItemCalcs *_child_item_calcs = item_calcs + 1;

        uint16_t _w = 0, _h = 0;
        void (*_container_calc)(uint8_t ci) = NULL;

        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_CONTAINER_STACK) {
            sStGUI_DialogItemContainerStack const *_item_container_stack = (sStGUI_DialogItemContainerStack const *)_item_container;
            __extension__ void _container_calc_stack(uint8_t ci) {
                if (_item_container_stack->is_vertical) {
                    maxwith(_w, _child_item_calcs->min_width);
                    _h += _child_item_calcs->min_height;
                    if (ci > 0) _h += _item_container_stack->separator_length;
                } else {
                    _w += _child_item_calcs->min_width;
                    if (ci > 0) _w += _item_container_stack->separator_length;
                    maxwith(_h, _child_item_calcs->min_height);
                }
            }
            _container_calc = _container_calc_stack;
        }

        for (uint8_t _ci = 0; _ci < _item_container->child_items_count; _ci++) {
            uint16_t _citems = _calc_min_sizes(ctx, _item_container->child_items[_ci], _child_item_calcs);
            _container_calc(_ci);
            _child_item_calcs += _citems;
            _items += _citems;
        }
        item_calcs->min_width = _w;
        item_calcs->min_height = _h;
    } else if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_SINGLE) {
        sStGUI_DialogItemSingle const *_item_single = (sStGUI_DialogItemSingle const *)current_item;
        sStGUI_DialogItemCalcs *_child_item_calcs = item_calcs + 1;
        _items += _calc_min_sizes(ctx, _item_single->child_item, _child_item_calcs);

        item_calcs->min_width = _child_item_calcs->min_width;
        item_calcs->min_height = _child_item_calcs->min_height;
        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_SINGLE_SPACER) {
            sStGUI_DialogItemSpacer const *_item_spacer = (sStGUI_DialogItemSpacer const *)current_item;
            item_calcs->min_width += _item_spacer->space_left + _item_spacer->space_right;
            item_calcs->min_height += _item_spacer->space_top + _item_spacer->space_bottom;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_SINGLE_DIALOG) {
            sStGUI_Dialog const *_dialog = (sStGUI_Dialog const *)current_item;
            sStGUI_Primitive_FrameMeasure *_frame_measure = &item_calcs->helper.dialog.frame_measure;
            char const *_caption = _get_value(ctx, &_dialog->dialog_pointer_caption).text;
            StGUI_Primitive_FrameMeasure(ctx->font_caption, _caption, _frame_measure);
            item_calcs->min_width += (_frame_measure->left_right_bottom << 1) + (STGUI_DIALOG_SPACER_WIDTH << 1);
            item_calcs->min_height += (_frame_measure->top + _frame_measure->left_right_bottom + (STGUI_DIALOG_SPACER_WIDTH << 1));
        }
    } else if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_PRIMITIVE_TEXT) {
        sStGUI_DialogItemTextBase const *_base_item_text = (sStGUI_DialogItemTextBase const *)current_item;
        char const *_text = _get_value(ctx, &_base_item_text->dialog_pointer_text).text;

        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_LABEL) {
            item_calcs->min_width = StGUI_StringMeasure(ctx->font_items, _text);
            item_calcs->min_height = ctx->font_items->height;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_BUTTON) {
            sStGUI_Primitive_ButtonMeasure _button_measure;
            StGUI_Primitive_ButtonMeasure(ctx->font_items, &_button_measure, _text);
            item_calcs->min_width = _button_measure.width;
            item_calcs->min_height = _button_measure.height;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_CHECKBOX) {
            item_calcs->min_width = StGUI_Primitive_HighlightWithStringAndCheckboxMarkMeasureWidth(ctx->font_items, _text);
            item_calcs->min_height = ctx->highlight_item_height;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_CHOICES) {
            sStGUI_DialogItemChoices const *_item_choices = (sStGUI_DialogItemChoices const *)current_item;
            sStGUI_Primitive_ChoicesMeasure *_choices_measure = &item_calcs->helper.choices.measure;
            item_calcs->min_width = StGUI_Primitive_HighlightWithChoicesMeasureWidth(ctx->font_items, _text, _item_choices->text_choices, _item_choices->text_choices_count, _choices_measure);
            item_calcs->min_height = ctx->highlight_item_height;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_PROGRESS_STRING) {
            sStGUI_Primitive_ProgressWithStringMeasure _progress_with_string_measure;
            StGUI_Primitive_ProgressWithStringMeasure(ctx->font_items, _text, &_progress_with_string_measure);
            item_calcs->min_width = _progress_with_string_measure.width;
            item_calcs->min_height = _progress_with_string_measure.height;
        }

        maxwith(item_calcs->min_width, _base_item_text->min_width);
        if (_base_item_text->max_width) minwith(item_calcs->min_width, _base_item_text->max_width);
    } else {
        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_SEPARATOR) {
            sStGUI_DialogItemSeparator const *_item_separator = (sStGUI_DialogItemSeparator const *)current_item;
            item_calcs->min_width = _item_separator->width;
            item_calcs->min_height = _item_separator->height;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_BITMAP) {
            sStGUI_DialogItemBitmap const *_item_bitmap = (sStGUI_DialogItemBitmap const *)current_item;
            sStGUI_Bitmap const *_bitmaps = _get_value(ctx, &_item_bitmap->dialog_pointer_bitmap).bitmap;
            item_calcs->min_width = _bitmaps->width;
            item_calcs->min_height = _bitmaps->height;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_PROGRESS) {
            sStGUI_DialogItemProgress const *_item_progress = (sStGUI_DialogItemProgress const *)current_item;
            item_calcs->min_width = _item_progress->width;
            item_calcs->min_height = _item_progress->height;
        }
    }

    return _items;
}

static uint16_t _calc_placements(sStGUI_DialogContext const *ctx, sStGUI_DialogItemBase const *current_item, sStGUI_DialogItemCalcs *item_calcs) {
    uint16_t _items = 1;

    if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_CONTAINER) {
        sStGUI_DialogItemContainer const *_item_container = (sStGUI_DialogItemContainer const *)current_item;
        sStGUI_DialogItemCalcs *_child_item_calcs = item_calcs + 1;

        uint16_t _x = item_calcs->x, _y = item_calcs->y;
        void (*_container_calc)(uint8_t ci) = NULL;

        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_CONTAINER_STACK) {
            sStGUI_DialogItemContainerStack const *_item_container_stack = (sStGUI_DialogItemContainerStack const *)_item_container;
            uint16_t _left_space;
            if (_item_container_stack->is_vertical) {
                _left_space = item_calcs->height - item_calcs->min_height;
                if (_item_container_stack->container_alignment == DIALOG_CONTAINER_STACK_ALIGNMENT_CENTER) _y += (_left_space >> 1);
                else if (_item_container_stack->container_alignment == DIALOG_CONTAINER_STACK_ALIGNMENT_END) _y += _left_space;
            } else {
                _left_space = item_calcs->width - item_calcs->min_width;
                if (_item_container_stack->container_alignment == DIALOG_CONTAINER_STACK_ALIGNMENT_CENTER) _x += (_left_space >> 1);
                else if (_item_container_stack->container_alignment == DIALOG_CONTAINER_STACK_ALIGNMENT_END) _x += _left_space;
            }

            __extension__ void _container_calc_stack(uint8_t ci) {
                bool _do_spread = ((_item_container_stack->container_alignment == DIALOG_CONTAINER_STACK_ALIGNMENT_SPREAD_START && ci == 0) || (_item_container_stack->container_alignment == DIALOG_CONTAINER_STACK_ALIGNMENT_SPREAD_END && ci == _item_container->child_items_count - 1));

                if (_item_container_stack->is_vertical) {
                    _child_item_calcs->width = _item_container_stack->array_alignment == ALIGNMENT_EX_SPREAD ? item_calcs->width : _child_item_calcs->min_width;
                    _child_item_calcs->height = _do_spread ? _child_item_calcs->min_height + _left_space : _child_item_calcs->min_height;

                    uint16_t _item_left_space = item_calcs->width - _child_item_calcs->min_width;
                    _y += _child_item_calcs->height + _item_container_stack->separator_length;
                    if (_item_container_stack->array_alignment == ALIGNMENT_EX_CENTER) _child_item_calcs->x += (_item_left_space >> 1);
                    else if (_item_container_stack->array_alignment == ALIGNMENT_EX_END) _child_item_calcs->x += _item_left_space;
                } else {
                    _child_item_calcs->width = _do_spread ? _child_item_calcs->min_width + _left_space : _child_item_calcs->min_width;
                    _child_item_calcs->height = _item_container_stack->array_alignment == ALIGNMENT_EX_SPREAD ? item_calcs->height : _child_item_calcs->min_height;

                    uint16_t _item_left_space = item_calcs->height - _child_item_calcs->min_height;
                    _x += _child_item_calcs->width + _item_container_stack->separator_length;
                    if (_item_container_stack->array_alignment == ALIGNMENT_EX_CENTER) _child_item_calcs->y += (_item_left_space >> 1);
                    else if (_item_container_stack->array_alignment == ALIGNMENT_EX_END) _child_item_calcs->y += _item_left_space;
                }
            }
            _container_calc = _container_calc_stack;
        }

        for (uint8_t _ci = 0; _ci < _item_container->child_items_count; _ci++) {
            _child_item_calcs->x = _x;
            _child_item_calcs->y = _y;
            _container_calc(_ci);

            uint16_t _citems = _calc_placements(ctx, _item_container->child_items[_ci], _child_item_calcs);
            _child_item_calcs += _citems;
            _items += _citems;
        }
    } else if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_SINGLE) {
        sStGUI_DialogItemSingle const *_item_single = (sStGUI_DialogItemSingle const *)current_item;
        sStGUI_DialogItemCalcs *_child_item_calcs = item_calcs + 1;

        _child_item_calcs->x = item_calcs->x;
        _child_item_calcs->y = item_calcs->y;
        _child_item_calcs->width = item_calcs->width;
        _child_item_calcs->height = item_calcs->height;
        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_SINGLE_SPACER) {
            sStGUI_DialogItemSpacer const *_item_spacer = (sStGUI_DialogItemSpacer const *)current_item;
            _child_item_calcs->x += _item_spacer->space_left;
            _child_item_calcs->y += _item_spacer->space_top;
            _child_item_calcs->width -= (_item_spacer->space_left + _item_spacer->space_right);
            _child_item_calcs->height -= (_item_spacer->space_top + _item_spacer->space_bottom);
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_SINGLE_DIALOG) {
            sStGUI_Primitive_FrameMeasure *_frame_measure = &item_calcs->helper.dialog.frame_measure;
            _child_item_calcs->x += _frame_measure->left_right_bottom + STGUI_DIALOG_SPACER_WIDTH;
            _child_item_calcs->y += _frame_measure->top + STGUI_DIALOG_SPACER_WIDTH;
            _child_item_calcs->width -= ((_frame_measure->left_right_bottom << 1) + (STGUI_DIALOG_SPACER_WIDTH << 1));
            _child_item_calcs->height -= (_frame_measure->top + _frame_measure->left_right_bottom + (STGUI_DIALOG_SPACER_WIDTH << 1));
        }

        _items += _calc_placements(ctx, _item_single->child_item, _child_item_calcs);
    } else if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_PRIMITIVE_TEXT) {
        item_calcs->y += ((item_calcs->height - item_calcs->min_height) >> 1);
    } else {
        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_BITMAP) {
            sStGUI_DialogItemBitmap const *_item_bitmap = (sStGUI_DialogItemBitmap const *)current_item;
            uint16_t _left_space;
            _left_space = item_calcs->width - item_calcs->min_width;
            if (_item_bitmap->alignment_horizontal == ALIGNMENT_CENTER) item_calcs->x += (_left_space >> 1);
            else if (_item_bitmap->alignment_horizontal == ALIGNMENT_END) item_calcs->x += _left_space;
            _left_space = item_calcs->height - item_calcs->min_height;
            if (_item_bitmap->alignment_vertical == ALIGNMENT_CENTER) item_calcs->y += (_left_space >> 1);
            else if (_item_bitmap->alignment_vertical == ALIGNMENT_END) item_calcs->y += _left_space;
        }
    }

    return _items;
}

static uint16_t _draw_items(sStGUI_DialogContext const *ctx, sStGUI_DialogItemBase const *current_item, sStGUI_DialogItemCalcs *item_calcs, uint32_t ms10ticks) {
    uint16_t _items = 1;
    uint8_t _x = item_calcs->x;
    uint8_t _y = item_calcs->y;

    if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_CONTAINER) {
        sStGUI_DialogItemContainer const *_item_container = (sStGUI_DialogItemContainer const *)current_item;
        sStGUI_DialogItemCalcs *_child_item_calcs = item_calcs + 1;
        for (uint8_t _ci = 0; _ci < _item_container->child_items_count; _ci++) {
            uint16_t _citems = _draw_items(ctx, _item_container->child_items[_ci], _child_item_calcs, ms10ticks);
            _child_item_calcs += _citems;
            _items += _citems;
        }
    } else if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_SINGLE) {
        sStGUI_DialogItemSingle const *_item_single = (sStGUI_DialogItemSingle const *)current_item;
        sStGUI_DialogItemCalcs *_child_item_calcs = item_calcs + 1;

        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_SINGLE_DIALOG) {
            sStGUI_Dialog const *_dialog = (sStGUI_Dialog const *)current_item;
            sStGUI_Primitive_FrameMeasure *_frame_measure = &item_calcs->helper.dialog.frame_measure;
            StGUI_Primitive_FrameDraw(item_calcs->x, item_calcs->y, item_calcs->width - (_frame_measure->left_right_bottom << 1), item_calcs->height - (_frame_measure->top + _frame_measure->left_right_bottom), ms10ticks, ctx->font_caption, _get_value(ctx, &_dialog->dialog_pointer_caption).text);
        }

        _items += _draw_items(ctx, _item_single->child_item, _child_item_calcs, ms10ticks);
    } else if (current_item->dialog_item_type >= STGUI_DIALOG_TYPE_PRIMITIVE_TEXT) {
        sStGUI_DialogItemTextBase const *_base_item_text = (sStGUI_DialogItemTextBase const *)current_item;
        char const *_text = _get_value(ctx, &_base_item_text->dialog_pointer_text).text;

        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_LABEL) {
            sStGUI_DialogItemLabel const *_item_label = (sStGUI_DialogItemLabel const *)current_item;
            StGUI_StringDrawMaxWidth(ctx->font_items, item_calcs->x, item_calcs->y, item_calcs->width, 0, _item_label->alignment, DRAW_MODE_OR, ms10ticks, _text);
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_BUTTON) {
            sStGUI_DialogItemButton const *_item_button = (sStGUI_DialogItemButton const *)current_item;
            StGUI_Primitive_ButtonDraw(item_calcs->x, item_calcs->y, item_calcs->width, _is_selected(ctx, current_item), ms10ticks, ctx->font_items, _text);
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_CHECKBOX) {
            sStGUI_DialogItemCheckbox const *_item_checkbox = (sStGUI_DialogItemCheckbox const *)current_item;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_CHOICES) {
            sStGUI_DialogItemChoices const *_item_choices = (sStGUI_DialogItemChoices const *)current_item;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_PROGRESS_STRING) {
            sStGUI_DialogItemProgressString const *_item_progress_string = (sStGUI_DialogItemProgressString const *)current_item;
            StGUI_Primitive_ProgressWithStringDraw(item_calcs->x, item_calcs->y, item_calcs->width, _get_value(ctx, &_item_progress_string->dialog_pointer_progress).progress, ms10ticks, ctx->font_items, _text);
        }
    } else {
        if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_SEPARATOR) {
            sStGUI_DialogItemSeparator const *_item_separator = (sStGUI_DialogItemSeparator const *)current_item;
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_BITMAP) {
            sStGUI_DialogItemBitmap const *_item_bitmap = (sStGUI_DialogItemBitmap const *)current_item;
            sStGUI_Bitmap const *_bitmaps = _get_value(ctx, &_item_bitmap->dialog_pointer_bitmap).bitmap;
            StGUI_BitmapDraw(_bitmaps, item_calcs->x, item_calcs->y, DRAW_MODE_OR, ms10ticks);
        } else if (current_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_PROGRESS) {
            sStGUI_DialogItemProgress const *_item_progress = (sStGUI_DialogItemProgress const *)current_item;
            StGUI_Primitive_ProgressDraw(item_calcs->x, item_calcs->y, item_calcs->width, item_calcs->height, _get_value(ctx, &_item_progress->dialog_pointer_progress).progress, DRAW_MODE_OR, ms10ticks);
        }
    }

    return _items;
}

uint16_t StGUI_ShowDialog(sStGUI_Controller *controller, sStGUI_DialogController const *dialog_controller, sStGUI_DialogItemBase const *root, sStGUI_Font const *font_caption, sStGUI_Font const *font_items, eStGUI_DialogFlags dialog_flags) {
    uint16_t _total_items = _calc_items(root);
    sStGUI_DialogItemCalcs _item_calcs[_total_items];
    _assing_items(root, _item_calcs);
    sStGUI_DialogContext _ctx = {
        .dialog_controller = dialog_controller,
        .highlight_indents = StGUI_Primitive_HighlightIndents(),
        .highlight_item_height = font_items->height + (_ctx.highlight_indents->top_bottom << 1),
        .item_calcs = _item_calcs,
        .total_items = _total_items,
        .selected_item = UINT16_MAX,
        .font_caption = font_caption,
        .font_items = font_items
    };
    _select_next(&_ctx);

    uint32_t _ms10_start = controller->get_ms10ticks();
    d_stgui_redraw _redraw_before_prev = controller->redraw_before;

    __extension__ void _redraw_before() {
        if (_redraw_before_prev != nullptr) _redraw_before_prev();
        uint32_t _ms10ticks = controller->get_ms10ticks() - _ms10_start;
        _draw_items(&_ctx, root, _item_calcs, _ms10ticks);
    }

    uint16_t _action_code = 0;
    bool _is_recalc = true;

    __extension__ bool _idle(bool is_recalc) {
        _is_recalc |= is_recalc;

        if (StGUI_RedrawFlag(controller)) {
            if (_is_recalc) {
                _is_recalc = false;
                _calc_min_sizes(&_ctx, root, _item_calcs);
                if (dialog_flags & DIALOG_FLAGS_FULL_SCREEN) {
                    _item_calcs->x = _item_calcs->y = 0;
                    _item_calcs->width = (_item_calcs->min_width > GLCD_WIDTH) ? _item_calcs->min_width : GLCD_WIDTH;
                    _item_calcs->height = (_item_calcs->min_height > GLCD_HEIGHT) ? _item_calcs->min_height : GLCD_HEIGHT;
                } else {
                    _item_calcs->x = (_item_calcs->min_width > GLCD_WIDTH) ? 0 : (GLCD_WIDTH - _item_calcs->min_width) >> 1;
                    _item_calcs->y = (_item_calcs->min_height > GLCD_HEIGHT) ? 0 : (GLCD_HEIGHT - _item_calcs->min_height) >> 1;
                    _item_calcs->width = _item_calcs->min_width;
                    _item_calcs->height = _item_calcs->min_height;
                }
                _calc_placements(&_ctx, root, _item_calcs);
            }
            if (dialog_flags & DIALOG_FLAGS_CLEAR) StGUI_Primitive_Clear(_item_calcs->x, _item_calcs->y, _item_calcs->width, _item_calcs->height);
            _redraw_before();
            if (controller->redraw_after != nullptr) controller->redraw_after();
        }

        eStGUI_ControlActions _control_actions = controller->get_control_actions();
        if (_control_actions == MENU_CONTROL_ESCAPE) {
            _action_code = 0;
            if (!dialog_controller->process_action || dialog_controller->process_action(MENU_CONTROL_ESCAPE, 0, &_is_recalc)) return true;
        } else if (_control_actions & MENU_CONTROL_ENTER) {
            sStGUI_DialogItemBase const *_selected_item = _get_selected(&_ctx);
            if (_selected_item != NULL) {
                if (_selected_item->dialog_item_type == DIALOG_ITEM_TYPE_PRIMITIVE_BUTTON) {
                    sStGUI_DialogItemButton const *_item_button = (sStGUI_DialogItemButton const *)_selected_item;
                    _action_code = _item_button->action_code;
                    return true;
                }

                if (!dialog_controller->process_action || dialog_controller->process_action(MENU_CONTROL_ENTER, _action_code, &_is_recalc)) return true;
            }
        } else if (_control_actions & MENU_CONTROL_FORWARD) {
            _select_next(&_ctx);
        } else if (_control_actions & MENU_CONTROL_BACKWARD) {
            _select_prev(&_ctx);
        }

        return false;
    }

    if (dialog_controller->procedure && dialog_controller->procedure(_idle, &_action_code)) return _action_code;

    while (!_idle(false));

    controller->redraw_before = _redraw_before_prev;
    return _action_code;
}
