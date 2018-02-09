/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.c
 ***********************************************************************/

#include "main.h"
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "eta_adc.h"
#include "eta_crc.h"
#include "eta_pusart.h"

__attribute__((naked, noreturn)) ISR(WDT_vect) { __asm("reti"); }

#define wdt_setup(value) \
__asm__ __volatile__ ( \
	"cli"        "\n\t"  \
	"wdr"        "\n\t"  \
	"out %0, %1" "\n\t" \
	"sei"        "\n\t" \
	"out %0, %2" "\n \t" \
	: /* no outputs */ \
	: "I" (_SFR_IO_ADDR(_WD_CONTROL_REG)), \
	  "r" ((uint8_t)(_BV(_WD_CHANGE_BIT) | _BV(WDE))), \
	  "r" ((uint8_t) ((value & 0x08 ? _WD_PS3_MASK : 0x00) | _BV(WDE) | _BV(WDTIE) | (value & 0x07))) \
)
#define wdt_setinterrupt() WDTCR |= (1 << WDTIE)

static void init() {
	pusart_init();
	DIDR0 = (1 << ADC1D);
	SW_LOCKMOTOR_DDR |= (1 << SW_LOW_LOCKMOTOR_PIN) | (1 << SW_HIGH_LOCKMOTOR_PIN);
	wdt_setup(WDTO_250MS);
}

static uint8_t cmd_put(uint8_t crc, uint16_t data) { pusart_put_value(data); return crc8_update(crc, (uint8_t)data); }
static void cmd_send(uint8_t cmd, uint8_t data) {
	uint8_t _crc = 0;
	_crc = cmd_put(_crc, DEFAULT_ADDR | 0x100);
	_crc = cmd_put(_crc, cmd);
	_crc = cmd_put(_crc, 1);
	_crc = cmd_put(_crc, data);
	cmd_put(_crc, _crc);
}

static void lock_drive(eLockDrive drive) { SW_LOCKMOTOR_PORT = (SW_LOCKMOTOR_PORT & ~LockDrive__Mask) | drive; }
static eLockState get_lock_state() {
	adc_start(); adcvalue_t _adc = adc_get_value();
	if(_adc <= ADC_UNLOCKED_VALUE) return LockState_Unlocked;
	if(_adc >= ADC_LOCKED_VALUE) return LockState_Locked;
	return LockState_Fail;
}

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
	eLockState _prev_state = (eLockState)-1;

	while(true) {
		adc_init(ADCPS_128, ADCVR_AVCC, ADCTS_NONE, 1);

		while(true) {
			// Watchdog reset
			wdt_setinterrupt();

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
				// Lock or unlock the door
				if((Cmd.Cmd == CMD_Lock || Cmd.Cmd == CMD_Unlock) && Cmd.Size == 1) {
					wdt_setup(WDTO_2S);
					eLockState _need_state = Cmd.Cmd == CMD_Lock ? LockState_Locked : LockState_Unlocked;
					eLockDrive _need_drive = Cmd.Cmd == CMD_Lock ? LockDrive_Lock : LockDrive_Unlock;
					pusart_counter_t _time_in = pusart_timer_get(), _timeout_after = 0;
					// forced
					if(Cmd.Data[0]) {
						lock_drive(_need_drive);
						_timeout_after = PERIOD_MOTOR_FORCED_POWER;
					}
					// sensor-driven
					else {
						_prev_state = get_lock_state();
						if(_prev_state != _need_state && _prev_state != LockState_Fail) {
							lock_drive(_need_drive);
							while(true) {
								_prev_state = get_lock_state();
								if(_prev_state == _need_state) {
									_timeout_after = PERIOD_MOTOR_SUCCESS_POWER;
									break;
								}
								if((pusart_timer_get() - _time_in) >= PERIOD_MOTOR_MAX_POWER) {
									lock_drive(Cmd.Cmd == CMD_Lock ? LockDrive_Unlock : LockDrive_Lock);
									_timeout_after = PERIOD_MOTOR_UNWIND_POWER;
									break;
								}
							}
						}
					}
					_time_in = pusart_timer_get(); while((pusart_timer_get() - _time_in) < _timeout_after);
					lock_drive(LockDrive_Idle);
					cmd_send(Cmd.Cmd, (_prev_state = get_lock_state()) == _need_state);
					wdt_setup(WDTO_250MS);
				}
				// Get lock state
				else if(Cmd.Cmd == CMD_GetState && !Cmd.Size) {
					cmd_send(Cmd.Cmd, (_prev_state = get_lock_state()));
				}

				Cmd.Step = CMDStep_Stop;
			}

			// Send updated state if changed
			eLockState _current_state = get_lock_state();
			if(_prev_state != _current_state) { _prev_state = _current_state; cmd_send(CMD_StateChanged, _prev_state); }

			// Power down timeout
			if(pusart_idle_counter_get() >= PERIOD_IDLE_POWERDOWN_TIMEOUT) {
				pusart_flush();
				adc_off();
				MCUCR = (0 << ISC00);
				set_sleep_mode(SLEEP_MODE_PWR_DOWN); sleep_mode();
				break;
			}
		}
	}

	__builtin_unreachable();
}
