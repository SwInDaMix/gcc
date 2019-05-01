#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "periph.h"
#include "settings.h"
#include "controls.h"
#include "sensors.h"
#include "bldc.h"
#include "drive.h"
#include "hud.h"
#include "ktlcd3.h"
#include "utils.h"

int main(void) {
    sSettings const *_settings;
    sControls const *_controls;
    sSensors const *_sensors;

    periph_init();
    settings_init();
    controls_init();
    sensors_init();

    _settings = settings_get_current();
    _sensors = sensors_get_current();

    bldc_init(BLDC_IS_PURE_SINE, BLDC_IS_REVERSE, BLDC_IS_INVERT_HALLS, BLDC_CORRECTION_ANGLE);
    drv_init(_settings, _sensors, false, BLDC_VOLTAGE_TO_ERPS_FACTOR);
    hud_init(_settings, _sensors);

    periph_wdt_enable();

    while (true) {
//        uint8_t _tt, _dd;

        _settings = settings_get_current();
        _controls = controls_get_current();
        _sensors = sensors_get_current();

        drv_cycle(_settings, _controls, _sensors);
        hud_cycle(_settings, _controls, _sensors);

//        _tt = periph_get_adc_thumb_throttle();
//        //_tt = ADC_THUMB_MAX_VALUE;
//        if (_tt < ADC_THUMB_MIN_VALUE) {
//            bldc_output(false);
//            periph_set_taillight_pwm_duty_cycle(0);
//        }
//        else {
//            _dd = map8(_tt, ADC_THUMB_MIN_VALUE, ADC_THUMB_MAX_VALUE, 0, 255);
//            periph_set_taillight_pwm_duty_cycle(_dd);
//            bldc_set_pwm_duty_cycle(_dd);
//            bldc_output(true);
//        }

//        if(periph_get_is_braking()) {
//            bldc_output(true);
//            bldc_set_pwm_duty_cycle(20);
//        }
//        else {
//            bldc_output(false);
//        }

//        periph_set_headlight(periph_get_is_braking());
    }
}
