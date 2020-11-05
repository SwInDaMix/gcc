/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_smart_backlight.h
 ***********************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t backlight_wakeup;
    uint8_t backlight_background;
    uint8_t backlight_standby;
    uint8_t backlight;
    uint32_t timeout;
    uint32_t timeout_fadeout;
    bool is_background;
} sSmartBacklightInstance;

typedef void (*d_smart_backlight_block)();
typedef void (*d_smart_backlight_irq_safe)(d_smart_backlight_block block);
typedef void (*d_smart_backlight_set_brightness)(sSmartBacklightInstance *smart_backlight_instance);
typedef struct {
    d_smart_backlight_irq_safe irq_safe;
    d_smart_backlight_set_brightness set_brightness;
} sSmartBacklightInterface;

void SmartBacklight_Init(sSmartBacklightInterface const *smart_backlight_interface);
void SmartBacklight_Periodic(sSmartBacklightInstance *smart_backlight_instance);
void SmartBacklight_SetParams(sSmartBacklightInstance *smart_backlight_instance, uint8_t backlight_wakeup, uint8_t backlight_background, uint8_t backlight_standby, uint8_t timeout);
void SmartBacklight_SetBackgroundMode(sSmartBacklightInstance *smart_backlight_instance, bool is_background);
void SmartBacklight_WakeUp(sSmartBacklightInstance *smart_backlight_instance);
