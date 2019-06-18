#include "drive.h"
#include "periph.h"
#include "bldc.h"
#include "hud.h"
#include "utils.h"

#define DRV_MAKE_SMOOTH(x) ((uint16_t)((uint8_t)(x) << 8) | (uint8_t)(x))

#define BLDC_VOLTAGE_TO_ERPS_CALC_FILTERING 2U
#define DRV_EBRAKE_BOOST_SMOOTH DRV_MAKE_SMOOTH(DRV_EBRAKE_BOOST)
#define DRV_ACCELERATION_BOOST_SMOOTH DRV_MAKE_SMOOTH(DRV_ACCELERATION_BOOST)
#define DRV_ACCELERATION_SMOOTH_UNIT DRV_MAKE_SMOOTH(1U)
#define DRV_DUTY_CYCLE_THRESHOLD_SMOOTH DRV_MAKE_SMOOTH(DRV_DUTY_CYCLE_THRESHOLD)

static int16_t s_i_term;

static uint16_t s_max_erps_predicted, s_max_erps_predicted_acc;
static float s_voltage_to_erps_factor;
static eDrv_State s_state = DRV_STATE_IDLE;
static bool s_is_auto_stabilization = false;
static bool s_is_cruise_control_idle = false;
static uint8_t s_cruise_control_duty_cycle;

void drv_init(sSettings const *settings, sSensors const *sensors, bool is_auto_stabilization, float voltage_to_erps_factor) {
    (void)settings;
    s_is_auto_stabilization = is_auto_stabilization;
    s_voltage_to_erps_factor = voltage_to_erps_factor;

    s_max_erps_predicted = (uint16_t)(sensors->battery_volts * s_voltage_to_erps_factor);
    s_max_erps_predicted_acc = s_max_erps_predicted << BLDC_VOLTAGE_TO_ERPS_CALC_FILTERING;
}

void drv_cycle(sSettings const *settings, sControls const *controls, sSensors const *sensors) {
    static uint16_t _s_timer_prev;
    static uint8_t _s_duty_cycle;
    static uint16_t _s_duty_cycle_smooth;

    uint16_t _timer_current;

    _timer_current = periph_get_timer();
    if ((_timer_current - _s_timer_prev) > TIMER_TP_MS(DRV_CYCLE_PERIOD_MS)) {
        uint16_t _current_erps;

        _s_timer_prev = _timer_current;

        _current_erps = bldc_get_erps();

        // If we are braking or battery voltage is not above critical, there is only one option - reset all and turn off BLDC!
        if (controls->is_braking || sensors->battery_volts < HUD_BATTERY_PACK_VOLTS_CRITITICAL) {
            s_state = DRV_STATE_IDLE;
            s_cruise_control_duty_cycle = 0;
        } else {
            // calc predicted duty cycle
            uint8_t _s_predicted_duty_cycle = (uint8_t)map16(_current_erps, 0, s_max_erps_predicted, 0, UINT8_MAX);

            uint8_t _max_duty_cycle = controls->gear_ratio;
            uint16_t _max_duty_cycle_smooth = DRV_MAKE_SMOOTH(_max_duty_cycle);
            uint8_t _start_duty_cycle = (uint8_t)(UINT8_MAX * (uint32_t)DRV_START_DUTY_CYCLE_VOLTS_K / sensors->battery_volts);

            bool _is_wanna_ebrake = controls->thumb_brake >= DRV_THUMB_TOGGLE_THRESHOLD;
            bool _is_wanna_accelerate = _max_duty_cycle && (controls->thumb_throttle >= DRV_THUMB_TOGGLE_THRESHOLD || controls->is_walking);

            if(_is_wanna_ebrake || _is_wanna_accelerate) {
                uint16_t _duty_cycle_diff_smooth;
                uint16_t _current_limit;
                uint16_t _erps_limit;

                _current_limit = ((uint16_t)DRV_MAX_CURRENT_025A * settings->max_current_scale) / (uint8_t)UINT8_MAX;
                // scale max current according to gear ratio if needed
                if(settings->scale_max_current_by_gear) _current_limit = map16(controls->gear_ratio, 0, UINT8_MAX, 0, _current_limit);

                // reset new duty cycle if starting to drive
                if(s_state == DRV_STATE_IDLE) _s_duty_cycle_smooth = DRV_MAKE_SMOOTH(_s_predicted_duty_cycle < _start_duty_cycle ? _start_duty_cycle : _s_predicted_duty_cycle);
                // if we are trying to accelerate or ebrake after cruise control idle, then turn off cruise
                if(s_is_cruise_control_idle) { s_is_cruise_control_idle = false; s_cruise_control_duty_cycle = 0; }

                // calc max speed we can drive
                _erps_limit = settings->motor_settings.max_erps;
                if (controls->is_walking) _erps_limit = (uint16_t)((DRV_WALK_SPEED_KMH * settings->motor_settings.pole_pairs / 3.6 / settings->motor_settings.wheel_circumference) + 0.5f);

                if (sensors->motor_current > _current_limit) {
                    // if current is above the limited, then decrease (for acceleration, or increase for ebraking) duty cycle by acceleration ramp
                    if(s_state == DRV_STATE_EBRAKING) { if (_max_duty_cycle_smooth > DRV_MAX_ACCELERATION_RAMP_SMOOTH && _s_duty_cycle_smooth < (_max_duty_cycle_smooth - DRV_MAX_ACCELERATION_RAMP_SMOOTH)) _s_duty_cycle_smooth += DRV_MAX_ACCELERATION_RAMP_SMOOTH; else _s_duty_cycle_smooth = _max_duty_cycle_smooth; }
                    else if(s_state == DRV_STATE_ACCELERATING) { if (_s_duty_cycle_smooth > DRV_MAX_ACCELERATION_RAMP_SMOOTH) _s_duty_cycle_smooth -= DRV_MAX_ACCELERATION_RAMP_SMOOTH; else _s_duty_cycle_smooth = 0; }
                }
                else if(_erps_limit != 0 && _current_erps > _erps_limit) {
                    // if speed is above the limited, then slowly decrease duty cycle
                    if(_s_duty_cycle_smooth >> 8) _s_duty_cycle_smooth -= DRV_ACCELERATION_SMOOTH_UNIT; else _s_duty_cycle_smooth = 0;
                }
                else {
                    // no critical issues at this point

                    if(s_cruise_control_duty_cycle) {
                        // if on cruise control
                        static uint8_t _s_prev_gear_ratio;

                        // if gear ratio changed, turn of cruise control
                        if (_s_prev_gear_ratio != controls->gear_ratio) {
                            s_cruise_control_duty_cycle = 0;
                            s_state = DRV_STATE_IDLE;
                        }
                        else {
                            // otherwise keep duty cycle as requested by cruise control
                            _s_duty_cycle_smooth = s_cruise_control_duty_cycle << 8;
                            // but not less than predicted duty cycle minus threshold
                            // TODO: do we need this?
                            // if (_s_predicted_duty_cycle > _s_duty_cycle_smooth && (_s_predicted_duty_cycle - _s_duty_cycle_smooth) >= DRV_THUMB_TOGGLE_THRESHOLD) _s_duty_cycle_smooth = _s_predicted_duty_cycle - (uint8_t)(DRV_THUMB_TOGGLE_THRESHOLD - 1);
                        }

                        _s_prev_gear_ratio = controls->gear_ratio;
                    }

                    // if not on cruise control, process ebraking or acceleration
                    if(!s_cruise_control_duty_cycle) {
                        if(_is_wanna_ebrake) {
                            // wa are free to decrease duty cycle for ebraking
                            if(s_state != DRV_STATE_EBRAKING) { if(_s_duty_cycle_smooth > DRV_EBRAKE_BOOST_SMOOTH) _s_duty_cycle_smooth -= DRV_EBRAKE_BOOST_SMOOTH; else _s_duty_cycle_smooth = 0; }
                            _duty_cycle_diff_smooth = (uint16_t)((uint32_t)DRV_MAX_ACCELERATION_RAMP_SMOOTH * controls->thumb_brake / UINT8_MAX);
                            if(_s_duty_cycle_smooth > _duty_cycle_diff_smooth) _s_duty_cycle_smooth -= _duty_cycle_diff_smooth; else _s_duty_cycle_smooth = 0;
                            s_state = DRV_STATE_EBRAKING;
                        }
                        else if(_is_wanna_accelerate) {
                            // wa are free to increase duty cycle for acceleration
                            do {
                                uint16_t _wanted_duty_cycle_smooth;
                                if(s_state != DRV_STATE_ACCELERATING) {
                                    _wanted_duty_cycle_smooth = _s_duty_cycle_smooth < (UINT16_MAX - DRV_ACCELERATION_BOOST_SMOOTH) ? _s_duty_cycle_smooth + DRV_ACCELERATION_BOOST_SMOOTH : (uint16_t)UINT16_MAX;
                                    if(_wanted_duty_cycle_smooth > _max_duty_cycle_smooth) break; else _s_duty_cycle_smooth = _wanted_duty_cycle_smooth;
                                }
                                _duty_cycle_diff_smooth = controls->is_walking ? (uint16_t)DRV_ACCELERATION_SMOOTH_UNIT : (uint16_t)((uint32_t)DRV_MAX_ACCELERATION_RAMP_SMOOTH * controls->thumb_throttle / UINT8_MAX);
                                _wanted_duty_cycle_smooth = _s_duty_cycle_smooth < (UINT16_MAX - _duty_cycle_diff_smooth) ? _s_duty_cycle_smooth + _duty_cycle_diff_smooth : (uint16_t)UINT16_MAX;
                                if(_wanted_duty_cycle_smooth > _max_duty_cycle_smooth) {
                                    if(_wanted_duty_cycle_smooth > DRV_DUTY_CYCLE_THRESHOLD_SMOOTH && (_wanted_duty_cycle_smooth - DRV_DUTY_CYCLE_THRESHOLD_SMOOTH) > _max_duty_cycle_smooth) s_state = DRV_STATE_IDLE;
                                    break;
                                }
                                else _s_duty_cycle_smooth = _wanted_duty_cycle_smooth;
                                s_state = DRV_STATE_ACCELERATING;
                            } while (false);
                        }
                    }
                }
            }
            else {
                // if driving during cruise control without accleration or ebraking, note it
                if(s_state != DRV_STATE_IDLE && s_cruise_control_duty_cycle) s_is_cruise_control_idle = true;
                else {
                    // otherwise there is nothing to do
                    s_state = DRV_STATE_IDLE;
                    s_cruise_control_duty_cycle = 0;
                }
            }

            _s_duty_cycle = (uint8_t)(_s_duty_cycle_smooth >> 8);

            // if we are about to drive ...
            if(s_state != DRV_STATE_IDLE) {
                // ... and cruise control is requested, set it up
                if (s_cruise_control_duty_cycle == 0 && controls->is_cruise_control) {
                    s_cruise_control_duty_cycle = _s_duty_cycle;
                    s_is_cruise_control_idle = false;
                }
            }
        }

        // order BLDC
        if(s_state == DRV_STATE_IDLE || !bldc_can_drive()) {
            bldc_output(false);
            bldc_set_pwm_duty_cycle(0);
            s_state = DRV_STATE_IDLE;
            s_cruise_control_duty_cycle = 0;
        }
        else {
            bldc_set_pwm_duty_cycle(_s_duty_cycle);
            bldc_output(true);
        }

        // stabilize predication of max ERPS
        if (s_is_auto_stabilization) {
            // if driving, and duty cycle among with speed is large enough to predict
            if (s_state != DRV_STATE_IDLE && (_current_erps >= DRV_MIN_ERPS_TO_PREDICT && _s_duty_cycle >= DRV_MIN_DUTY_CYCLE_TO_PREDICT)) {
                uint16_t _new_erps_predicted = (_current_erps << 8) / _s_duty_cycle;
                s_max_erps_predicted_acc = s_max_erps_predicted_acc - s_max_erps_predicted + _new_erps_predicted;
                s_max_erps_predicted = s_max_erps_predicted_acc >> BLDC_VOLTAGE_TO_ERPS_CALC_FILTERING;
            }
        } else {
            uint16_t _new_erps_predicted = (uint16_t)(sensors->battery_volts * s_voltage_to_erps_factor);
            s_max_erps_predicted_acc = s_max_erps_predicted_acc - s_max_erps_predicted + _new_erps_predicted;
            s_max_erps_predicted = s_max_erps_predicted_acc >> BLDC_VOLTAGE_TO_ERPS_CALC_FILTERING;
        }

        DBGF("%3u, %3u | %3u, %3u === ",
             periph_get_adc_thumb_throttle(),
             periph_get_adc_thumb_break(),
             controls->thumb_throttle,
             controls->thumb_brake);

        DBGF("%3u, %3u, %3u, %3u, %5u, %3u === %3u, %3u, %3u, %3u, %3u, ",
             _current_erps,
             s_max_erps_predicted,
             _s_prev_duty_cycle,
             _s_duty_cycle,
             _s_duty_cycle_smooth,
             _s_predicted_duty_cycle,
             periph_get_adc_phase_b_current(),
             periph_get_adc_motor_current(),
             periph_get_adc_motor_current_filtered(),
             periph_get_adc_battery_voltage(),
             periph_get_adc_motor_temperature());

        bldc_debug();
        DBG("\n");
    }
}

eDrv_State drv_get_state() { return s_state; }
bool drv_is_cruise_control() { return s_cruise_control_duty_cycle != 0; }

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
