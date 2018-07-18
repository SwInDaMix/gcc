#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#define HUD_BATTERY_VOLTAGE_K 67

// not used due to low performance of calculation
//#define HUD_TEMP_OPP_RESISTANCE 2200.0f
//#define HUD_TEMP_NOMINAL_TEMP (25.0f + 273.15f)
//#define HUD_TEMP_NOMINAL_RESISTANCE 10000.0f
//#define HUD_TEMP_BCOEFFICIENT 3095.0f

typedef struct {
    uint16_t battery_volts;
    uint16_t motor_current;
    uint16_t motor_current_filtered;
    int8_t motor_temperature;
} sSensors;

void sensors_init();

sSensors const *sensors_get_current();
