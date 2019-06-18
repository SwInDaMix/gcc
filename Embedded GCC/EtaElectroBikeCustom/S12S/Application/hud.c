#include <math.h>
#include "hud.h"
#include "periph.h"
#include "bldc.h"
#include "drive.h"
#include "ktlcd3.h"
#include "utils.h"

void hud_init(sSettings const *settings, sSensors const *sensors) {
    (void)settings;
    (void)sensors;

    periph_uart_set_on_received_callback((periph_uart_on_received_callback_t)ktlcd3_eat_byte);
}

void hud_cycle(sSettings const *settings, sControls const *controls, sSensors const *sensors) {
    static uint16_t _s_timer_prev;

    uint16_t _timer_current;

    // check headligh feedback

    // Control head and tail lights
    uint8_t _taillight_pwm = 0;
    if (controls->is_headlight_on) {
        _taillight_pwm = HUD_TAILLIGHT_MARKER_PWM;
        periph_set_headlight(true);
    } else periph_set_headlight(false);

    _timer_current = periph_get_timer();
    if ((_timer_current - _s_timer_prev) > TIMER_TP_MS(HUD_CYCLE_PERIOD_MS)) {
        eKTLCD3_Battery _battery_status;
        eKTLCD3_Error _error = KTLCD3_Error_None;
        eKTLCD3_MovingStatus _moving_status = KTLCD3_MovingStatus_None;
        uint16_t _wheel_rotation_period;
        uint16_t _motor_current_filtered;
        uint16_t _erps;
        uint8_t _adc_thumb_throttle, _adc_thumb_brake;
        eDrv_State _drv_state;

        _s_timer_prev = _timer_current;

        _erps = bldc_get_erps();
        _adc_thumb_throttle = periph_get_adc_thumb_throttle();
        _adc_thumb_brake = periph_get_adc_thumb_break();

        // battery status
        if (sensors->battery_volts > HUD_BATTERY_PACK_VOLTS_100) _battery_status = KTLCD3_Battery_Charging;
        else if (sensors->battery_volts > HUD_BATTERY_PACK_VOLTS_80) _battery_status = KTLCD3_Battery_Full;
        else if (sensors->battery_volts > HUD_BATTERY_PACK_VOLTS_60) _battery_status = KTLCD3_Battery_3Bars;
        else if (sensors->battery_volts > HUD_BATTERY_PACK_VOLTS_40) _battery_status = KTLCD3_Battery_2Bars;
        else if (sensors->battery_volts > HUD_BATTERY_PACK_VOLTS_20) _battery_status = KTLCD3_Battery_1Bar;
        else if (sensors->battery_volts > HUD_BATTERY_PACK_VOLTS_0) _battery_status = KTLCD3_Battery_Empty;
        else _battery_status = KTLCD3_Battery_Flashing;

        // signal an error if any
        if (sensors->battery_volts <= HUD_BATTERY_PACK_VOLTS_CRITITICAL) _error = KTLCD3_Error_91_BatteryUnderVoltage;
        else if (bldc_is_error()) _error = KTLCD3_Error_03_Hall;
        else if (_adc_thumb_brake < HUD_ADC_THUMB_CRITICAL_MIN_VALUE || _adc_thumb_brake > HUD_ADC_THUMB_CRITICAL_MAX_VALUE) _error = KTLCD3_Error_02_Brake;
        else if (_adc_thumb_throttle < HUD_ADC_THUMB_CRITICAL_MIN_VALUE || _adc_thumb_throttle > HUD_ADC_THUMB_CRITICAL_MAX_VALUE) _error = KTLCD3_Error_01_Throttle;

        // display moving status
        _drv_state = drv_get_state();
        if (_drv_state == DRV_STATE_ACCELERATING) _moving_status |= KTLCD3_MovingStatus_Throttling;
        if (_drv_state == DRV_STATE_EBRAKING || controls->is_braking) {
            _moving_status |= KTLCD3_MovingStatus_Braking;
            _taillight_pwm = UINT8_MAX;
        }
        if (drv_is_cruise_control()) _moving_status |= KTLCD3_MovingStatus_CruiseControl;
        if (controls->is_walking) _moving_status |= KTLCD3_MovingStatus_Assist;

        // calc speed by wheel rotation period
        _wheel_rotation_period = 65535;
        if (_erps > 0) _wheel_rotation_period = (uint16_t)(1000LL * settings->motor_settings.pole_pairs / _erps);

        // display motor power by motor current
        _motor_current_filtered = sensors->motor_current_filtered;
        if (_motor_current_filtered < HUD_MOTOR_CURRENT_ZERO_TH) _motor_current_filtered = 0;

        periph_set_taillight_pwm_duty_cycle(_taillight_pwm);

#ifndef DEBUG
        ktlcd3_update(_battery_status, _error, _moving_status, _wheel_rotation_period, _motor_current_filtered, sensors->motor_temperature);
#endif
    }
}