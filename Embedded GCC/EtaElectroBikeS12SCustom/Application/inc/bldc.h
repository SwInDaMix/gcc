#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "interrupts.h"

#define BLDC_HALL_CORRECTNESS 6
#define BLDC_PWM_FULL_DUTY_BITS 10

void bldc_init(void);
void bldc_output(bool is_enabled);
void bldc_set_pwm_duty_cycle(uint8_t duty_cycle);
uint16_t bldc_get_erps();
void bldc_debug();
