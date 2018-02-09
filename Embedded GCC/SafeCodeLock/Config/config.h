/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config.h
 ***********************************************************************/

#pragma once

#include "Types.h"
#include <avr/sleep.h>

#define F_CPU 128000UL
#define V_CPU 3300

#define YIELD() { set_sleep_mode(SLEEP_MODE_IDLE); sleep_mode(); }

#define STDIO_MICROSIZE
//#define STDIO_ALLOWALIGNMENT
//#define STDIO_ALLOWFILLCHAR
//#define STDIO_ALLOWLONG
//#define STDIO_ALLOWFLOAT
//#define STDIO_ALLOWSIGNPADS

#define USARTINIT_CONFIG (USARTINIT_BITS_8 | USARTINIT_PARITY_NONE | USARTINIT_STOPBIT_1)
#define USARTINIT_BAUDRATE 8000
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
//#define FRAMES_RX_BUFFER_SIZESHIFT 8
//#define FRAMES_TX_BUFFER_SIZESHIFT 8
//#define FRAMES_YIELD() YIELD()

#define USART_BAUDRATE 8000
#define USART_DBG

//#define ADC_RECEIVER
//#define ADC_RX_BUFFER_SIZESHIFT 0
#define ADC_HIGHBYTE_ONLY
#define ADC_YIELD() {}
