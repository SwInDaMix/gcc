/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_dialogs.h
 ***********************************************************************/

#pragma once

#include "eta_stgui_fonts.h"
#include "eta_stgui_bitmaps.h"

#define STGUI_DIALOG_POINTER_MAGIC (0xC0DE0000)
#define STGUI_DIALOG_POINTER_MAGIC_MASK (0xFFFF0000)
#define STGUI_DIALOG_POINTER_INDEX_MASK (~(STGUI_DIALOG_POINTER_MAGIC_MASK))
#define STGUI_DIALOG_MAKE_POINTER(index) ((STGUI_DIALOG_POINTER_INDEX_MASK & (index)) | (STGUI_DIALOG_POINTER_MAGIC))

#define STGUI_DIALOG_TYPE_PRIMITIVE 0
#define STGUI_DIALOG_TYPE_PRIMITIVE_TEXT 64
#define STGUI_DIALOG_TYPE_SINGLE 128
#define STGUI_DIALOG_TYPE_CONTAINER 192

typedef union {
    char const *text;
    sStGUI_Bitmap const *bitmap;
    bool checkbox;
    uint8_t choice;
    uint16_t progress;
} sStGUI_DialogValue;

typedef bool (*d_stgui_dialog_procedure_idle)(bool is_recalc);
typedef bool (*d_stgui_dialog_procedure)(d_stgui_dialog_procedure_idle procedure_idle, uint16_t *action_code);
typedef sStGUI_DialogValue (*d_stgui_dialog_get_value)(uint16_t action_code);
typedef bool (*d_stgui_dialog_process_action)(eStGUI_ControlActions control_action, uint16_t code, bool *is_recalc);

typedef struct {
    d_stgui_dialog_procedure procedure;
    d_stgui_dialog_get_value get_value;
    d_stgui_dialog_process_action process_action;
} sStGUI_DialogController;

// Base dialog controls types
typedef enum {
    DIALOG_ITEM_TYPE_PRIMITIVE_SEPARATOR = STGUI_DIALOG_TYPE_PRIMITIVE,
    DIALOG_ITEM_TYPE_PRIMITIVE_BITMAP,
    DIALOG_ITEM_TYPE_PRIMITIVE_PROGRESS,
    DIALOG_ITEM_TYPE_PRIMITIVE_LABEL = STGUI_DIALOG_TYPE_PRIMITIVE_TEXT,
    DIALOG_ITEM_TYPE_PRIMITIVE_BUTTON,
    DIALOG_ITEM_TYPE_PRIMITIVE_CHECKBOX,
    DIALOG_ITEM_TYPE_PRIMITIVE_CHOICES,
    DIALOG_ITEM_TYPE_PRIMITIVE_PROGRESS_STRING,
    DIALOG_ITEM_TYPE_SINGLE_SPACER = STGUI_DIALOG_TYPE_SINGLE,
    DIALOG_ITEM_TYPE_SINGLE_DIALOG,
    DIALOG_ITEM_TYPE_CONTAINER_STACK = STGUI_DIALOG_TYPE_CONTAINER
} eStGUI_DialogItemType;

typedef union {
    sStGUI_DialogValue value;
    uint32_t index;
} sStGUI_DialogPointer;

/// ******************************************************************************************************
/// Base controls
/// ******************************************************************************************************

// Base dialog control
typedef struct {
    eStGUI_DialogItemType dialog_item_type;
} sStGUI_DialogItemBase;

// Primitive text label
typedef struct {
    sStGUI_DialogItemBase base;
    sStGUI_DialogPointer dialog_pointer_text;
    uint8_t min_width;
    uint8_t max_width;
} sStGUI_DialogItemTextBase;

// Single control
typedef struct {
    sStGUI_DialogItemBase base;
    sStGUI_DialogItemBase const *child_item;
} sStGUI_DialogItemSingle;

// Container control
typedef struct {
    sStGUI_DialogItemBase base;
    sStGUI_DialogItemBase const *const *child_items;
    uint8_t child_items_count;
} sStGUI_DialogItemContainer;

/// ******************************************************************************************************
/// Primitive controls
/// ******************************************************************************************************

// Primitive separator
typedef struct {
    sStGUI_DialogItemBase base;
    uint8_t width;
    uint8_t height;
} sStGUI_DialogItemSeparator;

// Primitive bitmap
typedef struct {
    sStGUI_DialogItemBase base;
    sStGUI_DialogPointer dialog_pointer_bitmap;
    eStGUI_Alignment alignment_horizontal;
    eStGUI_Alignment alignment_vertical;
} sStGUI_DialogItemBitmap;

// Arbitrary progress bar
typedef struct {
    sStGUI_DialogItemBase base;
    sStGUI_DialogPointer dialog_pointer_progress;
    uint8_t width;
    uint8_t height;
} sStGUI_DialogItemProgress;

/// ******************************************************************************************************
/// Primitive text controls
/// ******************************************************************************************************

// Primitive text label
typedef struct {
    sStGUI_DialogItemTextBase base;
    eStGUI_Alignment alignment;
} sStGUI_DialogItemLabel;

// Primitive button
typedef struct {
    sStGUI_DialogItemTextBase base;
    uint16_t action_code;
} sStGUI_DialogItemButton;

// Primitive checkbox
typedef struct {
    sStGUI_DialogItemTextBase base;
    bool *check_value;
} sStGUI_DialogItemCheckbox;

// Primitive choices selector
typedef struct {
    sStGUI_DialogItemTextBase base;
    char const *const *text_choices;
    uint8_t text_choices_count;
    uint8_t *choice_value;
} sStGUI_DialogItemChoices;

// Progress bar with string
typedef struct {
    sStGUI_DialogItemTextBase base;
    sStGUI_DialogPointer dialog_pointer_progress;
} sStGUI_DialogItemProgressString;

/// ******************************************************************************************************
/// Single controls
/// ******************************************************************************************************

// Single spacer
typedef struct {
    sStGUI_DialogItemSingle base;
    uint8_t space_left;
    uint8_t space_right;
    uint8_t space_top;
    uint8_t space_bottom;
} sStGUI_DialogItemSpacer;

// Single dialog frame
typedef struct {
    sStGUI_DialogItemSingle base;
    sStGUI_DialogPointer dialog_pointer_caption;
} sStGUI_Dialog;

/// ******************************************************************************************************
/// Container controls
/// ******************************************************************************************************

// Stack container alignment
typedef enum {
    DIALOG_CONTAINER_STACK_ALIGNMENT_SPREAD_START,
    DIALOG_CONTAINER_STACK_ALIGNMENT_START,
    DIALOG_CONTAINER_STACK_ALIGNMENT_CENTER,
    DIALOG_CONTAINER_STACK_ALIGNMENT_END,
    DIALOG_CONTAINER_STACK_ALIGNMENT_SPREAD_END
} eStGUI_DialogContainerAlignment;

// Container control
typedef struct {
    sStGUI_DialogItemContainer base;
    bool is_vertical;
    eStGUI_DialogContainerAlignment container_alignment;
    eStGUI_AlignmentEx array_alignment;
    uint8_t separator_length;
} sStGUI_DialogItemContainerStack;

/// ******************************************************************************************************
/// ******************************************************************************************************
/// ******************************************************************************************************

typedef enum {
    DIALOG_FLAGS_FULL_SCREEN = (1 << 0),
    DIALOG_FLAGS_CLEAR = (1 << 1),
} eStGUI_DialogFlags;

uint16_t StGUI_ShowDialog(sStGUI_Controller *controller, sStGUI_DialogController const *dialog_controller, sStGUI_DialogItemBase const *root, sStGUI_Font const *font_caption, sStGUI_Font const *font_items, eStGUI_DialogFlags dialog_flags);
