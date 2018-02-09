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

#define PERIOD_IDLE_POWERDOWN_TIMEOUT (4 * PUSART_TIMER_TICKS_PER_SECOND)

#define STDIO_MICROSIZE
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
//#define PUSART_TIMER_COUNTER
#define PUSART_IDLE_COUNTER
#define PUSART_BITS 9			// ���������� ���
#define PUSART_TXPORT PORTB		// ��� ����� ��� ��������
#define PUSART_RXPORT PINB		// ��� ����� �� �����
#define PUSART_TXDDR DDRB		// ������� ����������� ����� �� ��������
#define PUSART_RXDDR DDRB		// ������� ����������� ����� �� �����
#define PUSART_TXPIN 0			// ����� ���� ����� ��� ������������� �� ��������
#define PUSART_RXPIN 1			// ����� ���� ����� ��� ������������� �� �����
#define PUSART_BAUDRATE 2500

#define ADC_YIELD() YIELD()
#define ADC_CHANNEL_BATTERY 1

#define SW_SINK_PORT PORTB
#define SW_SINK_DDR DDRB
#define SW_LOW_SINK_PIN 3
#define SW_HIGH_SINK_PIN 4
