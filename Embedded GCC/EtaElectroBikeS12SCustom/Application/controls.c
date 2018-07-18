#include "controls.h"
#include "ktlcd3.h"
#include "utils.h"

static sControls s_controls;

void controls_init() { }

sControls const *controls_get_current() {
    bool _was_ever_updated = ktlcd3_was_ever_updated();
    eKTLCD3_AssistLevel _assist_level = _was_ever_updated ? ktlcd3_get_assist_level() : KTLCD3_AssistLevel5;

    s_controls.is_headlight_on = _was_ever_updated ? ktlcd3_get_is_headlight_on() : false;
    s_controls.is_headlight_feedback = periph_get_is_headlight_on();
    s_controls.is_braking = periph_get_is_braking();
    s_controls.thumb_throttle = map8(periph_get_adc_thumb_throttle(), ADC_THUMB_MIN_VALUE, ADC_THUMB_MAX_VALUE, 0, UINT8_MAX);
    s_controls.thumb_brake = map8(periph_get_adc_thumb_break(), ADC_THUMB_MIN_VALUE, ADC_THUMB_MAX_VALUE, 0, UINT8_MAX);
    s_controls.gear_ratio = _was_ever_updated ? ktlcd3_get_assist_level_duty_cycle(_assist_level) : (uint8_t)UINT8_MAX;
    s_controls.is_walking = _assist_level == KTLCD3_AssistLevelWalk;
    s_controls.is_cruise_control = _was_ever_updated ? ktlcd3_get_cruise_control() : false;

    return &s_controls;
}
