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

#include "eta_adc.h"
#include "eta_crc.h"
#include "eta_pusart.h"

static void init() {
	pusart_init();
	DIDR0 = (1 << ADC1D);
	SW_SINK_DDR |= (1 << SW_LOW_SINK_PIN) | (1 << SW_HIGH_SINK_PIN);
	sei();
}

static uint8_t cmd_put(uint8_t crc, uint16_t data) { pusart_put_value(data); return crc8_update(crc, (uint8_t)data); }
static void cmd_send(uint8_t cmd, bool is_eeprom, void const *data, uint8_t data_len) {
	uint8_t _crc = 0;
	_crc = cmd_put(_crc, DEFAULT_ADDR | 0x100);
	_crc = cmd_put(_crc, cmd);
	_crc = cmd_put(_crc, data_len);
	uint8_t const *_data = (uint8_t const *)data;
	while(data_len) { _crc = cmd_put(_crc, is_eeprom ? eeprom_read_byte(_data) : *_data); _data++; data_len--; }
	cmd_put(_crc, _crc);
}

static void switch_sink(eSink sink) { SW_SINK_PORT = (SW_SINK_PORT & ~((1 << SW_LOW_SINK_PIN) | (1 << SW_HIGH_SINK_PIN))) | (sink & SINK__Mask); }

__attribute__((OS_main))
void main() {
	init();

	typedef enum {
		CMDStep_Stop,
		CMDStep_Command,
		CMDStep_Size,
		CMDStep_Data,
		CMDStep_CRC,
		CMDStep_Ready,
	} sCmdStep;

	struct {
		sCmdStep Step;
		uint8_t Cmd;
		uint8_t Size;
		uint8_t Index;
		uint8_t CRC;
		uint8_t Data[CMD_MAX_DATA_SIZE];
	} Cmd;

	Cmd.Step = CMDStep_Stop;

	while(true) {
		adc_init(ADCPS_128, ADCVR_INTERNAL, ADCTS_NONE, ADC_CHANNEL_BATTERY);

		while(true) {
//			if(pusart_is_value_available()) {
//				pusart_value_t _data =  pusart_get_value(); uint8_t _byte = (uint8_t)_data;
//				if((_data & 0x100)) { Cmd.Cmd = _byte; Cmd.CRC = 0; Cmd.Step = (Cmd.Cmd & CMD_ADDR_MASK) == (DEFAULT_ADDR << CMD_ADDR_SHIFT) ? CMDStep_Size : CMDStep_Stop; }
//				else if(Cmd.Step == CMDStep_Size) { if(_byte > CMD_MAX_DATA_SIZE) Cmd.Step = CMDStep_Stop; else { Cmd.Size = _byte; Cmd.Index = 0; Cmd.Step = Cmd.Size ? CMDStep_Data : CMDStep_CRC; } }
//				else if(Cmd.Step == CMDStep_Data) { Cmd.Data[Cmd.Index++] = _byte; if(Cmd.Index >= Cmd.Size) Cmd.Step = CMDStep_CRC; }
//				else if(Cmd.Step == CMDStep_CRC) { Cmd.Step = Cmd.CRC == _byte ? CMDStep_Ready : CMDStep_Stop; }
//				Cmd.CRC = crc8_update(Cmd.CRC, _byte);
//			}
//
//			if(Cmd.Step == CMDStep_Ready) {
//				cmd_send(Cmd.Cmd, false, Cmd.Data, Cmd.Size);
//				Cmd.Step = CMDStep_Stop;
//			}

			// PUSART Command Receiver
			if(pusart_is_value_available()) {
				pusart_value_t _data =  pusart_get_value(); uint8_t _byte = (uint8_t)_data;
				if((_data & 0x100)) { Cmd.CRC = 0; Cmd.Step = _byte == DEFAULT_ADDR ? CMDStep_Command : CMDStep_Stop; }
				else if(Cmd.Step == CMDStep_Command) { Cmd.Cmd = _byte; Cmd.Step = CMDStep_Size; }
				else if(Cmd.Step == CMDStep_Size) { if(_byte > CMD_MAX_DATA_SIZE) Cmd.Step = CMDStep_Stop; else { Cmd.Size = _byte; Cmd.Index = 0; Cmd.Step = Cmd.Size ? CMDStep_Data : CMDStep_CRC; } }
				else if(Cmd.Step == CMDStep_Data) { Cmd.Data[Cmd.Index++] = _byte; if(Cmd.Index >= Cmd.Size) Cmd.Step = CMDStep_CRC; }
				else if(Cmd.Step == CMDStep_CRC) { Cmd.Step = Cmd.CRC == _byte ? CMDStep_Ready : CMDStep_Stop; }
				Cmd.CRC = crc8_update(Cmd.CRC, _byte);
			}

			// Command Processor
			if(Cmd.Step == CMDStep_Ready) {
				uint8_t _addr = (Cmd.Cmd & CMD_SUB_MASK) << (CMD_MAX_DATA_SIZE_SHIFT - CMD_SUB_SHIFT);

				// Write to user EEPROM
				if((Cmd.Cmd & CMD_CMD_MASK) == (CMD_WriteEEPROM << CMD_CMD_SHIFT) && Cmd.Size == sizeof(sEEPROM)) {
					eeprom_write_block(Cmd.Data, (void *)(uint16_t)_addr, sizeof(sEEPROM));
					cmd_send(Cmd.Cmd, false, nullptr, 0);
				}
				// Read from user EEPROM
				else if((Cmd.Cmd & CMD_CMD_MASK) == (CMD_ReadEEPROM << CMD_CMD_SHIFT) && !Cmd.Size) {
					cmd_send(Cmd.Cmd, true, (void *)(uint16_t)_addr, sizeof(sEEPROM));
				}
				// Get ADC value
				else if((Cmd.Cmd & CMD_CMD_MASK) == (CMD_GetVoltage << CMD_CMD_SHIFT) && !Cmd.Size) {
					uint16_t _adc = 0;
					for(uint8_t _adc_repeat = (1 << ADC_REPEAT_SHIFT); _adc_repeat; _adc_repeat--) { adc_start(); _adc += adc_get_value(); }
					cmd_send(Cmd.Cmd, false, &_adc, sizeof(uint16_t));
				}
				// Switch sinking
				else if((Cmd.Cmd & CMD_CMD_MASK) == (CMD_SwitchSink << CMD_CMD_SHIFT) && Cmd.Size == 1) {
					switch_sink(Cmd.Data[0]);
					cmd_send(Cmd.Cmd, false, nullptr, 0);
				}

				Cmd.Step = CMDStep_Stop;
			}

			// Power down timeout
			if(pusart_idle_counter_get() >= PERIOD_IDLE_POWERDOWN_TIMEOUT) {
				switch_sink(0); adc_off();
				MCUCR = (0 << ISC00);
				set_sleep_mode(SLEEP_MODE_PWR_DOWN); sleep_mode();
				break;
			}
		}
	}

	__builtin_unreachable();
}
