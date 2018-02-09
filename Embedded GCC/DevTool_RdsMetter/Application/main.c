/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.c
 ***********************************************************************/

#include "main.h"
#include <stdbool.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "eta_crc.h"
#include "eta_adc.h"
#include "eta_dbg.h"
#include "eta_usart.h"
#include "eta_spi.h"
#include "eta_hd44780.h"
#include "eta_ade7753.h"

static uint16_t adc_ds_zero_current;
static bool is_overprobe;
static uint8_t switches;
static uint32_t ds_current_x10000, ds_drop_voltage_x10000, ds_resistance_x100;

static void init() {
	PORTB = 0x00;
	DDRB = 0x00;
	PORTC = 0x00;
	DDRC = (((1 << PORT_RelaySwitches_PINs) - 1) << PORT_RelaySwitches_PINOffset);
	PORTD = (1 << BTN_Function_PIN);
	DDRD = (1 << PORT_GateDrive_PIN) | (1 << ALCD_CTL_RS_PIN) | (1 << ALCD_CTL_RW_PIN) | (1 << ALCD_CTL_E_PIN);

	usartinit_init();
	adc_init(ADCPS_128, ADCVR_INTERNAL, ADCTS_NONE, 0);

	sei();

	DBG_PutString_P(PSTR("MOSFET Rds(on) Metter"NL));
	ALCD_Init();
}

void greetings(const char *msg) {
	ALCD_GoToXY(0, 0); ALCD_PutString_P(PSTR(" Rds(on) Metter "));
	ALCD_GoToXY(0, 1); ALCD_PutString_P(msg);
}

void probe_now() {
	uint24_t _adc_ds_current = 0, _adc_ds_drop_voltage = 0;
	for(uint8_t _i = (1 << MC_ADC_ProbesShift); _i; _i--) {
		uint16_t _adc_ds_current_tmp, _adc_ds_drop_voltage_tmp;
		adc_mux(ADC_Channel_DSCurrent); adc_start(); _adc_ds_current += (_adc_ds_current_tmp = adc_get_value());
		adc_mux(ADC_Channel_DSDropVoltage); adc_start(); _adc_ds_drop_voltage += (_adc_ds_drop_voltage_tmp = adc_get_value());

		#if defined MC_TEST_ADC_DETAILS
		DBG_PutFormat_P(PSTR(" ADC C:%d, DS:%d"NL), _adc_ds_current_tmp, _adc_ds_drop_voltage_tmp);
		#endif
	}
	uint16_t _adc_ds_current2 = ((uint16_t)(_adc_ds_current >> MC_ADC_ProbesShift)); if(_adc_ds_current2 > adc_ds_zero_current) _adc_ds_current2 = adc_ds_zero_current;
	uint16_t _adc_ds_drop_voltage2 = (uint16_t)(_adc_ds_drop_voltage >> MC_ADC_ProbesShift);
	DBG_PutFormat_P(PSTR("=ADC C:%d, DS:%d"NL), _adc_ds_current2, _adc_ds_drop_voltage2);
	ds_current_x10000 = MC_ADC_DSCurrent_2_Current_x10000(adc_ds_zero_current - _adc_ds_current2);
	ds_drop_voltage_x10000 = MC_ADC_DSDropVoltage_2_Voltage_x10000(_adc_ds_drop_voltage2);
	ds_resistance_x100 = ds_current_x10000 ? (((uint32_t)ds_drop_voltage_x10000 * 100000) / ds_current_x10000) : 0;

	is_overprobe = _adc_ds_drop_voltage2 > 1015;
}

void probe_format() {
	ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("%7w4lA "), ds_current_x10000); DBG_PutFormat_P(PSTR("Probe results: %7w4lA "), ds_current_x10000);
	if(is_overprobe) { ALCD_PutString_P(PSTR("!over!V")); ALCD_PutString_P(PSTR("!over!V"NL)); ALCD_GoToXY(0, 1); ALCD_PutString_P(PSTR("                ")); }
	else {
		ALCD_PutFormat_P(PSTR("%7w4lV"), ds_drop_voltage_x10000); DBG_PutFormat_P(PSTR("%7w4lV "), ds_drop_voltage_x10000);
		ALCD_GoToXY(0, 1); ALCD_PutFormat_P(PSTR("  %8w2lmOhm  "), ds_resistance_x100); DBG_PutFormat_P(PSTR("%8w2lmOhm"NL), ds_resistance_x100);
	}
}

void probe_switch() {
	DBG_PutFormat_P(PSTR("SW:%d"NL), switches);
	PORT_GateDrive_PORT &= (~(1 << PORT_GateDrive_PIN));
	_delay_ms(MC_GateSwitchingTime);
	PORT_RelaySwitches_PORT = (PORT_RelaySwitches_PORT & (~((1 << PORT_RelaySwitches_PINs) - 1))) | (switches << PORT_RelaySwitches_PINOffset);
	_delay_ms(MC_RelaySwitchingTime);
	PORT_GateDrive_PORT |= (1 << PORT_GateDrive_PIN);
	_delay_ms(MC_GateSwitchingTime);
	probe_now();
}

void probe_switch_best() {
	greetings(PSTR("  Mettering...  "));
	switches = 1;

	#if defined MC_TEST_GAMMA
		while(true) {
			probe_switch(switches++);
			PORT_GateDrive_PORT &= (~(1 << PORT_GateDrive_PIN));
			probe_format();
			if(is_overprobe || switches >= (1 << PORT_RelaySwitches_PINs)) { break; }
		}
	#elif defined MC_TEST_SPECTER
	while(true) {
		probe_switch(switches); switches <<= 1;
		PORT_GateDrive_PORT &= (~(1 << PORT_GateDrive_PIN));
		probe_format();
		if(is_overprobe || switches >= (1 << PORT_RelaySwitches_PINs)) { break; }
	}
	#else
		enum {
			SWBF_Recalc,
			SWBF_Recalc2,
			SWBF_Over,
		} _swbf = SWBF_Recalc;
		while(true) {
			probe_switch();
			if(!is_overprobe && (_swbf == SWBF_Recalc || _swbf == SWBF_Recalc2)) {
				_swbf++;
				switches = ((uint24_t)switches * 10950 / ds_drop_voltage_x10000);
				if(switches >= (1 << PORT_RelaySwitches_PINs)) { _swbf = SWBF_Over; switches = (1 << PORT_RelaySwitches_PINs) - 1; }
			}
			else if(is_overprobe || _swbf == SWBF_Over) {
				if(!is_overprobe || !(--switches)) break;
			}
		}
	#endif
}

void probe_switch_off() {
	DBG_PutString_P(PSTR("SW:Off"NL));
	PORT_GateDrive_PORT &= (~(1 << PORT_GateDrive_PIN));
	_delay_ms(MC_GateSwitchingTime);
	PORT_RelaySwitches_PORT &= (~((1 << PORT_RelaySwitches_PINs) - 1));
	_delay_ms(MC_RelaySwitchingTime);
}

__attribute__((OS_main))
void main() {
	init();

	// Calibrate
	greetings(PSTR(" Calibrating... "));
	adc_mux(ADC_Channel_DSCurrent); uint32_t _adc_ds_zero_current_big = 0;
	for(uint16_t _i = (1 << 11); _i; _i--) { adc_start(); _adc_ds_zero_current_big += adc_get_value(); }
	adc_ds_zero_current = (_adc_ds_zero_current_big >> 11);
	greetings(PSTR("  -= Ready =-   "));
	DBG_PutFormat_P(PSTR("0 current: %d"NL), adc_ds_zero_current);

	if(!(BTN_Function_PINPORT & (1 << BTN_Function_PIN))) {
		while(true) {
			probe_format();
			probe_now();
		}
		_delay_ms(300);
	}

	while(true) {
		if(!(BTN_Function_PINPORT & (1 << BTN_Function_PIN))) {
			probe_switch_best();
			while(true) {
				probe_format();
				if((BTN_Function_PINPORT & (1 << BTN_Function_PIN))) break;
				probe_now();
			}
			probe_switch_off();
		}
	}

	__builtin_unreachable();
}
