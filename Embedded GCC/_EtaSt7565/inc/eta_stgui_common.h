/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_common.h
 ***********************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>

// Simple alignment
typedef enum {
    ALIGNMENT_START,
    ALIGNMENT_CENTER,
    ALIGNMENT_END
} eStGUI_Alignment;

// Extended alignment
typedef enum {
    ALIGNMENT_EX_START,
    ALIGNMENT_EX_CENTER,
    ALIGNMENT_EX_END,
    ALIGNMENT_EX_SPREAD,
} eStGUI_AlignmentEx;

// Dialog controller
typedef enum {
    MENU_CONTROL_ESCAPE = (1 << 0),
    MENU_CONTROL_ENTER = (1 << 1),
    MENU_CONTROL_FORWARD = (1 << 2),
    MENU_CONTROL_BACKWARD = (1 << 3)
} eStGUI_ControlActions;
typedef void (*d_stgui_redraw)();
typedef eStGUI_ControlActions (*d_stgui_get_control_actions)();
typedef uint32_t (*d_stgui_get_ms10ticks)();
typedef struct {
    d_stgui_redraw redraw_before;                       // yields execution just before redraw - this allows to render background under the dialog
    d_stgui_redraw redraw_after;                        // yields execution just after redraw - this allows to render some item over the dialog
    d_stgui_get_control_actions get_control_actions;    // retrieves control action flags
    d_stgui_get_ms10ticks get_ms10ticks;                // retrieves 10 ms ticks counter
} sStGUI_Controller;

bool StGUI_RedrawFlag(sStGUI_Controller const *controller);
