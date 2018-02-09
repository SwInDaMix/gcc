/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config.h
 ***********************************************************************/

#pragma once

#include "Types.h"
#include <avr/sleep.h>

#define F_CPU 1200000UL
#define V_CPU 3300

#define YIELD() {  }

#define PERIOD_IDLE_POWERDOWN_TIMEOUT PUSART_TIMER_PERIOD(4000)
#define PERIOD_MOTOR_FORCED_POWER PUSART_TIMER_PERIOD(1000)
#define PERIOD_MOTOR_MAX_POWER PUSART_TIMER_PERIOD(325)
#define PERIOD_MOTOR_SUCCESS_POWER PUSART_TIMER_PERIOD(125)
#define PERIOD_MOTOR_UNWIND_POWER PUSART_TIMER_PERIOD(175)

//#define STDIO_MICROSIZE
//#define STDIO_ALLOWALIGNMENT
//#define STDIO_ALLOWFILLCHAR
//#define STDIO_ALLOWFRACTIONS
//#define STDIO_ALLOWLONG
//#define STDIO_ALLOWFLOAT
//#define STDIO_ALLOWSIGNPADS

//#define USART_DBG
//#define USART_RECEIVER
//#define USART_TRANSMITTER
//#define USART_RX_BUFFER_SIZESHIFT 8
//#define USART_TX_BUFFER_SIZESHIFT 8
#define USART_YIELD() YIELD()

//#define FRAMES_DBG
//#define FRAMES_RECEIVER
//#define FRAMES_TRANSMITTER
//#define FRAMES_SIGNATURE_SIZESHIFT 3
//#define FRAMES_SIGNATURE 0x72, 0xC5, 0xD9, 0xAC, 0x86, 0xE3, 0xBA, 0x9E
//#define FRAMES_STUBBYTE 0xAA
//#define FRAMES_RX_BUFFER_SIZESHIFT 8
//#define FRAMES_TX_BUFFER_SIZESHIFT 8
//#define FRAMES_YIELD() YIELD()

#define PUSART_RECEIVER
#define PUSART_TRANSMITTER
#define PUSART_TIMER_COUNTER
#define PUSART_IDLE_COUNTER
#define PUSART_BITS 9			// Количество бит
//#define PUSART_8BITCOUNTERS 5	// 8 битные счетчики
#define PUSART_TXPORT PORTB		// Имя порта для передачи
#define PUSART_RXPORT PINB		// Имя порта на прием
#define PUSART_TXDDR DDRB		// Регистр направления порта на передачу
#define PUSART_RXDDR DDRB		// Регистр направления порта на прием
#define PUSART_TXPIN 0			// Номер бита порта для использования на передачу
#define PUSART_RXPIN 1			// Номер бита порта для использования на прием
#define PUSART_BAUDRATE 2500

#define ADC_HIGHBYTE_ONLY
#define ADC_YIELD() YIELD()
#define ADC_CHANNEL_LOCK_SENSOR 1

#define SW_LOCKMOTOR_PORT PORTB
#define SW_LOCKMOTOR_DDR DDRB
#define SW_LOW_LOCKMOTOR_PIN 3
#define SW_HIGH_LOCKMOTOR_PIN 4
