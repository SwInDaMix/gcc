#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

typedef struct {
    float wheel_circumference;
    uint8_t motor_polus_pairs;
    uint16_t erps_limit;
    uint8_t max_current_scale;
    int8_t min_voltage_correct;
    bool throttle_when_moving;
    bool scale_max_current_by_gear;
} sSettings;

void settings_init();

sSettings const *settings_get_current();