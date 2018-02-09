/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_adc.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include <avr/io.h>

#if	defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__) \
 || defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PA__) \
 || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) \
 || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) \
 || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) \
 || defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
	#define ADC_EXISTS

#if	defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__) \
 || defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PA__) \
 || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) \
 || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) \
 || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) \
 || defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
	#define ADC_RX_VECT			ADC_vect
	#define ADC_ADCL			ADCL
	#define ADC_ADCH			ADCH
	#define ADC_ADCW			ADCW
	#define ADC_ADMUX			ADMUX
	#define ADC_REFS0			REFS0
	#define ADC_REFS_BITWIDTH	2
	#define ADC_REFS_MASK		((1 << ADC_REFS_BITWIDTH) - 1)
	#define ADC_ADLAR			ADLAR
	#define ADC_MUX0			MUX0
	#define ADC_MUX_BITWIDTH	4
	#define ADC_MUX_MASK		((1 << ADC_MUX_BITWIDTH) - 1)
	#define ADC_ADCSRA			ADCSRA
	#define ADC_ADEN    		ADEN
	#define ADC_ADSC    		ADSC
	#define ADC_ADIF    		ADIF
	#define ADC_ADIE    		ADIE
	#define ADC_ADPS0   		ADPS0
	#define ADC_ADPS_BITWIDTH	3
	#define ADC_ADPS_MASK		((1 << ADC_ADPS_BITWIDTH) - 1)
#endif
#if	defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PA__) \
 || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) \
 || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) \
 || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) \
 || defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
	#define ADC_ADFR_OR_ADATE	ADATE
	#define ADC_ADCSRB			ADCSRB
	#define ADC_ADTS0   		ADTS0
	#define ADC_ADTS_BITWIDTH	3
	#define ADC_ADTS_MASK		((1 << ADC_ADTS_BITWIDTH) - 1)
#endif
#if	defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__)
	#define ADC_ADFR_OR_ADATE	ADFR
#endif

#ifdef ADC_HIGHBYTE_ONLY
typedef uint8_t adcvalue_t;
#define ADC_MAXVALUE 255
#else
typedef uint16_t adcvalue_t;
#define ADC_MAXVALUE 1023
#endif

typedef enum {
	#if defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
	ADCVR_AVCC		= 0,
	ADCVR_INTERNAL	= 1,
	#else
	ADCVR_AREF		= 0,
	ADCVR_AVCC		= 1,
	ADCVR_INTERNAL	= 3,
	#endif
} eADC_VoltageReference;

typedef enum {
	ADCPS_2		= 0,
	ADCPS_4		= 2,
	ADCPS_8		= 3,
	ADCPS_16	= 4,
	ADCPS_32	= 5,
	ADCPS_64	= 6,
	ADCPS_128	= 7,
} eADC_Prescaler;

#if	defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PA__) \
 || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) \
 || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) \
 || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) \
 || defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
typedef enum {
	ADCTS_NONE						= -1,
	ADCTS_FREE_RUNNING				= 0,
	ADCTS_ANALOG_COMPARATOR			= 1,
	ADCTS_EXTERNAL_INTERRUPT_0		= 2,
	ADCTS_TC0_COMPARE_MATCH_A		= 3,
	ADCTS_TC0_OVERFLOW				= 4,
	ADCTS_TC1_COMPARE_MATCH_B		= 5,
	ADCTS_TC1_OVERFLOW				= 6,
	ADCTS_TC1_CAPTURE_EVENT			= 7,
} eADC_TriggerSource;
#endif

#ifdef ADC_RECEIVER
// This flag is set on ADC Receiver buffer overflow
extern bit adc_rx_buffer_overflow;
#endif

#if defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PA__) \
 || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) \
 || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) \
 || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) \
 || defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
void adc_init(eADC_Prescaler prescaler, eADC_VoltageReference voltage_reference, eADC_TriggerSource trigger_source, uint8_t init_channel);
#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__)
void adc_init(eADC_Prescaler prescaler, eADC_VoltageReference voltage_reference, bit is_free_running, uint8_t init_channel);
#endif

// Configure ADC mux for specified channel
void adc_mux(uint8_t channel);

// Starts ADC convertion
void adc_start();

// Terminates ADC convertion and turns it off
void adc_off();

// Get a value from the ADC
adcvalue_t adc_get_value(void);

// Gets a value indicating whether there is a ready value in the ADC
bool adc_is_value_available();

#endif
