#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#define NETWORK_PREAMBULA 0x4C8A39F1

typedef enum {
    NetworkCmd_LCD,
    NetworkCmd_Controller,
    NetworkCmd__Max
} eNetworkCmd;

typedef enum {
    NetworkBatterySoC_None,
    NetworkBatterySoC_Flashing,
    NetworkBatterySoC_Empty,
    NetworkBatterySoC_1Bar,
    NetworkBatterySoC_2Bars,
    NetworkBatterySoC_3Bars,
    NetworkBatterySoC_Full,
    NetworkBatterySoC_Charging0,
    NetworkBatterySoC_Charging1,
    NetworkBatterySoC_Charging2,
    NetworkBatterySoC_Charging3,
    NetworkBatterySoC_Recuperating,
    NetworkBatterySoC__Max
} eNetworkBatterySoC;

typedef enum {
    NetworkMotorPhase_0,
    NetworkMotorPhase_60,
    NetworkMotorPhase_120,
    NetworkMotorPhase_180,
    NetworkMotorPhase_240,
    NetworkMotorPhase_300,
    NetworkMotorPhase__Max
} eNetworkMotorPhase;

typedef enum {
    NetworkControllerControlFlag_None = 0,
    NetworkControllerControlFlag_Walking = (1 << 0),        // is walking mode
    NetworkControllerControlFlag_CruiseControl = (1 << 1),  // is cruise requested
    NetworkControllerControlFlag_Light = (1 << 2),          // is lights on
    NetworkControllerControlFlag_Braking = (1 << 3),        // is braking by grips
} eNetworkControllerControlFlags;

typedef enum {
    NetworkLCDControlFlag_None = 0,
    NetworkLCDControlFlag_Accelerating = (1 << 0),          // is accelerating
    NetworkLCDControlFlag_Braking = (1 << 1),               // is braking
    NetworkLCDControlFlag_Assist = (1 << 2),                // is assisting
    NetworkLCDControlFlag_CruiseControl = (1 << 3),         // is cruise control active
} eNetworkLCDControlFlags;

typedef enum {
    eNetworkLCDNotifyFlag_None = 0,
    eNetworkLCDNotifyFlag_ResetOdometer = (1 << 0),         // reset odometer
    eNetworkLCDNotifyFlag_ResetRideTime = (1 << 1),         // reset total ride time
    eNetworkLCDNotifyFlag_SwitchingOff = (1 << 2),          // is turning off
} eNetworkLCDNotifyFlags;

typedef struct {
    eNetworkMotorPhase phase;           // motor phase angle
    int8_t correction_angle;            // motor correction angle
    uint16_t wheel_circumference;       // wheel circumference in millimeters
    uint8_t pole_pairs;                 // number of motor pole pair
    uint16_t max_erps;                  // max wheel rotations per second
} sNetworkMotorSettings;

typedef struct {
    uint8_t gear;                       // current gear
    uint8_t throttle_ratio;             // throttle ratio
    uint8_t brake_ratio;                // brake ratio
    eNetworkControllerControlFlags flags;
} sNetworkControl;

typedef struct {
    eNetworkLCDNotifyFlags notify_flags;
    sNetworkMotorSettings settings;
    sNetworkControl control;
    struct {
        uint16_t voltage;               // battery voltage in 0.002 V
        int16_t system_temp;            // temperature of LCD system
    } sensors;
} sNetworkPayload_Controller;

typedef struct {
    struct {
        eNetworkLCDControlFlags flags;
        uint8_t max_gear;               // max possible gear for controller
        eNetworkBatterySoC battery_soc; // battery state of charge
    } control;
    struct {
        uint16_t erps;                  // speed in electrical rotations per second
        uint16_t voltage;               // in 0.002 V
        uint16_t wattage;               // in watts
        int16_t temp_motor;             // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and drive_setting)
    } sensors;
    struct {
        uint32_t erotations;            // number of electrical wheel rotations (to calc the distance)
        uint32_t ride_time;             // ride time in seconds
        uint32_t total_erotations;      // total number of electrical wheel rotations (to calc the distance)
        uint32_t total_ride_time;       // total ride time in seconds
        uint16_t wattage_consumed;      // wattage consumed in watts
    } stat;
} sNetworkPayload_LCD;

void network_cycle();
void network_init();

sNetworkPayload_Controller const *network_get_payload_controller();

void network_update_payload_lcd(sNetworkPayload_LCD const *payload_lcd);
