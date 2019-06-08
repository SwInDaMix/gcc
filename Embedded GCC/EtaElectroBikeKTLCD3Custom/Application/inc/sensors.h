#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#define HUD_BATTERY_VOLTAGE_K 67U

// not used due to low performance of calculation
//#define HUD_TEMP_OPP_RESISTANCE 2200.0f
//#define HUD_TEMP_NOMINAL_TEMP (25.0f + 273.15f)
//#define HUD_TEMP_NOMINAL_RESISTANCE 10000.0f
//#define HUD_TEMP_BCOEFFICIENT 3095.0f

typedef struct {
    uint16_t voltage;               // in 0.002 V
    uint16_t system_temp;           // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and drive_setting)
} sSensors;

sSensors const *sensors_get_current();
