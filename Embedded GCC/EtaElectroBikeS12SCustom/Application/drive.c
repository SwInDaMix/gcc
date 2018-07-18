#include "drive.h"
#include "periph.h"
#include "bldc.h"
#include "utils.h"

#define BLDC_FOLTAGE_TO_ERPS_CALC_FILTERING 2

static int16_t s_i_term;

static uint16_t s_max_erps_predicted, s_max_erps_predicted_acc;
static bool s_is_driving = false;
static bool s_is_ebraking = false;
static bool s_is_auto_calibration = false;
static uint8_t s_cruise_control_min_duty_cycle;
static uint8_t s_erps_acceleration;

void drv_init(sSettings const *settings, sSensors const *sensors) {
    (void)settings;

    s_max_erps_predicted = (uint16_t)(sensors->battery_volts * BLDC_VOLTAGE_TO_ERPS_FACTOR);
    s_max_erps_predicted_acc = s_max_erps_predicted << BLDC_FOLTAGE_TO_ERPS_CALC_FILTERING;
}

void drv_cycle(sSettings const *settings, sControls const *controls, sSensors const *sensors) {
    static uint16_t _s_timer_prev;
    static uint8_t _s_new_duty_cycle, _s_prev_duty_cycle;
    static uint8_t _s_predicted_duty_cycle;

    uint16_t _timer_current;

    _timer_current = periph_get_timer();
    if ((_timer_current - _s_timer_prev) > TIMER_TP_MS(DRV_CYCLE_PERIOD_MS)) {
        static uint8_t _s_erps_staling;
        static uint16_t _prev_erps;

        uint16_t _current_erps;
        uint16_t _limit_current;
        uint8_t _wanted_duty_cycle;

        _s_timer_prev = _timer_current;

        _current_erps = bldc_get_erps();
        _limit_current = ((uint16_t)DRV_MAX_CURRENT_025A * settings->max_current_scale) / (uint8_t)255;

        s_is_ebraking = controls->thumb_brake >= DRV_ACCELERATION_THRESHOLD;

        // are we accelerating or regenerating
        if (_current_erps > _prev_erps && s_erps_acceleration < UINT8_MAX) {
            s_erps_acceleration++;
            _s_erps_staling = 0;
        } else if (_current_erps == _prev_erps) {
            _s_erps_staling++;
            if (_s_erps_staling >= (DRV_MIN_ACCELERATING_CYCLES >> 2)) s_erps_acceleration = _s_erps_staling = 0;
            else if (s_erps_acceleration != 0 && s_erps_acceleration < UINT8_MAX) s_erps_acceleration++;
        } else s_erps_acceleration = _s_erps_staling = 0;
        _prev_erps = _current_erps;

        // recalc predicted max ERPS
        if (s_is_auto_calibration) {
            // if driving and duty cycle among with speed is large enough to predict
            if (s_is_driving && (_current_erps >= DRV_MIN_ERPS_TO_PREDICT || _s_prev_duty_cycle >= DRV_MIN_DUTY_CYCLE_TO_PREDICT)) {
                uint16_t _new_erps_predicted = (_current_erps << 8) / _s_prev_duty_cycle;
                s_max_erps_predicted_acc = s_max_erps_predicted_acc - s_max_erps_predicted + _new_erps_predicted;
                s_max_erps_predicted = s_max_erps_predicted_acc >> BLDC_FOLTAGE_TO_ERPS_CALC_FILTERING;
            }
        } else {
            uint16_t _new_erps_predicted = (uint16_t)(sensors->battery_volts * BLDC_VOLTAGE_TO_ERPS_FACTOR);
            s_max_erps_predicted_acc = s_max_erps_predicted_acc - s_max_erps_predicted + _new_erps_predicted;
            s_max_erps_predicted = s_max_erps_predicted_acc >> BLDC_FOLTAGE_TO_ERPS_CALC_FILTERING;
        }

        // calc wanted duty cycle
        _s_predicted_duty_cycle = (uint8_t)map16(_current_erps, 0, s_max_erps_predicted, 0, UINT8_MAX);
        // wanted duty cycle always zero if throttling is allowed only when moving but motor is not rotating
        // if walking then always throttle to walk speed
        _wanted_duty_cycle = (settings->throttle_when_moving && _current_erps < DRV_MIN_MOVING_ERPS) ? (uint8_t)0 : (controls->is_walking ? (uint8_t)UINT8_MAX : map8(controls->thumb_throttle, 0, UINT8_MAX, 0, controls->gear_ratio));

        if (controls->is_braking) {
            // If we are braking, there is only one option - duty cycle to zero!
            _s_new_duty_cycle = 0;
        } else {
            uint16_t _limit_erps;

            _limit_erps = settings->erps_limit;
            if (controls->is_walking) _limit_erps = (uint16_t)((DRV_WALK_SPEED_KMH * settings->motor_polus_pairs / 3.6 / settings->wheel_circumference) + 0.5f);

            if (s_is_driving) {
                // if BLDC is driving the motor and we want to accelerate...
                if (_wanted_duty_cycle > _s_new_duty_cycle) {
                    if (sensors->motor_current > _limit_current) {
                        // ... but current is above the limited, then decrease duty cycle by acceleration ramp
                        if (_s_new_duty_cycle > DRV_THUMB_MAX_ACCELERATION_RAMP) _s_new_duty_cycle -= DRV_THUMB_MAX_ACCELERATION_RAMP; else _s_new_duty_cycle = 0;
                    } else if (_limit_erps != 0 && _current_erps > _limit_erps) {
                        // ... but speed is above the limited, then slowly decrease duty cycle
                        if (_s_new_duty_cycle > 0) _s_new_duty_cycle--;
                    } else {
                        // we can and want to accelerate, then calculate the acceleration ramp
                        // acceleration ramp is subtract of max predicted and current speed transformed to duty cycle units
                        uint8_t _acceleration_ramp = _limit_erps != 0 ? ((uint8_t)((uint16_t)((uint16_t)(_limit_erps - _current_erps) * (uint8_t)255) / s_max_erps_predicted)) : (uint8_t)DRV_THUMB_MAX_ACCELERATION_RAMP;

                        // but we should limit it to the maximum ramp
                        if (_acceleration_ramp > DRV_THUMB_MAX_ACCELERATION_RAMP) _acceleration_ramp = DRV_THUMB_MAX_ACCELERATION_RAMP;

                        // allow new duty cycle to be increased but not on more than acceleration ramp
                        if ((_wanted_duty_cycle - _s_new_duty_cycle) > _acceleration_ramp) _s_new_duty_cycle += _acceleration_ramp;
                        else _s_new_duty_cycle = _wanted_duty_cycle;

                        // limit new duty cycle to be not far away from predicted one
                        if (_s_new_duty_cycle > _s_predicted_duty_cycle && (_s_new_duty_cycle - _s_predicted_duty_cycle) > DRV_ACCELERATION_THRESHOLD) _s_new_duty_cycle = _s_predicted_duty_cycle + (uint8_t)DRV_ACCELERATION_THRESHOLD;
                    }
                } else _s_new_duty_cycle = _wanted_duty_cycle;
            } else {
                // if BLDC is not driving the motor, allow the new duty cycle to be not above than the sum of predicted duty cycle plus its threshold
                if (_s_new_duty_cycle < _wanted_duty_cycle) {
                    _s_new_duty_cycle = _wanted_duty_cycle;
                    if (_s_new_duty_cycle > _s_predicted_duty_cycle && (_s_new_duty_cycle - _s_predicted_duty_cycle) > DRV_THUMB_TOGGLE_THRESHOLD) _s_new_duty_cycle = _s_predicted_duty_cycle + (uint8_t)DRV_THUMB_TOGGLE_THRESHOLD;
                } else _s_new_duty_cycle = _wanted_duty_cycle;
            }
        }

        // set cruise control if requested
        if (s_is_driving && s_cruise_control_min_duty_cycle == 0 && controls->is_cruise_control) {
            s_cruise_control_min_duty_cycle = _s_new_duty_cycle;
        }
            // otherwise if on cruise control
        else if (s_cruise_control_min_duty_cycle > 0) {
            static uint8_t _s_prev_gear_ratio;

            // if wanted duty cycle is far much more than cruise control duty cycle, turn of cruise control
            if (_wanted_duty_cycle > s_cruise_control_min_duty_cycle && (_wanted_duty_cycle - s_cruise_control_min_duty_cycle) >= DRV_THUMB_TOGGLE_THRESHOLD) s_cruise_control_min_duty_cycle = 0;
                // if gear ratio changed, turn of cruise control
            else if (_s_prev_gear_ratio != controls->gear_ratio) s_cruise_control_min_duty_cycle = 0;
            else {
                // otherwise keep duty cycle as requested by cruise control
                _s_new_duty_cycle = s_cruise_control_min_duty_cycle;
                // but not less than predicted duty cycle minus threshold
                if (_s_predicted_duty_cycle > _s_new_duty_cycle && (_s_predicted_duty_cycle - _s_new_duty_cycle) >= DRV_THUMB_TOGGLE_THRESHOLD) _s_new_duty_cycle = _s_predicted_duty_cycle - (uint8_t)(DRV_THUMB_TOGGLE_THRESHOLD - 1);
            }

            _s_prev_gear_ratio = controls->gear_ratio;
        }

        DBGF("%3d, %3d === ",
             periph_get_adc_thumb_throttle(),
             periph_get_adc_thumb_break());

        DBGF("%3d, %3d, %3d, %3d, %3d === %3d, %3d, %3d, %3d, %3d, ",
             _current_erps,
             s_max_erps_predicted,
             _s_prev_duty_cycle,
             _s_new_duty_cycle,
             _s_predicted_duty_cycle,
             periph_get_adc_phase_b_current(),
             periph_get_adc_motor_current(),
             periph_get_adc_motor_current_filtered(),
             periph_get_adc_battery_voltage(),
             periph_get_adc_motor_temperature());

        bldc_debug();
        DBG("\n");

        _s_prev_duty_cycle = _s_new_duty_cycle;
    }

    if (s_is_driving) {
        if (controls->is_braking || _s_new_duty_cycle < DRV_THUMB_TOGGLE_THRESHOLD || (_s_predicted_duty_cycle > _s_new_duty_cycle && (_s_predicted_duty_cycle - _s_new_duty_cycle) >= DRV_THUMB_TOGGLE_THRESHOLD)) {
            bldc_output(false);
            bldc_set_pwm_duty_cycle(0);
            s_is_driving = false;
            s_cruise_control_min_duty_cycle = 0;
        } else {
            bldc_set_pwm_duty_cycle(_s_new_duty_cycle);
        }
    } else {
        if (bldc_can_drive() && _s_new_duty_cycle > _s_predicted_duty_cycle && (_s_new_duty_cycle - _s_predicted_duty_cycle) >= DRV_THUMB_TOGGLE_THRESHOLD) {
            bldc_set_pwm_duty_cycle(_s_new_duty_cycle);
            bldc_output(true);
            s_is_driving = true;
        }
    }
}

bool drv_is_driving() { return s_is_driving; }
bool drv_is_ebraking() { return s_is_ebraking; }
bool drv_is_accelerating() { return s_erps_acceleration >= DRV_MIN_ACCELERATING_CYCLES; }
bool drv_is_cruise_control() { return s_cruise_control_min_duty_cycle != 0; }

void drv_pi_reset() { s_i_term = 0; }
uint8_t drv_pi_calc(uint16_t current, uint16_t target) {
    int16_t _res;
    int16_t _error;
    int16_t _p_term;
    _error = (int16_t)(target - current);
    _p_term = (int16_t)((_error * (uint8_t)P_FACTOR_DIVIDENT) >> (uint8_t)P_FACTOR_DIVISOR);
    s_i_term += (int16_t)((_error * (uint8_t)I_FACTOR_DIVIDENT) >> (uint8_t)I_FACTOR_DIVISOR);
    if (s_i_term > 255) s_i_term = 255;
    else if (s_i_term < 0) s_i_term = 0;
    _res = _p_term + s_i_term;
    if (_res > 255) _res = 255;
    else if (_res < 5) _res = 0;
    return (uint8_t)_res;
}
