#pragma once

#include <stdint.h>
#include "config.h"
#include "sensors.h"
#include "settings.h"

typedef enum {
    LogicState_Drive,
    LogicState_QuickSettings,
    LogicState_Settings,
    LogicState__Max,
} eLogicState;

typedef struct {
    uint16_t session_time;
    uint16_t session_distance;
} sLogicStatistic;

void logic_init();
void logic_cycle(sSensors const *sensors, sSettings *settings);
