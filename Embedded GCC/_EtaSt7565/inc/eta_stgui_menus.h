/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_menus.h
 ***********************************************************************/

#pragma once

#include "eta_stgui_dialogs.h"
#include "eta_stgui_fonts.h"

// Base menu item types
typedef enum {
    MENU_ITEM_TYPE_MENU,
    MENU_ITEM_TYPE_ACTION,
    MENU_ITEM_TYPE_CHECKBOX,
    MENU_ITEM_TYPE_CHOICES
} eStGUI_MenuItemType;

// Base menu item structure
typedef struct {
    char const *text;
    eStGUI_MenuItemType menu_item_type;
} sStGUI_MenuItemBase;

// Simple action menu item
typedef struct {
    sStGUI_MenuItemBase base;
    uint16_t action_code;
} sStGUI_MenuItemAction;

// Checkbox menu item
typedef struct {
    sStGUI_MenuItemBase base;
    bool *check_value;
} sStGUI_MenuItemCheckbox;

// Action menu item with choices
typedef struct {
    sStGUI_MenuItemBase base;
    char const *const *text_choices;
    uint8_t text_choices_count;
    uint8_t *choice_value;
} sStGUI_MenuItemChoices;

// Menu container
typedef struct {
    sStGUI_MenuItemBase base;
    sStGUI_MenuItemBase const *const *sub_items;
    uint8_t sub_items_count;
} sStGUI_Menu;

uint16_t StGUI_OpenMenu(sStGUI_Controller *controller, sStGUI_Menu const *menu, sStGUI_Font const *font_caption, sStGUI_Font const *font_items, uint8_t max_visible_items);
