#pragma once

#include <stdint.h>
#include "lcd.h"

#define DISP_CRITICAL_MOTOR_TEMP 70

typedef enum {
    DispState_AnimateWithDigits,
    DispState_Main
} eDispState;

typedef enum {
    DispBatterySoc_None,
    DispBatterySoc_Flashing,
    DispBatterySoc_Charging0,
    DispBatterySoc_Charging1,
    DispBatterySoc_Charging2,
    DispBatterySoc_Charging3,
    DispBatterySoc_Recuperating,
    DispBatterySoc_Progress,
    DispBatterySoc_Empty,
    DispBatterySoc_1Bar,
    DispBatterySoc_2Bars,
    DispBatterySoc_3Bars,
    DispBatterySoc_Full,
} eDispBatterySoC;

typedef enum {
    DispMainState_Drive,
    DispMainState_Statistic1,
    DispMainState_Statistic2,
    DispMainState__Max,
} eDispMainState;

typedef enum {
    DispMainMeasureUnit_Metric,
    DispMainMeasureUnit_Imperic,
} eDispMainMeasureUnit;

typedef enum {
    DispMainFlags_Light = (1 << 0),
    DispMainFlags_Brake = (1 << 1),
    DispMainFlags_Walking = (1 << 2),
    DispMainFlags_Assist = (1 << 3),
    DispMainFlags_Cruise = (1 << 4),
    DispMainFlags_Accelerating = (1 << 5),
} eDispMainFlags;


typedef struct {
    eDispMainState state;
    eDispMainMeasureUnit measure_unit;
    eDispMainFlags flags;
    eDispBatterySoC battery_soc;
    uint8_t gear;                   // 0 is parking mode;
    struct {
        uint16_t current;           // in 0.002 Kmh/mph, max 49974
        uint16_t max;               // in 0.002 Kmh/mph, max 49974
        uint16_t avg;               // in 0.002 Kmh/mph, max 49974
    } speed;
    struct {
        uint16_t battery_voltage;   // in 0.002 V
        uint16_t current;           // in 0.001 A
        uint16_t wattage_consumed;  // wattage consumed
        int16_t temp_system;        // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and flashing)
        int16_t temp_motor;         // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and flashing)
    } sensors;
    struct {
        uint32_t session;           // 0.1 Km/Mil (9999.9 max)
        uint32_t odometer;          // 0.1 Km/Mil (9999.9 max)
    } distance;
    struct {
        uint16_t session;           // in min (59999 max - 999:59)
        uint16_t total;             // in min (59999 max - 999:59)
        bool half_second;
    } time;
    eLCDDigit test_dig;
} sDispMain;

void disp_cycle();
eDispState disp_get_state();

void disp_state_animate_digits();

void disp_state_main(sDispMain const *main);
