#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

// not used due to low performance of calculation
//#define HUD_TEMP_OPP_RESISTANCE 20000.0f
//#define HUD_TEMP_NOMINAL_TEMP (25.0f + 273.15f)
//#define HUD_TEMP_NOMINAL_RESISTANCE 20000.0f
//#define HUD_TEMP_BCOEFFICIENT 3095.0f

typedef struct {
    uint16_t voltage;               // in 0.002 V
    int8_t system_temp;             // from -128 to 127 C/F (200 to 399 treated as 0 to 199 C/F and flashing)
} sSensors;

sSensors const *sensors_get_current();
