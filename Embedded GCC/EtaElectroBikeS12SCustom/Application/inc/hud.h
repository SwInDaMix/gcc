#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "settings.h"
#include "controls.h"
#include "sensors.h"

#define HUD_CYCLE_PERIOD_MS 75

#define HUD_BATTERY_CELL_VOLTS_100       4.20f
#define HUD_BATTERY_CELL_VOLTS_80        4.02f
#define HUD_BATTERY_CELL_VOLTS_60        3.87f
#define HUD_BATTERY_CELL_VOLTS_40        3.80f
#define HUD_BATTERY_CELL_VOLTS_20        3.73f
#define HUD_BATTERY_CELL_VOLTS_0         3.27f
#define HUD_BATTERY_CELL_VOLTS_CRTITICAL 3.17f

#define HUD_BATTERY_PACK_VOLTS_100       ((HUD_BATTERY_CELL_VOLTS_100 * BATTERY_CELLS_NUMBER) * 512)
#define HUD_BATTERY_PACK_VOLTS_80        ((HUD_BATTERY_CELL_VOLTS_80 * BATTERY_CELLS_NUMBER) * 512)
#define HUD_BATTERY_PACK_VOLTS_60        ((HUD_BATTERY_CELL_VOLTS_60 * BATTERY_CELLS_NUMBER) * 512)
#define HUD_BATTERY_PACK_VOLTS_40        ((HUD_BATTERY_CELL_VOLTS_40 * BATTERY_CELLS_NUMBER) * 512)
#define HUD_BATTERY_PACK_VOLTS_20        ((HUD_BATTERY_CELL_VOLTS_20 * BATTERY_CELLS_NUMBER) * 512)
#define HUD_BATTERY_PACK_VOLTS_0         ((HUD_BATTERY_CELL_VOLTS_0 * BATTERY_CELLS_NUMBER) * 512)
#define HUD_BATTERY_PACK_VOLTS_CRTITICAL ((HUD_BATTERY_CELL_VOLTS_CRTITICAL * BATTERY_CELLS_NUMBER) * 512)

#define HUD_TAILLIGHT_MARKER_PWM 15
#define HUD_MOTOR_CURRENT_ZERO_TH 2

#define HUD_ADC_THUMB_CRITICAL_MIN_VALUE 32
#define HUD_ADC_THUMB_CRITICAL_MAX_VALUE 240

void hud_init(sSettings const *settings, sSensors const *sensors);
void hud_cycle(sSettings const *settings, sControls const *controls, sSensors const *sensors);
