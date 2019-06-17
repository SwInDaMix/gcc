#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

typedef struct {
    bool is_headlight_on;
    bool is_headlight_feedback;
    bool is_braking;
    uint8_t thumb_throttle;
    uint8_t thumb_brake;
    uint8_t gear_ratio;
    bool is_walking;
    bool is_cruise_control;
} sControls;

void controls_init();

sControls const *controls_get_current();
