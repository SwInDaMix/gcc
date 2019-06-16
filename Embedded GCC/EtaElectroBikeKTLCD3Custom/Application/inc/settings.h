#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "display.h"

typedef struct {
    eDispSettingsFlags flags;
    eDispMainMeasureUnit measure_unit;
    uint8_t backlight_brightness;
    uint16_t voltage_coefficient;
    sNetworkMotorSettings motor_settings;
} sSettings;

void settings_init();

void settings_flush();
sSettings *settings_get_current();
