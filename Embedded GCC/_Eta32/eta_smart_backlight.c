/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_smart_backlight.c
 ***********************************************************************/

#include "config.h"
#include "eta_smart_backlight.h"

#define SMARTBACKLIGHT_FADEOUT_DELAY (PERIODIC_TICKS_PER_SECOND * 2 / UINT8_MAX)

static sSmartBacklightInterface const *g_smart_backlight_interface;

void SmartBacklight_Init(sSmartBacklightInterface const *smart_backlight_interface) { g_smart_backlight_interface = smart_backlight_interface; }

void SmartBacklight_Periodic(sSmartBacklightInstance *smart_backlight_instance) {
    if (!g_smart_backlight_interface) return;

    if (smart_backlight_instance->timeout_fadeout) smart_backlight_instance->timeout_fadeout--;
    else {
        int32_t _tdiff = smart_backlight_instance->backlight - (smart_backlight_instance->is_background ? smart_backlight_instance->backlight_background : smart_backlight_instance->backlight_standby);
        if (_tdiff != 0) {
            if (_tdiff > 0) smart_backlight_instance->backlight--; else smart_backlight_instance->backlight++;
            smart_backlight_instance->timeout_fadeout = SMARTBACKLIGHT_FADEOUT_DELAY;
            g_smart_backlight_interface->set_brightness(smart_backlight_instance);
        }
    }
}

void SmartBacklight_SetParams(sSmartBacklightInstance *smart_backlight_instance, uint8_t backlight_wakeup, uint8_t backlight_background, uint8_t backlight_standby, uint8_t timeout_sec) {
    __extension__ void _block() {
        smart_backlight_instance->backlight_wakeup = backlight_wakeup;
        smart_backlight_instance->backlight_background = backlight_background * backlight_wakeup / UINT8_MAX;
        smart_backlight_instance->backlight_standby = backlight_standby * backlight_background * backlight_wakeup / (UINT8_MAX * UINT8_MAX);
        smart_backlight_instance->timeout = timeout_sec * PERIODIC_TICKS_PER_SECOND;
    }

    if (g_smart_backlight_interface) {
        g_smart_backlight_interface->irq_safe(_block);
        SmartBacklight_WakeUp(smart_backlight_instance);
    } else _block();
}

void SmartBacklight_SetBackgroundMode(sSmartBacklightInstance *smart_backlight_instance, bool is_background) {
    smart_backlight_instance->is_background = is_background;
}

void SmartBacklight_WakeUp(sSmartBacklightInstance *smart_backlight_instance) {
    smart_backlight_instance->timeout_fadeout = smart_backlight_instance->timeout;
    smart_backlight_instance->backlight = smart_backlight_instance->backlight_wakeup;

    if (g_smart_backlight_interface) g_smart_backlight_interface->set_brightness(smart_backlight_instance);
}
