#pragma once

#include "config.h"
#include "interrupts.h"
#include "stm8s_gpio.h"

/*
 * PD0          | HT1622 /CS
 * PA4          | HT1622 /READ
 * PB7          | HT1622 /WRITE
 * PC5          | HT1622 DATA
 *
 * PA2          | Enable power for the controller (switch a very small mosfet to GND that goes to A1013 emitter pin)
 * PC4          | enables LCD backlight
 *
 * PB2          | /button down
 * PB1          | /button up
 * PB0          | /button ON/OFF

 *
 *
 * PE6 (AIN9)   | connects to header J6 (this header is not connected and has 2 pads: GND and  PE6)
 * PE7 (AIN8)   | battery voltage
 *
 * PE3          | power up pin. used to increase power given to the controller in case of lower battery voltage
 * PG1          | input digital from a mosfet output, enabled buy the LCD Power ON button
 *
 * PB4          | connects to HT1622 VDD pin 8
 */

#define BUTTON_UP__PORT                     GPIOB
#define BUTTON_UP__PIN                      GPIO_PIN_1
#define BUTTON_UP__MODE                     GPIO_MODE_IN_PU_NO_IT

#define BUTTON_ONOFF__PORT                  GPIOG
#define BUTTON_ONOFF__PIN                   GPIO_PIN_1
#define BUTTON_ONOFF__MODE                  GPIO_MODE_IN_FL_NO_IT

#define BUTTON_DOWN__PORT                   GPIOB
#define BUTTON_DOWN__PIN                    GPIO_PIN_2
#define BUTTON_DOWN__MODE                   GPIO_MODE_IN_PU_NO_IT

#define ENABLE_BACKLIGHT__PORT              GPIOC
#define ENABLE_BACKLIGHT__PIN               GPIO_PIN_4
#define ENABLE_BACKLIGHT__MODE              GPIO_MODE_OUT_PP_LOW_FAST

#define ONOFF_POWER__PORT                   GPIOA
#define ONOFF_POWER__PIN                    GPIO_PIN_2
#define ONOFF_POWER__MODE                   GPIO_MODE_OUT_PP_LOW_FAST

#define POWER_UP__PORT                      GPIOE
#define POWER_UP__PIN                       GPIO_PIN_3
#define POWER_UP__MODE                      GPIO_MODE_OUT_PP_LOW_FAST

#define HT1622_CS__PORT                     GPIOD
#define HT1622_CS__PIN                      GPIO_PIN_0
#define HT1622_CS__MODE                     GPIO_MODE_OUT_PP_LOW_FAST

#define HT1622_WRITE__PORT                  GPIOB
#define HT1622_WRITE__PIN                   GPIO_PIN_7
#define HT1622_WRITE__MODE                  GPIO_MODE_OUT_PP_LOW_FAST

#define HT1622_READ__PORT                   GPIOA
#define HT1622_READ__PIN                    GPIO_PIN_4
#define HT1622_READ__MODE                   GPIO_MODE_OUT_PP_LOW_FAST

#define HT1622_DATA__PORT                   GPIOC
#define HT1622_DATA__PIN                    GPIO_PIN_5
#define HT1622_DATA__MODE                   GPIO_MODE_OUT_PP_LOW_FAST

#define HT1622_VDD__PORT                    GPIOB
#define HT1622_VDD__PIN                     GPIO_PIN_4
#define HT1622_VDD__MODE                    GPIO_MODE_OUT_PP_LOW_FAST

#define BATTERY_VOLTAGE__PORT               GPIOE
#define BATTERY_VOLTAGE__PIN                GPIO_PIN_7
#define BATTERY_VOLTAGE__MODE               GPIO_MODE_IN_FL_NO_IT
#define BATTERY_VOLTAGE__CH                 ADC1_CHANNEL_8

#define TIMER_TP_MS(msecs) ((uint16_t)((FCPU / 1000) * (uint64_t)msecs / 8192L))

#define PERIPH_ADC_STABILIZATION_PERIOD_MS 500U

typedef void (*periph_lcd_timer_overflow_callback_t)();
typedef void (*periph_uart_on_received_callback_t)(uint8_t byte);

void ADC1_IRQHandler() __interrupt(ADC1_IRQHANDLER);
void UART2_IRQHandler() __interrupt(UART2_IRQHANDLER);
void TIM3_UPD_OVF_BRK_IRQHandler() __interrupt(TIM3_UPD_OVF_BRK_IRQHANDLER);

void periph_init();

uint16_t periph_get_timer();

uint16_t periph_get_adc_counter();
uint16_t periph_get_adc_battery_voltage();

void periph_set_enable_backlight(bool enable_backlight);
void periph_set_onoff_power(bool onoff_power);
void periph_set_power_up(bool power_up);

void periph_set_ht1622_vdd(bool vdd);
void periph_set_ht1622_cs(bool cs);
void periph_set_ht1622_write(bool write);
void periph_set_ht1622_read(bool read);
void periph_set_ht1622_data(bool data);

void periph_uart_set_on_received_callback(periph_uart_on_received_callback_t callback);
void periph_uart_putbyte(uint8_t byte);

void periph_wdt_enable();
void periph_wdt_reset();
