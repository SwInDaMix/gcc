#include <string.h>
#include <display.h>
#include "display.h"
#include "lcd.h"
#include "lcd.h"
#include "periph.h"
#include "utils.h"

#define DISP_FLAG(f) (1L << (f))

#define DISP_DIV_0 DISP_FLAG(0)
#define DISP_DIV_1 DISP_FLAG(1)
#define DISP_DIV_2 DISP_FLAG(2)
#define DISP_DIV_3 DISP_FLAG(3)

static eDispState s_state = true;
static uint32_t s_bits = 0;
static uint32_t s_bits_flashing = 0;
static eLCDDigit s_digits[LCDDigitOffset__Max];
static uint32_t s_digits_flashing = 0;
static eLCDBatterySocBits s_battery_soc_bits = (eLCDBatterySocBits)0;
static uint8_t s_disp_animation_main = 0;

static void disp_fill_digits16(uint16_t *number, eLCDDigitOffset digit_offset, bool is_revers, uint8_t digits, uint8_t forced_digits) {
    while ((digits--)) {
        if(*number) {
            uint8_t _rem = divu16(number, 10);
            s_digits[digit_offset] = LCDDigit_0 + _rem;
        } else {
            if(forced_digits) s_digits[digit_offset] = LCDDigit_0;
            else break;
        }
        if(is_revers) digit_offset++; else digit_offset--;
        if(forced_digits) forced_digits--;
    }
}

static void disp_fill_digits32(uint32_t *number, eLCDDigitOffset digit_offset, bool is_revers, uint8_t digits, uint8_t forced_digits) {
    while ((digits--)) {
        if(*number) {
            uint8_t _rem = divu32(number, 10);
            s_digits[digit_offset] = LCDDigit_0 + _rem;
        } else {
            if(forced_digits) s_digits[digit_offset] = LCDDigit_0;
            else break;
        }
        if(is_revers) digit_offset++; else digit_offset--;
        if(forced_digits) forced_digits--;
    }
}

static uint8_t s_disp_animation_with_digits = 0;
static uint8_t s_disp_animation_battery_soc0 = UINT8_MAX;
static uint8_t s_disp_animation_battery_soc3 = UINT8_MAX;

// ==============================
// Animate with digits processing
// ==============================
static void disp_cycle_animate_with_digits() {
    #define ANIMATE_WITH_DIGITS_COUNT (LCDDigit_ROTD5 - LCDDigit_ROT0 + 1)

    if(s_disp_animation_with_digits >= ANIMATE_WITH_DIGITS_COUNT) s_state = DispState_Main;
    else memset(s_digits, LCDDigit_ROT0 + s_disp_animation_with_digits, LCDDigitOffset__Max);
}

// ======================
// battery_soc processing
// ======================
static sDispMain s_main;
static void disp_cycle_main() {
    #define SOC_LEVEL_COUNT 4

    // process battery SoC
    {
        uint8_t _level = 0;
        bool _is_rlevel = false;

        if(s_main.battery_soc == DispBatterySoc_Flashing) {
            if(s_disp_animation_battery_soc3 >= 2) s_disp_animation_battery_soc3 = 0;
            if(s_disp_animation_battery_soc3 & 1) s_battery_soc_bits = LCDBatterySoC_Body;
        } else if(s_main.battery_soc >= DispBatterySoc_Charging0 && s_main.battery_soc <= DispBatterySoc_Charging3) {
            s_battery_soc_bits |= LCDBatterySoC_Body;
            if(s_disp_animation_battery_soc3 >= (SOC_LEVEL_COUNT + 1)) s_disp_animation_battery_soc3 = s_main.battery_soc - DispBatterySoc_Charging0;
            _level = s_disp_animation_battery_soc3;
        } else if(s_main.battery_soc == DispBatterySoc_Recuperating || s_main.battery_soc == DispBatterySoc_Progress) {
            if(s_main.battery_soc == DispBatterySoc_Recuperating) s_battery_soc_bits |= LCDBatterySoC_Body;
            if(s_disp_animation_battery_soc0 >= (SOC_LEVEL_COUNT * 4 + 1)) s_disp_animation_battery_soc0 = 0;
            _level = s_disp_animation_battery_soc0;
            if(_level >= (SOC_LEVEL_COUNT * 2)) {
                _level -= (SOC_LEVEL_COUNT * 2);
                _is_rlevel = true;
            }
            if(_level >= SOC_LEVEL_COUNT) {
                _is_rlevel = !_is_rlevel;
                _level = (SOC_LEVEL_COUNT * 2) - _level;
            }
        } else if(s_main.battery_soc >= DispBatterySoc_Empty && s_main.battery_soc <= DispBatterySoc_Full) {
            s_battery_soc_bits |= LCDBatterySoC_Body;
            _level = s_main.battery_soc - DispBatterySoc_Empty;
        }

        if(_is_rlevel) {
            if(_level > 0) s_battery_soc_bits |= LCDBatterySoC_SoC_4;
            if(_level > 1) s_battery_soc_bits |= LCDBatterySoC_SoC_3;
            if(_level > 2) s_battery_soc_bits |= LCDBatterySoC_SoC_2;
            if(_level > 3) s_battery_soc_bits |= LCDBatterySoC_SoC_1;
        } else {
            if(_level > 0) s_battery_soc_bits |= LCDBatterySoC_SoC_1;
            if(_level > 1) s_battery_soc_bits |= LCDBatterySoC_SoC_2;
            if(_level > 2) s_battery_soc_bits |= LCDBatterySoC_SoC_3;
            if(_level > 3) s_battery_soc_bits |= LCDBatterySoC_SoC_4;
        }
    }

    if(s_main.flags & DispMainFlags_Light) { s_bits |= DISP_FLAG(LCDBit_Light); }
    if(s_main.flags & DispMainFlags_Brake) { s_bits |= DISP_FLAG(LCDBit_Brake); }

    // process gear
    {
        if(s_main.gear == 0) {
            s_digits[LCDDigitOffset_Gear] = LCDDigit_P;
            s_digits_flashing |= DISP_FLAG(LCDDigitOffset_Gear);
        } else if(s_main.gear < 10) {
            s_digits[LCDDigitOffset_Gear] = LCDDigit_0 + s_main.gear;
            if(s_main.flags & DispMainFlags_Assist) s_bits |= DISP_FLAG(LCDBit_Gear_Assist);
            if(s_main.flags & DispMainFlags_Cruise) { s_bits |= DISP_FLAG(LCDBit_Gear_Cruise); s_bits_flashing |= DISP_FLAG(LCDBit_Gear_Cruise); }
        } else s_digits[LCDDigitOffset_Gear] = LCDDigit_o;
    }

    // process speed
    {
        uint16_t _speed;
        if(s_main.state == DispMainState_Statistic1) { _speed = s_main.speed.max; s_bits |= DISP_FLAG(LCDBit_Speed_MXS); }
        else if(s_main.state == DispMainState_Statistic2) { _speed = s_main.speed.avg; s_bits |= DISP_FLAG(LCDBit_Speed_AVS); }
        else _speed = s_main.speed.current;

        _speed /= 50;
        if(_speed < 1000) {
            disp_fill_digits16(&_speed, LCDDigitOffset_Speed_3, true, 3, 2);
            s_bits |= DISP_FLAG(s_main.measure_unit == DispMainMeasureUnit_Imperic ? LCDBit_Speed_Mph : LCDBit_Speed_Kmh);
            s_bits |= DISP_FLAG(LCDBit_Speed_Dot);
        } else s_digits[LCDDigitOffset_Speed_1] = s_digits[LCDDigitOffset_Speed_2] = LCDDigit_o;
        if(s_main.flags & DispMainFlags_Walking) { s_bits |= DISP_FLAG(LCDBit_Speed_Walking); s_bits_flashing |= DISP_FLAG(LCDBit_Speed_Walking); }
    }

    // process temperature and amp/wattage
    {
        if(s_main.state == DispMainState_Statistic1) {
            // system temperature
            uint16_t _temp = UINT16_MAX;
            bool _negative = false, _is_below_10 = false;
            if(s_main.sensors.temp_system >= 0 && s_main.sensors.temp_system <= 399) {
                _temp = s_main.sensors.temp_system;
                if(_temp >= 200) { _temp -= 200; s_digits_flashing |= (DISP_FLAG(LCDDigitOffset_PowerTemp_1) | DISP_FLAG(LCDDigitOffset_PowerTemp_2) | DISP_FLAG(LCDDigitOffset_PowerTemp_3)); }
            }
            else if(s_main.sensors.temp_system < 0 && s_main.sensors.temp_system >= -99) { _temp = -s_main.sensors.temp_system; _negative = true; _is_below_10 = _temp < 10; }
            if(_temp != UINT16_MAX) {
                disp_fill_digits16(&_temp, LCDDigitOffset_PowerTemp_3, true, 3, 1);
                if(_negative) s_digits[_is_below_10 ? LCDDigitOffset_PowerTemp_2 : LCDDigitOffset_PowerTemp_1] = LCDDigit_Minus;
                s_bits |= DISP_FLAG(s_main.measure_unit == DispMainMeasureUnit_Imperic ? LCDBit_PowerTemp_F : LCDBit_PowerTemp_C);
            } else s_digits[LCDDigitOffset_PowerTemp_2] = s_digits[LCDDigitOffset_PowerTemp_3] = LCDDigit_o;
        } else {
            // wattage
            uint16_t _wattage = s_main.state == DispMainState_Statistic2 ? s_main.sensors.wattage_consumed : ((uint32_t)(s_main.sensors.battery_voltage / 50) * (uint16_t)(s_main.sensors.current / 100)) / 100;
            if(_wattage < 2000) {
                disp_fill_digits16(&_wattage, LCDDigitOffset_PowerTemp_3, true, 3, 1);
                if(_wattage) s_bits |= DISP_FLAG(LCDBit_PowerTemp_One);
                s_bits |= DISP_FLAG(LCDBit_PowerTemp_W);
            } else s_digits[LCDDigitOffset_PowerTemp_2] = s_digits[LCDDigitOffset_PowerTemp_3] = LCDDigit_o;
            if(s_main.state == DispMainState_Drive) s_bits |= DISP_FLAG(LCDBit_PowerTemp_Motor);
        }
    }

    // motor temperature / acceleration animation
    if(s_main.flags & DispMainFlags_Accelerating) {
        #define ANIMATE_ACCELERATION_COUNT (LCDDigit_ROTD5 - LCDDigit_ROTD0 + 1)

        if(s_disp_animation_with_digits >= ANIMATE_ACCELERATION_COUNT) s_disp_animation_with_digits = 0;
        s_digits[LCDDigitOffset_Temp_1] = s_digits[LCDDigitOffset_Temp_2] = LCDDigit_ROTD0 + s_disp_animation_with_digits;
    } else {
        uint16_t _temp = UINT16_MAX;
        bool _negative = false, _is_below_10 = false;
        if(s_main.sensors.temp_motor >= 0 && s_main.sensors.temp_motor <= 399) {
            _temp = s_main.sensors.temp_motor;
            if(_temp >= 200) { _temp -= 200; s_bits_flashing |= DISP_FLAG(LCDBit_Temp_One); s_digits_flashing |= (DISP_FLAG(LCDDigitOffset_Temp_1) | DISP_FLAG(LCDDigitOffset_Temp_2)); }
        }
        else if(s_main.sensors.temp_motor < 0 && s_main.sensors.temp_motor >= -99) { _temp = -s_main.sensors.temp_motor; _negative = true; _is_below_10 = _temp < 10; }
        if(_temp != UINT16_MAX) {
            disp_fill_digits16(&_temp, LCDDigitOffset_Temp_2, false, 2, 1);
            if(_negative) {
                if(_is_below_10) s_digits[LCDDigitOffset_Temp_1] = LCDDigit_Minus;
                else s_bits |= DISP_FLAG(LCDBit_Temp_Minus);
            }
            if(_temp) s_bits |= DISP_FLAG(LCDBit_Temp_One);
            s_bits |= DISP_FLAG(s_main.measure_unit == DispMainMeasureUnit_Imperic ? LCDBit_Temp_F : LCDBit_Temp_C);
        } else s_digits[LCDDigitOffset_Temp_1] = s_digits[LCDDigitOffset_Temp_2] = LCDDigit_o;
    }

    // process distance
    {
        if(s_main.state == DispMainState_Statistic1) {
            // system voltage
            uint16_t _voltage = s_main.sensors.battery_voltage / 50;
            disp_fill_digits16(&_voltage, LCDDigitOffset_ODOVolts_5, false, 5, 2);
            s_bits |= DISP_FLAG(LCDBit_ODOVolts_Dot);
            s_bits |= DISP_FLAG(LCDBit_ODOVolts_Vol);
        } else {
            // distance
            uint32_t  _dist;
            if(s_main.state == DispMainState_Statistic2) { _dist = s_main.distance.odometer; s_bits |= DISP_FLAG(LCDBit_ODOVolts_ODO); }
            else { _dist = s_main.distance.session; s_bits |= DISP_FLAG(LCDBit_ODOVolts_DST); }
            if(_dist < 100000L) {
                disp_fill_digits32(&_dist, LCDDigitOffset_ODOVolts_5, false, 5, 2);
                s_bits |= DISP_FLAG(LCDBit_ODOVolts_Dot);
                s_bits |= DISP_FLAG(s_main.measure_unit == DispMainMeasureUnit_Imperic ? LCDBit_ODOVolts_Mil : LCDBit_ODOVolts_Km);
            } else s_digits[LCDDigitOffset_ODOVolts_3] = s_digits[LCDDigitOffset_ODOVolts_4] = LCDDigit_o;
        }
    }

    // process traveling time
    {
        uint16_t _time;
        if(s_main.state == DispMainState_Drive) { _time = s_main.time.session; s_bits |= DISP_FLAG(LCDBit_TravelTime_TM); }
        else { _time = s_main.time.total; s_bits |= DISP_FLAG(LCDBit_TravelTime_TTM); }
        if(_time < 60000) {
            uint16_t _min = divu16(&_time, 60);
            disp_fill_digits16(&_min, LCDDigitOffset_TravelTime_5, true, 2, 2);
            disp_fill_digits16(&_time, LCDDigitOffset_TravelTime_3, true, 3, 1);
            if(s_main.state != DispMainState_Drive || s_main.time.half_second) s_bits |= DISP_FLAG(LCDBit_TravelTime_Colon);
        } else s_digits[LCDDigitOffset_TravelTime_4] = s_digits[LCDDigitOffset_TravelTime_5] = LCDDigit_o;
    }
}

void disp_cycle() {
    static uint16_t _s_timer_prev;
    uint16_t _timer_current;

    _timer_current = periph_get_timer();
    if ((_timer_current - _s_timer_prev) > TIMER_TP_MS(50)) {
        eDispState _state_prev;
        _s_timer_prev = _timer_current;

        // process display states
        do {
            _state_prev = s_state;

            s_battery_soc_bits = (eLCDBatterySocBits)0;
            s_bits = (eLCDBit)0;
            s_bits_flashing = 0;
            memset(s_digits, LCDDigit_None, LCDDigitOffset__Max);
            s_digits_flashing = 0;

            if (s_state == DispState_AnimateWithDigits) {
                disp_cycle_animate_with_digits();
            } else if(s_state == DispState_Main) {
                disp_cycle_main();
            } else s_state = DispState_Main;
        } while (_state_prev != s_state);

        // update lcd
        lcd_set_battery_soc_bits(s_battery_soc_bits);
        {
            uint8_t _cnt = 0;
            uint32_t _msk = 1;
            for (_cnt = 0; _cnt < LCDBit__Max; _cnt++) {
                bool _bit = s_bits & _msk;
                if(s_bits_flashing & _msk && !(s_disp_animation_main & DISP_DIV_3)) _bit = false;
                lcd_set_bit((eLCDBit)_cnt, _bit);
                _msk <<= 1;
            }
        }
        {
            uint8_t _cnt = 0;
            uint32_t _msk = 1;
            for (_cnt = 0; _cnt < LCDDigitOffset__Max; _cnt++) {
                eLCDDigit _digit = s_digits[_cnt];
                if(s_digits_flashing & _msk && !(s_disp_animation_main & DISP_DIV_3)) _digit = LCDDigit_None;
                lcd_set_digit((eLCDDigitOffset)_cnt, _digit);
                _msk <<= 1;
            }
        }
        lcd_flush();

        // increment animations
        if(s_disp_animation_main & DISP_DIV_0) {
            if(s_disp_animation_main & DISP_DIV_1) {
                if(s_disp_animation_main & DISP_DIV_2) {
                    s_disp_animation_battery_soc3++;
                }
            }
            s_disp_animation_with_digits++;
        }
        s_disp_animation_main++;
        s_disp_animation_battery_soc0++;
    }
}

eDispState disp_get_state() { return s_state; }

void disp_state_animate_digits() {
    s_disp_animation_with_digits = 0;
    s_state = DispState_AnimateWithDigits;
}

void disp_state_main(sDispMain const *main) {
    memcpy(&s_main, main, sizeof(sDispMain));
}
