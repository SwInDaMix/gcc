#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "display.h"

typedef enum {
    SettingsFlag_None,
    SettingsFlag_AlwaysBacklight = (1 << 0)
} eSettingsFlags;

typedef struct {
    eSettingsFlags flags;
    eDispMainMeasureUnit measure_unit;
    uint8_t backlight_brightness;
    sNetworkMotorSettings motor_settings;
} sSettings;

void settings_init();

void settings_flush();
sSettings *settings_get_current();
