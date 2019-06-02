#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "display.h"

typedef struct {
    eDispMainMeasureUnit measure_unit;
    uint32_t odometer;              // 0.01 Km/Mil (9999.99 max)
    uint16_t ttm;
    uint16_t wheel_circumference;   // wheel circumference in millimeters
    uint8_t motor_pole_pairs;       // number of motor poles
    uint16_t max_kmh;               // in 0.1 Kmh, max 999
    uint16_t max_mph;               // in 0.1 mph, max 999
    uint8_t backlight_brightness;
} sSettings;

void settings_init();

void settings_flush();
sSettings *settings_get_current();