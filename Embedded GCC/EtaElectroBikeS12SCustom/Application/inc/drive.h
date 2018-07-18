#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "settings.h"
#include "controls.h"
#include "sensors.h"

#define DRV_THUMB_TOGGLE_THRESHOLD 6
#define DRV_ACCELERATION_THRESHOLD 64
#define DRV_THUMB_MAX_ACCELERATION_RAMP 5
#define DRV_MIN_DUTY_CYCLE_TO_PREDICT 128
#define DRV_MIN_ERPS_TO_PREDICT BLDC_INTERPOLATION_360_ERPS
#define DRV_WALK_SPEED_KMH 5
#define DRV_MIN_MOVING_ERPS 15
#define DRV_MIN_ACCELERATING_CYCLES 8

#define DRV_CYCLE_PERIOD_MS 25

void drv_init(sSettings const *settings, sSensors const *sensors);
void drv_cycle(sSettings const *settings, sControls const *controls, sSensors const *sensors);

bool drv_is_driving();
bool drv_is_ebraking();
bool drv_is_accelerating();
bool drv_is_cruise_control();
