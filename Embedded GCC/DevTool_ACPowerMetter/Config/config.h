/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config.h
 ***********************************************************************/

#pragma once

#include "Types.h"
#include <avr/sleep.h>

#define F_CPU 8000000UL
#define V_CPU 5000

#define ADE_F_CPU 3579545UL

#define YIELD() {  }

#define STDIO_MICROSIZE
#define STDIO_ALLOWALIGNMENT
//#define STDIO_ALLOWFILLCHAR
#define STDIO_ALLOWFRACTIONS
#define STDIO_ALLOWLONG
//#define STDIO_ALLOWFLOAT
#define STDIO_ALLOWSIGNPADS

#define USARTINIT_CONFIG (USARTINIT_BITS_8 | USARTINIT_PARITY_NONE | USARTINIT_STOPBIT_1)
#define USARTINIT_BAUDRATE 19200
#define USARTINIT_WRITE_DATA(data) { USARTINIT_UDR = data; }
#define USARTINIT_ENABLE_TX_INTERRUPT() { USARTINIT_UCSRB |= (1 << USARTINIT_UDRIE); }
void frames8b_push_data_from_isr(uint8_t);
bool frames8b_pop_data_from_isr();
#define USARTINIT_RECEIVER(data) ( frames8b_push_data_from_isr(data) )
#define USARTINIT_TRANSMITTER() ( frames8b_pop_data_from_isr() )

#define USART_RECEIVER
#define USART_TRANSMITTER
#define USART_RX_BUFFER_SIZESHIFT 7
#define USART_TX_BUFFER_SIZESHIFT 7
#define USART_YIELD() YIELD()

#define FRAME_ADDRESS 0x00
#define FRAMES_DBG
#define FRAMES_8B
#define FRAMES_RECEIVER
#define FRAMES_TRANSMITTER
#define FRAMES_RX_BUFFER_SIZESHIFT 7
#define FRAMES_TX_BUFFER_SIZESHIFT 8
#define FRAMES_YIELD() YIELD()

#define ALCD_DATAPORT PORTC
#define ALCD_HALFDUPLEX 2
#define ALCD_CTL_RS_PORT PORTD
#define ALCD_CTL_RS_PIN 5
#define ALCD_CTL_RW_PORT PORTC
#define ALCD_CTL_RW_PIN 0
#define ALCD_CTL_E_PORT PORTC
#define ALCD_CTL_E_PIN 1
#define ALCD_DELAY() __asm("nop\n\t" "nop\n\t");

#define ADE7753
#define ADE7753_CS_PORT PORTB
#define ADE7753_CS_PIN 2
#define ADE7753_DELAY() __asm("nop\n\t" "nop\n\t");
#define ADE7753_SPI_CSOn() { ADE7753_CS_PORT &= (~(1 << ADE7753_CS_PIN)); ADE7753_DELAY(); }
#define ADE7753_SPI_CSOff() { ADE7753_DELAY(); ADE7753_CS_PORT |= (1 << ADE7753_CS_PIN); }
extern uint8_t SPI_ReadWrite(uint8_t);
#define ADE7753_SPI_ReadWrite(byte) SPI_ReadWrite(byte)

#define SPI_TRANSMITTER
#define SPI_YIELD() YIELD()

#define BTN_Function1_PINPORT PIND
#define BTN_Function1_PIN 3
#define BTN_Function2_PINPORT PIND
#define BTN_Function2_PIN 4

//#define ADC_RECEIVER
//#define ADC_RX_BUFFER_SIZESHIFT 0
//#define ADC_HIGHBYTE_ONLY
#define ADC_YIELD() YIELD()
