/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_adc.c
 ***********************************************************************/

#include <avr/interrupt.h>
#include "eta_adc.h"

#ifdef ADC_EXISTS

#ifdef ADC_HIGHBYTE_ONLY
#define ADC_ADC_VALUE ADC_ADCH
#define ADC_ADLARBIT (1 << ADC_ADLAR)
#else
#define ADC_ADC_VALUE ADC_ADCW
#define ADC_ADLARBIT 0
#endif

#ifdef ADC_RECEIVER
#define ADC_ADIEBIT (1 << ADC_ADIE)
#else
#define ADC_ADIEBIT 0
#endif

/// ********************************************************************
/// ADC Receiver Capability
/// ********************************************************************

#ifdef ADC_RECEIVER
// This flag is set on ADC Receiver buffer overflow
bit adc_rx_buffer_overflow;

#define ADC_RX_BUFFER_SIZE (1 << ADC_RX_BUFFER_SIZESHIFT)
#define ADC_RX_BUFFER_SIZEMASK (ADC_RX_BUFFER_SIZE - 1)

static adcvalue_t rx_buffer[ADC_RX_BUFFER_SIZE];
#if ADC_RX_BUFFER_SIZESHIFT <= 8
static unsigned char rx_wr_index, rx_rd_index;
#else
static unsigned int rx_wr_index, rx_rd_index;
#endif
#if ADC_RX_BUFFER_SIZESHIFT <= 7
static volatile unsigned char rx_counter;
#else
static volatile unsigned int rx_counter;
#endif

// ADC Receiver interrupt service routine
ISR(ADC_RX_VECT) {
	if(rx_counter < ADC_RX_BUFFER_SIZE) {
		rx_counter++; rx_buffer[rx_wr_index++] = ADC_ADC_VALUE;
		#if ADC_RX_BUFFER_SIZESHIFT != 8
		rx_wr_index &= ADC_RX_BUFFER_SIZEMASK;
		#endif
	}
	else adc_rx_buffer_overflow = true;
}
#endif

// Turns on and initializes ADC unit
#if	defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PA__) \
 || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) \
 || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) \
 || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) \
 || defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
void adc_init(eADC_Prescaler prescaler, eADC_VoltageReference voltage_reference, eADC_TriggerSource trigger_source, uint8_t init_channel) {
	#define ADC_ADFR_OR_ADATE_VALUE (trigger_source != ADCTS_NONE ? 1 : 0)
	if(trigger_source != ADCTS_NONE) ADC_ADCSRB = (ADC_ADCSRB & ~(ADC_ADTS_MASK << ADC_ADTS0)) | (trigger_source << ADC_ADTS0);
#elif	defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__)
void adc_init(eADC_Prescaler prescaler, eADC_VoltageReference voltage_reference, bit is_free_running, uint8_t init_channel) {
	#define ADC_ADFR_OR_ADATE_VALUE (is_free_running ? 1 : 0)
#endif
	ADC_ADMUX = ADC_ADLARBIT | (voltage_reference << ADC_REFS0) | ((init_channel & ADC_MUX_MASK) << ADC_MUX0);
	ADC_ADCSRA = (1 << ADC_ADEN) | (ADC_ADFR_OR_ADATE_VALUE << ADC_ADFR_OR_ADATE) | ADC_ADIEBIT | (prescaler << ADC_ADPS0);
}

// Configure ADC mux for specified channel
void adc_mux(uint8_t channel) { ADC_ADMUX = (ADC_ADMUX & ~(ADC_MUX_MASK << ADC_MUX0)) | ((channel & ADC_MUX_MASK) << ADC_MUX0); }

// Starts ADC convertion
void adc_start() { ADC_ADCSRA |= (1 << ADC_ADSC); }

// Terminates ADC convertion and turns it off
void adc_off() { ADC_ADCSRA = 0; }

// Get a value from the ADC
adcvalue_t adc_get_value(void) {
	adcvalue_t _value;
	#ifdef ADC_RECEIVER
	while (!rx_counter) ADC_YIELD();
	_value = rx_buffer[rx_rd_index++];
	#if ADC_RX_BUFFER_SIZESHIFT != 8
	rx_rd_index &= ADC_RX_BUFFER_SIZEMASK;
	#endif
	cli(); rx_counter--; sei();
	#else
	while (!(ADC_ADCSRA & (1 << ADC_ADIF))) ADC_YIELD();
	ADC_ADCSRA |= (1 << ADC_ADIF);
	_value = ADC_ADC_VALUE;
	#endif
	return _value;
}

// Gets a value indicating whether there is a ready value in the ADC
bool adc_is_value_available() {
	#ifdef ADC_RECEIVER
	return rx_counter;
	#else
	return (ADC_ADCSRA & (1 << ADC_ADIF));
	#endif
}

#endif

