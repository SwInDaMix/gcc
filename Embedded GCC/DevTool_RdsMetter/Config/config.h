/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config.h
 ***********************************************************************/

#pragma once

#include "Types.h"
#include <avr/sleep.h>

#define F_CPU 8000000UL
#define V_CPU 5000

#define YIELD() {  }

#define STDIO_MICROSIZE
#define STDIO_ALLOWALIGNMENT
//#define STDIO_ALLOWFILLCHAR
#define STDIO_ALLOWFRACTIONS
#define STDIO_ALLOWLONG
//#define STDIO_ALLOWFLOAT
#define STDIO_ALLOWSIGNPADS

#define USARTINIT_CONFIG (USARTINIT_BITS_8 | USARTINIT_PARITY_NONE | USARTINIT_STOPBIT_1)
#define USARTINIT_BAUDRATE 125000
#define USARTINIT_WRITE_DATA(data) { USARTINIT_UDR = data; }
#define USARTINIT_ENABLE_TX_INTERRUPT() { USARTINIT_UCSRB |= (1 << USARTINIT_UDRIE); }
//void usart_push_char_from_isr(char);
bool usart_pop_char_from_isr();
//#define USARTINIT_RECEIVER(data) ( usart_push_char_from_isr(data) )
#define USARTINIT_TRANSMITTER() ( usart_pop_char_from_isr() )

#define USART_DBG
//#define USART_RECEIVER
#define USART_TRANSMITTER
//#define USART_RX_BUFFER_SIZESHIFT 7
#define USART_TX_BUFFER_SIZESHIFT 8
#define USART_YIELD() YIELD()

//#define FRAMES_RECEIVER
//#define FRAMES_TRANSMITTER
//#define FRAMES_SIGNATURE_SIZESHIFT 3
//#define FRAMES_SIGNATURE 0x72, 0xC5, 0xD9, 0xAC, 0x86, 0xE3, 0xBA, 0x9E
//#define FRAMES_STUBBYTE 0xAA
//#define FRAMES_RX_BUFFER_SIZESHIFT 7
//#define FRAMES_TX_BUFFER_SIZESHIFT 7
//#define FRAMES_YIELD() YIELD()

#define ALCD_DATAPORT PORTB
#define ALCD_HALFDUPLEX 0
#define ALCD_CTL_RS_PORT PORTD
#define ALCD_CTL_RS_PIN 5
#define ALCD_CTL_RW_PORT PORTD
#define ALCD_CTL_RW_PIN 6
#define ALCD_CTL_E_PORT PORTD
#define ALCD_CTL_E_PIN 7
#define ALCD_DELAY() __asm(	"nop\n\t" "nop\n\t");

#define BTN_Function_PINPORT PIND
#define BTN_Function_PIN 2

//#define ADC_RECEIVER
//#define ADC_RX_BUFFER_SIZESHIFT 0
//#define ADC_HIGHBYTE_ONLY
#define ADC_YIELD() YIELD()

#define ADC_Channel_DSCurrent 6
#define ADC_Channel_DSDropVoltage 7

#define PORT_GateDrive_PORT PORTD
#define PORT_GateDrive_PIN 3

#define PORT_RelaySwitches_PORT PORTC
#define PORT_RelaySwitches_PINOffset 0
#define PORT_RelaySwitches_PINs 6

#define MC_ADC_ProbesShift 4

#define MC_MV_PER_AMP 760
#define MC_MV_FACTOR (110000000UL / MC_MV_PER_AMP)

#define MC_ADC_DSCurrent_2_Current_x10000(value) ((((uint32_t)value) * MC_MV_FACTOR) >> 10)
#define MC_ADC_DSDropVoltage_2_Voltage_x10000(value) ((((uint24_t)value) * 11000) >> 10)

#define MC_RelaySwitchingTime 100
#define MC_GateSwitchingTime 10
