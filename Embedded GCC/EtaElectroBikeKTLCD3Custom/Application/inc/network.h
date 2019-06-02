#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "display.h"

#define NETWORK_PREAMBULA 0x4C8A39F1

typedef enum {
    NetworkCmd_LCD,
    NetworkCmd_Controller,
    NetworkCmd__Max,
} eNetworkCmd;

typedef struct {
    struct {
        uint16_t voltage;               // battery voltage in 0.002 V
        int16_t system_temp;            // temperature of LCD system
    } sensors;
    struct {
        uint16_t wheel_circumference;   // wheel circumference in millimeters
        uint8_t motor_pole_pairs;       // number of motor poles
        uint16_t max_erps;              // max wheel rotations per second
    } settings;
    struct {
        uint8_t gear;                   // current gear
        uint8_t throttle_ratio;         // throttle ratio
        uint8_t brake_ratio;            // brake ratio
        bool is_walking;                // is whalking mode
        bool is_cruise_control;         // is cruise control
        bool is_light;                  // is lights on
        bool is_braking;                // is braking by grips
    } control;
} sNetworkPayload_Controller;

typedef struct {
    struct {
        uint8_t max_gear;               // max possible gear for controller
        eDispBatterySoC battery_soc;    // battery state of charge
    } drive;
    struct {
        uint16_t erps;                  // speed in electrical rotations per second
        uint16_t voltage;               // in 0.002 V
        uint16_t wattage;               // in watts
        int16_t temp_motor;             // from -99 to 199 C/F (200 to 399 treated as 0 to 199 C/F and flashing)
    } sensors;
    struct {
        uint32_t total_erotations;      // number of electrical wheel rotations (to calc the distance)
        uint16_t total_ride_time;       // total ride time in seconds
        uint16_t wattage_consumed;      // wattage consumed in watts
    } stat;
} sNetworkPayload_LCD;

void network_cycle();
void network_init();

sNetworkPayload_LCD const *network_get_payload_lcd();

void network_send_payload_controller(sNetworkPayload_Controller const *payload_controller);

