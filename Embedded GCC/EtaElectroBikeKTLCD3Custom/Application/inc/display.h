#pragma once

#include <stdint.h>

typedef enum {
    DispBatterySoc_None,
    DispBatterySoc_Flashing,
    DispBatterySoc_Charging0,
    DispBatterySoc_Charging1,
    DispBatterySoc_Charging2,
    DispBatterySoc_Charging3,
    DispBatterySoc_Recuperating,
    DispBatterySoc_Empty,
    DispBatterySoc_1Bar,
    DispBatterySoc_2Bars,
    DispBatterySoc_3Bars,
    DispBatterySoc_Full,
} eDispBatterySoC;

void disp_cycle();
void disp_set_battery_soc(eDispBatterySoC battery_soc);
