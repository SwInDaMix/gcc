#pragma once

#include <stdint.h>
#include "periph.h"
#include "network.h"

#define ktlcd3_sendbyte(byte) periph_uart_putbyte(byte)

typedef enum {
    KTLCD3_Battery_None = 0,
    KTLCD3_Battery_Flashing = 1,
    KTLCD3_Battery_Charging = 2,
    KTLCD3_Battery_Empty = 3,
    KTLCD3_Battery_1Bar = 4,
    KTLCD3_Battery_2Bars = 8,
    KTLCD3_Battery_3Bars = 12,
    KTLCD3_Battery_Full = 16
} eKTLCD3_Battery;

typedef enum {
    KTLCD3_Error_None = 0x0,
    KTLCD3_Error_01_Throttle = 0x22,
    KTLCD3_Error_02_Brake = 0x23,
    KTLCD3_Error_03_Hall = 0x24,
    KTLCD3_Error_04_Torque = 0x26,
    KTLCD3_Error_05_Axis = 0x28,
    KTLCD3_Error_06_ShortCircuit = 0x21,
    KTLCD3_Error_91_BatteryUnderVoltage = 0x91
} eKTLCD3_Error;

typedef enum {
    KTLCD3_MovingStatus_None = 0,
    KTLCD3_MovingStatus_Throttling = (1 << 1),
    KTLCD3_MovingStatus_CruiseControl = (1 << 3),
    KTLCD3_MovingStatus_Assist = (1 << 4),
    KTLCD3_MovingStatus_Braking = (1 << 5)
} eKTLCD3_MovingStatus;

typedef enum {
    KTLCD3_WheelSize_6 = 18,
    KTLCD3_WheelSize_8 = 10,
    KTLCD3_WheelSize_10 = 14,
    KTLCD3_WheelSize_12 = 2,
    KTLCD3_WheelSize_14 = 6,
    KTLCD3_WheelSize_16 = 0,
    KTLCD3_WheelSize_18 = 4,
    KTLCD3_WheelSize_20 = 8,
    KTLCD3_WheelSize_22 = 12,
    KTLCD3_WheelSize_24 = 16,
    KTLCD3_WheelSize_26 = 20,
    KTLCD3_WheelSize_28 = 28,
    KTLCD3_WheelSize_29 = 30,
    KTLCD3_WheelSize_700c = 24
} eKTLCD3_WheelSize;

typedef enum {
    KTLCD3_AssistLevelPark = 0,
    KTLCD3_AssistLevel1 = 1,
    KTLCD3_AssistLevel2 = 2,
    KTLCD3_AssistLevel3 = 3,
    KTLCD3_AssistLevel4 = 4,
    KTLCD3_AssistLevel5 = 5,
    KTLCD3_AssistLevelWalk = 6,
} eKTLCD3_AssistLevel;

typedef enum {
    KTLCD3_MaxCurrent_100,
    KTLCD3_MaxCurrent_250,
    KTLCD3_MaxCurrent_333,
    KTLCD3_MaxCurrent_500,
    KTLCD3_MaxCurrent_667,
    KTLCD3_MaxCurrent_750,
    KTLCD3_MaxCurrent_800,
    KTLCD3_MaxCurrent_833,
    KTLCD3_MaxCurrent_870,
    KTLCD3_MaxCurrent_910,
    KTLCD3_MaxCurrent_1000
} eKTLCD3_MaxCurrent;

typedef enum {
    KTLCD3_MinVoltage_m20,
    KTLCD3_MinVoltage_m15,
    KTLCD3_MinVoltage_m10,
    KTLCD3_MinVoltage_m05,
    KTLCD3_MinVoltage_Default,
    KTLCD3_MinVoltage_p05,
    KTLCD3_MinVoltage_p10,
    KTLCD3_MinVoltage_p15
} eKTLCD3_MinVoltage;

typedef enum {
    KTLCD3_RegenStrength_None,
    KTLCD3_RegenStrength_Min,
    KTLCD3_RegenStrength_Small,
    KTLCD3_RegenStrength_Medium,
    KTLCD3_RegenStrength_Good,
    KTLCD3_RegenStrength_Max
} eKTLCD3_RegenStrength;

typedef enum {
    KTLCD3_PowerAssistSens_Weak,
    KTLCD3_PowerAssistSens_Medium,
    KTLCD3_PowerAssistSens_Max
} eKTLCD3_PowerAssistSens;

typedef enum {
    KTLCD3_Shifts_AssistLevel = 0,
    KTLCD3_Shifts_MaxSpeed1 = 3,
    KTLCD3_Shifts_MaxSpeed2 = 5,
    KTLCD3_Shifts_WheelSize1 = 0,
    KTLCD3_Shifts_WheelSize2 = 6,
    KTLCD3_Shifts_p2 = 0,
    KTLCD3_Shifts_p3 = 3,
    KTLCD3_Shifts_p4 = 4,
    KTLCD3_Shifts_c1 = 3,
    KTLCD3_Shifts_c2 = 0,
    KTLCD3_Shifts_c5 = 0,
    KTLCD3_Shifts_c14 = 5,
    KTLCD3_Shifts_c4 = 5,
    KTLCD3_Shifts_c12 = 0,
    KTLCD3_Shifts_c13 = 2,
    KTLCD3_Shifts_MaxSpeedPart2 = 5,
    KTLCD3_Shifts_WheelSizePart2 = 2
} eKTLCD3_Shifts;

typedef enum {
    KTLCD3_Masks_AssistLevel = (7 << KTLCD3_Shifts_AssistLevel),
    KTLCD3_Masks_FrontLight = (1 << 7),
    KTLCD3_Masks_MaxSpeed1 = (31 << KTLCD3_Shifts_MaxSpeed1),
    KTLCD3_Masks_MaxSpeed2 = (1 << KTLCD3_Shifts_MaxSpeed2),
    KTLCD3_Masks_WheelSize1 = (7 << KTLCD3_Shifts_WheelSize1),
    KTLCD3_Masks_WheelSize2 = (3 << KTLCD3_Shifts_WheelSize2),
    KTLCD3_Masks_CruiseControl = (1 << 4),
    KTLCD3_Masks_p2 = (7 << KTLCD3_Shifts_p2),
    KTLCD3_Masks_p3 = (1 << KTLCD3_Shifts_p3),
    KTLCD3_Masks_p4 = (1 << KTLCD3_Shifts_p4),
    KTLCD3_Masks_c1 = (7 << KTLCD3_Shifts_c1),
    KTLCD3_Masks_c2 = (7 << KTLCD3_Shifts_c2),
    KTLCD3_Masks_c5 = (15 << KTLCD3_Shifts_c5),
    KTLCD3_Masks_c14 = (3 << KTLCD3_Shifts_c14),
    KTLCD3_Masks_c4 = (7 << KTLCD3_Shifts_c4),
    KTLCD3_Masks_c12 = (15 << KTLCD3_Shifts_c12),
    KTLCD3_Masks_c13 = (7 << KTLCD3_Shifts_c13)
} eKTLCD3_Masks;

typedef struct {
    uint8_t _mark65;
    uint8_t battery;
    uint8_t _mark48;
    uint16_t wheel_period_ms;
    uint8_t error;
    uint8_t crc;
    uint8_t moving_status;
    uint8_t motor_current;
    int8_t motor_temperature;
    uint16_t _mark0;
} sKTLCD3_To;

typedef struct {
    uint8_t p5;
    uint8_t assist_level_front_light;
    uint8_t wheel_size_max_speed;
    uint8_t p1;
    uint8_t wheel_size_max_speed_p2_p3_p4;
    uint8_t crc;
    uint8_t c1_c2;
    uint8_t c5_c14;
    uint8_t c4_cruise;
    uint8_t c12;
    uint8_t c13;
    uint8_t _mark50;
    uint8_t _mark14;
} sKTLCD3_From;

bool ktlcd3_was_updated();
bool ktlcd3_was_ever_updated();
uint8_t ktlcd3_get_power_monitoring_settings();
bool ktlcd3_get_is_headlight_on();
eKTLCD3_AssistLevel ktlcd3_get_assist_level();
uint8_t ktlcd3_get_assist_level_duty_cycle(eKTLCD3_AssistLevel assist_level);
uint8_t ktlcd3_get_motor_poluses();
eKTLCD3_WheelSize ktlcd3_get_wheel_size();
uint16_t ktlcd3_get_wheel_size_circumference(eKTLCD3_WheelSize wheel_size);
uint8_t ktlcd3_get_max_speed();
uint8_t ktlcd3_get_wheel_speed_pulses_per_revolution();
bool ktlcd3_get_ignore_pas_gear_ratio();
bool ktlcd3_get_throttle_when_moving();
uint8_t ktlcd3_get_throttle_startup_settings();
eNetworkMotorPhase ktlcd3_get_motor_phase();
uint8_t ktlcd3_get_handlebar_function();
bool ktlcd3_get_cruise_control();
eKTLCD3_MaxCurrent ktlcd3_get_max_current();
uint8_t ktlcd3_get_max_current_scale(eKTLCD3_MaxCurrent max_current);
eKTLCD3_MinVoltage ktlcd3_get_min_voltage();
int8_t ktlcd3_get_min_voltage_correct(eKTLCD3_MinVoltage min_voltage);
eKTLCD3_RegenStrength ktlcd3_get_regen_strength();
eKTLCD3_PowerAssistSens ktlcd3_get_power_assist_sens();

bool ktlcd3_eat_byte(uint8_t byte);

/**
 * @brief Update infromation on KT-LCD display according to the specified arguments.
 * @param battery Current battery level @see=eKTLCD3_Battery
 * @param error Current Error state @see=eKTLCD3_Error
 * @param moving_status Current moving status @see=eKTLCD3_MovingStatus
 * @param wheel_period_ms Wheel rotation period in ms
 * @param motor_current Motor current in 0.25A units
 * @param motor_temperature Current motor temperature in C -15, eg. 0 = 15C and 20 = 35C
 */
void ktlcd3_update(eKTLCD3_Battery battery, eKTLCD3_Error error, eKTLCD3_MovingStatus moving_status, uint16_t wheel_period_ms, uint8_t motor_current, int8_t motor_temperature);
