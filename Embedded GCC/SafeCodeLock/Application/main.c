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

#include "eta_adc.h"
#include "eta_crc.h"
#include "eta_usart.h"
#include "eta_dbg.h"

uint16_t EEMEM SettingsEEMEMCRC16;
uint16_t EEMEM PenaltiesEEMEMCRC16;
sSettings EEMEM SettingsEEMEM;
sPenalties EEMEM PenaltiesEEMEM;
sSettings Settings;
sPenalties Penalties;

uint8_t volatile current_beep_config, prev_beep_config;
static void process_beep() {
	uint8_t _prev_beep_config_tmp = current_beep_config & 1;
	if(prev_beep_config != _prev_beep_config_tmp) {
		prev_beep_config = _prev_beep_config_tmp;
		if(_prev_beep_config_tmp) { DDRD |= 0x40; } else { PORTD &= ~0x40; DDRD &= ~0x40; }
	}
	current_beep_config = current_beep_config >> 1;
}

uint8_t volatile global_ticks;
uint8_t volatile current_gate_ticks;
ISR(TIMER1_COMPA_vect) {
	global_ticks++;
	if(current_gate_ticks) { if(!(--current_gate_ticks)) { PORTD &= ~0x08; DDRD &= ~0x08; } }
	if(!(global_ticks & 1)) process_beep();
	PORTD &= ~0x80; DDRD &= ~0x80;
}

static void beep(uint8_t beep_config) { current_beep_config = beep_config; }
static void gate(uint8_t gate_ticks) { current_gate_ticks = gate_ticks; DDRD |= 0x08; }

static void settings_set_defaults() {
	static char const PROGMEM __default_password[] = { SAFE_PASSWORD_DEFAULT };
	memcpy_P(Settings.Password, __default_password, (Settings.PasswordLength = sizeof_array(__default_password)));
	DBG_PutString_P(PSTR("Settings set to defaults"NL));
	beep(BEEPCFG_EEPROM_FAILED);
}
static bool settings_sanity_check() {
	if(Settings.PasswordLength >= SAFE_PASSWORD_LENGTH_MIN && Settings.PasswordLength <= SAFE_PASSWORD_LENGTH_MAX) {
		uint8_t _password_length = Settings.PasswordLength;
		char *_password_ptr = Settings.Password;
		while(_password_length) { char _c = *_password_ptr++; if(_c < '0' || _c > '9') { break; } _password_length--; }
		if(!_password_length) return true;
	}
	DBG_PutString_P(PSTR("Settings sanity failed"NL));
	return false;
}
static void settings_save() {
	uint16_t SettingsCRC16;
	eeprom_write_word(&SettingsEEMEMCRC16, (SettingsCRC16 = crc16(0xFFFF, &Settings, sizeof(sSettings))));
	eeprom_write_block(&Settings, &SettingsEEMEM, sizeof(sSettings));
	DBG_PutString_P(PSTR("Settings saved"NL));
}
static void settings_load() {
	eeprom_read_block(&Settings, &SettingsEEMEM, sizeof(sSettings));
	if(eeprom_read_word(&SettingsEEMEMCRC16) != crc16(0xFFFF, &Settings, sizeof(sSettings)) || !settings_sanity_check()) {
		settings_set_defaults();
		settings_save();
	}
	DBG_PutString_P(PSTR("Settings loaded"NL));
}

static void penalties_set_defaults() {
	Penalties.PenaltyTicks = 0; Penalties.WrongPasswordsEntered = 0;
	DBG_PutString_P(PSTR("Penalties set to defaults"NL));
	beep(BEEPCFG_EEPROM_FAILED);
}
static bool penalties_sanity_check() {
	if(Penalties.PenaltyTicks <= PENALTY_TICKS && Penalties.WrongPasswordsEntered < PENALTY_MAX_WRONG_PASSWORDS) {
		if(!Penalties.PenaltyTicks || !Penalties.WrongPasswordsEntered) return true;
	}
	DBG_PutString_P(PSTR("Penalties sanity failed"NL));
	return false;
}
static void penalties_save() {
	uint16_t PenaltiesCRC16;
	eeprom_write_word(&PenaltiesEEMEMCRC16, (PenaltiesCRC16 = crc16(0xFFFF, &Penalties, sizeof(sPenalties))));
	eeprom_write_block(&Penalties, &PenaltiesEEMEM, sizeof(sPenalties));
	DBG_PutString_P(PSTR("Penalties saved"NL));
}
static void penalties_load() {
	eeprom_read_block(&Penalties, &PenaltiesEEMEM, sizeof(sPenalties));
	if(eeprom_read_word(&PenaltiesEEMEMCRC16) != crc16(0xFFFF, &Penalties, sizeof(sPenalties)) || !penalties_sanity_check()) {
		penalties_set_defaults();
		penalties_save();
	}
	DBG_PutString_P(PSTR("Penalties loaded"NL));
}
static void penalties_reset() {
	if(Penalties.WrongPasswordsEntered) { Penalties.WrongPasswordsEntered = 0; penalties_save(); }
}

static bool init() {
	//frames_init();
	usartinit_init();
	adc_init(ADCPS_2, ADCVR_AVCC, ADCTS_NONE, 0);

	MCUCR |= (1 << PUD); ACSR = (1 << ACD);
	DDRB = 0x03; PORTB = 0xFF; DDRD = 0x00; PORTD = 0x30; EICRA = 0x00; EIMSK &= ~(1 << INT0);
	DIDR0 = 0x3F; DIDR1 = 0x00;

	// Beep timer CTC mode
	OCR0A = 16; TCCR0A = (1 << COM0A0) | (2 << WGM00); TCCR0B = (1 << CS00);
	// Tick timer every 100ms
	OCR1A = (F_CPU / PERIOD_1S) - 1; TCCR1B = (1 << CS10) | (1 << WGM12); TIMSK1 = (1 << OCIE1A); TIFR1 = (1 << OCF1A);
	// Gate timer fast PWM
	OCR2B = GATE_PWM_DUTY_8BIT; TCCR2A = (2 << COM2B0) | (3 << WGM20); TCCR2B = (1 << CS20);

	sei();

	DBG_PutFormat_P(PSTR("Safe M-Locks Controller %S (Eta8 %S)"NL), PSTR(VER_APP), PSTR(VER_ETA8));
	settings_load(); penalties_load();

	while(global_ticks < PERIOD_BATTERY_MONITOR_LOAD);
	adc_mux(1); adc_start(); adcvalue_t _battery_adc = adc_get_value(); PORTB &= ~0x02; adc_mux(0);
	DBG_PutFormat_P(PSTR("Battery adc: %u is "), _battery_adc);
	if(_battery_adc < (adcvalue_t)BATTERY_ADC_CUTOFF) { DBG_PutString_P(PSTR("Low"NL)); return true; } else { DBG_PutString_P(PSTR("OK"NL)); return false; }
}

static char try_read_key() {
	adc_start(); adcvalue_t _adc_keypad = adc_get_value();
	if(_adc_keypad <= KEYMID_01) return '0';
	if(_adc_keypad <= KEYMID_12) return '1';
	if(_adc_keypad <= KEYMID_23) return '2';
	if(_adc_keypad <= KEYMID_34) return '3';
	if(_adc_keypad <= KEYMID_45) return '4';
	if(_adc_keypad <= KEYMID_56) return '5';
	if(_adc_keypad <= KEYMID_67) return '6';
	if(_adc_keypad <= KEYMID_78) return '7';
	if(_adc_keypad <= KEYMID_89) return '8';
	if(_adc_keypad <= KEYMID_9STAR) return '9';
	if(_adc_keypad <= KEYMID_STARCHARP) return '*';
	if(_adc_keypad <= KEYMID_CUTOFF) return '#';
	return 0;
}

typedef enum {
	MODE_NORMAL,
	MODE_OLD_PASSWORD,
	MODE_NEW_PASSWORD,
	MODE_REENTER_PASSWORD1,
	MODE_REENTER_PASSWORD2,
} eMode;

typedef enum {
	KEYFLAG_SHORT	= (1 << 0),
	KEYFLAG_LONG	= (1 << 1),
} eKeyFlags;

__attribute__((OS_main))
void main() {
	static uint8_t _key_press_beep_cfg = BEEPCFG_KEYPRESS;
	static eMode _mode = MODE_NORMAL;
	static uint8_t _period_power_down = PERIOD_POWER_DOWN;
	static char _current_password[SAFE_PASSWORD_LENGTH_MAX + 1];
	static uint8_t _current_password_len;
	static bit _already_wrong_password = false;
	static uint8_t _timein_powerdown; static uint8_t _timein_penalty_tick;
	static uint8_t _timein_key; static char _key_prev; static eKeyFlags _key_flags;

	static uint8_t _global_ticks_tmp;

	if(init()) _key_press_beep_cfg = BEEPCFG_KEYPRESS_LOW_BATTERY;

	while(true) {
		_global_ticks_tmp = global_ticks;

		// Penalty ticks handling
		if((uint8_t)(_global_ticks_tmp - _timein_penalty_tick) >= PERIOD_PENALTY_ONETICK) {
			_timein_penalty_tick = _global_ticks_tmp;
			if(Penalties.PenaltyTicks) {
				Penalties.PenaltyTicks--; DDRD |= 0x80; PORTD |= 0x80;
				DBG_PutFormat_P(PSTR("- penalty ticks left: %u"NL), Penalties.PenaltyTicks);
				penalties_save();
			}
		}

		// KeyPad handling
		char _key = try_read_key();
		if(_key_prev != _key) {
			if(_key_prev) _key_prev = 0;
			else { _key_prev = _key; _timein_key = _global_ticks_tmp; _key_flags = (eKeyFlags)0; }
		}
		else if(_key) {
			uint8_t _key_ticks_diff = _global_ticks_tmp - _timein_key;
			if(_key_ticks_diff == PERIOD_KEY_SHORT && !(_key_flags & KEYFLAG_SHORT)) {
				_key_flags |= KEYFLAG_SHORT; _timein_powerdown = _global_ticks_tmp;
				DBG_PutFormat_P(PSTR("- key short %c"NL), _key);
				if(Penalties.PenaltyTicks) beep(BEEPCFG_PASSWORD_WRONG);
				else if(_key == '*') {
					beep(_key_press_beep_cfg);
					DBG_PutString_P(PSTR("Over again"NL));
					_current_password_len = 0; _already_wrong_password = false;
				}
				else if(_key == '#') {
					_current_password[_current_password_len] = 0;
					DBG_PutFormat_P(PSTR("Password \"%s\" "), _current_password);
					// normal password check
					if(_mode == MODE_OLD_PASSWORD || _mode == MODE_NORMAL || _mode == MODE_REENTER_PASSWORD1 || _mode == MODE_REENTER_PASSWORD2) {
						#ifdef SAFE_PASSWORD_MASTER
						static char const PROGMEM __master_password[] = { SAFE_PASSWORD_MASTER };
						#endif
						if(!_already_wrong_password && _current_password_len == Settings.PasswordLength && !memcmp(_current_password, Settings.Password, _current_password_len)) {
							if(_mode == MODE_OLD_PASSWORD) {
								beep(BEEPCFG_PASSWORD_OK);
								DBG_PutString_P(PSTR("confirmed old. Waiting for new one"NL));
								_mode = MODE_NEW_PASSWORD;
							}
							else if(_mode == MODE_NORMAL) {
								beep(BEEPCFG_PASSWORD_OK);
								DBG_PutString_P(PSTR("OK"NL));
								penalties_reset();
								gate(PERIOD_GATE);
							}
							else if(_mode == MODE_REENTER_PASSWORD1) {
								beep(BEEPCFG_PASSWORD_NEW_OK);
								DBG_PutString_P(PSTR("confirmed 1"NL));
								_mode = MODE_REENTER_PASSWORD2;
							}
							else if(_mode == MODE_REENTER_PASSWORD2) {
								beep(BEEPCFG_PASSWORD_CHANGED);
								DBG_PutString_P(PSTR("confirmed 2 and successfully changed"NL));
							_SavePassword:;
								settings_save(); penalties_reset();
								_mode = MODE_NORMAL; _period_power_down = PERIOD_POWER_DOWN;
							}
						}
						#ifdef SAFE_PASSWORD_MASTER
						else if(_mode == MODE_OLD_PASSWORD && _current_password_len == sizeof_array(__master_password) && !memcmp_P(_current_password, __master_password, _current_password_len)) {
							DBG_PutString_P(PSTR("MASTERed to default"NL));
							settings_set_defaults();
							goto _SavePassword;
						}
						#endif
						else {
							DBG_PutString_P(PSTR("wrong"NL));
							beep(BEEPCFG_PASSWORD_WRONG);
							if(_mode == MODE_NORMAL || _mode == MODE_OLD_PASSWORD) {
								if(++Penalties.WrongPasswordsEntered >= PENALTY_MAX_WRONG_PASSWORDS) {
									Penalties.WrongPasswordsEntered = 0; Penalties.PenaltyTicks = PENALTY_TICKS;
									DBG_PutString_P(PSTR("Too many wrong passwords entered!"NL));
								}
								penalties_save();
							}
							if(_mode != MODE_NORMAL) goto _DiscardAndExitSettingMode;
						}
					}
					// password changing
					else if(_mode == MODE_NEW_PASSWORD) {
						if(!_already_wrong_password && _current_password_len >= SAFE_PASSWORD_LENGTH_MIN) {
							beep(BEEPCFG_PASSWORD_NEW_OK);
							DBG_PutString_P(PSTR("accepted as new"NL));
							memcpy(Settings.Password, _current_password, (Settings.PasswordLength = _current_password_len));
							_mode = MODE_REENTER_PASSWORD1;
						}
						else {
							beep(BEEPCFG_PASSWORD_WRONG);
							DBG_PutString_P(PSTR("invalid as new"NL));
							goto _DiscardAndExitSettingMode;
						}
					}
					_current_password_len = 0; _already_wrong_password = false;
				}
				else if(_key >= '0' && _key <= '9') {
					beep(_key_press_beep_cfg);
					if(_current_password_len >= SAFE_PASSWORD_LENGTH_MAX) _already_wrong_password = true;
					else _current_password[_current_password_len++] = _key;
				}
				_key_press_beep_cfg = BEEPCFG_KEYPRESS;
			}
			else if(!Penalties.PenaltyTicks && _key_ticks_diff == PERIOD_KEY_LONG && !(_key_flags & KEYFLAG_LONG)) {
				_key_flags |= KEYFLAG_LONG; _timein_powerdown = _global_ticks_tmp;
				DBG_PutFormat_P(PSTR("- key long %c"NL), _key);
				if(_key == '*') {
					if(_mode == MODE_NORMAL) {
						beep(BEEPCFG_PASSWORD_CHANGE);
						DBG_PutString_P(PSTR("Password change. Waiting for old password"NL));
						_mode = MODE_OLD_PASSWORD; _period_power_down = PERIOD_POWER_DOWN_SETTINGS;
					}
					else {
						beep(BEEPCFG_PASSWORD_WRONG);
					_DiscardAndExitSettingMode:;
						DBG_PutString_P(PSTR("Leaving password change"NL));
						_mode = MODE_NORMAL; _period_power_down = PERIOD_POWER_DOWN; settings_load();
					}
				}
			}
		}

		// Power down handling
		if((uint8_t)(_global_ticks_tmp - _timein_powerdown) >= _period_power_down) {
			_timein_powerdown = _global_ticks_tmp;
			if(!Penalties.PenaltyTicks) {
				DBG_PutString_P(PSTR("Power down"NL));
				adc_off();
				PORTB = 0xFC; DDRB = 0x00; PORTD = 0x33; DDRD = 0x00; DIDR1 |= 0x03;
				DBG_Flush();
				EICRA = 0x00; EIMSK |= (1 << INT0);
				while(true) { set_sleep_mode(SLEEP_MODE_PWR_DOWN); sleep_mode(); }
			}
		}

		if(!_key_prev) YIELD();
	}

	__builtin_unreachable();
}
