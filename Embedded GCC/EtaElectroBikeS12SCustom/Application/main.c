/*
 * BMSBattery S series motor controllers firmware
 *
 * Copyright (C) Casainho, 2017.
 *
 * Released under the GPL License, Version 3
 */

#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "periph.h"
#include "main.h"
#include "utils.h"
#include "bldc.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(void) {
    uint8_t _tt, _dd;

    periph_init();
    bldc_init();

    while (true) {
        DBGF("%3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, ",
             map8(periph_get_adc_thumb_throttle(), ADC_THUMB_MIN_VALUE, ADC_THUMB_MAX_VALUE, 0, 255),
             periph_get_adc_phase_b_current(),
             periph_get_adc_motor_current(),
             periph_get_adc_motor_current_filtered(),
             periph_get_adc_battery_voltage(),
             periph_get_adc_motor_temperature(),
             periph_get_adc_thumb_throttle(),
             periph_get_adc_thumb_break());

        bldc_debug();

        DBG("\n");

        _tt = periph_get_adc_thumb_throttle();
        if(_tt < ADC_THUMB_MIN_VALUE) {
            bldc_output(false);
            periph_set_taillight_pwm_duty_cycle(0);
        }
        else {
            bldc_set_pwm_duty_cycle(255);
            bldc_output(true);
            _dd = map8(_tt, ADC_THUMB_MIN_VALUE, ADC_THUMB_MAX_VALUE, 0, 255);
            periph_set_taillight_pwm_duty_cycle(_dd);
            bldc_set_pwm_duty_cycle(_dd);
            bldc_output(true);
        }

        periph_set_headlight(periph_get_is_braking());
    }
}
#pragma clang diagnostic pop


