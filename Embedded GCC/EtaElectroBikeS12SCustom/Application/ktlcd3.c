#include <string.h>
#include <stdbool.h>
#include <ktlcd3.h>
#include <stdio.h>
#include "config.h"
#include "ktlcd3.h"
#include "utils.h"

static uint8_t const s_max_current_lookup[] = {
        [KTLCD3_MaxCurrent_100] = (uint8_t)(255.0f * 100 / 1000),
        [KTLCD3_MaxCurrent_250] = (uint8_t)(255.0f * 250 / 1000),
        [KTLCD3_MaxCurrent_333] = (uint8_t)(255.0f * 333 / 1000),
        [KTLCD3_MaxCurrent_500] = (uint8_t)(255.0f * 500 / 1000),
        [KTLCD3_MaxCurrent_667] = (uint8_t)(255.0f * 667 / 1000),
        [KTLCD3_MaxCurrent_750] = (uint8_t)(255.0f * 750 / 1000),
        [KTLCD3_MaxCurrent_800] = (uint8_t)(255.0f * 800 / 1000),
        [KTLCD3_MaxCurrent_833] = (uint8_t)(255.0f * 833 / 1000),
        [KTLCD3_MaxCurrent_870] = (uint8_t)(255.0f * 870 / 1000),
        [KTLCD3_MaxCurrent_910] = (uint8_t)(255.0f * 910 / 1000),
        [KTLCD3_MaxCurrent_1000] = (uint8_t)(255.0f * 1000 / 1000)
};

#define _wheel_diam_correct (0.998f)
#define _wheel_diam_i_to_m(diam) ((diam) * 0.0254f)
#define _calc_wheel_circumference(diam) ((float)(diam) * _wheel_diam_correct * 3.1415926f)
static float const s_wheel_size_lookup[] = {
        [KTLCD3_WheelSize_6] = _calc_wheel_circumference(_wheel_diam_i_to_m(6)),
        [KTLCD3_WheelSize_8] = _calc_wheel_circumference(_wheel_diam_i_to_m(8)),
        [KTLCD3_WheelSize_10] = _calc_wheel_circumference(_wheel_diam_i_to_m(10)),
        [KTLCD3_WheelSize_12] = _calc_wheel_circumference(_wheel_diam_i_to_m(12)),
        [KTLCD3_WheelSize_14] = _calc_wheel_circumference(_wheel_diam_i_to_m(14)),
        [KTLCD3_WheelSize_16] = _calc_wheel_circumference(_wheel_diam_i_to_m(16)),
        [KTLCD3_WheelSize_18] = _calc_wheel_circumference(_wheel_diam_i_to_m(18)),
        [KTLCD3_WheelSize_20] = _calc_wheel_circumference(_wheel_diam_i_to_m(20)),
        [KTLCD3_WheelSize_22] = _calc_wheel_circumference(_wheel_diam_i_to_m(22)),
        [KTLCD3_WheelSize_24] = _calc_wheel_circumference(_wheel_diam_i_to_m(24)),
        [KTLCD3_WheelSize_26] = _calc_wheel_circumference(_wheel_diam_i_to_m(26)),
        [KTLCD3_WheelSize_28] = _calc_wheel_circumference(_wheel_diam_i_to_m(28)),
        [KTLCD3_WheelSize_29] = _calc_wheel_circumference(_wheel_diam_i_to_m(29)),
        [KTLCD3_WheelSize_700c] = _calc_wheel_circumference(0.7f)
};

static int8_t const s_min_voltage_correct_lookup[] = {
        [KTLCD3_MinVoltage_m20] = -20,
        [KTLCD3_MinVoltage_m15] = -15,
        [KTLCD3_MinVoltage_m10] = -10,
        [KTLCD3_MinVoltage_m05] = -5,
        [KTLCD3_MinVoltage_Default] = 0,
        [KTLCD3_MinVoltage_p05] = 5,
        [KTLCD3_MinVoltage_p10] = 10,
        [KTLCD3_MinVoltage_p15] = 15
};

#define _calc_dc_by_ratio(ratio) ((uint8_t)(255 * ratio))
static uint8_t const s_assist_level_duty_cycles_lookup[] = {
        [KTLCD3_AssistLevelPark] = _calc_dc_by_ratio(0.0),
        [KTLCD3_AssistLevel1] = _calc_dc_by_ratio(0.2),
        [KTLCD3_AssistLevel2] = _calc_dc_by_ratio(0.4),
        [KTLCD3_AssistLevel3] = _calc_dc_by_ratio(0.6),
        [KTLCD3_AssistLevel4] = _calc_dc_by_ratio(0.8),
        [KTLCD3_AssistLevel5] = _calc_dc_by_ratio(1.0),
        [KTLCD3_AssistLevelWalk] = _calc_dc_by_ratio(0.1),
};

static sKTLCD3_From s_received_struct;
static bool s_need_update = false;
static bool s_was_updated = false;
static bool s_was_ever_updated = false;

static uint8_t s_power_monitoring_settings;
static bool s_is_headlight_on;
static eKTLCD3_AssistLevel s_assist_level;
static uint8_t s_motor_poluses;
static eKTLCD3_WheelSize s_wheel_size;
static uint8_t s_max_speed;
static uint8_t s_wheel_speed_pulses_per_revolution;
static bool s_ignore_pas_gear_ratio;
static bool s_throttle_when_moving;
static uint8_t s_throttle_startup_settings;
static eKTLCD3_MotorPhase s_motor_phase;
static uint8_t s_handlebar_function;
static bool s_cruise_control;
static eKTLCD3_MaxCurrent s_max_current;
static eKTLCD3_MinVoltage s_min_voltage;
static eKTLCD3_RegenStrength s_regen_strength;
static eKTLCD3_PowerAssistSens s_power_assist_sens;

static void _ktlcd3_reparse_struct() {
    if (s_need_update) {
        s_need_update = false;
        s_power_monitoring_settings = s_received_struct.p5;
        s_is_headlight_on = s_received_struct.assist_level_front_light & KTLCD3_Masks_FrontLight;
        s_assist_level = (eKTLCD3_AssistLevel)(s_received_struct.assist_level_front_light & KTLCD3_Masks_AssistLevel) >> KTLCD3_Shifts_AssistLevel;
        s_motor_poluses = s_received_struct.p1;
        s_max_speed = (((s_received_struct.wheel_size_max_speed & KTLCD3_Masks_MaxSpeed1) >> KTLCD3_Shifts_MaxSpeed1) | ((s_received_struct.wheel_size_max_speed_p2_p3_p4 & KTLCD3_Masks_MaxSpeed2) << (KTLCD3_Shifts_MaxSpeedPart2 - KTLCD3_Shifts_MaxSpeed2))) + (uint8_t)10;
        s_wheel_size = (eKTLCD3_WheelSize)((s_received_struct.wheel_size_max_speed & KTLCD3_Masks_WheelSize1) << (KTLCD3_Shifts_WheelSizePart2 - KTLCD3_Shifts_WheelSize1)) | ((s_received_struct.wheel_size_max_speed_p2_p3_p4 & KTLCD3_Masks_WheelSize2) >> KTLCD3_Shifts_WheelSize2);
        s_wheel_speed_pulses_per_revolution = ((s_received_struct.wheel_size_max_speed_p2_p3_p4 & KTLCD3_Masks_p2) >> KTLCD3_Shifts_p2);
        s_ignore_pas_gear_ratio = s_received_struct.wheel_size_max_speed_p2_p3_p4 & KTLCD3_Masks_p3;
        s_throttle_when_moving = s_received_struct.wheel_size_max_speed_p2_p3_p4 & KTLCD3_Masks_p4;
        s_throttle_startup_settings = (s_received_struct.c1_c2 & KTLCD3_Masks_c1) >> KTLCD3_Shifts_c1;
        s_motor_phase = (eKTLCD3_MotorPhase)(s_received_struct.c1_c2 & KTLCD3_Masks_c2) >> KTLCD3_Shifts_c2;
        s_handlebar_function = (s_received_struct.c4_cruise & KTLCD3_Masks_c4) >> KTLCD3_Shifts_c4;
        s_cruise_control = s_received_struct.c4_cruise & KTLCD3_Masks_CruiseControl;
        s_max_current = (eKTLCD3_MaxCurrent)(s_received_struct.c5_c14 & KTLCD3_Masks_c5) >> KTLCD3_Shifts_c5;
        s_min_voltage = (eKTLCD3_MinVoltage)(s_received_struct.c12 & KTLCD3_Masks_c12) >> KTLCD3_Shifts_c12;
        s_regen_strength = (eKTLCD3_RegenStrength)(s_received_struct.c13 & KTLCD3_Masks_c13) >> KTLCD3_Shifts_c13;
        s_power_assist_sens = (eKTLCD3_PowerAssistSens)(s_received_struct.c5_c14 & KTLCD3_Masks_c14) >> KTLCD3_Shifts_c14;
    }
}

bool ktlcd3_was_updated() { bool _res = s_was_updated; s_was_updated = false; return _res; }
bool ktlcd3_was_ever_updated() { return s_was_ever_updated; }
uint8_t ktlcd3_get_power_monitoring_settings() { _ktlcd3_reparse_struct(); return s_power_monitoring_settings; }
bool ktlcd3_get_is_headlight_on() { _ktlcd3_reparse_struct(); return s_is_headlight_on; }
eKTLCD3_AssistLevel ktlcd3_get_assist_level() { _ktlcd3_reparse_struct(); return s_assist_level; }
uint8_t ktlcd3_get_assist_level_duty_cycle(eKTLCD3_AssistLevel assist_level) { return s_assist_level_duty_cycles_lookup[assist_level]; }
uint8_t ktlcd3_get_motor_poluses() { _ktlcd3_reparse_struct(); return s_motor_poluses; }
eKTLCD3_WheelSize ktlcd3_get_wheel_size() { _ktlcd3_reparse_struct(); return s_wheel_size; }
float ktlcd3_get_wheel_size_circumference(eKTLCD3_WheelSize wheel_size) { return s_wheel_size_lookup[wheel_size]; }
uint8_t ktlcd3_get_max_speed() { _ktlcd3_reparse_struct(); return s_max_speed; }
uint8_t ktlcd3_get_wheel_speed_pulses_per_revolution() { _ktlcd3_reparse_struct(); return s_wheel_speed_pulses_per_revolution; }
bool ktlcd3_get_ignore_pas_gear_ratio() { _ktlcd3_reparse_struct(); return s_ignore_pas_gear_ratio; }
bool ktlcd3_get_throttle_when_moving() { _ktlcd3_reparse_struct(); return s_throttle_when_moving; }
uint8_t ktlcd3_get_throttle_startup_settings() { _ktlcd3_reparse_struct(); return s_throttle_startup_settings; }
eKTLCD3_MotorPhase ktlcd3_get_motor_phase() { _ktlcd3_reparse_struct(); return s_motor_phase; }
uint8_t ktlcd3_get_handlebar_function() { _ktlcd3_reparse_struct(); return s_handlebar_function; }
bool ktlcd3_get_cruise_control() { _ktlcd3_reparse_struct(); return s_cruise_control; }
eKTLCD3_MaxCurrent ktlcd3_get_max_current() { _ktlcd3_reparse_struct(); return s_max_current; }
uint8_t ktlcd3_get_max_current_scale(eKTLCD3_MaxCurrent max_current) { _ktlcd3_reparse_struct(); return s_max_current_lookup[max_current]; }
eKTLCD3_MinVoltage ktlcd3_get_min_voltage() { _ktlcd3_reparse_struct(); return s_min_voltage; }
int8_t ktlcd3_get_min_voltage_correct(eKTLCD3_MinVoltage min_voltage) { _ktlcd3_reparse_struct(); return s_min_voltage_correct_lookup[min_voltage]; }
eKTLCD3_RegenStrength ktlcd3_get_regen_strength() { _ktlcd3_reparse_struct(); return s_regen_strength; }
eKTLCD3_PowerAssistSens ktlcd3_get_power_assist_sens() { _ktlcd3_reparse_struct(); return s_power_assist_sens; }

bool ktlcd3_eat_byte(uint8_t byte) {
    static uint8_t s_received_buf[sizeof(sKTLCD3_From)];
    static uint8_t s_received_buf_idx = 0;
    static bool s_fullbuf = false;

    s_received_buf[s_received_buf_idx++] = byte;
    if (s_received_buf_idx >= sizeof(sKTLCD3_From)) { s_received_buf_idx = 0; s_fullbuf = true; }

    if(s_fullbuf) {
        uint8_t _crc_orig = 0, _crc = 0;
        uint8_t _i;
        uint8_t _idx = s_received_buf_idx;
        uint8_t _mark1 = 0, _mark2 = 0;

        for(_i = 0; _i < sizeof(sKTLCD3_From); _i++) {
            if(_i == 5) _crc_orig = s_received_buf[_idx];
            else if(_i != 0 && _i != 12) _crc ^= s_received_buf[_idx];
            if(_i == 11) _mark1 = s_received_buf[_idx];
            if(_i == 12) _mark2 = s_received_buf[_idx];

            _idx++;
            if (_idx >= sizeof(sKTLCD3_From)) _idx = 0;
        }

        if(_crc == _crc_orig && _mark1 == 50 && _mark2 == 14) {
            if (s_received_buf_idx == 0) memcpy(&s_received_struct, s_received_buf, sizeof(sKTLCD3_From));
            else {
                memcpy(&s_received_struct, s_received_buf + s_received_buf_idx, sizeof(sKTLCD3_From) - s_received_buf_idx);
                memcpy(((uint8_t *)&s_received_struct) + sizeof(sKTLCD3_From) - s_received_buf_idx, s_received_buf, s_received_buf_idx);
            }
            s_need_update = s_was_updated = s_was_ever_updated = true;
            return true;
        }
    }
    return false;
}

void ktlcd3_update(eKTLCD3_Battery battery, eKTLCD3_Error error, eKTLCD3_MovingStatus moving_status, uint16_t wheel_period_ms, uint8_t motor_current, int8_t motor_temperature) {
    sKTLCD3_To _to_send;
    uint8_t *_ptr;
    uint8_t _crc = 0;
    uint8_t _i;

    _to_send._mark65 = 65;
    _to_send._mark48 = 32;
    _to_send._mark0 = 0;
    _to_send.crc = 0;
    _to_send.battery = battery;
    _to_send.error = error;
    _to_send.moving_status = moving_status;
    _to_send.wheel_period_ms = wheel_period_ms;
    _to_send.motor_current = motor_current;
    _to_send.motor_temperature = motor_temperature;

    _ptr = (uint8_t *)&_to_send + 1;
    for (_i = 0; _i < sizeof(sKTLCD3_To) - 1; _i++) { _crc ^= *_ptr++; }
    _to_send.crc = _crc;
    _ptr = (uint8_t *)&_to_send;
    for (_i = 0; _i < sizeof(sKTLCD3_To); _i++) { ktlcd3_sendbyte(*_ptr++); }
}