/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.c
 ***********************************************************************/

#include "main.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "eta_dbg.h"
#include "eta_adc.h"
#include "eta_crc.h"
#include "eta_frames.h"
#include "eta_spi.h"
#include "eta_usart.h"
#include "eta_hd44780.h"
#include "eta_mmcsd.h"

static EEMEM sCC_Calibration CCCalibration;
static EEMEM sCC_Memory CCMemory;

static uint16_t CCCalibrationVoltageARef;
static uint16_t CCCalibrationCurrentMvPerAmp;
static int8_t CCCalibrationCurrentOffset;
static uint8_t CCCalibrationWiremOhm;
static sCC_MemoryCountIndex CCMemoryCountIndex;
static uint32_t CCMMCSDCurrentSector;
static uint32_t CCMMCSDSamples;

typedef struct {
	char const *text_name;
	uint16_t discharge_undervoltage;
	uint16_t charge_overvoltage;
} sCCCellInfo;
static PROGMEM char const CCCellInfoText_LiFePo4[] = "LiFePo4";
static PROGMEM char const CCCellInfoText_LiFePo4_Full[] = "LiFePo4 Full";
static PROGMEM char const CCCellInfoText_LiIon[] = "LiIon";
static PROGMEM char const CCCellInfoText_LiPo[] = "LiPo";
static PROGMEM char const CCCellInfoText_PbAcid3[] = "PbAcid 3 cells";
static PROGMEM char const CCCellInfoText_PbAcid6[] = "PbAcid 6 cells";
static PROGMEM sCCCellInfo const CCCellInfo[] = {
	{ .text_name = CCCellInfoText_LiFePo4, .discharge_undervoltage = 2600, .charge_overvoltage = 3600 },
	{ .text_name = CCCellInfoText_LiFePo4_Full, .discharge_undervoltage = 2600, .charge_overvoltage = 3800 },
	{ .text_name = CCCellInfoText_LiIon, .discharge_undervoltage = 3100, .charge_overvoltage = 4200 },
	{ .text_name = CCCellInfoText_LiPo, .discharge_undervoltage = 3100, .charge_overvoltage = 4200 },
	{ .text_name = CCCellInfoText_PbAcid3, .discharge_undervoltage = 5400, .charge_overvoltage = 7350 },
	{ .text_name = CCCellInfoText_PbAcid6, .discharge_undervoltage = 10800, .charge_overvoltage = 14700 },
};

static uint32_t volatile ACPM_GlobalTicks;
static uint16_t volatile CCCurrentADC;
static uint16_t volatile CCVoltageADC;
ISR(TIMER1_COMPA_vect) {
	static uint16_t _cell_current_adc = 0, _cell_voltage_adc = 0;
	static bool _is_voltage_div = false;

	ACPM_GlobalTicks++;
	adc_mux(ADC_CURRENT_CH); adc_start(); _cell_current_adc += adc_get_value();
	adc_mux(ADC_VOLTAGE_CH); adc_start(); uint16_t _cell_voltage_adc_this = adc_get_value(); _cell_voltage_adc += _is_voltage_div ? (_cell_voltage_adc_this * 3) : _cell_voltage_adc_this;
	if(!(ACPM_GlobalTicks & (PERIOD_1S_HALF - 1))) {
		CCCurrentADC = _cell_current_adc; CCVoltageADC = _cell_voltage_adc;
		_cell_current_adc = _cell_voltage_adc = 0;
	}
	if(_cell_voltage_adc_this > ADC_VOLTAGE_DivThresholdOn) { _is_voltage_div = true; SW_VoltageDiv_PORT |= (1 << SW_VoltageDiv_PIN); }
	if(_cell_voltage_adc_this < ADC_VOLTAGE_DivThresholdOff) { _is_voltage_div = false; SW_VoltageDiv_PORT &= (~(1 << SW_VoltageDiv_PIN)); }
}

static void init() {
	PORTB = 0x01; DDRB = 0x2E;
	PORTC = 0x00; DDRC = 0x03;
	PORTD = 0xE0; DDRD = 0x0C;

	// Tick timer every 1/32 sec
	OCR1A = (F_CPU / 8 / PERIOD_1S) - 1; TCCR1B = (1 << CS11) | (1 << WGM12); TIMSK1 = (1 << OCIE1A); TIFR1 = (1 << OCF1A);

	// Reset watchdog
	MCUSR &= ~(1 << WDRF); wdt_disable();

	//frames_init();
	usartinit_init();
	adc_init(ADCPS_128, ADCVR_AVCC, ADCTS_NONE, 0);
	SPI_Init(true, false, SPICfg_CPOL0_CPHA0, SPIPrescaler_16);

	sei();

	DBG_PutString_P(PSTR("Initializing Peripheral..."NL));
	ALCD_Init();
	ALCD_Clear();
	ALCD_PutString_P(PSTR("   " VER_APP));
}

void calibartion_init() {
	uint16_t _signature = eeprom_read_word(&CCCalibration.signature);
	if(_signature != CC_CALIBRATION_SIGNATURE) {
		DBG_PutString_P(PSTR("Bad calibration signature. Reseting calibration."));
		eeprom_write_word(&CCCalibration.signature, CC_CALIBRATION_SIGNATURE);
		eeprom_write_word(&CCCalibration.voltage_aref, (CCCalibrationVoltageARef = CC_CALIBRATION_DEF_voltage_aref));
		eeprom_write_word(&CCCalibration.current_mv_per_amp, (CCCalibrationCurrentMvPerAmp = CC_CALIBRATION_DEF_current_mv_per_amp));
		eeprom_write_byte((uint8_t *)&CCCalibration.current_offset, (uint8_t)(CCCalibrationCurrentOffset = CC_CALIBRATION_DEF_current_offset));
		eeprom_write_byte(&CCCalibration.wire_mohm, (CCCalibrationWiremOhm = CC_CALIBRATION_DEF_wire_mohm));
	}
	else {
		CCCalibrationVoltageARef = eeprom_read_word(&CCCalibration.voltage_aref);
		CCCalibrationCurrentMvPerAmp = eeprom_read_word(&CCCalibration.current_mv_per_amp);
		CCCalibrationCurrentOffset = (int8_t)eeprom_read_byte((uint8_t const *)&CCCalibration.current_offset);
		CCCalibrationWiremOhm = eeprom_read_byte(&CCCalibration.wire_mohm);
		DBG_PutString_P(PSTR("Calibration OK."));
	}
	DBG_PutFormat_P(PSTR(" voltage_aref %ud"NL" current_mv_per_amp %ud"NL" current offset %ud"NL" wire_mohm %ud."NL), CCCalibrationVoltageARef, CCCalibrationCurrentMvPerAmp, CCCalibrationCurrentOffset, CCCalibrationWiremOhm);
}
void calibartion_save() {
	eeprom_write_word(&CCCalibration.voltage_aref, CCCalibrationVoltageARef);
	eeprom_write_word(&CCCalibration.current_mv_per_amp, CCCalibrationCurrentMvPerAmp);
	eeprom_write_byte((uint8_t *)&CCCalibration.current_offset, (uint8_t)CCCalibrationCurrentOffset);
	eeprom_write_byte(&CCCalibration.wire_mohm, CCCalibrationWiremOhm);
	DBG_PutString_P(PSTR("Calibration Saved."NL));
}

void memory_init() {
	uint16_t _signature = eeprom_read_word(&CCMemory.signature);
	if(_signature != CC_MEMORY_SIGNATURE) {
		DBG_PutString_P(PSTR("Bad memory signature. Reseting memory."NL));
		eeprom_write_word(&CCMemory.signature, CC_MEMORY_SIGNATURE);
		CCMemoryCountIndex.value = 0; eeprom_write_dword(&CCMemory.countindex.value, 0);
	}
	else {
		CCMemoryCountIndex.value = eeprom_read_dword(&CCMemory.countindex.value);
		DBG_PutFormat_P(PSTR("Memory OK. %ud used cycles."NL), CCMemoryCountIndex.count);
	}
}
void memory_writenew(sCC_Cycle const *cycle, bool is_prev) {
	if(is_prev) {
		DBG_PutString_P(PSTR("Overwriting last cycle."NL));
		uint16_t _index = CCMemoryCountIndex.index; if(_index) _index--; else _index = CC_MEMORY_CYCLES - 1;
		eeprom_write_block(cycle, &CCMemory.cycles[_index], sizeof(sCC_Cycle));
	}
	else {
		DBG_PutString_P(PSTR("Writing new cycle."NL));
		if(CCMemoryCountIndex.count < CC_MEMORY_CYCLES) CCMemoryCountIndex.count++;
		eeprom_write_block(cycle, &CCMemory.cycles[CCMemoryCountIndex.index], sizeof(sCC_Cycle));
		CCMemoryCountIndex.index++; if(CCMemoryCountIndex.index >= CC_MEMORY_CYCLES) CCMemoryCountIndex.index = 0;
		eeprom_write_dword(&CCMemory.countindex.value, CCMemoryCountIndex.value);
	}
}
bool memory_read(sCC_Cycle *cycle, uint16_t index) {
	if(index >= CCMemoryCountIndex.count) return false;
	index++;
	if(index > CCMemoryCountIndex.index) index -= CC_MEMORY_CYCLES;
	index = CCMemoryCountIndex.index - index;
	eeprom_read_block(cycle, &CCMemory.cycles[index], sizeof(sCC_Cycle));
	return true;
}
void memory_reset() {
	CCMemoryCountIndex.value = 0; eeprom_write_dword(&CCMemory.countindex.value, 0);
	DBG_PutString_P(PSTR("Reseting memory."NL));
}

bool sdcard_reset() {
	bool _res = false;
	if(CCMMCSDCurrentSector != (uint32_t)-1) {
		DBG_PutString_P(PSTR("Reseting SD card"NL));
		uint32_t _signature = SDCARD_SD_SIGNATURE; CCMMCSDCurrentSector = 0;
		if((_res = MMCSD_WriteInit(0) && MMCSD_WriteBlock(&_signature, sizeof(_signature)) && MMCSD_WriteBlock(&CCMMCSDCurrentSector, sizeof(CCMMCSDCurrentSector)) && MMCSD_WriteFinalize())) {  }
	}
	return _res;
}
bool sdcard_start() {
	bool _res = false;
	if(CCMMCSDCurrentSector != (uint32_t)-1) {
		if((_res = MMCSD_WriteInit(CCMMCSDCurrentSector + 2))) CCMMCSDSamples = 0;
	}
	return _res;
}
bool sdcard_end(eCC_CellType cell_type, bool is_discharging) {
	bool _res = false;
	if(CCMMCSDCurrentSector != (uint32_t)-1) {
		uint32_t _signature = SDCARD_ENTRY_SIGNATURE;
		uint8_t _ch_disch = is_discharging ? 0xFF : 0x00;
		if((_res = MMCSD_WriteFinalize() && MMCSD_WriteInit(CCMMCSDCurrentSector + 1) && MMCSD_WriteBlock(&_signature, sizeof(_signature)) && MMCSD_WriteBlock(&CCMMCSDSamples, sizeof(CCMMCSDSamples)) && MMCSD_WriteBlock(&cell_type, sizeof(cell_type)) && MMCSD_WriteBlock(&_ch_disch, sizeof(_ch_disch)) && MMCSD_WriteFinalize() && MMCSD_WriteInit(0))) {
			CCMMCSDCurrentSector += (((CCMMCSDSamples * sizeof(sCC_Values)) + 1023) >> 9); _signature = SDCARD_SD_SIGNATURE;
			if((_res = MMCSD_WriteBlock(&_signature, sizeof(_signature)) && MMCSD_WriteBlock(&CCMMCSDCurrentSector, sizeof(CCMMCSDCurrentSector)) && MMCSD_WriteFinalize())) {  }
		}
	}
	return _res;
}
bool sdcard_write_sample(sCC_Values const *values) {
	bool _res = false;
	if(CCMMCSDCurrentSector != (uint32_t)-1) {
		if((_res = MMCSD_WriteBlock(values, sizeof(sCC_Values)))) { CCMMCSDSamples++; }
	}
	return _res;
}
bool sdcard_init() {
	bool _res = false;
	DBG_PutString_P(PSTR("Initializing SD card..."));
	if((_res = MMCSD_Init())) {
		DBG_PutFormat_P(PSTR("Found SD card %02X."NL), MMCSD_CardType_get());
		uint32_t _signature;
		if((_res = MMCSD_ReadInit(0) && MMCSD_ReadBlock(&_signature, sizeof(_signature)) && MMCSD_ReadBlock(&CCMMCSDCurrentSector, sizeof(CCMMCSDCurrentSector)) && MMCSD_ReadFinalize())) {
			if(_signature == SDCARD_SD_SIGNATURE) {
				DBG_PutFormat_P(PSTR("Current SD card sector %lud"NL), CCMMCSDCurrentSector);
			}
			else {
				DBG_PutString_P(PSTR("SD card bad signature. "));
				CCMMCSDCurrentSector = 0; _res = sdcard_reset();
			}
		}
	}
	if(!_res) { CCMMCSDCurrentSector = -1; DBG_PutString_P(PSTR("SD card failure"NL)); }
	return _res;
}

void switch_off_all() { SW_Charge_PORT &= ~(1 << SW_Charge_PIN); SW_Discharge_PORT &= ~(1 << SW_Discharge_PIN); }
void switch_charge() { switch_off_all(); _delay_ms(100); SW_Charge_PORT |= (1 << SW_Charge_PIN); }
void switch_discharge() { switch_off_all(); _delay_ms(100); SW_Discharge_PORT |= (1 << SW_Discharge_PIN); }

__attribute__((OS_main))
void main() {
	init();
	ALCD_GoToXY(0, 1); ALCD_PutString_P(PSTR(" Initializing..."));
	calibartion_init(); memory_init();
	ALCD_GoToXY(0, 1); ALCD_PutString_P(PSTR("   SD card...   "));
	ALCD_GoToXY(0, 1); ALCD_PutString_P(sdcard_init() ? PSTR("  SD card OK!  ") : PSTR(" SD card failed "));
	_delay_ms(1000);

	eCC_Screen _screen = (BTN_Escape_PINPORT & (1 << BTN_Escape_PIN)) ? CCScreen_Program : CCScreen_Calibration; eCC_CellType _cell_type = CCCellType_LiFePo4; eCC_Program _program = CCProgram_Charge1; eCC_State _state = CCState_Idle, _state_prev = CCState_Idle, _state_next = CCState_Idle; eCC_CalibrationValue _calibration_value = CCCalibrationValue_Voltage;
	eCC_Button _buttons_prev = 0; uint16_t _cycle_index = 0;
	uint8_t _cycles = 0; uint32_t _global_ticks_start = 0; sCC_Cycle _cycle; memset(&_cycle, 0, sizeof(sCC_Cycle));

	if(!(BTN_Prev_PINPORT & (1 << BTN_Prev_PIN)) && !(BTN_Next_PINPORT & (1 << BTN_Next_PIN))) memory_reset();
	if(!(BTN_Prev_PINPORT & (1 << BTN_Prev_PIN)) && !(BTN_Enter_PINPORT & (1 << BTN_Enter_PIN))) sdcard_reset();

	switch_off_all();
	cli(); uint16_t _adc_current_init = CCCurrentADC; sei();
	DBG_PutFormat_P(PSTR("Current 0 value %ud"NL), _adc_current_init);

	ALCD_Clear();

	wdt_enable(WDTO_1S); sei();

	bool _update_screen = true;
	uint64_t _cell_cap_ah = 0, _cell_cap_power = 0; sCCCellInfo const *_cell_info = CCCellInfo;
	uint8_t _global_ticks_short_prev = 0, _global_ticks_half_sec_prev = 0;
	int24_t _signed_current = 0; sCC_Values _values; memset(&_values, 0, sizeof(sCC_Values));

	while(true) {
		uint8_t _global_ticks_short = (uint8_t)ACPM_GlobalTicks;
		if(_global_ticks_short_prev != _global_ticks_short) {
			_global_ticks_short_prev = _global_ticks_short;
			bool _2s_change = !(_global_ticks_short & (PERIOD_2S - 1)), _2s_bit = _global_ticks_short & PERIOD_2S;

			cli(); uint32_t _global_ticks_tmp = ACPM_GlobalTicks; sei();
			uint32_t _ticks_elapsed = _global_ticks_tmp - _global_ticks_start;
			uint24_t _seconds_elapsed = _ticks_elapsed >> PERIOD_1S_SHIFT;

			uint16_t _adc_current_0 = (1 << (ADC_BITS + PERIOD_1S_SHIFT - 2)) + CCCalibrationCurrentOffset;

			if(_state_prev != _state) {
				if(_state == CCState_Idle || _state == CCState_Relieve) { switch_off_all(); sdcard_end(_cell_type, _state_prev == CCState_Discharge); }
				else if(_state == CCState_Discharge) { switch_discharge(); sdcard_start(); }
				else if(_state == CCState_Charge) { switch_charge(); sdcard_start(); }
				_state_prev = _state;
				frames_put_frame(FRAME_ADDRESS, ACPMCommand_NewState, &_state, sizeof(eCC_State));
			}

			// Half second periodic routines
			uint8_t _global_ticks_half_sec = _global_ticks_short >> (PERIOD_1S_SHIFT - 1);
			if(_global_ticks_half_sec_prev != _global_ticks_half_sec) {
				_update_screen = true;

				_global_ticks_half_sec_prev = _global_ticks_half_sec;
				cli(); uint16_t _cell_current_adc = CCCurrentADC; sei();
				cli(); uint16_t _cell_voltage_adc = CCVoltageADC; sei();
				_signed_current = ADC_CURRENT_Value(ADC_CURRENT_adc(_adc_current_0, _cell_current_adc), CCCalibrationCurrentMvPerAmp, (PERIOD_1S_SHIFT - 1));
				_values.cell_current = labs((int32_t)_signed_current);
				_values.sensed_voltage = ADC_VOLTAGE_Value(_cell_voltage_adc, CCCalibrationVoltageARef, (PERIOD_1S_SHIFT - 1));
				int16_t _drop_voltage = _signed_current * CCCalibrationWiremOhm / 10000;
				_values.cell_voltage = _values.sensed_voltage + _drop_voltage;
				if(_values.cell_voltage > 16000) _values.cell_voltage = 0;
				frames_put_frame(FRAME_ADDRESS, ACPMCommand_ValuesUpdated, &_values, sizeof(sCC_Values));

				if(_state == CCState_Discharge || _state == CCState_Charge) {
					sdcard_write_sample(&_values);
					_cell_cap_ah += _values.cell_current; _cell_cap_power += ((uint64_t)_values.cell_current * _values.cell_voltage);

					__extension__ void make_cycle_value(sCC_CycleValue *cycle_value) {
						cycle_value->seconds = _seconds_elapsed;
						cycle_value->cap_ah = _cell_cap_ah / 720000UL;
						cycle_value->cap_power = _cell_cap_power / 720000000ULL;
						DBG_PutFormat_P(PSTR(" %td seconds, capacity is %w2udAh, %w2udW"NL), cycle_value->seconds, cycle_value->cap_ah, cycle_value->cap_power);
					};

					if(_screen != CCScreen_Calibration && _seconds_elapsed > 2) {
						if(_state == CCState_Discharge) {
							if(_values.cell_voltage < pgm_read_word(&_cell_info->discharge_undervoltage)) {
								DBG_PutString_P(PSTR("Discharging ended in"));
								make_cycle_value(&_cycle.discharge);
								if(_cycle.discharge.seconds > 5) memory_writenew(&_cycle, false);
								cli(); _global_ticks_start = ACPM_GlobalTicks; sei();
								_state = CCState_Relieve; _state_next = CCState_Charge;
								DBG_PutString_P(PSTR("Starting relieve."NL));
							}
						}
						else if(_state == CCState_Charge) {
							if(_values.cell_voltage > pgm_read_word(&_cell_info->charge_overvoltage)) {
								DBG_PutString_P(PSTR("Charging ended in"));
								make_cycle_value(&_cycle.charge);
								if(_cycle.charge.seconds > 5) memory_writenew(&_cycle, _cycle.discharge.seconds);
								if(_cycles) {
									DBG_PutFormat_P(PSTR("%d cycles left."NL), _cycles);
									_cycles--;
									cli(); _global_ticks_start = ACPM_GlobalTicks; sei();
									_state = CCState_Relieve; _state_next = CCState_Discharge;
									DBG_PutString_P(PSTR("Starting relieve."NL));
								}
								else { _state = CCState_Idle; _screen = CCScreen_Program; _update_screen = true; }
							}
						}
					}
				}
				else if(_state == CCState_Relieve) {
					if(_seconds_elapsed >= PERIOD_RELIEVE) {
						cli(); _global_ticks_start = ACPM_GlobalTicks; sei(); _cell_cap_ah = _cell_cap_power = 0;
						if(_state_next == CCState_Discharge) memset(&_cycle, 0, sizeof(sCC_Cycle));
						_state = _state_next;
						DBG_PutString_P(_state == CCState_Discharge ? PSTR("Starting discharge."NL) : PSTR("Starting charge."NL));
					}
				}
			}

			// Process buttons
			eCC_Button _buttons = 0; if(!(BTN_Escape_PINPORT & (1 << BTN_Escape_PIN))) _buttons |= CCButton_Escape; if(!(BTN_Prev_PINPORT & (1 << BTN_Prev_PIN))) _buttons |= CCButton_Prev; if(!(BTN_Next_PINPORT & (1 << BTN_Next_PIN))) _buttons |= CCButton_Next; if(!(BTN_Enter_PINPORT & (1 << BTN_Enter_PIN))) _buttons |= CCButton_Enter;
			eCC_Button _buttons_new = _buttons & (_buttons ^ _buttons_prev); _buttons_prev = _buttons;
			if(_screen == CCScreen_Program) {
				if(_buttons_new & CCButton_Escape) {
					_cell_type++; _update_screen = true;
					if(_cell_type >= CCCellType__Last) {
						_cell_type = CCCellType_LiFePo4;
						if(CCMemoryCountIndex.count) { _screen = CCScreen_MemoryInfo; _cycle_index = 0; memory_read(&_cycle, _cycle_index); }
					}
					_cell_info = &CCCellInfo[_cell_type];
				}
				if(_buttons_new & CCButton_Prev) { if(_program) _program--; else _program = CCProgram__Last - 1; _update_screen = true; }
				if(_buttons_new & CCButton_Next) { _program++; if(_program >= CCProgram__Last) _program = 0; _update_screen = true; }
				if(_buttons_new & CCButton_Enter) {
					_cycles = (_program & CCProgram__MaskCount);
					_state = _program & CCProgram__MaskDischarge ? CCState_Discharge : CCState_Charge;
					_screen = CCScreen_StatePower;
					cli(); _global_ticks_start = ACPM_GlobalTicks; sei(); memset(&_cycle, 0, sizeof(sCC_Cycle)); _cell_cap_ah = _cell_cap_power = 0;
					DBG_PutString_P(_program & CCProgram__MaskDischarge ? PSTR("Starting discharge."NL) : PSTR("Starting charge."NL));
				}
			}
			else if(_screen == CCScreen_MemoryInfo || _screen == CCScreen_MemoryValues) {
				if(_buttons_new & CCButton_Escape) { _screen = _screen == CCScreen_MemoryValues ? CCScreen_MemoryInfo : CCScreen_Program; _update_screen = true; }
				if(_buttons_new & CCButton_Prev) { if(_cycle_index) _cycle_index--; else _cycle_index = CCMemoryCountIndex.count - 1; memory_read(&_cycle, _cycle_index); _update_screen = true; }
				if(_buttons_new & CCButton_Next) { _cycle_index++; if(_cycle_index >= CCMemoryCountIndex.count) _cycle_index = 0; memory_read(&_cycle, _cycle_index); _update_screen = true; }
				if(_buttons_new & CCButton_Enter) { if(_screen == CCScreen_MemoryInfo) { _screen = CCScreen_MemoryValues; _update_screen = true; } }
				if(_screen == CCScreen_MemoryValues && _2s_change) _update_screen = true;
			}
			else if(_screen == CCScreen_StatePower) {
				if(_buttons_new & CCButton_Escape) { DBG_PutString_P(PSTR("User cancel."NL)); _state = CCState_Idle; _screen = CCScreen_Program; _update_screen = true; }
				if(_buttons_new & CCButton_Enter) { _screen = CCScreen_StatePowerAdditional; _update_screen = true; }
				if(_2s_change) _update_screen = true;
			}
			else if(_screen == CCScreen_StatePowerAdditional) {
				if(_buttons_new & CCButton_Escape) { _screen = CCScreen_StatePower; _update_screen = true; }
				if(_buttons_new & CCButton_Prev) { CCCalibrationWiremOhm--; _update_screen = true; }
				if(_buttons_new & CCButton_Next) { CCCalibrationWiremOhm++; _update_screen = true; }
				if(_buttons_new & CCButton_Enter) { calibartion_save(); }
			}
			else if(_screen == CCScreen_Calibration) {
				if(_buttons_new & CCButton_Escape) { _calibration_value++; if(_calibration_value == CCCalibrationValue__Last) _calibration_value = 0; _update_screen = true; }
				if(_buttons_new & CCButton_Prev) { if(_calibration_value == CCCalibrationValue_Voltage) CCCalibrationVoltageARef--; else if(_calibration_value == CCCalibrationValue_Current) CCCalibrationCurrentMvPerAmp--; else if(_calibration_value == CCCalibrationValue_CurrentOffset) CCCalibrationCurrentOffset--; _update_screen = true; }
				if(_buttons_new & CCButton_Next) { if(_calibration_value == CCCalibrationValue_Voltage) CCCalibrationVoltageARef++; else if(_calibration_value == CCCalibrationValue_Current) CCCalibrationCurrentMvPerAmp++; else if(_calibration_value == CCCalibrationValue_CurrentOffset) CCCalibrationCurrentOffset++; _update_screen = true; }
				if(_buttons_new & CCButton_Enter) { calibartion_save(); }
			}

			// Update screen info
			if(_update_screen) {
				if(_screen == CCScreen_Program) {
					ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("%-9S %ud cyc."), _program & CCProgram__MaskDischarge ? PSTR("Discharge") : PSTR("Charge"), (_program & CCProgram__MaskCount) + 1);
					ALCD_GoToXY(0, 1); ALCD_PutFormat_P(PSTR("%16S"), pgm_read_ptr(&_cell_info->text_name));
				}
				else if(_screen == CCScreen_MemoryInfo) {
					ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("Cycle %ud of %ud    "), _cycle_index, CCMemoryCountIndex.count);
					ALCD_GoToXY(0, 1); ALCD_PutFormat_P(PSTR("%16c"), ' ');
				}
				else if(_screen == CCScreen_MemoryValues) {
					ldiv_t _div_discharge_hours = ldiv((uint32_t)_cycle.discharge.seconds, 60 * 60);
					div_t _div_discharge_minutes = div(_div_discharge_hours.rem, 60);
					ldiv_t _div_charge_hours = ldiv((uint32_t)_cycle.charge.seconds, 60 * 60);
					div_t _div_charge_minutes = div(_div_charge_hours.rem, 60);
					ALCD_GoToXY(0, 0);
					if(_2s_bit) ALCD_PutFormat_P(PSTR("%6w2udW "), _cycle.discharge.cap_power); else ALCD_PutFormat_P(PSTR("%5w2udAh "), _cycle.discharge.cap_ah);
					ALCD_PutFormat_P(PSTR("%02ud:%02ud:%02ud"), (uint16_t)_div_discharge_hours.quot, _div_discharge_minutes.quot, _div_discharge_minutes.rem);
					ALCD_GoToXY(0, 1);
					if(_2s_bit) ALCD_PutFormat_P(PSTR("%6w2udW "), _cycle.charge.cap_power); else ALCD_PutFormat_P(PSTR("%5w2udAh "), _cycle.charge.cap_ah);
					ALCD_PutFormat_P(PSTR("%02ud:%02ud:%02ud"), (uint16_t)_div_charge_hours.quot, _div_charge_minutes.quot, _div_charge_minutes.rem);
				}
				else if(_screen == CCScreen_StatePower) {
					ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("%6w3udV%c%7w4tudA"), _2s_bit ? _values.sensed_voltage : _values.cell_voltage, _2s_bit ? 's' : ' ', _values.cell_current);
					ALCD_GoToXY(0, 1);
					ldiv_t _div_hours = ldiv(_state == CCState_Relieve ? PERIOD_RELIEVE - _seconds_elapsed : _seconds_elapsed, 60 * 60);
					div_t _div_minutes = div(_div_hours.rem, 60);
					if(_state == CCState_Relieve) { ALCD_PutString_P(PSTR("Relieve ")); }
					else { if(_2s_bit) ALCD_PutFormat_P(PSTR("%6w2udW "), (uint16_t)(_cell_cap_power / 720000000ULL)); else ALCD_PutFormat_P(PSTR("%5w2udAh "), (uint16_t)(_cell_cap_ah / 720000UL)); }
					ALCD_PutFormat_P((_ticks_elapsed & PERIOD_1S_HALF) ? PSTR("%02ud %02ud %02ud") : PSTR("%02ud:%02ud:%02ud"), (uint16_t)_div_hours.quot, _div_minutes.quot, _div_minutes.rem);
				}
				else if(_screen == CCScreen_StatePowerAdditional) {
					ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("%6w3udV %2d left"), _values.cell_voltage, _cycles);
					ALCD_GoToXY(0, 1); ALCD_PutFormat_P(PSTR("WireRes %3d mOhm"), CCCalibrationWiremOhm);
				}
				else if(_screen == CCScreen_Calibration) {
					ALCD_GoToXY(0, 0);
					ALCD_PutFormat_P(PSTR("%6w3udV%c% 6w4tdA"), _2s_bit ? _values.sensed_voltage : _values.cell_voltage, _2s_bit ? 's' : ' ', _signed_current);
					ALCD_GoToXY(0, 1);
					if(_calibration_value == CCCalibrationValue_Voltage) ALCD_PutFormat_P(PSTR("%4ud%12S"), CCCalibrationVoltageARef, PSTR("Voltage"));
					else if(_calibration_value == CCCalibrationValue_Current) ALCD_PutFormat_P(PSTR("%4ud%12S"), CCCalibrationCurrentMvPerAmp, PSTR("Current"));
					else if(_calibration_value == CCCalibrationValue_CurrentOffset) ALCD_PutFormat_P(PSTR("%4d%12S"), CCCalibrationCurrentOffset, PSTR("CurrentOff"));
				}
				_update_screen = false;
			}
		}

		wdt_reset();
	}
	__builtin_unreachable();
}
