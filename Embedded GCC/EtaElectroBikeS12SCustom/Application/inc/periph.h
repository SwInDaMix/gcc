#pragma once

#include "config.h"
#include "interrupts.h"
#include "stm8s_gpio.h"

/*
 * Free Port
 *
 * A1=X6, A2=X7, A3, A5, A6
 * D3, D4
 * C6
 * E5
 * G0, G1
 *
 */

#define PHASE_B_CURRENT__PINS               GPIO_PIN_5
#define PHASE_B_CURRENT__PORT               GPIOB
#define PHASE_B_CURRENT__MODE               GPIO_MODE_IN_PU_NO_IT
#define PHASE_B_CURRENT__CH                 ADC1_CHANNEL_5

#define MOTOR_CURRENT__PINS                 GPIO_PIN_6
#define MOTOR_CURRENT__PORT                 GPIOB
#define MOTOR_CURRENT__MODE                 GPIO_MODE_IN_PU_NO_IT
#define MOTOR_CURRENT__CH                   ADC1_CHANNEL_6

#define MOTOR_FILTERED_CURRENT__PINS        GPIO_PIN_7
#define MOTOR_FILTERED_CURRENT__PORT        GPIOE
#define MOTOR_FILTERED_CURRENT__MODE        GPIO_MODE_IN_PU_NO_IT
#define MOTOR_FILTERED_CURRENT__CH          ADC1_CHANNEL_8

#define BATTERY_VOLTAGE__PINS               GPIO_PIN_6
#define BATTERY_VOLTAGE__PORT               GPIOE
#define BATTERY_VOLTAGE__MODE               GPIO_MODE_IN_PU_NO_IT
#define BATTERY_VOLTAGE__CH                 ADC1_CHANNEL_9

#define MOTOR_TEMPERATURE__PINS             GPIO_PIN_7
#define MOTOR_TEMPERATURE__PORT             GPIOB
#define MOTOR_TEMPERATURE__MODE             GPIO_MODE_IN_PU_NO_IT
#define MOTOR_TEMPERATURE__CH               ADC1_CHANNEL_7

#define THUMB_THROTTLE__PINS                GPIO_PIN_4
#define THUMB_THROTTLE__PORT                GPIOB
#define THUMB_THROTTLE__MODE                GPIO_MODE_IN_PU_NO_IT
#define THUMB_THROTTLE__CH                  ADC1_CHANNEL_4

#define THUMB_BRAKE__PINS                   GPIO_PIN_3
#define THUMB_BRAKE__PORT                   GPIOB
#define THUMB_BRAKE__MODE                   GPIO_MODE_IN_PU_NO_IT
#define THUMB_BRAKE__CH                     ADC1_CHANNEL_3

#define HALL_SENSORS__PINS                  (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2)
#define HALL_SENSORS__PORT                  GPIOE
#define HALL_SENSORS__MODE                  GPIO_MODE_IN_PU_NO_IT

#define MOTOR_OVERCURRENT__PINS             GPIO_PIN_7
#define MOTOR_OVERCURRENT__PORT             GPIOD
#define MOTOR_OVERCURRENT__MODE             GPIO_MODE_IN_PU_IT
#define MOTOR_OVERCURRENT__ITPORT           EXTI_PORT_GPIOD
#define MOTOR_OVERCURRENT__SENS             EXTI_SENSITIVITY_FALL_ONLY

#define BRAKE__PINS                         GPIO_PIN_4
#define BRAKE__PORT                         GPIOA
#define BRAKE__MODE                         GPIO_MODE_IN_PU_IT
#define BRAKE__ITPORT                       EXTI_PORT_GPIOA
#define BRAKE__SENS                         EXTI_SENSITIVITY_RISE_FALL

#define PAS__PINS                           GPIO_PIN_0
#define PAS__PORT                           GPIOD
#define PAS__MODE                           GPIO_MODE_IN_PU_IT
#define PAS__ITPORT                         EXTI_PORT_GPIOD
#define PAS__SENS                           EXTI_SENSITIVITY_FALL_ONLY

#define SPEED__PINS                         GPIO_PIN_5
#define SPEED__PORT                         GPIOC
#define SPEED__MODE                         GPIO_MODE_IN_PU_IT
#define SPEED__ITPORT                       EXTI_PORT_GPIOC
#define SPEED__SENS                         EXTI_SENSITIVITY_FALL_ONLY

#define HEAD_LIGHT__PINS                    GPIO_PIN_4
#define HEAD_LIGHT__PORT                    GPIOC
#define HEAD_LIGHT__MODE                    GPIO_MODE_OUT_PP_LOW_SLOW

#define HEAD_LIGHT_FEEDBACK__PINS           GPIO_PIN_7
#define HEAD_LIGHT_FEEDBACK__PORT           GPIOC
#define HEAD_LIGHT_FEEDBACK__MODE           GPIO_MODE_IN_PU_NO_IT

#define TAIL_LIGHT__PINS                    GPIO_PIN_2
#define TAIL_LIGHT__PORT                    GPIOD
#define TAIL_LIGHT__MODE                    GPIO_MODE_IN_PU_NO_IT

#define TIMER_TP_MS(msecs) ((uint16_t)(FCPU * (uint64_t)msecs / 8192000L))

#define PERIPH_ADC_STABLE 500
#define PERIPH_ADC_STABLE_READ 2000

typedef void (*periph_bldc_pwm_overflow_callback_t)();
typedef void (*periph_overcurrent_callback_t)();
typedef void (*periph_pas_callback_t)();
typedef void (*periph_speed_callback_t)();
typedef void (*periph_brake_callback_t)(bool is_braking);
typedef void (*periph_uart_on_received_callback_t)(uint8_t byte);

void TIM1_UPD_OVF_TRG_BRK_IRQHandler() __interrupt(TIM1_UPD_OVF_TRG_BRK_IRQHANDLER);
void ADC1_IRQHandler() __interrupt(ADC1_IRQHANDLER);
void EXTI_PORTA_IRQHandler(void) __interrupt(EXTI_PORTA_IRQHANDLER);
void EXTI_PORTC_IRQHandler(void) __interrupt(EXTI_PORTC_IRQHANDLER);
void EXTI_PORTD_IRQHandler(void) __interrupt(EXTI_PORTD_IRQHANDLER);
void UART2_IRQHandler(void) __interrupt(UART2_IRQHANDLER);

void periph_init();

void periph_set_bldc_pwm_overflow_callback(periph_bldc_pwm_overflow_callback_t callback);
uint8_t periph_get_bldc_hall_sensors();
void periph_set_bldc_pwm_duty_cucles(uint16_t dc_a, uint16_t dc_b, uint16_t dc_c);
void periph_set_bldc_pwm_outputs(bool is_enabled);

uint16_t periph_get_timer();

uint8_t periph_get_adc_counter();
uint8_t periph_get_adc_phase_b_current();
uint8_t periph_get_adc_motor_current();
uint8_t periph_get_adc_motor_current_filtered();
uint8_t periph_get_adc_battery_voltage();
uint8_t periph_get_adc_motor_temperature();
uint8_t periph_get_adc_thumb_throttle();
uint8_t periph_get_adc_thumb_break();

void periph_set_overcurrent_callback(periph_overcurrent_callback_t callback);
void periph_set_pas_callback(periph_pas_callback_t callback);
void periph_set_speed_callback(periph_speed_callback_t callback);
void periph_set_brake_callback(periph_brake_callback_t callback);

bool periph_get_is_braking();
bool periph_get_is_headlight_on();

void periph_set_headlight(bool is_enabled);
void periph_set_taillight_pwm_duty_cycle(uint8_t duty_cycle);

void periph_uart_set_on_received_callback(periph_uart_on_received_callback_t callback);
void periph_uart_putbyte(uint8_t byte);
