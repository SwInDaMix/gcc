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

    uint32_t odometer;                  // 0.01 Km/Mil
    uint16_t total_ride_time;

    sNetworkMotorSettings motor_settings;
} sSettings;

void settings_init();

void settings_flush();
sSettings *settings_get_current();
