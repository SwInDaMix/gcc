#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "interrupts.h"

#define BLDC_HALL_CORRECTNESS 6U
#define BLDC_PWM_FULL_DUTY_BITS 10U

#define BLDC_ANGLE(a) ((((a) * 64 / 45) + 1) >> 1)
#define BLDC_ANGLE_0 BLDC_ANGLE(0U)
#define BLDC_ANGLE_60 BLDC_ANGLE(60U)
#define BLDC_ANGLE_120 BLDC_ANGLE(120U)
#define BLDC_ANGLE_180 BLDC_ANGLE(180U)
#define BLDC_ANGLE_240 BLDC_ANGLE(240U)
#define BLDC_ANGLE_300 BLDC_ANGLE(300U)

void bldc_init(bool is_pure_sine, bool is_reverse, bool is_invert_halls, int8_t correction_angle);
void bldc_output(bool is_enabled);
void bldc_set_pwm_duty_cycle(uint8_t duty_cycle);

uint16_t bldc_get_erps();
bool bldc_is_rotating();
bool bldc_is_block_commutation();
bool bldc_can_drive();
bool bldc_is_error();
void bldc_debug();
