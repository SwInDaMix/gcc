#pragma once

#include <stdint.h>
#include "config.h"
#include "lcd.h"
#include "network.h"

typedef enum {
    DispState_AnimateWithDigits,
    DispState_Main,
    DispState_Settings,
} eDispState;

typedef enum {
    DispMainState_Drive,
    DispMainState_Statistic1,
    DispMainState_Statistic2,
    DispMainState__Max
} eDispMainState;

typedef enum {
    DispMainMeasureUnit_Metric,
    DispMainMeasureUnit_Imperic,
    DispMainMeasureUnit__Max
} eDispMainMeasureUnit;

typedef enum {
    DispMainFlags_None,
    DispMainFlags_Light = (1 << 0),
    DispMainFlags_Braking = (1 << 1),
    DispMainFlags_Walking = (1 << 2),
    DispMainFlags_Assist = (1 << 3),
    DispMainFlags_Cruise = (1 << 4),
    DispMainFlags_Accelerating = (1 << 5)
} eDispMainFlags;

typedef enum {
    DispMainFlashing_None,
    DispMainFlashing_MaxSpeed,
    DispMainFlashing_TTM,
    DispMainFlashing_ODO,
    DispMainFlashing_MeasureUnitMetric,
    DispMainFlashing_MeasureUnitImperic,
    DispMainFlashing__Max
} eDispMainFlashing;

typedef struct {
    eDispMainState state;
    eDispMainMeasureUnit measure_unit;
    eDispMainFlags flags;
    eDispMainFlashing flashing;
    eNetworkBatterySoC battery_soc;
    uint8_t gear;                   // 0 is parking mode;
    struct {
        uint16_t current;           // in 0.002 Kmh/mph, max 49974
        uint16_t max;               // in 0.002 Kmh/mph, max 49974
        uint16_t avg;               // in 0.002 Kmh/mph, max 49974
    } speed;
    struct {
        uint16_t voltage;           // in 0.002 V
        uint16_t wattage;           // in watts
        int16_t temp_system;        // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and flashing)
        int16_t temp_motor;         // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and flashing)
    } sensors;
    struct {
        uint16_t wattage_consumed;  // wattage consumed
        struct {
            uint32_t session;       // 0.01 Km/Mil (9999.99 max)
            uint32_t odometer;      // 0.01 Km/Mil (9999.99 max)
        } distance;
        struct {
            uint32_t session;       // in seconds (3599999 max - 999h 59m 59s)
            uint32_t ride;          // in seconds (3599999 max - 999h 59m 59s)
            uint32_t total_ride;    // in seconds (3599999 max - 999h 59m 59s)
        } time;
    } stat;
} sDispScreenMain;


typedef struct {
    uint8_t dummy;
} sDispScreenSettings;

eDispState disp_cycle(eDispState state);

void disp_set_screens(sDispScreenMain *screen_main, sDispScreenSettings *screen_settings);
