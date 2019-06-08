#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "interrupts.h"

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

#define TIMER_MS2TT(msecs) (msecs)

#define PERIPH_ADC_STABILIZATION_PERIOD_MS 500U

typedef enum {
    PeriphButton__None = 0,
    PeriphButton_Up = (1 << 0),
    PeriphButton_OnOff = (1 << 1),
    PeriphButton_Down = (1 << 2),
    PeriphButton__UpDown = PeriphButton_Up | PeriphButton_Down,
    PeriphButton__All = PeriphButton_Up | PeriphButton_OnOff | PeriphButton_Down
} ePeriphButton;

typedef void (*periph_uart_on_received_callback_t)(uint8_t byte);
typedef uint8_t (*periph_uart_on_transmit_callback_t)();

void ADC1_IRQHandler() __interrupt(ADC1_IRQHANDLER);
void UART2_TXIRQHandler() __interrupt(UART2_TXIRQHANDLER);
void UART2_RXIRQHandler() __interrupt(UART2_RXIRQHANDLER);
void TIM3_UPD_OVF_BRK_IRQHandler() __interrupt(TIM3_UPD_OVF_BRK_IRQHANDLER);

void periph_init();

void periph_atom_start();
void periph_atom_end();

uint16_t periph_get_timer();
uint16_t periph_get_mseconds();
uint32_t periph_get_seconds();

uint16_t periph_get_adc_voltage();

ePeriphButton periph_get_buttons();

void periph_set_onoff_power(bool onoff_power);
void periph_set_power_up(bool power_up);

void periph_set_backlight_pwm_duty_cycle(uint8_t duty_cycle);

void periph_set_ht1622_vdd(bool vdd);
void periph_set_ht1622_cs(bool cs);
void periph_set_ht1622_write(bool write);
void periph_set_ht1622_read(bool read);
void periph_set_ht1622_data(bool data);

void periph_uart_set_on_received_callback(periph_uart_on_received_callback_t callback);
void periph_uart_set_on_transmit_callback(periph_uart_on_transmit_callback_t callback);
void periph_uart_putbyte(uint8_t byte);

void periph_eeprom_read(void *dst, uint16_t offset, size_t size);
void periph_eeprom_write(void const *src, uint16_t offset, size_t size);

void periph_wdt_reset();

void periph_shutdown();
