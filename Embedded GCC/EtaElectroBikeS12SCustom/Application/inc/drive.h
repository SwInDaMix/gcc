#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "settings.h"
#include "controls.h"
#include "sensors.h"

#define DRV_THUMB_TOGGLE_THRESHOLD 6
#define DRV_MIN_DUTY_CYCLE_TO_PREDICT 128
#define DRV_MIN_ERPS_TO_PREDICT BLDC_INTERPOLATION_360_ERPS
#define DRV_MIN_MOVING_ERPS 15
#define DRV_MIN_ACCELERATING_CYCLES 8

#define DRV_CYCLE_PERIOD_MS 25

typedef enum {
    DRV_STATE_IDLE,
    DRV_STATE_EBRAKING,
    DRV_STATE_ACCELERATING
} eDrv_State;

void drv_init(sSettings const *settings, sSensors const *sensors);
void drv_cycle(sSettings const *settings, sControls const *controls, sSensors const *sensors);

eDrv_State drv_get_state();
bool drv_is_cruise_control();
