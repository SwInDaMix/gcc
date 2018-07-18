/*
 * EGG OpenSource EBike firmware
 *
 * Copyright (C) Casainho, 2015, 2106, 2017.
 *
 * Released under the GPL License, Version 3
 */

#include <stdint.h>
#include "stm8s_gpio.h"
#include "stm8s_tim1.h"
#include "main.h"
#include "periph.h"
#include "bldc.h"

#define BLDC_MAX_TABLE_PWM_PERIOD 255
#define BLDC_MIDDLE_TABLE_PWM_PERIOD (BLDC_MAX_TABLE_PWM_PERIOD >> 1)
#define BLDC_MAX_TIMER_PWM_PERIOD (1 << BLDC_PWM_FULL_DUTY_BITS)
#define BLDC_MIDDLE_TIMER_PWM_PERIOD (BLDC_MAX_TIMER_PWM_PERIOD >> 1)
#define BLDC_PWM_COUNTS_PER_SECOND (FCPU / (1 << BLDC_PWM_FULL_DUTY_BITS))

typedef enum {
    BLDC_HallPosition_Invalid,
    BLDC_HallPosition_0,
    BLDC_HallPosition_60,
    BLDC_HallPosition_120,
    BLDC_HallPosition_180,
    BLDC_HallPosition_240,
    BLDC_HallPosition_300,
} sBLDC_HallPosition;

typedef enum {
    BLDC_MotorState_Idle = 0,
    BLDC_MotorState_LowSpeed = 1,
    BLDC_MotorState_Interpolation60 = 2,
    BLDC_MotorState_Interpolation360 = 3,
    BLDC_MotorState_Error = 4,
    BLDC_MotorState__MaskRotating = 3,
    BLDC_MotorState__MaskError = 4
} sBLDC_MotorState;

uint8_t const s_pwm_table_3harm_sine[256] = {
        0, 6, 13, 19, 25, 31, 37, 44, 50, 56, 62, 68, 74, 80, 86, 92,
        98, 103, 109, 115, 120, 126, 131, 136, 142, 147, 152, 157, 162, 167, 171, 176,
        180, 185, 189, 193, 197, 201, 205, 208, 212, 215, 219, 222, 225, 228, 231, 233,
        236, 238, 240, 242, 244, 246, 247, 249, 250, 251, 252, 253, 254, 254, 255, 255,
        255, 255, 255, 254, 254, 253, 252, 251, 250, 249, 247, 246, 244, 242, 240, 238,
        236, 233, 231, 228, 225, 222, 223, 226, 229, 231, 234, 236, 239, 241, 243, 245,
        246, 248, 249, 250, 252, 253, 253, 254, 254, 255, 255, 255, 255, 255, 254, 254,
        253, 252, 251, 250, 248, 247, 245, 243, 241, 239, 237, 235, 232, 230, 227, 224,
        221, 218, 214, 211, 207, 204, 200, 196, 192, 188, 183, 179, 174, 170, 165, 160,
        155, 150, 145, 140, 135, 129, 124, 118, 113, 107, 101, 96, 90, 84, 78, 72,
        66, 60, 54, 48, 42, 35, 29, 23, 17, 10, 4, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
uint8_t const s_pwm_table_pure_sine[256] = {
        127, 130, 133, 136, 140, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173,
        176, 179, 182, 185, 187, 190, 193, 195, 198, 201, 203, 206, 208, 211, 213, 215,
        218, 220, 222, 224, 226, 228, 230, 232, 233, 235, 237, 238, 240, 241, 243, 244,
        245, 246, 248, 249, 249, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255,
        255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 249, 249, 248, 246,
        245, 244, 243, 241, 240, 238, 237, 235, 233, 232, 230, 228, 226, 224, 222, 220,
        218, 215, 213, 211, 208, 206, 203, 201, 198, 195, 193, 190, 187, 185, 182, 179,
        176, 173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 140, 136, 133, 130,
        128, 125, 122, 119, 115, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82,
        79, 76, 73, 70, 68, 65, 62, 60, 57, 54, 52, 49, 47, 44, 42, 40,
        37, 35, 33, 31, 29, 27, 25, 23, 22, 20, 18, 17, 15, 14, 12, 11,
        10, 9, 7, 6, 6, 5, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 6, 6, 7, 9,
        10, 11, 12, 14, 15, 17, 18, 20, 22, 23, 25, 27, 29, 31, 33, 35,
        37, 40, 42, 44, 47, 49, 52, 54, 57, 60, 62, 65, 68, 70, 73, 76,
        79, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112, 115, 119, 122, 125,
};

/// Forward ///
// BLDC_HallPosition_240, BLDC_HallPosition_120, BLDC_HallPosition_180, BLDC_HallPosition_0, BLDC_HallPosition_300, BLDC_HallPosition_60
// BLDC_HallPosition_180, BLDC_HallPosition_60, BLDC_HallPosition_120, BLDC_HallPosition_300, BLDC_HallPosition_240, BLDC_HallPosition_0
// BLDC_HallPosition_120, BLDC_HallPosition_0, BLDC_HallPosition_60, BLDC_HallPosition_240, BLDC_HallPosition_180, BLDC_HallPosition_300
// BLDC_HallPosition_60, BLDC_HallPosition_300, BLDC_HallPosition_0, BLDC_HallPosition_180, BLDC_HallPosition_120, BLDC_HallPosition_240
// BLDC_HallPosition_0, BLDC_HallPosition_240, BLDC_HallPosition_300, BLDC_HallPosition_120, BLDC_HallPosition_60, BLDC_HallPosition_180
// BLDC_HallPosition_300, BLDC_HallPosition_180, BLDC_HallPosition_240, BLDC_HallPosition_60, BLDC_HallPosition_0, BLDC_HallPosition_120

/// Backward ///
// BLDC_HallPosition_60, BLDC_HallPosition_180, BLDC_HallPosition_120, BLDC_HallPosition_300, BLDC_HallPosition_0, BLDC_HallPosition_240
// BLDC_HallPosition_0, BLDC_HallPosition_120, BLDC_HallPosition_60, BLDC_HallPosition_240, BLDC_HallPosition_300, BLDC_HallPosition_180
// BLDC_HallPosition_300, BLDC_HallPosition_60, BLDC_HallPosition_0, BLDC_HallPosition_180, BLDC_HallPosition_240, BLDC_HallPosition_120
// BLDC_HallPosition_240, BLDC_HallPosition_0, BLDC_HallPosition_300, BLDC_HallPosition_120, BLDC_HallPosition_180, BLDC_HallPosition_60
// BLDC_HallPosition_180, BLDC_HallPosition_300, BLDC_HallPosition_240, BLDC_HallPosition_60, BLDC_HallPosition_120, BLDC_HallPosition_0
// BLDC_HallPosition_120, BLDC_HallPosition_240, BLDC_HallPosition_180, BLDC_HallPosition_0, BLDC_HallPosition_60, BLDC_HallPosition_300

sBLDC_HallPosition const s_hall_positions_forward[8] = { BLDC_HallPosition_Invalid, BLDC_HallPosition_120, BLDC_HallPosition_0, BLDC_HallPosition_60, BLDC_HallPosition_240, BLDC_HallPosition_180, BLDC_HallPosition_300, BLDC_HallPosition_Invalid };
sBLDC_HallPosition const s_hall_positions_reverse[8] = { BLDC_HallPosition_Invalid, BLDC_HallPosition_60, BLDC_HallPosition_180, BLDC_HallPosition_120, BLDC_HallPosition_300, BLDC_HallPosition_0, BLDC_HallPosition_240, BLDC_HallPosition_Invalid };
uint8_t s_angle_correction_forward = (uint8_t)(BLDC_ANGLE_CORRECTION + 128);
uint8_t s_angle_correction_backward = (uint8_t)(BLDC_ANGLE_CORRECTION + 10);

uint8_t const s_hall_angles[8] = {
        [BLDC_HallPosition_0] = ANGLE_0,
        [BLDC_HallPosition_60] = ANGLE_60,
        [BLDC_HallPosition_120] = ANGLE_120,
        [BLDC_HallPosition_180] = ANGLE_180,
        [BLDC_HallPosition_240] = ANGLE_240,
        [BLDC_HallPosition_300] = ANGLE_300,
};
uint8_t const s_hall_diff_multiplicant[8] = {
        [BLDC_HallPosition_0] = 1,
        [BLDC_HallPosition_60] = 2,
        [BLDC_HallPosition_120] = 3,
        [BLDC_HallPosition_180] = 4,
        [BLDC_HallPosition_240] = 5,
        [BLDC_HallPosition_300] = 6,
};
sBLDC_HallPosition const s_hall_prev_forward_pos[8] = {
        [BLDC_HallPosition_0] = BLDC_HallPosition_300,
        [BLDC_HallPosition_60] = BLDC_HallPosition_0,
        [BLDC_HallPosition_120] = BLDC_HallPosition_60,
        [BLDC_HallPosition_180] = BLDC_HallPosition_120,
        [BLDC_HallPosition_240] = BLDC_HallPosition_180,
        [BLDC_HallPosition_300] = BLDC_HallPosition_240,
};
sBLDC_HallPosition const s_hall_prev_backward_pos[8] = {
        [BLDC_HallPosition_0] = BLDC_HallPosition_60,
        [BLDC_HallPosition_60] = BLDC_HallPosition_120,
        [BLDC_HallPosition_120] = BLDC_HallPosition_180,
        [BLDC_HallPosition_180] = BLDC_HallPosition_240,
        [BLDC_HallPosition_240] = BLDC_HallPosition_300,
        [BLDC_HallPosition_300] = BLDC_HallPosition_0,
};

static bool s_is_pure_sine;
static bool s_is_reverse = true;
static bool s_is_enabled;
static uint8_t s_duty_cycle;
static uint16_t s_erps;

static sBLDC_MotorState _s_motor_state = BLDC_MotorState_Idle;
static uint8_t _s_hall_correctness = BLDC_HALL_CORRECTNESS;
static sBLDC_HallPosition _s_hall_position_prev;

static uint16_t _s_cycles_hall_cnt = 65535;
static uint16_t _s_cycles_revolution_cnt = 65535;
static uint16_t _s_cycles_revolution_prev = 65535;
static uint16_t _s_cycles_revolution = 65535;
static uint16_t _s_cycles_hall_max;
static uint16_t _s_cycles_revolution_max;
static uint16_t _s_cycles_hall_min;
static uint16_t _s_cycles_revolution_min;
static int16_t _s_cycles_revolution_diff;
static int16_t _s_cycles_revolution_diff_current;

static uint8_t _s_hall_angle;
static uint8_t _s_rotation_angle;
static int8_t _s_correction_angle;
static bool _s_is_rotates_backward;

static void _pwm_overflow_callback() {
    sBLDC_HallPosition _s_hall_position;

    bool _is_pure_sine = s_is_pure_sine;
    bool _is_reverse = s_is_reverse;

    uint8_t _pwm_angle;
    uint8_t _pwm_angle_phase;
    uint8_t _pwm_value;
    uint16_t _pwm_duty_cycle_a;
    uint16_t _pwm_duty_cycle_b;
    uint16_t _pwm_duty_cycle_c;

#define _is_rotating() (_s_motor_state & BLDC_MotorState__MaskRotating)
#define _is_error() (_s_motor_state & BLDC_MotorState__MaskError)

    /// ====================================== ///
    /// Check counters sanity and hall sensors ///
    /// ====================================== ///

    if (_s_cycles_revolution_cnt < 65535) _s_cycles_revolution_cnt++;
    if (_s_cycles_hall_cnt < 65535) _s_cycles_hall_cnt++;

    _s_hall_position = (_is_reverse ? s_hall_positions_reverse : s_hall_positions_forward)[periph_get_bldc_hall_sensors()];

    if (_s_hall_position == BLDC_HallPosition_Invalid) _s_motor_state = BLDC_MotorState_Error; // validate hall sensors state
    else if (_s_cycles_revolution_cnt >= BLDC_PWM_COUNTS_PER_SECOND) { // if no full revolution during a second, consider the motor in idle state
        s_erps = 0;
        _s_motor_state = BLDC_MotorState_Idle;
        _s_is_rotates_backward = false;
    }

    /// ==================== ///
    /// Process hall sensors ///
    /// ==================== ///

    if (_s_hall_position_prev != _s_hall_position) {
        if (_s_hall_position == BLDC_HallPosition_Invalid) _s_hall_correctness = 0;
        else {
            // check hall sensors correctness
            if (_s_hall_position_prev == s_hall_prev_forward_pos[_s_hall_position]) {
                // motor hall sensors in predicted state and motor rotates forward
                _s_is_rotates_backward = false;
                if (_s_hall_correctness < BLDC_HALL_CORRECTNESS && _s_hall_position == BLDC_HallPosition_0) _s_hall_correctness++;
            }
            else if (_s_hall_position_prev == s_hall_prev_backward_pos[_s_hall_position]) {
                // motor hall sensors in predicted state BUT motor rotates backward!
                // its normal situation so here we need just to disable PWM for freewheeling
                _s_is_rotates_backward = true;
            }
            else {
                // hall sensors in incorrect state
                _s_hall_correctness = 0;
                // if hall sensors failed during rotation, state an error
                if (_is_rotating()) _s_motor_state = BLDC_MotorState_Error;
            }

            if (_s_hall_position == BLDC_HallPosition_0) {
                // update revolution cycle counters
                if (_s_cycles_revolution_prev >= BLDC_PWM_COUNTS_PER_SECOND || _s_cycles_revolution_prev == 0) _s_cycles_revolution_prev = _s_cycles_revolution_cnt;
                else _s_cycles_revolution_prev = _s_cycles_revolution;
                _s_cycles_revolution = _s_cycles_revolution_cnt;
                _s_cycles_revolution_cnt = 0;

                // on interpolation if hall sensors didn't change its state during its predicted period, state an error
                if (_s_motor_state == BLDC_MotorState_Interpolation60) {
                    if (_s_cycles_hall_cnt > _s_cycles_hall_max || _s_cycles_hall_cnt < _s_cycles_hall_min) _s_motor_state = BLDC_MotorState_Error;
                }
                else if (_s_motor_state == BLDC_MotorState_Interpolation360) {
                    if (_s_cycles_revolution_cnt > _s_cycles_revolution_max || _s_cycles_revolution_cnt < _s_cycles_revolution_min) _s_motor_state = BLDC_MotorState_Error;
                }

                // calc maximum and minimum for next iteration
                // diff against previous counter to correct interpolation
                _s_cycles_hall_max = _s_cycles_revolution >> 2; // maximum cycles per hall change
                _s_cycles_revolution_max = _s_cycles_revolution << 1; // maximum cycles per revolution change
                _s_cycles_hall_min = _s_cycles_revolution >> 3; // minimum cycles per hall change
                _s_cycles_revolution_min = _s_cycles_revolution >> 1; // minimum cycles per revolution change
                _s_cycles_revolution_diff = _s_cycles_revolution_prev - _s_cycles_revolution;

                // calc erps speed
                s_erps = (uint16_t)BLDC_PWM_COUNTS_PER_SECOND / _s_cycles_revolution;

                if (_s_hall_correctness == BLDC_HALL_CORRECTNESS) {
                    // hear we can start interpolation if motor rotates fast enough
                    if (s_erps >= BLDC_INTERPOLATION_360_ERPS) _s_motor_state = BLDC_MotorState_Interpolation360;
                    else if (s_erps >= BLDC_INTERPOLATION_60_ERPS) _s_motor_state = BLDC_MotorState_Interpolation60;
                    else if (s_erps > 0) _s_motor_state = BLDC_MotorState_LowSpeed;
                }
            }

            _s_cycles_hall_cnt = 0;
            _s_hall_angle = s_hall_angles[_s_hall_position] + (s_is_reverse ? s_angle_correction_backward : s_angle_correction_forward);

            // calc current diff
            _s_cycles_revolution_diff_current = _s_cycles_revolution_diff > 0 ? (_s_cycles_revolution_diff * s_hall_diff_multiplicant[_s_hall_position] / (uint8_t)6) : (uint8_t)0;
        }
        _s_hall_position_prev = _s_hall_position;
    }

    /// =============================================== ///
    /// Process interpolation if in interpolation state ///
    /// =============================================== ///

    // interpolate the right rotation angle
    if (_s_motor_state == BLDC_MotorState_Interpolation60) { _s_rotation_angle = _s_hall_angle + (uint8_t)((_s_cycles_hall_cnt << 8) / (_s_cycles_revolution + _s_cycles_revolution_diff_current)); }
    else if (_s_motor_state == BLDC_MotorState_Interpolation360) { _s_rotation_angle = (s_is_reverse ? s_angle_correction_backward : s_angle_correction_forward) + (uint8_t)((_s_cycles_revolution_cnt << 8) / (_s_cycles_revolution + _s_cycles_revolution_diff_current)); }
    else {
        _s_rotation_angle = _s_hall_angle;
        _s_correction_angle = 0;
    }

    /// ======================================================================================== ///
    /// Update pwm according to motor state, duty cycle, rotor angle position and its correction ///
    /// ======================================================================================== ///

    if (!s_is_enabled || _is_error() || _s_is_rotates_backward) periph_set_bldc_pwm_outputs(false);
    else {
        _pwm_angle = _s_rotation_angle + _s_correction_angle;

        if (_is_pure_sine) {
            // Pure sine control requires the PWM to be driven from middle point
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            _pwm_value = s_pwm_table_pure_sine[_pwm_angle];
            if (_pwm_value > BLDC_MIDDLE_TABLE_PWM_PERIOD) _pwm_duty_cycle_a = (((_pwm_value - (uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD) * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS)) + (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD;
            else _pwm_duty_cycle_a = (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD - ((((uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD - _pwm_value) * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS));

            _pwm_value = s_pwm_table_pure_sine[(uint8_t)(_pwm_angle + (uint8_t)ANGLE_120)];
            if (_pwm_value > BLDC_MIDDLE_TABLE_PWM_PERIOD) _pwm_duty_cycle_b = (((_pwm_value - (uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD) * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS)) + (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD;
            else _pwm_duty_cycle_b = (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD - ((((uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD - _pwm_value) * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS));

            _pwm_value = s_pwm_table_pure_sine[(uint8_t)(_pwm_angle + (uint8_t)ANGLE_240)];
            if (_pwm_value > BLDC_MIDDLE_TABLE_PWM_PERIOD) _pwm_duty_cycle_c = (((_pwm_value - (uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD) * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS)) + (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD;
            else _pwm_duty_cycle_c = (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD - ((((uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD - _pwm_value) * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS));
        }
        else {
            // In case of sine with third-order harmonic, PWM must be driven from the ground
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            _pwm_value = s_pwm_table_3harm_sine[_pwm_angle];
            _pwm_duty_cycle_a = (_pwm_value * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS);

            if (_is_reverse) _pwm_angle_phase = (uint8_t)(_pwm_angle - (uint8_t)ANGLE_120); else _pwm_angle_phase = (uint8_t)(_pwm_angle + (uint8_t)ANGLE_120);
            _pwm_value = s_pwm_table_3harm_sine[_pwm_angle_phase];
            _pwm_duty_cycle_b = (_pwm_value * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS);

            if (_is_reverse) _pwm_angle_phase = (uint8_t)(_pwm_angle - (uint8_t)ANGLE_240); else _pwm_angle_phase = (uint8_t)(_pwm_angle + (uint8_t)ANGLE_240);
            _pwm_value = s_pwm_table_3harm_sine[_pwm_angle_phase];
            _pwm_duty_cycle_c = (_pwm_value * (uint16_t)s_duty_cycle) >> (16 - BLDC_PWM_FULL_DUTY_BITS);
        }

        periph_set_bldc_pwm_duty_cucles(_pwm_duty_cycle_c, _pwm_duty_cycle_b, _pwm_duty_cycle_a);

        periph_set_bldc_pwm_outputs(true);
    }
}

void bldc_init() {
    periph_set_bldc_pwm_overflow_callback(_pwm_overflow_callback);
    _s_hall_position_prev = s_hall_prev_forward_pos[(s_is_reverse ? s_hall_positions_reverse : s_hall_positions_forward)[periph_get_bldc_hall_sensors()]];
}
void bldc_output(bool is_enabled) {
    s_is_enabled = is_enabled;
    if (!s_is_enabled) periph_set_bldc_pwm_outputs(false);
}
void bldc_set_pwm_duty_cycle(uint8_t duty_cycle) { s_duty_cycle = duty_cycle; }
uint16_t bldc_get_erps() { return s_erps; }

void bldc_debug() {
    DBGF("bldc: hp: %d, hc: %d, ms: %d, ha: %3d, ra: %3d, ca: %3d, erps: %3d, ",
         _s_hall_position_prev,
         _s_hall_correctness,
         _s_motor_state,
         _s_hall_angle,
         _s_rotation_angle,
         _s_correction_angle,
         s_erps);
}