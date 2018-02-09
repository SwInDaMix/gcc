/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_pusart.h
 ***********************************************************************/

#pragma once

#include "config.h" // avialable bhtes

#if defined PUSART_RECEIVER || defined PUSART_TRANSMITTER
//#define PUSART_TIMDIV 1
//#define PUSART_TIMDIV 2
#define PUSART_TIMDIV 2

#if PUSART_TIMDIV == 1
 #define PUSART_TIMDIV_FACTOR 1
#elif PUSART_TIMDIV == 2
 #define PUSART_TIMDIV_FACTOR 8
#elif PUSART_TIMDIV == 3
 #define PUSART_TIMDIV_FACTOR 64
#endif
#define PUSART_TIMBAUD (((F_CPU / PUSART_TIMDIV_FACTOR) / PUSART_BAUDRATE) - 1)					// Значение частоты порта в тиках таймера
#define PUSART_TIMER_TICKS_PER_SECOND ((F_CPU / PUSART_TIMDIV_FACTOR) / (PUSART_TIMBAUD + 1))	// Количество тиков таймера за 1 сек

#ifdef PUSART_8BITCOUNTERS
typedef uint8_t pusart_counter_t;
#define PUSART_TIMER_PERIOD(msec) ((pusart_counter_t)((msec) * PUSART_TIMER_TICKS_PER_SECOND / 16000))
#else
typedef uint16_t pusart_counter_t;
#define PUSART_TIMER_PERIOD(msec) ((pusart_counter_t)((msec) * PUSART_TIMER_TICKS_PER_SECOND / 1000))
#endif

#if PUSART_BITS > 8
typedef uint16_t pusart_value_t;
#else
typedef uint8_t pusart_value_t;
#endif

#ifdef PUSART_TIMER_COUNTER
pusart_counter_t pusart_timer_get();
#endif
#ifdef PUSART_IDLE_COUNTER
pusart_counter_t pusart_idle_counter_get();
#endif

#endif

#ifdef PUSART_RECEIVER
// Check is there a byte already received
bool pusart_is_value_available();

// Get a value from the PUSART Receiver buffer
pusart_value_t pusart_get_value();
#endif

#ifdef PUSART_TRANSMITTER
// Write a value to the PUSART Transmitter buffer
void pusart_put_value(pusart_value_t value);
// Waits output flush
void pusart_flush();
#endif

#if defined PUSART_RECEIVER || defined PUSART_TRANSMITTER
// Inits interrupt-driven USART port
void pusart_init();
#endif
