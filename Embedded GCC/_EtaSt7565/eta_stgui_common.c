/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_stgui_common.c
 ***********************************************************************/

#include "eta_stgui_common.h"

#define STGUI_REDRAW_PERIOD_DIVIDER 3

bool StGUI_RedrawFlag(sStGUI_Controller const *controller) {
    static uint32_t g_redraw_ticks = 0;

    bool _do_redraw = false;
    uint32_t _ticks = controller->get_ms10ticks() >> STGUI_REDRAW_PERIOD_DIVIDER;
    if (g_redraw_ticks != _ticks) {
        g_redraw_ticks = _ticks;
        _do_redraw = true;
    }
    return _do_redraw;
}

