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
void frames8b_push_data_from_isr(uint8_t);
bool frames8b_pop_data_from_isr();
#define USARTINIT_RECEIVER(data) ( frames8b_push_data_from_isr(data) )
#define USARTINIT_TRANSMITTER() ( frames8b_pop_data_from_isr() )

//#define USART_DBG
//#define USART_RECEIVER
//#define USART_TRANSMITTER
//#define USART_RX_BUFFER_SIZESHIFT 8
//#define USART_TX_BUFFER_SIZESHIFT 8
//#define USART_YIELD() YIELD()

#define FRAME_ADDRESS 0x00
#define FRAMES_DBG
#define FRAMES_8B
#define FRAMES_RECEIVER
#define FRAMES_TRANSMITTER
#define FRAMES_RX_BUFFER_SIZESHIFT 8
#define FRAMES_TX_BUFFER_SIZESHIFT 8
#define FRAMES_YIELD() YIELD()

#define ALCD_DATAPORT PORTC
#define ALCD_HALFDUPLEX 2
#define ALCD_CTL_RS_PORT PORTC
#define ALCD_CTL_RS_PIN 0
#define ALCD_CTL_RW_PORT PORTC
#define ALCD_CTL_RW_PIN 1
#define ALCD_CTL_E_PORT PORTD
#define ALCD_CTL_E_PIN 2
#define ALCD_DELAY() __asm("nop\n\t" "nop\n\t" "nop\n\t");

#define BTN_Escape_PINPORT PIND
#define BTN_Escape_PIN 5
#define BTN_Prev_PINPORT PIND
#define BTN_Prev_PIN 6
#define BTN_Next_PINPORT PIND
#define BTN_Next_PIN 7
#define BTN_Enter_PINPORT PINB
#define BTN_Enter_PIN 0

#define SW_Charge_PORT PORTB
#define SW_Charge_PIN 1
#define SW_Discharge_PORT PORTB
#define SW_Discharge_PIN 2
#define SW_VoltageDiv_PORT PORTD
#define SW_VoltageDiv_PIN 3

//#define ADC_RECEIVER
//#define ADC_RX_BUFFER_SIZESHIFT 0
//#define ADC_HIGHBYTE_ONLY
#define ADC_AREF 5005
#define ADC_BITS 10
#define ADC_YIELD() YIELD()
#define ADC_MV_PER_AMP 185
#define ADC_CURRENT_CH 6
#define ADC_VOLTAGE_CH 7

#define SPI_TRANSMITTER
#define SPI_YIELD() YIELD()
#define MMCSD
#define MMCSD_CS_PORT PORTD
#define MMCSD_CS_DDRPORT DDRD
#define MMCSD_CS_PIN 4
#define MMCSD_SPI_CSOn() { MMCSD_CS_DDRPORT |= (1 << MMCSD_CS_PIN); }
#define MMCSD_SPI_CSOff() { MMCSD_CS_DDRPORT &= (~(1 << MMCSD_CS_PIN)); }
extern uint8_t SPI_ReadWrite(uint8_t);
#define MMCSD_SPI_ReadWrite(byte) SPI_ReadWrite(byte)
