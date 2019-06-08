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
    DispStatusFlags_None,
    DispStatusFlags_Light = (1 << 0),
    DispStatusFlags_Braking = (1 << 1),
} eDispStatusFlags;

typedef enum {
    DispMainFlags_None,
    DispMainFlags_Walking = (1 << 0),
    DispMainFlags_Assist = (1 << 1),
    DispMainFlags_Cruise = (1 << 2),
    DispMainFlags_Accelerating = (1 << 3)
} eDispMainFlags;

typedef enum {
    DispMainDriveSetting_None,
    DispMainDriveSetting_MaxSpeed,
    DispMainDriveSetting_TTM,
    DispMainDriveSetting_ODO,
    DispMainDriveSetting_MeasureUnitMetric,
    DispMainDriveSetting_MeasureUnitImperic,
    DispMainDriveSetting__Max
} eDispMainDriveSetting;

typedef enum {
    DispSetting_MotorPhase,
    DispSetting_MotorPolePairs,
    DispSetting_WheelCircumference,
    DispSetting_CorrectionAngle,
    DispSetting__Max,
} eDispSetting;

typedef struct {
    eDispStatusFlags flags;
    eNetworkBatterySoC battery_soc;
} sDispScreenStatus;

typedef struct {
    eDispMainState state;
    eDispMainFlags flags;
    eDispMainMeasureUnit measure_unit;
    eDispMainDriveSetting drive_setting;
    uint8_t gear;                   // 0 is parking mode;
    struct {
        uint16_t current;           // in 0.01 Kmh/mph
        uint16_t max;               // in 0.01 Kmh/mph
        uint16_t avg;               // in 0.01 Kmh/mph
    } speed;
    struct {
        uint16_t voltage;           // in 0.002 V
        uint16_t wattage;           // in watts
        int16_t temp_system;        // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and drive_setting)
        int16_t temp_motor;         // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and drive_setting)
    } sensors;
    struct {
        uint16_t wattage_consumed;  // wattage consumed
        struct {
            uint32_t session;       // 0.01 Km/Mil
            uint32_t odometer;      // 0.01 Km/Mil
        } distance;
        struct {
            uint32_t session;       // in seconds
            uint32_t ride;          // in seconds
            uint32_t total_ride;    // in seconds
        } time;
    } stat;
} sDispScreenMain;

typedef struct {
    eDispSetting setting;
    sNetworkMotorSettings motor_settings;
} sDispScreenSettings;

eDispState disp_cycle(eDispState state);

void disp_set_screens(sDispScreenStatus *screen_status, sDispScreenMain *screen_main, sDispScreenSettings *screen_settings);
