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

#include "eta_dbg.h"
#include "eta_crc.h"
#include "eta_frames.h"
#include "eta_spi.h"
#include "eta_hd44780.h"
#include "eta_ade7753.h"

static sADE7753_Config const PROGMEM ADE7753_Config = {
	.MODE = ADE7753Mode_CYCMODE | ADE7753Mode_DTRT_128 | ADE7753Mode_WAVSEL_ACTPWR,
	.IRQEN = ADE7753Interrupt_SAG | ADE7753Interrupt_CYCEND | ADE7753Interrupt_ZX | ADE7753Interrupt_TEMP | ADE7753Interrupt_PKV | ADE7753Interrupt_PKI,
	.GAIN = ADE7753Gain_I_PGA_1 | ADE7753Gain_i_FullScale_250 | ADE7753Gain_V_PGA_1,
	.LINECYC = 50,
	.ZXTOUT = 0xFFF,
	.SAGCYC = 0xFF,
};
static sADE7753_Calibration const PROGMEM ADE7753_CalibrationDefault = {
	.CH1OS = 0,
	.CH2OS = 0,
	.PHCAL = 0x0D,
	.APOS = 0,
	.WGAIN = 0x0,
	.WDIV = 0,
	.CFNUM = 0x3F,
	.CFDEN = 0x3F,
	.IRMSOS = 0,
	.VRMSOS = 0,
	.VAGAIN = 0,
	.VADIV = 0,
	.SAGLVL = 0,
	.IPKLVL = 0xFF,
	.VPKLVL = 0xFF,
};
static uint16_t EEMEM ADE7753_CalibrationCRC16;
static sADE7753_Calibration EEMEM ADE7753_CalibrationEEPROM;
static sADE7753_Calibration ADE7753_Calibration;

struct {
	uint8_t UpdateCycValues:1;
	uint8_t UpdateTempValue:1;
	uint8_t RecordWaveform:1;
	uint8_t WaveformReady:1;
} volatile ACPM_Flags;

static uint24_t volatile ACPM_ISR_CurrentRMS;
static uint24_t volatile ACPM_ISR_VoltageRMS;

static sACPM_ADEValues ACPM_ADEValues;

static uint16_t ACPM_Waveform[ACPM_MAX_WAVEFORM_SAMPLES];
static uint16_t ACPM_WaveformIdx = ACPM_MAX_WAVEFORM_SAMPLES;
static int8_t ACPM_WaveformScale;

static uint32_t volatile ACPM_GlobalTicks;
ISR(TIMER1_COMPA_vect) { ACPM_GlobalTicks++; }

ISR(INT0_vect) {
	static bool _b_zx;

	eADE7753_Interrupt _int = ADE7753_Read2_ISR(ADE7753Register_RSTSTATUS);
	if(_int & ADE7753Interrupt_WSMP) {
		if(ACPM_WaveformIdx < ACPM_MAX_WAVEFORM_SAMPLES) {
			int24_t _sample = (int24_t)(ADE7753_ReadFast3_ISR(ADE7753Register_WAVEFORM) >> ACPM_WaveformScale);
			ACPM_Waveform[ACPM_WaveformIdx++] = (_sample > INT16_MAX) ? INT16_MAX : ((_sample < INT16_MIN) ? INT16_MIN : (int16_t)(_sample));
		}
		else { ACPM_Flags.WaveformReady = true; ADE7753_Write2_ISR(ADE7753Register_IRQEN, ADE7753_Read2_ISR(ADE7753Register_IRQEN) & ~ADE7753Interrupt_WSMP); }
	}
	if(_int & ADE7753Interrupt_ZX) {
		if(!_b_zx) _b_zx = true; else { _b_zx = false; if(ACPM_Flags.RecordWaveform) { ACPM_Flags.RecordWaveform = ACPM_Flags.WaveformReady = false; ACPM_WaveformIdx = 0; ADE7753_Write2_ISR(ADE7753Register_IRQEN, ADE7753_Read2_ISR(ADE7753Register_IRQEN) | ADE7753Interrupt_WSMP); } }
		if(ACPM_WaveformIdx >= ACPM_MAX_WAVEFORM_SAMPLES) {
			ACPM_ISR_CurrentRMS = ADE7753_Read3_ISR(ADE7753Register_IRMS); ACPM_ISR_VoltageRMS = ADE7753_Read3_ISR(ADE7753Register_VRMS);
		}
	}
	if(_int & ADE7753Interrupt_CYCEND) {
		ACPM_Flags.UpdateCycValues = true;
	}
	if(_int & ADE7753Interrupt_TEMP) {
		ACPM_Flags.UpdateTempValue = true;
	}
}

static void calibration_save() {
	eeprom_write_block(&ADE7753_Calibration, &ADE7753_CalibrationEEPROM, sizeof(sADE7753_Calibration));
	eeprom_write_word(&ADE7753_CalibrationCRC16, crc16(0, &ADE7753_Calibration, sizeof(sADE7753_Calibration)));
	DBG_PutString_P(PSTR("Calibration saved"NL));
}
static void calibration_load() {
	eeprom_read_block(&ADE7753_Calibration, &ADE7753_CalibrationEEPROM, sizeof(sADE7753_Calibration));
	if(eeprom_read_word(&ADE7753_CalibrationCRC16) != crc16(0, &ADE7753_Calibration, sizeof(sADE7753_Calibration))) {
		memcpy_P(&ADE7753_Calibration, &ADE7753_CalibrationDefault, sizeof(sADE7753_Calibration));
		DBG_PutString_P(PSTR("Calibration defaulted"NL));
		calibration_save();
	}
	else DBG_PutString_P(PSTR("Calibration loaded"NL));
}

static void init() {
	PORTB = 0x04; DDRB = 0x2C;
	PORTC = 0x00; DDRC = 0x03;
	PORTD = 0x1C; DDRD = 0x20;

	// Tick timer every 1/32 sec
	OCR1A = (F_CPU / 8 / PERIOD_1S) - 1; TCCR1B = (1 << CS11) | (1 << WGM12); TIMSK1 = (1 << OCIE1A); TIFR1 = (1 << OCF1A);

	// INT0 low level
	EICRA = 0; EIMSK = (1 << INT0);

	usartinit_init();
	SPI_Init(true, false, SPICfg_CPOL0_CPHA1, SPIPrescaler_2);
	ALCD_Init();
	ALCD_Clear();
	ALCD_GoToXY(1, 0); ALCD_PutFormat_P(PSTR("Initializing..."));
	calibration_load();
	ADE7753_Init_ISR(&ADE7753_Config, &ADE7753_Calibration);

	sei();

	DBG_PutString_P(PSTR("-= AC Power Metter =-"NL));
	DBG_PutFormat_P(PSTR("ADE DIE revision: %d"NL), ADE7753_Read1(ADE7753Register_DIEREV));
}

__attribute__((OS_main))
void main() {
	init();

	eACPM_Screen _screen = ACPMScreen_Generic;
	bool _prev_btn_function1 = false, _prev_btn_function2 = false, _update_screen = true, _values_updated = false;
	struct {
		uint16_t CurrentRMS, VoltageRMS, Frequency, PFC;
		int8_t Temp;
	} _locals;

	ALCD_Clear();
	ADE7753_Read3(ADE7753Register_RAENERGY); ADE7753_Read3(ADE7753Register_RVAENERGY);
	ACPM_Flags.UpdateCycValues = true;

	memset(&_locals, 0, sizeof(_locals));
	while(true) {
		uint8_t _global_ticks_now = (uint8_t)ACPM_GlobalTicks;

		if(_screen == ACPMScreen_Counter && !(_global_ticks_now & (PERIOD_1S_HALF - 1))) _update_screen = true;

		bool _btn_function1 = !(BTN_Function1_PINPORT & (1 << BTN_Function1_PIN)), _btn_function2 = !(BTN_Function2_PINPORT & (1 << BTN_Function2_PIN));
		if(_prev_btn_function1 != _btn_function1) { if((_prev_btn_function1 = _btn_function1)) { if(!_screen) _screen = ACPMScreen__Last; _screen--; _update_screen = true; } }
		else if(_prev_btn_function2 != _btn_function2) { if((_prev_btn_function2 = _btn_function2)) { _screen++; if(_screen == ACPMScreen__Last) _screen = (eACPM_Screen)0; _update_screen = true; } }

		// Update Interrupt requested values
		if(ACPM_Flags.UpdateCycValues) {
			cli(); ACPM_Flags.UpdateCycValues = false; sei();
			cli(); ACPM_ADEValues.CurrentRMS = ACPM_ISR_CurrentRMS; sei();
			cli(); ACPM_ADEValues.VoltageRMS = ACPM_ISR_VoltageRMS; sei();
			ACPM_ADEValues.EnergyActive += (int24_t)ADE7753_Read3(ADE7753Register_RAENERGY); ACPM_ADEValues.EnergyApparent += ADE7753_Read3(ADE7753Register_RVAENERGY);
			ACPM_ADEValues.Period = ADE7753_Read2(ADE7753Register_PERIOD);
			ACPM_ADEValues.LinePowerActive = (int24_t)ADE7753_Read3(ADE7753Register_LAENERGY); ACPM_ADEValues.LinePowerApparent = ADE7753_Read3(ADE7753Register_LVAENERGY); ACPM_ADEValues.LinePowerReactive = (int24_t)ADE7753_Read3(ADE7753Register_LVARENERGY);
			ACPM_ADEValues.CurrentPeak = ADE7753_Read3_ISR(ADE7753Register_RSTIPEAK); ACPM_ADEValues.VoltagePeak = ADE7753_Read3_ISR(ADE7753Register_RSTVPEAK);
			_locals.CurrentRMS = (uint16_t)((uint32_t)ACPM_ADEValues.CurrentRMS * ACPM_INPUT_DIV / ACPM_ADE_IRMS_DIVIDOR); _locals.VoltageRMS = (uint16_t)((uint32_t)ACPM_ADEValues.VoltageRMS * ACPM_INPUT_DIV / ACPM_ADE_VRMS_DIVIDOR);
			_locals.Frequency = (ACPM_ADEValues.Period > 5000 && ACPM_ADEValues.Period < 10000) ? (ACPM_ADE_PERIOD_NOMINATOR / ACPM_ADEValues.Period) : 0;
			/// *****************
			_locals.CurrentRMS = 18435; _locals.VoltageRMS = 220;
			ACPM_ADEValues.EnergyActive = -72345; ACPM_ADEValues.EnergyApparent = 84567;
			ACPM_ADEValues.LinePowerActive = -1237; ACPM_ADEValues.LinePowerApparent = 3345; ACPM_ADEValues.LinePowerReactive = -2345;
			_locals.Frequency = 501; _locals.Temp = 28;
			/// *****************
			_locals.PFC = ACPM_ADEValues.LinePowerApparent ? ((uint16_t)(abs(ACPM_ADEValues.LinePowerActive) * 10000 / ACPM_ADEValues.LinePowerApparent)) : 0;
			_values_updated = true;
		}
		if(ACPM_Flags.UpdateTempValue) {
			cli(); ACPM_Flags.UpdateTempValue = false; sei();
			ACPM_ADEValues.Temp = (int8_t)ADE7753_Read1(ADE7753Register_TEMP);
			_locals.Temp = (((ACPM_ADEValues.Temp) << 1) / 3) - 25;
			_values_updated = true;
		}

		if(_values_updated) _update_screen = true;

		// Update screen info
		if(_update_screen) {
			if(_screen == ACPMScreen_Generic) {
				ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("%3dV "), _locals.VoltageRMS);
				if(_locals.Frequency) { ALCD_PutFormat_P(PSTR("%4w1dHz "), _locals.Frequency); } else { ALCD_PutString_P(PSTR("--.-Hz ")); }
				if(ACPM_ADEValues.Temp) { ALCD_PutFormat_P(PSTR("% 3d "), _locals.Temp); } else { ALCD_PutString_P(PSTR(" -- ")); }
				ALCD_GoToXY(0, 1); ALCD_PutFormat_P(PSTR("%6w3dA% 8w2ldW"), _locals.CurrentRMS, (int32_t)ACPM_ADEValues.LinePowerActive);
			}
			else if(_screen == ACPMScreen_PFC) {
				ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("%6w3dA% 8w2ldW"), _locals.CurrentRMS, (int32_t)ACPM_ADEValues.LinePowerActive);
				ALCD_GoToXY(0, 1); ALCD_PutFormat_P(PSTR("  PFC: %6w2d%%  "), _locals.PFC);
			}
			else if(_screen == ACPMScreen_ApparentReactive) {
				ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("%6w3dA% 8w2ldW"), _locals.CurrentRMS, (uint32_t)ACPM_ADEValues.LinePowerApparent);
				ALCD_GoToXY(0, 1); ALCD_PutFormat_P(PSTR("%6w2d%%% 8w2ldW"), _locals.PFC, (int32_t)ACPM_ADEValues.LinePowerReactive);
			}
			else if(_screen == ACPMScreen_Counter) {
				cli(); int32_t _global_ticks_tmp = ACPM_GlobalTicks; sei();
				ldiv_t _div_days = ldiv(_global_ticks_tmp, 60UL * 60 * 24 * PERIOD_1S);
				ldiv_t _div_hours = ldiv(_div_days.rem, 60UL * 60 * PERIOD_1S);
				ldiv_t _div_minutes = ldiv(_div_hours.rem, 60 * PERIOD_1S);
				ALCD_GoToXY(0, 0); ALCD_PutFormat_P(PSTR("%6ldW% 8w2ldW"), (int32_t)ACPM_ADEValues.EnergyActive, (int32_t)ACPM_ADEValues.LinePowerActive);
				ALCD_GoToXY(0, 1); ALCD_PutFormat_P((_global_ticks_now & PERIOD_1S_HALF) ? PSTR("%2ddays %02d %02d %02d ") : PSTR("%2ddays %02d:%02d:%02d "), (int)_div_days.quot, (int)_div_hours.quot, (int)_div_minutes.quot, (((int)_div_minutes.rem) >> PERIOD_1S_SHIFT));
			}
			_update_screen = false;
		}

		// Check temp
		if(!(_global_ticks_now & ((PERIOD_1S << 2) - 1))) { ADE7753_Write2(ADE7753Register_MODE, ADE7753_Read2(ADE7753Register_MODE) | ADE7753Mode_TEMPSEL); }

		// Process frames
		if(frames_is_frame_available()) {
			eACPM_Command _frame_cmd = (eACPM_Command)frames_get_frame_command();
			uint8_t _frame_size = frames_get_frame_size();
			uint8_t _frame_data[_frame_size];
			frames_get_frame_data(_frame_data, 0, _frame_size);
			frames_remove_frame();

			if(_frame_cmd == ACPMCommand_ReadValues && !_frame_size) {
				_values_updated = false;
				cli(); ACPM_ADEValues.GlobalTicks = ACPM_GlobalTicks; sei();
				frames_put_frame(FRAME_ADDRESS, ACPMCommand_ReadValues, &ACPM_ADEValues, sizeof(sACPM_ADEValues));
			}
			else if(_frame_cmd == ACPMCommand_WaveformRequest && _frame_size == 3) {
				eADE7753_Mode _mode_this = ADE7753_Read2(ADE7753Register_MODE) & ~(ADE7753Mode_DTRT__MASK | ADE7753Mode_WAVSEL__MASK);
				_mode_this |= ((_frame_data[0] << ADE7753Mode_DTRT__SHIFT) & ADE7753Mode_DTRT__MASK) | ((_frame_data[1] << ADE7753Mode_WAVSEL__SHIFT) & ADE7753Mode_WAVSEL__MASK);
				ACPM_WaveformScale = _frame_data[3];
				ADE7753_Write2(ADE7753Register_MODE, _mode_this);
				cli(); ACPM_Flags.RecordWaveform = true; sei();
			}
			else if(_frame_cmd == ACPMCommand_WaveformPage && _frame_size == 1) {
				uint8_t _waveform_page = _frame_data[0];
				if(_waveform_page < ACPM_MAX_WAVEFORM_SAMPLES_PAGES) { frames_put_frame(FRAME_ADDRESS, ACPMCommand_WaveformPage, &ACPM_Waveform[(_waveform_page << ACPM_MAX_WAVEFORM_SAMPLES_PART_SHIFT)], ACPM_MAX_WAVEFORM_SAMPLES_PART << 1); }
			}
			else if(_frame_cmd == ACPMCommand_ReadCalibration && !_frame_size) {
				frames_put_frame(FRAME_ADDRESS, ACPMCommand_ReadCalibration, &ADE7753_Calibration, sizeof(sADE7753_Calibration));
			}
			else if(_frame_cmd == ACPMCommand_WriteCalibration && _frame_size == sizeof(sADE7753_Calibration)) {
				memcpy(&ADE7753_Calibration, _frame_data, sizeof(sADE7753_Calibration));
				ADE7753_InitCalibration(&ADE7753_Calibration);
				frames_put_frame(FRAME_ADDRESS, ACPMCommand_WriteCalibration, nullptr, 0);
			}
			else if(_frame_cmd == ACPMCommand_SaveCalibration && !_frame_size) {
				calibration_save();
				frames_put_frame(FRAME_ADDRESS, ACPMCommand_SaveCalibration, nullptr, 0);
			}
			else if(_frame_cmd == ACPMCommand_Reset && !_frame_size) {
				frames_put_frame(FRAME_ADDRESS, ACPMCommand_Reset, nullptr, 0); cli(); _delay_ms(10); frames_flush();
				__asm("jmp 0\n\t");
			}
		}
		if(ACPM_Flags.WaveformReady && !ACPM_Flags.RecordWaveform) { cli(); ACPM_Flags.WaveformReady = false; sei(); frames_put_frame(FRAME_ADDRESS, ACPMCommand_WaveformRequest, nullptr, 0); }
		if(_values_updated && frames_is_transmitter_free()) {
			_values_updated = false;
			cli(); ACPM_ADEValues.GlobalTicks = ACPM_GlobalTicks; sei();
			frames_put_frame(FRAME_ADDRESS, ACPMCommand_ValuesUpdated, &ACPM_ADEValues, sizeof(sACPM_ADEValues));
		}
	}
	__builtin_unreachable();
}
