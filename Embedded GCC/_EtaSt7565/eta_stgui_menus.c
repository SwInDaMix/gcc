/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_menus.c
 ***********************************************************************/

#include "eta_stgui_clip_draw.h"
#include "eta_stgui_primitives.h"
#include "eta_stgui_menus.h"

#define STGUI_MENU_ITEMS_INDENT 1

typedef union {
    struct {
        sStGUI_Primitive_ChoicesMeasure measure;
    } choices;
} sStGUI_MenuHelper;

uint16_t StGUI_OpenMenu(sStGUI_Controller *controller, sStGUI_Menu const *menu, sStGUI_Font const *font_caption, sStGUI_Font const *font_items, uint8_t max_visible_items) {
    // Calculate menu size
    // ~~~~~~~~~~~~~~~~~~~

    sStGUI_Primitive_FrameMeasure _frame_measure;
    StGUI_Primitive_FrameMeasure(font_caption, menu->base.text, &_frame_measure);
    uint16_t _highlight_height = StGUI_Primitive_HighlightWithStringMeasureHeight(font_items);

    uint8_t _max_possible_items = (GLCD_HEIGHT - _frame_measure.top - _frame_measure.left_right_bottom - (STGUI_MENU_ITEMS_INDENT << 1)) / (_highlight_height);
    if (max_visible_items > _max_possible_items) max_visible_items = _max_possible_items;
    if (max_visible_items > menu->sub_items_count) max_visible_items = menu->sub_items_count;

    uint16_t _items_max_width = 0;
    sStGUI_MenuHelper _menu_helpers[menu->sub_items_count];

    for (uint8_t _mi = 0; _mi < menu->sub_items_count; _mi++) {
        sStGUI_MenuItemBase const *_base_item = menu->sub_items[_mi];
        uint32_t _iw = 0;
        if (_base_item->menu_item_type == MENU_ITEM_TYPE_MENU) {
            _iw = StGUI_Primitive_HighlightWithStringAndArrowMarkMeasureWidth(font_items, _base_item->text);
        } else if (_base_item->menu_item_type == MENU_ITEM_TYPE_ACTION) {
            _iw = StGUI_Primitive_HighlightWithStringMeasureWidth(font_items, _base_item->text);
        } else if (_base_item->menu_item_type == MENU_ITEM_TYPE_CHECKBOX) {
            _iw = StGUI_Primitive_HighlightWithStringAndCheckboxMarkMeasureWidth(font_items, _base_item->text);
        } else if (_base_item->menu_item_type == MENU_ITEM_TYPE_CHOICES) {
            sStGUI_MenuItemChoices const *_menu_choices = (sStGUI_MenuItemChoices const *)_base_item;
            sStGUI_Primitive_ChoicesMeasure *_choices_measure = &_menu_helpers[_mi].choices.measure;
            _iw = StGUI_Primitive_HighlightWithChoicesMeasureWidth(font_items, _base_item->text, _menu_choices->text_choices, _menu_choices->text_choices_count, _choices_measure);
        }
        maxwith(_items_max_width, _iw);
    }

    uint16_t _frame_width = _items_max_width + (STGUI_MENU_ITEMS_INDENT << 1);
    uint16_t _frame_height = (max_visible_items * _highlight_height) + (STGUI_MENU_ITEMS_INDENT << 1);

    minwith(_frame_width, GLCD_WIDTH - (_frame_measure.left_right_bottom << 1));
    uint8_t _x = (GLCD_WIDTH - (_frame_width + (_frame_measure.left_right_bottom << 1))) >> 1;
    uint8_t _y = (GLCD_HEIGHT - (_frame_height + _frame_measure.top - _frame_measure.left_right_bottom)) >> 1;

    _items_max_width = _frame_width - (STGUI_MENU_ITEMS_INDENT << 1);

    // Draw menu cycle
    // ~~~~~~~~~~~~~~~

    uint32_t _ms10_start = controller->get_ms10ticks();
    uint8_t _menu_item_start = 0;
    uint8_t _menu_item_selected = 0;
    uint8_t _menu_choices_count = 0;
    uint8_t _menu_choices_selected_item = 0;

    d_stgui_redraw _redraw_before_prev = controller->redraw_before;

    __extension__ void _redraw_before() {
        if (_redraw_before_prev != nullptr) _redraw_before_prev();

        uint32_t _ms10ticks = controller->get_ms10ticks() - _ms10_start;

        StGUI_Primitive_FrameDraw(_x, _y, _frame_width, _frame_height, _ms10ticks, font_caption, menu->base.text);

        uint8_t _items_x = _x + _frame_measure.left_right_bottom + STGUI_MENU_ITEMS_INDENT;
        uint8_t _items_y = _y + _frame_measure.top + STGUI_MENU_ITEMS_INDENT;
        for (uint8_t _mi = _menu_item_start; _mi < (_menu_item_start + max_visible_items); _mi++) {
            sStGUI_MenuItemBase const *_base_item = menu->sub_items[_mi];
            if (_base_item->menu_item_type == MENU_ITEM_TYPE_MENU) {
                StGUI_Primitive_HighlightWithStringAndArrowMarkDraw(_items_x, _items_y, _items_max_width, _mi == _menu_item_selected, _ms10ticks, font_items, _base_item->text);
            } else if (_base_item->menu_item_type == MENU_ITEM_TYPE_ACTION) {
                StGUI_Primitive_HighlightWithStringDraw(_items_x, _items_y, _items_max_width, ALIGNMENT_START, _mi == _menu_item_selected, _ms10ticks, font_items, _base_item->text);
            } else if (_base_item->menu_item_type == MENU_ITEM_TYPE_CHECKBOX) {
                sStGUI_MenuItemCheckbox const *_menu_checkbox = (sStGUI_MenuItemCheckbox const *)_base_item;
                StGUI_Primitive_HighlightWithStringAndCheckboxMarkDraw(_items_x, _items_y, _items_max_width, _mi == _menu_item_selected, *_menu_checkbox->check_value, _ms10ticks, font_items, _base_item->text);
            } else if (_base_item->menu_item_type == MENU_ITEM_TYPE_CHOICES) {
                sStGUI_MenuItemChoices const *_menu_choices = (sStGUI_MenuItemChoices const *)_base_item;
                sStGUI_Primitive_ChoicesHighlight _choices_highlight = _mi == _menu_item_selected ? (_menu_choices_count > 0 ? PRIMITIVE_CHOICES_HIGHLIGHT_CHOICE_ONLY : PRIMITIVE_CHOICES_HIGHLIGHT_FULL) : PRIMITIVE_CHOICES_HIGHLIGHT_NONE;
                uint8_t _choices_selected = (_menu_choices_count > 0 && _mi == _menu_item_selected) ? _menu_choices_selected_item : *_menu_choices->choice_value;
                StGUI_Primitive_HighlightWithChoicesDraw(_items_x, _items_y, _items_max_width, _choices_highlight, _choices_selected, _ms10ticks, font_items, _base_item->text, _menu_choices->text_choices, &_menu_helpers[_mi].choices.measure);
            }
            _items_y += _highlight_height;
        }
    }

    controller->redraw_before = _redraw_before;
    uint16_t _action_code = 0;

    while (true) {
        if (StGUI_RedrawFlag(controller)) {
            _redraw_before();
            if (controller->redraw_after != nullptr) controller->redraw_after();
        }

        eStGUI_ControlActions _control_actions = controller->get_control_actions();
        if (_control_actions & MENU_CONTROL_ESCAPE) {
            if (_menu_choices_count > 0) _menu_choices_count = 0;
            else {
                _action_code = 0;
                break;
            }
        } else if (_control_actions & MENU_CONTROL_ENTER) {
            sStGUI_MenuItemBase const *_selected_menu_item = menu->sub_items[_menu_item_selected];
            if (_selected_menu_item->menu_item_type == MENU_ITEM_TYPE_MENU) {
                sStGUI_Menu const *_sub_menu = (sStGUI_Menu const *)_selected_menu_item;
                _action_code = StGUI_OpenMenu(controller, _sub_menu, font_caption, font_items, 3);
                if (_action_code != 0) break;
            } else if (_selected_menu_item->menu_item_type == MENU_ITEM_TYPE_ACTION) {
                sStGUI_MenuItemAction const *_menu_item_action = (sStGUI_MenuItemAction const *)_selected_menu_item;
                _action_code = _menu_item_action->action_code;
                break;
            } else if (_selected_menu_item->menu_item_type == MENU_ITEM_TYPE_CHECKBOX) {
                sStGUI_MenuItemCheckbox const *_menu_item_checkbox = (sStGUI_MenuItemCheckbox const *)_selected_menu_item;
                *_menu_item_checkbox->check_value = !*_menu_item_checkbox->check_value;
            } else if (_selected_menu_item->menu_item_type == MENU_ITEM_TYPE_CHOICES) {
                sStGUI_MenuItemChoices const *_menu_item_choices = (sStGUI_MenuItemChoices const *)_selected_menu_item;
                if (_menu_choices_count > 0) {
                    *_menu_item_choices->choice_value = _menu_choices_selected_item;
                    _menu_choices_count = 0;
                } else {
                    _menu_choices_selected_item = *_menu_item_choices->choice_value;
                    _menu_choices_count = _menu_item_choices->text_choices_count;
                }
            }
        } else if (_control_actions & MENU_CONTROL_FORWARD) {
            if (_menu_choices_count > 0) {
                if (_menu_choices_selected_item < (_menu_choices_count - 1)) _menu_choices_selected_item++;
            } else if (_menu_item_selected < (menu->sub_items_count - 1)) {
                _menu_item_selected++;
                if ((_menu_item_selected - _menu_item_start) >= max_visible_items) _menu_item_start++;
            }
        } else if (_control_actions & MENU_CONTROL_BACKWARD) {
            if (_menu_choices_count > 0) {
                if (_menu_choices_selected_item > 0) _menu_choices_selected_item--;
            } else if (_menu_item_selected > 0) {
                _menu_item_selected--;
                if (_menu_item_start > _menu_item_selected) _menu_item_start = _menu_item_selected;
            }
        }
    }

    controller->redraw_before = _redraw_before_prev;
    return _action_code;
}
