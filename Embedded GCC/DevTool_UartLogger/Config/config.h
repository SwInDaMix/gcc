/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config.h
 ***********************************************************************/

#pragma once

#include "Types.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

#define F_CPU 8000000UL
#define V_CPU 5000

#define YIELD() { wdt_reset(); }

#define STDIO_MICROSIZE
//#define STDIO_ALLOWALIGNMENT
//#define STDIO_ALLOWFILLCHAR
//#define STDIO_ALLOWLONG
//#define STDIO_ALLOWFLOAT
//#define STDIO_ALLOWSIGNPADS

#define USARTINIT_CONFIG (USARTINIT_BITS_8 | USARTINIT_PARITY_NONE | USARTINIT_STOPBIT_1)
#define USARTINIT_BAUDRATE 125000
#define USARTINIT_WRITE_DATA(data) { USARTINIT_UDR = data; }
#define USARTINIT_ENABLE_TX_INTERRUPT() { USARTINIT_UCSRB |= (1 << USARTINIT_UDRIE); }
void usart_push_char_from_isr(char);
bool usart_pop_char_from_isr();
#define USARTINIT_RECEIVER(data) ( usart_push_char_from_isr(data) )
#define USARTINIT_TRANSMITTER() ( usart_pop_char_from_isr() )

#define USART_RECEIVER
#define USART_TRANSMITTER
#define USART_RX_BUFFER_SIZESHIFT 7
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

#define SPI_SDCS_PORT PORTB
#define SPI_SDCS_PIN 1
#define SPI_SCK_PORT PORTB
#define SPI_SCK_PIN 3
#define SPI_MOSI_PORT PORTB
#define SPI_MOSI_PIN 2
#define SPI_MISO_PORTPIN PINB
#define SPI_MISO_PIN 4

#define LED_MODE_PORT PORTD
#define LED_MODE_PIN 2
#define LED_ACTIVITY_PORT PORTD
#define LED_ACTIVITY_PIN 3
#define LED_SD_PORT PORTD
#define LED_SD_PIN 4
#define LED_ERROR_PORT PORTD
#define LED_ERROR_PIN 5

#define BTN_MODE_PORTPIN PIND
#define BTN_MODE_PIN 6
