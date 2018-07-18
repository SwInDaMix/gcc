#include <stdint.h>
#include "stm8s_gpio.h"
#include "stm8s_tim1.h"
#include "main.h"
#include "periph.h"
#include "bldc.h"

#define BLDC_MAX_TABLE_PWM_PERIOD UINT8_MAX
#define BLDC_MIDDLE_TABLE_PWM_PERIOD (BLDC_MAX_TABLE_PWM_PERIOD >> 1)
#define BLDC_MAX_TIMER_PWM_PERIOD (1 << BLDC_PWM_FULL_DUTY_BITS)
#define BLDC_MIDDLE_TIMER_PWM_PERIOD (BLDC_MAX_TIMER_PWM_PERIOD >> 1)
#define BLDC_PWM_COUNTS_PER_SECOND (FCPU / (1 << BLDC_PWM_FULL_DUTY_BITS))
#define BLDC_PWM_MAX_COUNTS_PER_REVOLUTION (BLDC_PWM_COUNTS_PER_SECOND / 2)

#define BLDC_BLOCK_CYCLES_PER_RAMP (BLDC_INTERPOLATION_60_ERPS * 6)
#define BLDC_BLOCK_RAMP_STEP (49152L * BLDC_BLOCK_CYCLES_PER_RAMP / BLDC_PWM_COUNTS_PER_SECOND)
#define BLDC_BLOCK_RAMP_HIGH (BLDC_BLOCK_RAMP_STEP * BLDC_PWM_COUNTS_PER_SECOND / BLDC_BLOCK_CYCLES_PER_RAMP)

typedef enum {
    BLDC_HallPosition_Invalid,
    BLDC_HallPosition_0,
    BLDC_HallPosition_60,
    BLDC_HallPosition_120,
    BLDC_HallPosition_180,
    BLDC_HallPosition_240,
    BLDC_HallPosition_300,
} eBLDC_HallPosition;

typedef enum {
    BLDC_MotorState_Idle = 0,
    BLDC_MotorState_BlockCommutation = 1,
    BLDC_MotorState_Interpolation60 = 2,
    BLDC_MotorState_Interpolation360 = 3,
    BLDC_MotorState_Error_Hall = 4,
    BLDC_MotorState__MaskRotating = 3,
    BLDC_MotorState__MaskError = 4
} eBLDC_MotorState;

uint8_t const s_pwm_table_3harm_sine[256] = {
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
        0, 6, 13, 19, 25, 31, 37, 44, 50, 56, 62, 68, 74, 80, 86, 92,
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
// BLDC_HallPosition_120, BLDC_HallPosition_0, BLDC_HallPosition_60, BLDC_HallPosition_240, BLDC_HallPosition_180, BLDC_HallPosition_300  c - b - a
// BLDC_HallPosition_60, BLDC_HallPosition_300, BLDC_HallPosition_0, BLDC_HallPosition_180, BLDC_HallPosition_120, BLDC_HallPosition_240
// BLDC_HallPosition_0, BLDC_HallPosition_240, BLDC_HallPosition_300, BLDC_HallPosition_120, BLDC_HallPosition_60, BLDC_HallPosition_180
// BLDC_HallPosition_300, BLDC_HallPosition_180, BLDC_HallPosition_240, BLDC_HallPosition_60, BLDC_HallPosition_0, BLDC_HallPosition_120

/// Backward ///
// BLDC_HallPosition_60, BLDC_HallPosition_180, BLDC_HallPosition_120, BLDC_HallPosition_300, BLDC_HallPosition_0, BLDC_HallPosition_240
// BLDC_HallPosition_0, BLDC_HallPosition_120, BLDC_HallPosition_60, BLDC_HallPosition_240, BLDC_HallPosition_300, BLDC_HallPosition_180
// BLDC_HallPosition_300, BLDC_HallPosition_60, BLDC_HallPosition_0, BLDC_HallPosition_180, BLDC_HallPosition_240, BLDC_HallPosition_120
// BLDC_HallPosition_240, BLDC_HallPosition_0, BLDC_HallPosition_300, BLDC_HallPosition_120, BLDC_HallPosition_180, BLDC_HallPosition_60
// BLDC_HallPosition_180, BLDC_HallPosition_300, BLDC_HallPosition_240, BLDC_HallPosition_60, BLDC_HallPosition_120, BLDC_HallPosition_0
// BLDC_HallPosition_120, BLDC_HallPosition_240, BLDC_HallPosition_180, BLDC_HallPosition_0, BLDC_HallPosition_60, BLDC_HallPosition_300 a - b - c

eBLDC_HallPosition const s_hall_positions_forward[8] = { BLDC_HallPosition_Invalid, BLDC_HallPosition_120, BLDC_HallPosition_0, BLDC_HallPosition_60, BLDC_HallPosition_240, BLDC_HallPosition_180, BLDC_HallPosition_300, BLDC_HallPosition_Invalid };
eBLDC_HallPosition const s_hall_positions_reverse[8] = { BLDC_HallPosition_Invalid, BLDC_HallPosition_120, BLDC_HallPosition_240, BLDC_HallPosition_180, BLDC_HallPosition_0, BLDC_HallPosition_60, BLDC_HallPosition_300, BLDC_HallPosition_Invalid };

uint8_t const s_hall_angles[8] = {
        [BLDC_HallPosition_0] = BLDC_ANGLE_0,
        [BLDC_HallPosition_60] = BLDC_ANGLE_60,
        [BLDC_HallPosition_120] = BLDC_ANGLE_120,
        [BLDC_HallPosition_180] = BLDC_ANGLE_180,
        [BLDC_HallPosition_240] = BLDC_ANGLE_240,
        [BLDC_HallPosition_300] = BLDC_ANGLE_300,
};
uint8_t const s_hall_diff_multiplicand[8] = {
        [BLDC_HallPosition_0] = 1,
        [BLDC_HallPosition_60] = 2,
        [BLDC_HallPosition_120] = 3,
        [BLDC_HallPosition_180] = 4,
        [BLDC_HallPosition_240] = 5,
        [BLDC_HallPosition_300] = 6,
};
eBLDC_HallPosition const s_hall_prev_forward_pos[8] = {
        [BLDC_HallPosition_0] = BLDC_HallPosition_300,
        [BLDC_HallPosition_60] = BLDC_HallPosition_0,
        [BLDC_HallPosition_120] = BLDC_HallPosition_60,
        [BLDC_HallPosition_180] = BLDC_HallPosition_120,
        [BLDC_HallPosition_240] = BLDC_HallPosition_180,
        [BLDC_HallPosition_300] = BLDC_HallPosition_240,
};
eBLDC_HallPosition const s_hall_prev_backward_pos[8] = {
        [BLDC_HallPosition_0] = BLDC_HallPosition_60,
        [BLDC_HallPosition_60] = BLDC_HallPosition_120,
        [BLDC_HallPosition_120] = BLDC_HallPosition_180,
        [BLDC_HallPosition_180] = BLDC_HallPosition_240,
        [BLDC_HallPosition_240] = BLDC_HallPosition_300,
        [BLDC_HallPosition_300] = BLDC_HallPosition_0,
};

static bool s_is_pure_sine;
static bool s_is_reverse;
static bool s_is_invert_halls;
static bool s_is_enabled;
static int8_t s_correction_angle;
static uint8_t s_duty_cycle;
static uint16_t s_erps;
static uint16_t s_current_0;
static uint16_t s_current_1;
static uint16_t s_current_2;
static uint16_t s_current_3;
static uint16_t s_current_4;
static uint16_t s_current_5;

static eBLDC_MotorState _s_motor_state = BLDC_MotorState_Idle;
static uint8_t _s_hall_correctness = BLDC_HALL_CORRECTNESS;
static eBLDC_HallPosition _s_hall_position_prev;

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

static uint16_t _pwm_block_ramp_up;
static uint16_t _pwm_block_ramp_down;
static uint8_t _s_hall_angle;
static uint8_t _s_rotation_angle;
static int8_t _s_foc_angle;
static bool _s_is_rotates_backward;

#define _is_rotating() (_s_motor_state & BLDC_MotorState__MaskRotating)
#define _is_block_commutation() (_s_motor_state == BLDC_MotorState_Idle || _s_motor_state == BLDC_MotorState_BlockCommutation)
#define _is_interpolating() (_s_motor_state == BLDC_MotorState_Interpolation60 || _s_motor_state == BLDC_MotorState_Interpolation360)
#define _is_error() (_s_motor_state & BLDC_MotorState__MaskError)
#define _can_drive() (!_is_error() && !_s_is_rotates_backward)

static void _pwm_overflow_callback() {
    eBLDC_HallPosition _s_hall_position;

    bool _is_pure_sine = s_is_pure_sine;
    bool _is_reverse = s_is_reverse;

    uint8_t _pwm_angle;
    uint8_t _pwm_value_a;
    uint8_t _pwm_value_b;
    uint8_t _pwm_value_c;
    uint16_t _pwm_duty_cycle_a;
    uint16_t _pwm_duty_cycle_b;
    uint16_t _pwm_duty_cycle_c;

    periph_wdt_reset();

    /// ====================================== ///
    /// Check counters sanity and hall sensors ///
    /// ====================================== ///

    if (_s_cycles_revolution_cnt < UINT16_MAX) _s_cycles_revolution_cnt++;
    if (_s_cycles_hall_cnt < UINT16_MAX) _s_cycles_hall_cnt++;

    _s_hall_position = (_is_reverse ? s_hall_positions_reverse : s_hall_positions_forward)[periph_get_bldc_hall_sensors() ^ (s_is_invert_halls ? 7 : 0)];

    // validate hall sensors state
    if (_s_hall_position == BLDC_HallPosition_Invalid) {
        _s_motor_state = BLDC_MotorState_Error_Hall;
        _s_hall_correctness = 0;
    } else {
        // on interpolation if counters overflow its predicted period, switch to block commutation
        if ((_s_motor_state == BLDC_MotorState_Interpolation60 && _s_cycles_hall_cnt > _s_cycles_hall_max) ||
            (_s_motor_state == BLDC_MotorState_Interpolation360 && _s_cycles_revolution_cnt > _s_cycles_revolution_max)) {
            _s_motor_state = BLDC_MotorState_BlockCommutation;
            _s_hall_correctness = 0;
        }

        if (_s_cycles_revolution_cnt >= BLDC_PWM_MAX_COUNTS_PER_REVOLUTION) { // if cycles revolution counter overflows before full revolution, consider the motor in idle state
            s_erps = 0;
            _s_motor_state = BLDC_MotorState_Idle;
            _s_is_rotates_backward = false;
        }
    }

    // on block commutation do ramping
    if (_is_block_commutation()) {
        if (_pwm_block_ramp_up < (uint16_t)BLDC_BLOCK_RAMP_HIGH) _pwm_block_ramp_up += (uint16_t)BLDC_BLOCK_RAMP_STEP;
        if (_pwm_block_ramp_down >= (uint16_t)BLDC_BLOCK_RAMP_STEP) _pwm_block_ramp_down -= (uint16_t)BLDC_BLOCK_RAMP_STEP;
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
            } else if (_s_hall_position_prev == s_hall_prev_backward_pos[_s_hall_position]) {
                // motor hall sensors in predicted state BUT motor rotates backward!
                // its normal situation so here we need just to disable PWM for freewheeling
                _s_is_rotates_backward = true;
            } else {
                // hall sensors in incorrect state
                _s_hall_correctness = 0;
                // if hall sensors failed during rotation, state an error
                if (_is_rotating()) _s_motor_state = BLDC_MotorState_Error_Hall;
            }

            if (_s_hall_position == BLDC_HallPosition_0) {
                // on interpolation if hall sensors switched to fast against its predicted period, switch to block commutation
                if ((_s_motor_state == BLDC_MotorState_Interpolation60 && _s_cycles_hall_cnt < _s_cycles_hall_min) ||
                    (_s_motor_state == BLDC_MotorState_Interpolation360 && _s_cycles_revolution_cnt < _s_cycles_revolution_min)) {
                    _s_motor_state = BLDC_MotorState_BlockCommutation;
                    _s_hall_correctness = 0;
                }

                // update revolution cycle counters
                if (_s_cycles_revolution_prev >= BLDC_PWM_COUNTS_PER_SECOND || _s_cycles_revolution_prev == 0) _s_cycles_revolution_prev = _s_cycles_revolution_cnt;
                else _s_cycles_revolution_prev = _s_cycles_revolution;
                _s_cycles_revolution = _s_cycles_revolution_cnt;
                _s_cycles_revolution_cnt = 0;

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
                    else if (s_erps > 0) _s_motor_state = BLDC_MotorState_BlockCommutation;
                } else if (s_erps > 0) _s_motor_state = BLDC_MotorState_BlockCommutation;
            }

            _s_cycles_hall_cnt = 0;
            _s_hall_angle = s_hall_angles[_s_hall_position] + s_correction_angle;

            // calc current diff
            _s_cycles_revolution_diff_current = _s_cycles_revolution_diff > 0 ? (_s_cycles_revolution_diff * s_hall_diff_multiplicand[_s_hall_position] / (uint8_t)6) : (uint8_t)0;

            /// ============== ///
            /// FOC Processing ///
            /// ============== ///
            if (_is_interpolating()) {
                // TODO: figure out which hall position is right for b_current measurement for FOC process
                if (_s_hall_position == BLDC_HallPosition_0) {
                    s_current_0 = periph_get_adc_phase_b_current();
                } else if (_s_hall_position == BLDC_HallPosition_60) {
                    s_current_1 = periph_get_adc_phase_b_current();
                } else if (_s_hall_position == BLDC_HallPosition_120) {
                    s_current_2 = periph_get_adc_phase_b_current();
                } else if (_s_hall_position == BLDC_HallPosition_180) {
                    s_current_3 = periph_get_adc_phase_b_current();
                } else if (_s_hall_position == BLDC_HallPosition_240) {
                    s_current_4 = periph_get_adc_phase_b_current();
                } else if (_s_hall_position == BLDC_HallPosition_300) {
                    s_current_5 = periph_get_adc_phase_b_current();
                }
            }

            /// ========================= ///
            /// Block commutation ramping ///
            /// ========================= ///
            if (_is_block_commutation()) {
                _pwm_block_ramp_up = 0;
                _pwm_block_ramp_down = BLDC_BLOCK_RAMP_HIGH;
            }
        }
        _s_hall_position_prev = _s_hall_position;
    }

    /// =============================================== ///
    /// Process interpolation if in interpolation state ///
    /// =============================================== ///

    // interpolate the right rotation angle
    if (_s_motor_state == BLDC_MotorState_Interpolation60) { _s_rotation_angle = _s_hall_angle + (uint8_t)((_s_cycles_hall_cnt << 8) / (_s_cycles_revolution + _s_cycles_revolution_diff_current)); }
    else if (_s_motor_state == BLDC_MotorState_Interpolation360) { _s_rotation_angle = (uint8_t)((_s_cycles_revolution_cnt << 8) / (_s_cycles_revolution + _s_cycles_revolution_diff_current)) + s_correction_angle; }
    else {
        _s_rotation_angle = _s_hall_angle;
        _s_foc_angle = 0;
    }

    /// ========================================================================================= ///
    /// Update pwm according to motor state, duty cycle, rotor angle position and its corrections ///
    /// ========================================================================================= ///

    if (!s_is_enabled || !_can_drive()) periph_set_bldc_pwm_outputs(false);
    else {
        _pwm_angle = _s_rotation_angle + _s_foc_angle;

        if (_is_block_commutation()) {
            if (_s_hall_position == BLDC_HallPosition_0) {
                _pwm_duty_cycle_c = 0;
                _pwm_duty_cycle_b = BLDC_BLOCK_RAMP_HIGH;
                _pwm_duty_cycle_a = _pwm_block_ramp_up;
            } else if (_s_hall_position == BLDC_HallPosition_60) {
                _pwm_duty_cycle_c = 0;
                _pwm_duty_cycle_b = _pwm_block_ramp_down;
                _pwm_duty_cycle_a = BLDC_BLOCK_RAMP_HIGH;
            } else if (_s_hall_position == BLDC_HallPosition_120) {
                _pwm_duty_cycle_c = _pwm_block_ramp_up;
                _pwm_duty_cycle_b = 0;
                _pwm_duty_cycle_a = BLDC_BLOCK_RAMP_HIGH;
            } else if (_s_hall_position == BLDC_HallPosition_180) {
                _pwm_duty_cycle_c = BLDC_BLOCK_RAMP_HIGH;
                _pwm_duty_cycle_b = 0;
                _pwm_duty_cycle_a = _pwm_block_ramp_down;
            } else if (_s_hall_position == BLDC_HallPosition_240) {
                _pwm_duty_cycle_c = BLDC_BLOCK_RAMP_HIGH;
                _pwm_duty_cycle_b = _pwm_block_ramp_up;
                _pwm_duty_cycle_a = 0;
            } else if (_s_hall_position == BLDC_HallPosition_300) {
                _pwm_duty_cycle_c = _pwm_block_ramp_down;
                _pwm_duty_cycle_b = BLDC_BLOCK_RAMP_HIGH;
                _pwm_duty_cycle_a = 0;
            } else _pwm_duty_cycle_c = _pwm_duty_cycle_b = _pwm_duty_cycle_a = 0;

            _pwm_value_a = (uint8_t)(_pwm_duty_cycle_a >> 8);
            _pwm_value_b = (uint8_t)(_pwm_duty_cycle_b >> 8);
            _pwm_value_c = (uint8_t)(_pwm_duty_cycle_c >> 8);
        } else {
            if (_is_pure_sine) {
                _pwm_value_a = s_pwm_table_pure_sine[_pwm_angle];
                _pwm_value_b = s_pwm_table_pure_sine[(uint8_t)(_pwm_angle + (uint8_t)BLDC_ANGLE_120)];
                _pwm_value_c = s_pwm_table_pure_sine[(uint8_t)(_pwm_angle + (uint8_t)BLDC_ANGLE_240)];
            } else {
                _pwm_value_a = s_pwm_table_3harm_sine[_pwm_angle];
                _pwm_value_b = s_pwm_table_3harm_sine[(uint8_t)(_pwm_angle + (uint8_t)BLDC_ANGLE_120)];
                _pwm_value_c = s_pwm_table_3harm_sine[(uint8_t)(_pwm_angle + (uint8_t)BLDC_ANGLE_240)];
            }
        }

        if (_is_pure_sine) {
            // Pure sine control requires the PWM to be driven from middle point
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            if (_pwm_value_a > BLDC_MIDDLE_TABLE_PWM_PERIOD) _pwm_duty_cycle_a = ((uint16_t)((uint8_t)(_pwm_value_a - (uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD) * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS)) + (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD;
            else _pwm_duty_cycle_a = (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD - ((uint16_t)((uint8_t)((uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD - _pwm_value_a) * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS));

            if (_pwm_value_b > BLDC_MIDDLE_TABLE_PWM_PERIOD) _pwm_duty_cycle_b = ((uint16_t)((uint8_t)(_pwm_value_b - (uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD) * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS)) + (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD;
            else _pwm_duty_cycle_b = (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD - ((uint16_t)((uint8_t)((uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD - _pwm_value_b) * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS));

            if (_pwm_value_c > BLDC_MIDDLE_TABLE_PWM_PERIOD) _pwm_duty_cycle_c = ((uint16_t)((uint8_t)(_pwm_value_c - (uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD) * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS)) + (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD;
            else _pwm_duty_cycle_c = (uint16_t)BLDC_MIDDLE_TIMER_PWM_PERIOD - ((uint16_t)((uint8_t)((uint8_t)BLDC_MIDDLE_TABLE_PWM_PERIOD - _pwm_value_c) * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS));
        } else {
            // In case of sine with third-order harmonic, PWM must be driven from the ground
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            _pwm_duty_cycle_a = (uint16_t)(_pwm_value_a * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS);
            _pwm_duty_cycle_b = (uint16_t)(_pwm_value_b * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS);
            _pwm_duty_cycle_c = (uint16_t)(_pwm_value_c * s_duty_cycle) >> (16U - BLDC_PWM_FULL_DUTY_BITS);
        }

        if (_is_reverse)periph_set_bldc_pwm_duty_cucles(_pwm_duty_cycle_c, _pwm_duty_cycle_a, _pwm_duty_cycle_b);
        else periph_set_bldc_pwm_duty_cucles(_pwm_duty_cycle_c, _pwm_duty_cycle_b, _pwm_duty_cycle_a);

        periph_set_bldc_pwm_outputs(true);
    }
}

void bldc_init(bool is_pure_sine, bool is_reverse, bool is_invert_halls, int8_t correction_angle) {
    s_is_pure_sine = is_pure_sine;
    s_is_reverse = is_reverse;
    s_is_invert_halls = is_invert_halls;
    s_correction_angle = correction_angle;
    _s_hall_position_prev = s_hall_prev_forward_pos[(s_is_reverse ? s_hall_positions_reverse : s_hall_positions_forward)[periph_get_bldc_hall_sensors()]];
    periph_set_bldc_pwm_overflow_callback(_pwm_overflow_callback);
}
void bldc_output(bool is_enabled) {
    s_is_enabled = is_enabled;
    if (!s_is_enabled) periph_set_bldc_pwm_outputs(false);
}
void bldc_set_pwm_duty_cycle(uint8_t duty_cycle) { s_duty_cycle = duty_cycle; }

uint16_t bldc_get_erps() { return s_erps; }
bool bldc_is_rotating() { return _is_rotating(); }
bool bldc_is_block_commutation() { return _is_block_commutation(); }
bool bldc_can_drive() { return _can_drive(); }
bool bldc_is_error() { return _is_error(); }

void bldc_debug() {
    DBGF("ms: %d",
         _s_motor_state);
//    DBGF(" == %d, %d, %d, %d, %d",
//         s_current_0,
//         s_current_1,
//         s_current_2,
//         s_current_3,
//         s_current_4,
//         s_current_5);
}