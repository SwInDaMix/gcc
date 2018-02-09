/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : entry.c
 ***********************************************************************/

#include "stdperiph.h"
#include "entry.h"
#include "melody.h"
#include "eta_dbg.h"
#include "eta_frames.h"

#define LED_DELAY 32
#define LED_MAX_BRIGHT (LED_PERIOD - LED_DELAY)
#define BUZZER_PWM(freq) ((uint32_t)(F_CPU / freq * 512))
#define BUZZER_DELAY (6 * GLOBAL_TICKS_PER_SECOND / 1000)

#define REVOSM_LOWINLET_HYSTERESIS (GLOBAL_TICKS_PER_SECOND * 2)
#define REVOSM_HIGHTANK_HYSTERESIS (GLOBAL_TICKS_PER_SECOND * 2)
#define REVOSM_BTN_HYSTERESIS (GLOBAL_TICKS_PER_SECOND / 100)

#define REVOSM_VALVE_DELAY 3
#define REVOSM_HIGHTANK_LOWPRESSURE_DELAY 60
#define REVOSM_PRECLEANINGMEMBRANE_DELAY 50
#define REVOSM_POSTCLEANINGMEMBRANE_DELAY 50
#define REVOSM_MAX_FILTERING_TIME (60 * 60)
#define REVOSM_POSTFILTERING_TIME (45 * 60)
#define REVOSM_BTN_LONGPRESS_DELAY (GLOBAL_TICKS_PER_SECOND / 1)
#define REVOSM_BTN_REPEAT_DELAY (GLOBAL_TICKS_PER_SECOND / 5)
#define REVOSM_DEBUGMODE_TIMEOUT (10 * 60)

typedef struct {
	uint16_t SegA;
	uint16_t SegB;
	uint16_t SegC;
	uint16_t SegD;
	uint16_t SegE;
	uint16_t SegF;
	uint16_t SegG;
	uint16_t SegDP;
} sLCD_PwmDigit;

typedef struct {
	sLCD_PwmDigit PwmDigits[LED_DIGITS];
	uint16_t Pwm_WaterInlet;
	uint16_t Pwm_CleanMembrane;
	uint16_t Pwm_LowTank;
	uint16_t Pwm_NoWaterSource;
} sLCD_Pwm;

typedef struct {
	uint16_t Transition;
	eLCD_TransitionStep TransitionStep;
	eLCD_Mode Mode;
} sLCD_Param;

typedef struct {
	sLCD_Pwm From;
	sLCD_Pwm To;
	sLCD_Param ParamDigits[LED_DIGITS];
	sLCD_Param Param_WaterInlet;
	sLCD_Param Param_CleanMembrane;
	sLCD_Param Param_LowTank;
	sLCD_Param Param_NoWaterSource;
} sLCD;

static const uint32_t Buzzer_Frequencies[] = { BUZZER_PWM(16744.0), BUZZER_PWM(17739.2), BUZZER_PWM(18793.6), BUZZER_PWM(19912.0), BUZZER_PWM(21096.0), BUZZER_PWM(22350.4), BUZZER_PWM(23680.0), BUZZER_PWM(25088.0), BUZZER_PWM(26579.2), BUZZER_PWM(28160.0), BUZZER_PWM(29833.6), BUZZER_PWM(31608.0) };
static const uint8_t LCD_SymbolTable[] = {
	__extension__ 0b0000000, // None
	__extension__ 0b1111110, // 0
	__extension__ 0b0110000, // 1
	__extension__ 0b1101101, // 2
	__extension__ 0b1111001, // 3
	__extension__ 0b0110011, // 4
	__extension__ 0b1011011, // 5
	__extension__ 0b1011111, // 6
	__extension__ 0b1110010, // 7
	__extension__ 0b1111111, // 8
	__extension__ 0b1111011, // 9
	__extension__ 0b1110111, // A
	__extension__ 0b0011111, // b
	__extension__ 0b1001110, // C
	__extension__ 0b0111101, // d
	__extension__ 0b1001111, // E
	__extension__ 0b1000111, // F
	__extension__ 0b0000001, // MINUS
	__extension__ 0b0011100, // u
	__extension__ 0b0001101, // c
	__extension__ 0b0010101, // n
	__extension__ 0b0011001, // cr
	__extension__ 0b0011101, // o
	__extension__ 0b1100000, // Rotary
};

static uint32_t volatile GlobalTicks;

static sLCD volatile LCD;

static sBuzzerQuant const volatile *BuzzerMelodyPtr;
static int16_t volatile BuzzerQuantDuration;

static eButton volatile BTN_Pressed;
static eButton volatile BTN_Trigered;

#ifdef REVOSM_LOWINLET_SWITCH_HIGHTANK_SWITCH
static bool volatile Sens_LowInletSwitch;
#endif
#ifdef REVOSM_LOWINLET_SWITCH_HIGHTANK_SWITCH
static bool volatile Sens_HighTankSwitch;
#endif

void Buzzer_PlayMelody(sBuzzerQuant const *melody) { NVIC_DisableIRQ(LED_IRQn); BuzzerQuantDuration = -1; BuzzerMelodyPtr = melody; NVIC_EnableIRQ(LED_IRQn); }
void Buzzer_Stop() { BuzzerMelodyPtr = nullptr; }
void Buzzer_Beep(sBuzzerQuant quant) {
	static sBuzzerQuant __beep_melody[] = { { .Duration = 0, .Octave = 0, .Note = 0 }, { .Duration = 0, .Octave = 0, .Note = 0 } };
	__beep_melody[0] = quant;
	Buzzer_PlayMelody(__beep_melody);
}

eButton BTN_CheckPressed(eButton button) { return BTN_Pressed & button; }
eButton BTN_CheckTrigered(eButton button) {
	NVIC_DisableIRQ(LED_IRQn);
	eButton _btn_and = BTN_Trigered & button;
	BTN_Trigered &= ~(button);
	NVIC_EnableIRQ(LED_IRQn);
	return _btn_and;
}

static void LCD_DigitSetupSymbol(sLCD_PwmDigit volatile *pwm_digit, eLCD_Symbol symbol, bool dot_point) {
	uint8_t _symbmp = LCD_SymbolTable[symbol];
	pwm_digit->SegA = (_symbmp & __extension__ 0b1000000) ? LED_MAX_BRIGHT : 0;
	pwm_digit->SegB = (_symbmp & __extension__ 0b0100000) ? LED_MAX_BRIGHT : 0;
	pwm_digit->SegC = (_symbmp & __extension__ 0b0010000) ? LED_MAX_BRIGHT : 0;
	pwm_digit->SegD = (_symbmp & __extension__ 0b0001000) ? LED_MAX_BRIGHT : 0;
	pwm_digit->SegE = (_symbmp & __extension__ 0b0000100) ? LED_MAX_BRIGHT : 0;
	pwm_digit->SegF = (_symbmp & __extension__ 0b0000010) ? LED_MAX_BRIGHT : 0;
	pwm_digit->SegG = (_symbmp & __extension__ 0b0000001) ? LED_MAX_BRIGHT : 0;
	pwm_digit->SegDP = dot_point ? LED_MAX_BRIGHT : 0;
}
static void LCD_DigitBreakTransition(uint16_t transition, sLCD_PwmDigit volatile *pwm_digit_from, sLCD_PwmDigit volatile *pwm_digit_to) {
	pwm_digit_to->SegA = ((((uint32_t)pwm_digit_from->SegA) * transition) + (((uint32_t)pwm_digit_to->SegA) * ((UINT8_MAX + 1) - transition))) >> 8;
	pwm_digit_to->SegB = ((((uint32_t)pwm_digit_from->SegB) * transition) + (((uint32_t)pwm_digit_to->SegB) * ((UINT8_MAX + 1) - transition))) >> 8;
	pwm_digit_to->SegC = ((((uint32_t)pwm_digit_from->SegC) * transition) + (((uint32_t)pwm_digit_to->SegC) * ((UINT8_MAX + 1) - transition))) >> 8;
	pwm_digit_to->SegD = ((((uint32_t)pwm_digit_from->SegD) * transition) + (((uint32_t)pwm_digit_to->SegD) * ((UINT8_MAX + 1) - transition))) >> 8;
	pwm_digit_to->SegE = ((((uint32_t)pwm_digit_from->SegE) * transition) + (((uint32_t)pwm_digit_to->SegE) * ((UINT8_MAX + 1) - transition))) >> 8;
	pwm_digit_to->SegF = ((((uint32_t)pwm_digit_from->SegF) * transition) + (((uint32_t)pwm_digit_to->SegF) * ((UINT8_MAX + 1) - transition))) >> 8;
	pwm_digit_to->SegG = ((((uint32_t)pwm_digit_from->SegG) * transition) + (((uint32_t)pwm_digit_to->SegG) * ((UINT8_MAX + 1) - transition))) >> 8;
	pwm_digit_to->SegDP = ((((uint32_t)pwm_digit_from->SegDP) * transition) + (((uint32_t)pwm_digit_to->SegDP) * ((UINT8_MAX + 1) - transition))) >> 8;
}
static void LCD_LEDSetupValue(uint16_t volatile *pwm_led, bool value) {
	*pwm_led = value ? LED_MAX_BRIGHT : 0;
}
static void LCD_LEDBreakTransition(uint16_t transition, uint16_t volatile *pwm_led_from, uint16_t volatile *pwm_led_to) {
	*pwm_led_to = ((((uint32_t)*pwm_led_from) * transition) + (((uint32_t)*pwm_led_to) * ((UINT8_MAX + 1) - transition))) >> 8;
}

void Int_Update() {
	GlobalTicks++;

	// **********
	// LCD Update
	// **********

	static uint8_t _dig = 0;
	static GPIO_TypeDef *const __gpio_digs_ports[] = { LED_DIG1_PORT, LED_DIG2_PORT, LED_DIG3_PORT, LED_DIG4_PORT, LED_DIGLEDS_PORT };
	static uint16_t const __gpio_digs_pins[] = { (1 << LED_DIG1_PIN), (1 << LED_DIG2_PIN), (1 << LED_DIG3_PIN), (1 << LED_DIG4_PIN), (1 << LED_DIGLEDS_PIN) };
	GPIO_SetBits(GPIOA, LED_DIG_PINCFG_A); GPIO_SetBits(GPIOF, LED_DIG_PINCFG_F);
	GPIO_ResetBits(__gpio_digs_ports[_dig], __gpio_digs_pins[_dig]);
	TIM_Cmd(LED_TIM1, ENABLE);
	_dig++; if(_dig >= LED_DIGS) _dig = 0;
	if(_dig < LED_DIGITS) {
		uint16_t _transition = LCD.ParamDigits[_dig].Transition; uint8_t _transition_step = LCD.ParamDigits[_dig].TransitionStep; eLCD_Mode _mode = LCD.ParamDigits[_dig].Mode & LCD_Mode__Mask; uint8_t _mode_other = LCD.ParamDigits[_dig].Mode & ~LCD_Mode__Mask;
		sLCD_PwmDigit volatile *_pwm_digit_from = &LCD.From.PwmDigits[_dig], *_pwm_digit_to = &LCD.To.PwmDigits[_dig];
		uint32_t _pwm_a = ((((uint32_t)_pwm_digit_from->SegA) * _transition) + (((uint32_t)_pwm_digit_to->SegA) * ((UINT8_MAX + 1) - _transition))) >> 8; LED_SEGMENT_A_SetCompare(LED_PERIOD - (_pwm_a * _pwm_a / LED_MAX_BRIGHT));
		uint32_t _pwm_b = ((((uint32_t)_pwm_digit_from->SegB) * _transition) + (((uint32_t)_pwm_digit_to->SegB) * ((UINT8_MAX + 1) - _transition))) >> 8; LED_SEGMENT_B_SetCompare(LED_PERIOD - (_pwm_b * _pwm_b / LED_MAX_BRIGHT));
		uint32_t _pwm_c = ((((uint32_t)_pwm_digit_from->SegC) * _transition) + (((uint32_t)_pwm_digit_to->SegC) * ((UINT8_MAX + 1) - _transition))) >> 8; LED_SEGMENT_C_SetCompare(LED_PERIOD - (_pwm_c * _pwm_c / LED_MAX_BRIGHT));
		uint32_t _pwm_d = ((((uint32_t)_pwm_digit_from->SegD) * _transition) + (((uint32_t)_pwm_digit_to->SegD) * ((UINT8_MAX + 1) - _transition))) >> 8; LED_SEGMENT_D_SetCompare(LED_PERIOD - (_pwm_d * _pwm_d / LED_MAX_BRIGHT));
		uint32_t _pwm_e = ((((uint32_t)_pwm_digit_from->SegE) * _transition) + (((uint32_t)_pwm_digit_to->SegE) * ((UINT8_MAX + 1) - _transition))) >> 8; LED_SEGMENT_E_SetCompare(LED_PERIOD - (_pwm_e * _pwm_e / LED_MAX_BRIGHT));
		uint32_t _pwm_f = ((((uint32_t)_pwm_digit_from->SegF) * _transition) + (((uint32_t)_pwm_digit_to->SegF) * ((UINT8_MAX + 1) - _transition))) >> 8; LED_SEGMENT_F_SetCompare(LED_PERIOD - (_pwm_f * _pwm_f / LED_MAX_BRIGHT));
		uint32_t _pwm_g = ((((uint32_t)_pwm_digit_from->SegG) * _transition) + (((uint32_t)_pwm_digit_to->SegG) * ((UINT8_MAX + 1) - _transition))) >> 8; LED_SEGMENT_G_SetCompare(LED_PERIOD - (_pwm_g * _pwm_g / LED_MAX_BRIGHT));
		uint32_t _pwm_dp = ((((uint32_t)_pwm_digit_from->SegDP) * _transition) + (((uint32_t)_pwm_digit_to->SegDP) * ((UINT8_MAX + 1) - _transition))) >> 8; LED_SEGMENT_DP_SetCompare(LED_PERIOD - (_pwm_dp * _pwm_dp / LED_MAX_BRIGHT));
		if(_transition_step) {
			_transition += _transition_step;
			if(_transition > (UINT8_MAX + 1)) {
				*_pwm_digit_to = *_pwm_digit_from; _transition = 0;
				if(_mode == LCD_Mode_Transition) { _transition_step = 0; }
				else if(_mode == LCD_Mode_Rotation) {
					_pwm_digit_from->SegB = _pwm_digit_to->SegA;
					_pwm_digit_from->SegC = _pwm_digit_to->SegB;
					_pwm_digit_from->SegD = _pwm_digit_to->SegC;
					_pwm_digit_from->SegE = _pwm_digit_to->SegD;
					_pwm_digit_from->SegF = _pwm_digit_to->SegE;
					_pwm_digit_from->SegA = _pwm_digit_to->SegF;
				}
			}
		}
		LCD.ParamDigits[_dig].Transition = _transition; LCD.ParamDigits[_dig].TransitionStep = _transition_step; LCD.ParamDigits[_dig].Mode = _mode | _mode_other;
	}
	else {
		__extension__ uint16_t _process_led_pwm(sLCD_Param volatile *param, uint16_t volatile *pwm_led_from, uint16_t volatile *pwm_led_to) {
			uint16_t _transition = param->Transition; uint8_t _transition_step = param->TransitionStep; eLCD_Mode _mode = param->Mode & LCD_Mode__Mask; uint8_t _mode_other = param->Mode & ~LCD_Mode__Mask;
			uint16_t _pwm_ret = ((((uint32_t)*pwm_led_from) * _transition) + (((uint32_t)*pwm_led_to) * ((UINT8_MAX + 1) - _transition))) >> 8; _pwm_ret = (uint32_t)_pwm_ret * _pwm_ret / LED_MAX_BRIGHT;
			if(_transition_step) {
				_transition += _transition_step;
				if(_transition > (UINT8_MAX + 1)) {
					*pwm_led_to = *pwm_led_from; _transition = 0;
					if(_mode == LCD_Mode_Transition) { *pwm_led_to = *pwm_led_from; _transition_step = 0; }
					else if(_mode == LCD_Mode_Blinking) { LCD_LEDSetupValue(pwm_led_from, _mode_other); _mode_other ^= 1; }
				}
			}
			param->Transition = _transition; param->TransitionStep = _transition_step; param->Mode = _mode | _mode_other;
			return _pwm_ret;
		}
		LED_LED_WaterInlet_SetCompare(LED_PERIOD - _process_led_pwm(&LCD.Param_WaterInlet, &LCD.From.Pwm_WaterInlet, &LCD.To.Pwm_WaterInlet));
		LED_LED_CleanMembrane_SetCompare(LED_PERIOD - _process_led_pwm(&LCD.Param_CleanMembrane, &LCD.From.Pwm_CleanMembrane, &LCD.To.Pwm_CleanMembrane));
		LED_LED_LowTank_SetCompare(LED_PERIOD - _process_led_pwm(&LCD.Param_LowTank, &LCD.From.Pwm_LowTank, &LCD.To.Pwm_LowTank));
		LED_LED_NoWaterSource_SetCompare(LED_PERIOD - _process_led_pwm(&LCD.Param_NoWaterSource, &LCD.From.Pwm_NoWaterSource, &LCD.To.Pwm_NoWaterSource));
	}

	// *************
	// Melody Update
	// *************

	if(BuzzerMelodyPtr) {
		if(BuzzerQuantDuration > 0) {
			if(!(--BuzzerQuantDuration)) {
				TIM_SetCompare1(BUZZER_TIM, 0);
				BuzzerQuantDuration = -BUZZER_DELAY;
			}
		}
		else if(BuzzerQuantDuration < 0) {
			if(!(++BuzzerQuantDuration)) {
				sBuzzerQuant _quant = *BuzzerMelodyPtr++;
				eBuzzerNote _quant_note = _quant.Note;
				BuzzerQuantDuration = _quant.Duration << 2;
				if(BuzzerQuantDuration) {
					BuzzerQuantDuration -= BUZZER_DELAY;
					if(_quant_note == BuzzerNote_Silence) TIM_SetCompare1(BUZZER_TIM, 0);
					else {
						uint32_t _f_period = Buzzer_Frequencies[_quant_note - 1] >> _quant.Octave;
						uint16_t _prescaler = (_f_period >> 16); _f_period /= (_prescaler + 1);
						TIM_Cmd(BUZZER_TIM, DISABLE);
						TIM_PrescalerConfig(BUZZER_TIM, _prescaler, TIM_PSCReloadMode_Update);
						TIM_SetAutoreload(BUZZER_TIM, _f_period);
						TIM_SetCompare1(BUZZER_TIM, _f_period >> 1);
						TIM_Cmd(BUZZER_TIM, ENABLE);
					}
				}
			}
		}
		else { TIM_SetCompare1(BUZZER_TIM, 0); BuzzerMelodyPtr = nullptr; }
	}
	else { TIM_SetCompare1(BUZZER_TIM, 0); }

	// **************
	// Buttons Update
	// **************

	static struct sBTN {
		bool prev_state;
		uint16_t rep_btn;
		uint8_t hyst_btn;
	} __btn_prev, __btn_next, __btn_enter;
	/// ----------------------------------------------------------------------------------------------------------------------------------------------------------
	__extension__ uint32_t _process_btn(struct sBTN *btn_struct, eButton btn_flag, eButton btn_flaglong, bool state) {
		eButton _btn_pressed = 0, _btn_triggered = 0;
		if(btn_struct->prev_state != state) { btn_struct->prev_state = state; btn_struct->hyst_btn = btn_struct->rep_btn = 0; }
		else {
			if(btn_struct->hyst_btn >= REVOSM_BTN_HYSTERESIS) {
				if(state) {
					_btn_pressed |= btn_flag;
					if(btn_struct->rep_btn > REVOSM_BTN_LONGPRESS_DELAY) _btn_pressed |= btn_flaglong;
					if(!btn_struct->rep_btn) _btn_triggered |= btn_flag;
					if(btn_struct->rep_btn == REVOSM_BTN_LONGPRESS_DELAY) _btn_triggered |= btn_flaglong;
					if(btn_struct->rep_btn == (REVOSM_BTN_LONGPRESS_DELAY + REVOSM_BTN_REPEAT_DELAY)) { _btn_triggered |= btn_flag; btn_struct->rep_btn = REVOSM_BTN_LONGPRESS_DELAY; }
					btn_struct->rep_btn++;
				}
			}
			else btn_struct->hyst_btn++;
		}
		return _btn_pressed | (_btn_triggered << 16);
	}
	/// ----------------------------------------------------------------------------------------------------------------------------------------------------------
	uint32_t _btn_ret = (_process_btn(&__btn_prev, Button_Prev, Button_PrevLong, GPIO_ReadInputDataBit(BTN_PREV_PORT, (1 << BTN_PREV_PIN)) == Bit_RESET) |
						_process_btn(&__btn_next, Button_Next, Button_NextLong, GPIO_ReadInputDataBit(BTN_NEXT_PORT, (1 << BTN_NEXT_PIN)) == Bit_RESET) |
						_process_btn(&__btn_enter, Button_Enter, Button_EnterLong, GPIO_ReadInputDataBit(BTN_ENTER_PORT, (1 << BTN_ENTER_PIN)) == Bit_RESET));
	BTN_Pressed = _btn_ret & 0xFFFF; BTN_Trigered |= (_btn_ret >> 16);

	// **************
	// Sensors Update
	// **************

	#ifdef REVOSM_LOWINLET_SWITCH_HIGHTANK_SWITCH
	static bool __prev_low_inlet_switch = false; static uint16_t _rep_low_inlet = 0;
	bool _low_inlet_switch = GPIO_ReadInputDataBit(SENS_LOWINLET_PRESSURE_PORT, (1 << SENS_LOWINLET_PRESSURE_PIN)) == Bit_SET;
	if(__prev_low_inlet_switch != _low_inlet_switch) { __prev_low_inlet_switch = _low_inlet_switch; _rep_low_inlet = 0; }
	else { if(_rep_low_inlet >= REVOSM_LOWINLET_HYSTERESIS) { Sens_LowInletSwitch = _low_inlet_switch; } else _rep_low_inlet++; }
	#endif
	#ifdef REVOSM_LOWINLET_SWITCH_HIGHTANK_SWITCH
	static bool __prev_high_tank_switch = false; static uint16_t _rep_high_tank = 0;
	bool _high_tank_switch = GPIO_ReadInputDataBit(SENS_HIGHTANK_PRESSURE_PORT, (1 << SENS_HIGHTANK_PRESSURE_PIN)) == Bit_SET;
	if(__prev_high_tank_switch != _high_tank_switch) { __prev_high_tank_switch = _high_tank_switch; _rep_high_tank = 0; }
	else { if(_rep_high_tank >= REVOSM_HIGHTANK_HYSTERESIS) { Sens_HighTankSwitch = _high_tank_switch; } else _rep_high_tank++; }
	#endif
}
void LCD_DigitSetSymbol(uint8_t digit, eLCD_Symbol symbol, bool dot_point, eLCD_TransitionStep transition_step) {
	NVIC_DisableIRQ(LED_IRQn);

	sLCD_PwmDigit volatile *_pwm_digit_from = &LCD.From.PwmDigits[digit], *_pwm_digit_to = &LCD.To.PwmDigits[digit];
	uint16_t _transition = LCD.ParamDigits[digit].Transition;
	if(_transition && transition_step != LCD_DadeOut_None) { LCD_DigitBreakTransition(_transition, _pwm_digit_from, _pwm_digit_to); }
	LCD_DigitSetupSymbol(transition_step == LCD_DadeOut_None ? _pwm_digit_to : _pwm_digit_from, symbol, dot_point);
	LCD.ParamDigits[digit].Transition = 0; LCD.ParamDigits[digit].TransitionStep = transition_step; LCD.ParamDigits[digit].Mode = LCD_Mode_Transition;

	NVIC_EnableIRQ(LED_IRQn);
}
void LCD_DigitSetRotation(uint8_t digit, eLCD_TransitionStep transition_step) {
	NVIC_DisableIRQ(LED_IRQn);

	if((LCD.ParamDigits[digit].Mode & LCD_Mode__Mask) != LCD_Mode_Rotation) {
		sLCD_PwmDigit volatile *_pwm_digit_from = &LCD.From.PwmDigits[digit], *_pwm_digit_to = &LCD.To.PwmDigits[digit];
		uint16_t _transition = LCD.ParamDigits[digit].Transition;
		if(_transition) { LCD_DigitBreakTransition(_transition, _pwm_digit_from, _pwm_digit_to); }
		LCD_DigitSetupSymbol(_pwm_digit_from, LCD_Symbol_Rotary, false);
		LCD.ParamDigits[digit].Transition = 0; LCD.ParamDigits[digit].Mode = LCD_Mode_Rotation;
	}
	LCD.ParamDigits[digit].TransitionStep = transition_step;

	NVIC_EnableIRQ(LED_IRQn);
}
void LCD_LEDSetValue(eLCD_LEDs leds, bool value, eLCD_TransitionStep transition_step) {
	/// ----------------------------------------------------------------------------------------------------------------------------------------------------------
	__extension__ void _set_value(sLCD_Param volatile *param, uint16_t volatile *pwm_led_from, uint16_t volatile *pwm_led_to) {
		NVIC_DisableIRQ(LED_IRQn);

		uint16_t _transition = param->Transition;
		if(_transition && transition_step != LCD_DadeOut_None) { LCD_LEDBreakTransition(_transition, pwm_led_from, pwm_led_to); }
		LCD_LEDSetupValue((transition_step == LCD_DadeOut_None ? pwm_led_to : pwm_led_from), value);
		param->Transition = 0; param->TransitionStep = transition_step; param->Mode = LCD_Mode_Transition;

		NVIC_EnableIRQ(LED_IRQn);
	}
	/// ----------------------------------------------------------------------------------------------------------------------------------------------------------
	if(leds & LCD_LED_WaterInlet) _set_value(&LCD.Param_WaterInlet, &LCD.From.Pwm_WaterInlet, &LCD.To.Pwm_WaterInlet);
	if(leds & LCD_LED_CleanMembrane) _set_value(&LCD.Param_CleanMembrane, &LCD.From.Pwm_CleanMembrane, &LCD.To.Pwm_CleanMembrane);
	if(leds & LCD_LED_LowTank) _set_value(&LCD.Param_LowTank, &LCD.From.Pwm_LowTank, &LCD.To.Pwm_LowTank);
	if(leds & LCD_LED_NoWaterSource) _set_value(&LCD.Param_NoWaterSource, &LCD.From.Pwm_NoWaterSource, &LCD.To.Pwm_NoWaterSource);
}
void LCD_LEDSetBlinking(eLCD_LEDs leds, eLCD_TransitionStep transition_step) {
	/// ----------------------------------------------------------------------------------------------------------------------------------------------------------
	__extension__ void _set_value(sLCD_Param volatile *param, uint16_t volatile *pwm_led_from, uint16_t volatile *pwm_led_to) {
		NVIC_DisableIRQ(LED_IRQn);

		if((param->Mode & LCD_Mode__Mask) != LCD_Mode_Blinking) {
			uint16_t _transition = param->Transition;
			if(_transition) { LCD_LEDBreakTransition(_transition, pwm_led_from, pwm_led_to); }
			LCD_LEDSetupValue(pwm_led_from, true);
			param->Transition = 0; param->Mode = LCD_Mode_Blinking;
		}
		param->TransitionStep = transition_step;

		NVIC_EnableIRQ(LED_IRQn);
	}
	/// ----------------------------------------------------------------------------------------------------------------------------------------------------------
	if(leds & LCD_LED_WaterInlet) _set_value(&LCD.Param_WaterInlet, &LCD.From.Pwm_WaterInlet, &LCD.To.Pwm_WaterInlet);
	if(leds & LCD_LED_CleanMembrane) _set_value(&LCD.Param_CleanMembrane, &LCD.From.Pwm_CleanMembrane, &LCD.To.Pwm_CleanMembrane);
	if(leds & LCD_LED_LowTank) _set_value(&LCD.Param_LowTank, &LCD.From.Pwm_LowTank, &LCD.To.Pwm_LowTank);
	if(leds & LCD_LED_NoWaterSource) _set_value(&LCD.Param_NoWaterSource, &LCD.From.Pwm_NoWaterSource, &LCD.To.Pwm_NoWaterSource);
}

void LCD_DigitSetOp(eLCD_TransitionStep rotation_speed, eLCD_Symbol symbol, bool dot) {
	static eLCD_TransitionStep __prev_rotation_speed; static eLCD_Symbol __prev_symbol; static bool __prev_dot;

	bool _rotation_changed = __prev_rotation_speed != rotation_speed;
	if(_rotation_changed || __prev_symbol != symbol || __prev_dot != dot) {
		__prev_rotation_speed = rotation_speed; __prev_symbol = symbol; __prev_dot = dot;
		if(rotation_speed != LCD_DadeOut_None) { if(_rotation_changed) LCD_DigitSetRotation(0, rotation_speed); }
		else LCD_DigitSetSymbol(0, symbol, dot, LCD_DadeOut_Fast);
	}
}
void LCD_DigitSetNumberOrTime(uint16_t number_or_time, bool is_time, bool is_leadzeros, bool dots) {
	static uint16_t __prev_number_or_time; static bool __prev_is_time, __prev_dots;

	if(__prev_number_or_time != number_or_time || __prev_is_time != is_time || __prev_dots != dots) {
		__prev_number_or_time = number_or_time; __prev_is_time = is_time; __prev_dots = dots;
		div_t _div_3 = div(number_or_time, is_time ? 60 : 100), _div_2 = div(_div_3.rem, 10);
		if(_div_3.quot >= 10) { LCD_DigitSetSymbol(1, LCD_Symbol_None, dots, LCD_DadeOut_Fast); LCD_DigitSetSymbol(2, LCD_Symbol_None, dots, LCD_DadeOut_Fast); LCD_DigitSetSymbol(3, LCD_Symbol_None, dots, LCD_DadeOut_Fast); }
		else {
			LCD_DigitSetSymbol(1, (is_leadzeros || _div_3.quot) ? LCD_Symbol_0 + _div_3.quot : LCD_Symbol_None, dots, LCD_DadeOut_Fast);
			LCD_DigitSetSymbol(2, (is_leadzeros || (_div_3.quot || _div_2.quot)) ? LCD_Symbol_0 + _div_2.quot : LCD_Symbol_None, dots, LCD_DadeOut_Fast);
			LCD_DigitSetSymbol(3, LCD_Symbol_0 + _div_2.rem, dots, LCD_DadeOut_Fast);
		}
	}
}

void Valve_WaterInlet(bool state) {
	static bool __prev_state;

	if(__prev_state != state) {
		__prev_state = state;
		GPIO_WriteBit(VALVE_WATER_INLET_PORT, (1 << VALVE_WATER_INLET_PIN), state ? Bit_SET : Bit_RESET);
		LCD_LEDSetValue(LCD_LED_WaterInlet, state, LCD_DadeOut_Slow);
	}
}
void Valve_CleanMembrane(bool state) {
	static bool __prev_state;

	if(__prev_state != state) {
		__prev_state = state;
		GPIO_WriteBit(VALVE_CLEAN_MEMBRANE_PORT, (1 << VALVE_CLEAN_MEMBRANE_PIN), state ? Bit_SET : Bit_RESET);
		if(state) LCD_LEDSetBlinking(LCD_LED_CleanMembrane, LCD_DadeOut_Fast); else LCD_LEDSetValue(LCD_LED_CleanMembrane, false, LCD_DadeOut_Fast);
	}
}

void Indicator_NoWater(bool state) {
	static bool __prev_state;

	if(__prev_state != state) {
		__prev_state = state;
		if(state) LCD_LEDSetBlinking(LCD_LED_NoWaterSource, LCD_DadeOut_Slow);
		else LCD_LEDSetValue(LCD_LED_NoWaterSource, false, LCD_DadeOut_Slow);
	}
}
void Indicator_LowTank(bool state, bool is_post) {
	static bool __prev_state, __prev_is_post;

	if(__prev_state != state || __prev_is_post != is_post) {
		__prev_state = state; __prev_is_post = is_post;
		if(state || is_post) LCD_LEDSetBlinking(LCD_LED_LowTank, is_post ? LCD_DadeOut_Slow : LCD_DadeOut_Normal);
		else LCD_LEDSetValue(LCD_LED_LowTank, false, LCD_DadeOut_Normal);
	}
}

bool Sensor_NoWaterSource() { return !Sens_LowInletSwitch; }
bool Sensor_LowTankPressure() { return Sens_HighTankSwitch; }

void EtaReverseOsmosisMainLoop(bool volatile *is_abort_requested, bool volatile *is_inited, void (*yield)()) {
	DBG_PutString(	"\nEta Reverse Osmosis Management System"NL
					" - Platform v "VER_PLATFORM""NL
					" - Application v "VER_APP""NL);

	typedef enum {
		ROState_Idle,
		ROState_Prepearing,
		ROState_Cleaning1,
		ROState_Cleaning2,
		ROState_Filtering1,
		ROState_Filtering2,
		ROState_Filtering3,
		ROState_PostFiltering1,
		ROState_PostFiltering2,
		ROState_PostCleaning1,
		ROState_PostCleaning2,
		ROState_PostCleaning3,
		ROState_ErrorHighTankPressure,
		ROState_NoWaterSource,
	} eROState;

	eROState _ro_state = ROState_Idle;
	uint32_t _prev_global_ticks_seconds = 0, _seconds = 0, _filtering_seconds = 0; bool _halfsecond = false, _second_changed = false;
	bool _is_debug_mode = false, _is_water_inlet = false, _is_clean_membrane = false, _is_time_remaining = true;

	while(true) {
		eButton _buttons_pressed = BTN_CheckPressed(Button_All);
		eButton _buttons_trigered = BTN_CheckTrigered(Button_All);

		// Seconds processing
		_second_changed = false;
		if((_prev_global_ticks_seconds - GlobalTicks) > (GLOBAL_TICKS_PER_SECOND >> 1)) {
			_prev_global_ticks_seconds += (GLOBAL_TICKS_PER_SECOND >> 1);
			IWDG_ReloadCounter();
			if(_halfsecond) { _halfsecond = false; } else { _halfsecond = true; _seconds++; _second_changed = true; }
		}

		// Water source processing
		if(!_is_debug_mode && _ro_state != ROState_ErrorHighTankPressure) {
			if(Sensor_NoWaterSource() && _ro_state != ROState_NoWaterSource) {
				DBG_PutString("No water source"NL);
				Valve_WaterInlet(false); Valve_CleanMembrane(false);
				LCD_DigitSetOp(LCD_DadeOut_None, LCD_Symbol_None, false); LCD_DigitSetNumberOrTime(-1, false, false, false);
				_ro_state = ROState_NoWaterSource;
			}
			else if(!Sensor_NoWaterSource() && _ro_state == ROState_NoWaterSource) {
				DBG_PutString("Water source appeared"NL"Entering idle state"NL);
				_ro_state = ROState_Idle;
			}
		}

		// Process indicators
		Indicator_NoWater(Sensor_NoWaterSource());
		Indicator_LowTank(Sensor_LowTankPressure(), _ro_state == ROState_PostFiltering1 || _ro_state == ROState_PostFiltering2);

		// Global buttons operations
		if(_buttons_trigered & Button_Prev) _is_time_remaining = false;
		if(_buttons_trigered & Button_Next) _is_time_remaining = true;

		// Normal state processing
		if(_ro_state == ROState_Idle) {
			if(_is_debug_mode) {
				LCD_DigitSetOp(LCD_DadeOut_None, LCD_Symbol_d, _halfsecond); LCD_DigitSetNumberOrTime(-1, false, false, false);
				if(_buttons_trigered & Button_PrevLong) { _is_water_inlet = true; _is_clean_membrane = false; _seconds = 0; }
				if(_buttons_trigered & Button_NextLong) { _is_water_inlet = _is_clean_membrane = true; _seconds = 0; }
				if(_buttons_trigered & Button_Enter) { _is_water_inlet = _is_clean_membrane = false; _seconds = 0; }
				Valve_WaterInlet(_is_water_inlet); Valve_CleanMembrane(_is_clean_membrane);
			}
			else {
				LCD_DigitSetOp(LCD_DadeOut_None, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime(-1, false, false, false);
				Valve_WaterInlet(false); Valve_CleanMembrane(false);
				if(_buttons_trigered & Button_PrevLong) {
					DBG_PutString("User activated membrane cleaning."NL);
					Buzzer_PlayMelody(BuzzerMelody_CleanMembrane);
					_seconds = 0; _ro_state = ROState_PostCleaning1;
				}
				if(_buttons_trigered & Button_NextLong) {
					DBG_PutString("User forced filtering. Starting membrane cleaning before filtering."NL);
					Buzzer_PlayMelody(BuzzerMelody_LowTank);
					_seconds = 0; _ro_state = ROState_Cleaning1;
				}
				if(Sensor_LowTankPressure()) {
					DBG_PutString("Low tank pressure detected. Preparing for filtering"NL);
					_seconds = 0; _ro_state = ROState_Prepearing;
				}
			}
		}
		else if(_ro_state == ROState_Prepearing) {
			LCD_DigitSetOp(LCD_DadeOut_None, LCD_Symbol_u, _halfsecond); LCD_DigitSetNumberOrTime(-1, false, false, _halfsecond);
			Valve_WaterInlet(false); Valve_CleanMembrane(false);
			if(!Sensor_LowTankPressure()) {
				DBG_PutString("False low tank pressure detection. Return to idle state."NL);
				_ro_state = ROState_Idle;
			}
			else if(_seconds >= REVOSM_HIGHTANK_LOWPRESSURE_DELAY) {
				DBG_PutString("Low tank pressure approved. Starting membrane cleaning before filtering."NL);
				Buzzer_PlayMelody(BuzzerMelody_LowTank);
				_seconds = 0; _ro_state = ROState_Cleaning1;
			}
		}
		else if(_ro_state == ROState_Cleaning1) {
			LCD_DigitSetOp(LCD_DadeOut_Fast, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime(REVOSM_PRECLEANINGMEMBRANE_DELAY - _seconds, false, false, _halfsecond);
			Valve_WaterInlet(false); Valve_CleanMembrane(true);
			if(_seconds >= REVOSM_VALVE_DELAY) { _ro_state = ROState_Cleaning2; }
		}
		else if(_ro_state == ROState_Cleaning2) {
			LCD_DigitSetOp(LCD_DadeOut_Fast, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime(REVOSM_PRECLEANINGMEMBRANE_DELAY - _seconds, false, false, _halfsecond);
			Valve_WaterInlet(true); Valve_CleanMembrane(true);
			if((_buttons_trigered & Button_Enter) || _seconds >= REVOSM_PRECLEANINGMEMBRANE_DELAY) {
				DBG_PutString(_buttons_trigered & Button_Enter ? "Membrane cleaned. " : "Membrane cleaning canceled by user. "); DBG_PutString("Starting filtering."NL);
				_seconds = 0; _ro_state = ROState_Filtering1;
			}
		}
		else if(_ro_state == ROState_Filtering1) {
			LCD_DigitSetOp(LCD_DadeOut_Slow, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime((_is_time_remaining ? REVOSM_MAX_FILTERING_TIME - _seconds + 59 : _seconds) / 60, true, false, _halfsecond);
			Valve_WaterInlet(false); Valve_CleanMembrane(true);
			if(_seconds >= REVOSM_VALVE_DELAY) { _ro_state = ROState_Filtering2; }
		}
		else if(_ro_state == ROState_Filtering2) {
			LCD_DigitSetOp(LCD_DadeOut_Slow, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime((_is_time_remaining ? REVOSM_MAX_FILTERING_TIME - _seconds + 59 : _seconds) / 60, true, false, _halfsecond);
			Valve_WaterInlet(false); Valve_CleanMembrane(false);
			if(_seconds >= REVOSM_VALVE_DELAY + 1) { _ro_state = ROState_Filtering3; }
		}
		else if(_ro_state == ROState_Filtering3) {
			LCD_DigitSetOp(LCD_DadeOut_Slow, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime((_is_time_remaining ? REVOSM_MAX_FILTERING_TIME - _seconds + 59 : _seconds) / 60, true, false, _halfsecond);
			Valve_WaterInlet(true); Valve_CleanMembrane(false);
			if(!Sensor_LowTankPressure()) {
				DBG_PutFormat("Full tank detected after %d seconds. Starting post-filtering."NL, _seconds);
				_filtering_seconds = _seconds; _seconds = 0; _ro_state = ROState_PostFiltering1;
			}
			else if(_seconds >= REVOSM_MAX_FILTERING_TIME) {
				DBG_PutString("Full tank is not detected during maximum allowed time. Possibly tank pressure sensor malfunction."NL);
				Buzzer_PlayMelody(BuzzerMelody_FatalError);
				_ro_state = ROState_ErrorHighTankPressure;
			}
		}
		else if(_ro_state == ROState_PostFiltering1) {
			LCD_DigitSetOp(LCD_DadeOut_Slow, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime((_is_time_remaining ? REVOSM_POSTFILTERING_TIME - _seconds + 59 : _seconds) / 60, true, false, _halfsecond);
			Valve_WaterInlet(true); Valve_CleanMembrane(false);
			if(Sensor_LowTankPressure()) {
				DBG_PutString("Low tank pressure detected again. Return to usual filtering."NL);
				_seconds += _filtering_seconds; _ro_state = ROState_Filtering3;
			}
			if((_buttons_trigered & Button_EnterLong) || _seconds >= REVOSM_POSTFILTERING_TIME) {
				DBG_PutString(_buttons_trigered & Button_EnterLong ? "Post-filtering is user canceled. " : "Post-filtering is finished. Tank is full. "); DBG_PutString("Starting membrane post-cleaning."NL);
				Buzzer_PlayMelody(BuzzerMelody_CleanMembrane);
				_seconds = 0; _ro_state = ROState_PostFiltering2;
			}
		}
		else if(_ro_state == ROState_PostFiltering2) {
			LCD_DigitSetOp(LCD_DadeOut_Slow, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime(0, false, false, _halfsecond);
			Valve_WaterInlet(false); Valve_CleanMembrane(false);
			if(_seconds >= REVOSM_VALVE_DELAY) { _seconds = 0; _ro_state = ROState_PostCleaning1; }
		}
		else if(_ro_state == ROState_PostCleaning1) {
			LCD_DigitSetOp(LCD_DadeOut_Fast, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime(REVOSM_POSTCLEANINGMEMBRANE_DELAY - _seconds, false, false, _halfsecond);
			Valve_WaterInlet(false); Valve_CleanMembrane(true);
			if(_seconds >= REVOSM_VALVE_DELAY) { _ro_state = ROState_PostCleaning2; }
		}
		else if(_ro_state == ROState_PostCleaning2) {
			LCD_DigitSetOp(LCD_DadeOut_Fast, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime(REVOSM_POSTCLEANINGMEMBRANE_DELAY - _seconds, false, false, _halfsecond);
			Valve_WaterInlet(true); Valve_CleanMembrane(true);
			if((_buttons_trigered & Button_EnterLong) || _seconds >= REVOSM_POSTCLEANINGMEMBRANE_DELAY) {
				DBG_PutString(_buttons_trigered & Button_EnterLong ? "Membrane cleaning is finished. " : "Membrane cleaning canceled by user. "); DBG_PutString("Entering idle state."NL);
				Buzzer_PlayMelody(BuzzerMelody_Success);
				_seconds = 0; _ro_state = ROState_PostCleaning3;
			}
		}
		else if(_ro_state == ROState_PostCleaning3) {
			LCD_DigitSetOp(LCD_DadeOut_Fast, LCD_Symbol_None, _halfsecond); LCD_DigitSetNumberOrTime(-1, false, false, false);
			Valve_WaterInlet(false); Valve_CleanMembrane(true);
			if(_seconds >= REVOSM_VALVE_DELAY) { _ro_state = ROState_Idle; }
		}
		else if(_ro_state == ROState_ErrorHighTankPressure) {
			LCD_DigitSetOp(LCD_DadeOut_None, LCD_Symbol_o, true); LCD_DigitSetNumberOrTime(-1, false, false, true);
			Valve_WaterInlet(false); Valve_CleanMembrane(false);
			if(_buttons_trigered & Button_EnterLong) {
				DBG_PutString("User approved malfunction. Entering idle state."NL);
				Buzzer_PlayMelody(BuzzerMelody_Success);
				_ro_state = ROState_Idle;
			}
		}

		// Debug mode
		if(_second_changed && ((_buttons_pressed & (Button_PrevLong | Button_NextLong | Button_EnterLong)) == (Button_PrevLong | Button_NextLong | Button_EnterLong) || (_is_debug_mode && _seconds >= REVOSM_DEBUGMODE_TIMEOUT))) {
			DBG_PutString(_is_debug_mode ? "=== Canceling Debug Mode ==="NL : "=== Entering Debug Mode ==="NL);
			_is_debug_mode = !_is_debug_mode;
			if(_is_debug_mode) {
				_is_water_inlet = _is_clean_membrane = false;
				_seconds = 0; _ro_state = ROState_Idle;
			}
		}

		if(Frames_IsFrameAvailable()) {
			eFrameCmd _frame_cmd = (eFrameCmd)Frames_GetFrameCommand();
			uint8_t _frame_size = Frames_GetFrameSize();
			uint8_t _frame_data[_frame_size];
			Frames_GetFrameData(_frame_data, 0, _frame_size);
			Frames_RemoveFrame();

			if(_frame_cmd == FRAME_CMD_TEST_COMMAND && _frame_size > 0) {
			}
		}
		else if(Frames_IsTransmitterFree()) {
			//Frames_PutFrame((uint8_t)FRAME_CMD_STATUS, &_status, sizeof(sStatus));
		}
	}
}
