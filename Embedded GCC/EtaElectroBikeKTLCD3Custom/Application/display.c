#include <string.h>
#include <display.h>

#include "display.h"
#include "lcd.h"
#include "buttons.h"
#include "periph.h"
#include "utils.h"

#define DISP_FLAG(f) (1UL << (f))

#define DISP_DIV_0 (DISP_FLAG(0))
#define DISP_DIV_1 (DISP_FLAG(1))
#define DISP_DIV_2 (DISP_FLAG(2))
#define DISP_DIV_3 (DISP_FLAG(3))

static uint32_t s_bits = 0;
static uint32_t s_bits_flashing = 0;
static eLCDDigit s_digits[LCDDigitOffset__Max];
static uint32_t s_digits_flashing = 0;
static eLCDBatterySocBits s_battery_soc_bits = LCDBatterySocBit__None;
static uint8_t s_disp_animation_main = 0;

static void disp_fill_digits16(uint16_t *number, eLCDDigitOffset digit_offset, bool is_revers, uint8_t digits, uint8_t forced_digits) {
    while((digits--)) {
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
    while((digits--)) {
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

// ======================================
// Animate with digits screen processing
// ======================================
static eDispState disp_cycle_animate_with_digits() {
    #define ANIMATE_WITH_DIGITS_COUNT (LCDDigit_ROTD5 - LCDDigit_ROT0 + 1)

    eDispState _res = DispState_AnimateWithDigits;

    if(s_disp_animation_with_digits >= ANIMATE_WITH_DIGITS_COUNT) { s_disp_animation_with_digits = 0; _res = DispState_Main; }
    else memset(s_digits, LCDDigit_ROT0 + s_disp_animation_with_digits, LCDDigitOffset__Max);

    return _res;
}

// =======================
// Main screen processing
// =======================
static sDispScreenMain *s_screen_main_ptr;
static sDispScreenMain s_screen_main;
static eDispState disp_cycle_main() {
    #define SOC_LEVEL_COUNT 4

    bool _is_measure_flashing;

    // TODO: mutex (not needed now because FW is single-threaded)
    if(s_screen_main_ptr) memcpy(&s_screen_main, s_screen_main_ptr, sizeof(sDispScreenMain));
    else memset(&s_screen_main, 0, sizeof(sDispScreenMain));
    // TODO: mutex (not needed now because FW is single-threaded)

    _is_measure_flashing = s_screen_main.flashing == DispMainFlashing_MeasureUnitMetric || s_screen_main.flashing == DispMainFlashing_MeasureUnitImperic;

#ifdef DEBUG
    // process test screen
    if(s_screen_main.state == DispMainState_Test) {
        s_screen_main.battery_soc = s_screen_main.test.battery_soc;
    }
#endif

    // process battery SoC
    {
        uint8_t _level = 0;
        bool _is_rlevel = false;

        if(s_screen_main.battery_soc == DispBatterySoc_Flashing) {
            if(s_disp_animation_battery_soc3 >= 2) s_disp_animation_battery_soc3 = 0;
            if(s_disp_animation_battery_soc3 & 1) s_battery_soc_bits = LCDBatterySocBit_Body;
        } else if(s_screen_main.battery_soc >= DispBatterySoc_Charging0 && s_screen_main.battery_soc <= DispBatterySoc_Charging3) {
            s_battery_soc_bits |= LCDBatterySocBit_Body;
            if(s_disp_animation_battery_soc3 >= (SOC_LEVEL_COUNT + 1)) s_disp_animation_battery_soc3 = s_screen_main.battery_soc - DispBatterySoc_Charging0;
            _level = s_disp_animation_battery_soc3;
        } else if(s_screen_main.battery_soc == DispBatterySoc_Recuperating) {
            s_battery_soc_bits |= LCDBatterySocBit_Body;
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
        } else if(s_screen_main.battery_soc >= DispBatterySoc_Empty && s_screen_main.battery_soc <= DispBatterySoc_Full) {
            s_battery_soc_bits |= LCDBatterySocBit_Body;
            _level = s_screen_main.battery_soc - DispBatterySoc_Empty;
        }

        if(_is_rlevel) {
            if(_level > 0) s_battery_soc_bits |= LCDBatterySocBit_SoC_4;
            if(_level > 1) s_battery_soc_bits |= LCDBatterySocBit_SoC_3;
            if(_level > 2) s_battery_soc_bits |= LCDBatterySocBit_SoC_2;
            if(_level > 3) s_battery_soc_bits |= LCDBatterySocBit_SoC_1;
        } else {
            if(_level > 0) s_battery_soc_bits |= LCDBatterySocBit_SoC_1;
            if(_level > 1) s_battery_soc_bits |= LCDBatterySocBit_SoC_2;
            if(_level > 2) s_battery_soc_bits |= LCDBatterySocBit_SoC_3;
            if(_level > 3) s_battery_soc_bits |= LCDBatterySocBit_SoC_4;
        }
    }

    if(s_screen_main.flags & DispMainFlags_Light) { s_bits |= DISP_FLAG(LCDBit_Light); }
    if(s_screen_main.flags & DispMainFlags_Brake) { s_bits |= DISP_FLAG(LCDBit_Brake); }

#ifdef DEBUG
    // process test screen
    if(s_screen_main.state == DispMainState_Test) {
        memset(s_digits, s_screen_main.test.digit, sizeof(s_digits));
        s_bits |= DISP_FLAG(s_screen_main.test.bit);
        return DispMainState_Test;
    }
#endif

    // process gear
    if(s_screen_main.flashing == DispMainFlashing_None) {
        if(s_screen_main.gear == 0) {
            s_digits[LCDDigitOffset_Gear] = LCDDigit_P;
            s_digits_flashing |= DISP_FLAG(LCDDigitOffset_Gear);
        } else if(s_screen_main.gear < 10) {
            s_digits[LCDDigitOffset_Gear] = LCDDigit_0 + s_screen_main.gear;
            if(s_screen_main.flags & DispMainFlags_Assist) s_bits |= DISP_FLAG(LCDBit_Gear_Assist);
            if(s_screen_main.flags & DispMainFlags_Cruise) { s_bits |= DISP_FLAG(LCDBit_Gear_Cruise); s_bits_flashing |= DISP_FLAG(LCDBit_Gear_Cruise); }
        } else s_digits[LCDDigitOffset_Gear] = LCDDigit_o;
    } else {
        s_digits[LCDDigitOffset_Gear] = LCDDigit_F;
    }

    // process measure unit flashing
    if(_is_measure_flashing) {
        uint32_t _flags = s_screen_main.flashing == DispMainFlashing_MeasureUnitImperic ?
                          DISP_FLAG(LCDBit_Temp_F) | DISP_FLAG(LCDBit_PowerTemp_F) | DISP_FLAG(LCDBit_ODOVolts_Mil) | DISP_FLAG(LCDBit_Speed_Mph) :
                          DISP_FLAG(LCDBit_Temp_C) | DISP_FLAG(LCDBit_PowerTemp_C) | DISP_FLAG(LCDBit_Speed_Kmh) | DISP_FLAG(LCDBit_ODOVolts_Km);
        s_bits |= _flags;
        s_bits_flashing |= _flags;
    } else {
        // process speed
        {
            bool _is_max_flashing = s_screen_main.flashing == DispMainFlashing_MaxSpeed;
            if(_is_max_flashing || s_screen_main.flashing == DispMainFlashing_None) {
                uint16_t _speed;

                if(s_screen_main.state == DispMainState_Statistic1 || _is_max_flashing) { _speed = s_screen_main.speed.max; s_bits |= DISP_FLAG(LCDBit_Speed_MXS); if(_is_max_flashing) s_bits_flashing |= DISP_FLAG(LCDBit_Speed_MXS); }
                else if(s_screen_main.state == DispMainState_Statistic2) { _speed = s_screen_main.speed.avg; s_bits |= DISP_FLAG(LCDBit_Speed_AVS); }
                else _speed = s_screen_main.speed.current;

                _speed = (_speed + 25) / 50;
                if(_speed < 1000) {
                    disp_fill_digits16(&_speed, LCDDigitOffset_Speed_3, true, 3, 2);
                    s_bits |= DISP_FLAG(s_screen_main.measure_unit == DispMainMeasureUnit_Imperic ? LCDBit_Speed_Mph : LCDBit_Speed_Kmh);
                    s_bits |= DISP_FLAG(LCDBit_Speed_Dot);
                } else s_digits[LCDDigitOffset_Speed_1] = s_digits[LCDDigitOffset_Speed_2] = LCDDigit_o;
                if(_is_max_flashing) {
                    s_digits_flashing |= (DISP_FLAG(LCDDigitOffset_Speed_1) | DISP_FLAG(LCDDigitOffset_Speed_2) | DISP_FLAG(LCDDigitOffset_Speed_3));
                    s_bits_flashing |= (DISP_FLAG(LCDBit_Speed_MXS) | DISP_FLAG(LCDBit_Speed_Dot) | DISP_FLAG(LCDBit_Speed_Kmh) | DISP_FLAG(LCDBit_Speed_Mph));
                } else if(s_screen_main.flags & DispMainFlags_Walking) { s_bits |= DISP_FLAG(LCDBit_Speed_Walking); s_bits_flashing |= DISP_FLAG(LCDBit_Speed_Walking); }
            }
        }

        // process temperature and amp/wattage
        if(s_screen_main.flashing == DispMainFlashing_None) {
            if(s_screen_main.state == DispMainState_Statistic1) {
                // system temperature
                uint16_t _temp = UINT16_MAX;
                bool _negative = false, _is_below_10 = false;
                if(s_screen_main.sensors.temp_system >= 0 && s_screen_main.sensors.temp_system <= 399) {
                    _temp = s_screen_main.sensors.temp_system;
                    if(_temp >= 200) { _temp -= 200; s_digits_flashing |= (DISP_FLAG(LCDDigitOffset_PowerTemp_1) | DISP_FLAG(LCDDigitOffset_PowerTemp_2) | DISP_FLAG(LCDDigitOffset_PowerTemp_3)); }
                }
                else if(s_screen_main.sensors.temp_system < 0 && s_screen_main.sensors.temp_system >= -99) { _temp = -s_screen_main.sensors.temp_system; _negative = true; _is_below_10 = _temp < 10; }
                if(_temp != UINT16_MAX) {
                    disp_fill_digits16(&_temp, LCDDigitOffset_PowerTemp_3, true, 3, 1);
                    if(_negative) s_digits[_is_below_10 ? LCDDigitOffset_PowerTemp_2 : LCDDigitOffset_PowerTemp_1] = LCDDigit_Minus;
                    s_bits |= DISP_FLAG(s_screen_main.measure_unit == DispMainMeasureUnit_Imperic ? LCDBit_PowerTemp_F : LCDBit_PowerTemp_C);
                } else s_digits[LCDDigitOffset_PowerTemp_2] = s_digits[LCDDigitOffset_PowerTemp_3] = LCDDigit_o;
            } else {
                // wattage
                uint16_t _wattage = s_screen_main.state == DispMainState_Statistic2 ? s_screen_main.stat.wattage_consumed : s_screen_main.sensors.wattage;
                if(_wattage < 2000) {
                    disp_fill_digits16(&_wattage, LCDDigitOffset_PowerTemp_3, true, 3, 1);
                    if(_wattage) s_bits |= DISP_FLAG(LCDBit_PowerTemp_One);
                    s_bits |= DISP_FLAG(LCDBit_PowerTemp_W);
                } else s_digits[LCDDigitOffset_PowerTemp_2] = s_digits[LCDDigitOffset_PowerTemp_3] = LCDDigit_o;
                if(s_screen_main.state == DispMainState_Drive) s_bits |= DISP_FLAG(LCDBit_PowerTemp_Motor);
            }
        }

        // motor temperature / acceleration animation
        if(s_screen_main.flags & DispMainFlags_Accelerating) {
            // acceleration animation
            #define ANIMATE_ACCELERATION_COUNT (LCDDigit_ROTD5 - LCDDigit_ROTD0 + 1)

            if(s_disp_animation_with_digits >= ANIMATE_ACCELERATION_COUNT) s_disp_animation_with_digits = 0;
            s_digits[LCDDigitOffset_Temp_1] = s_digits[LCDDigitOffset_Temp_2] = LCDDigit_ROTD0 + s_disp_animation_with_digits;
        } else if(s_screen_main.flashing == DispMainFlashing_None) {
            // motor temperature
            uint16_t _temp = UINT16_MAX;
            bool _negative = false, _is_below_10 = false;
            if(s_screen_main.sensors.temp_motor >= 0 && s_screen_main.sensors.temp_motor <= 399) {
                _temp = s_screen_main.sensors.temp_motor;
                if(_temp >= 200) { _temp -= 200; s_bits_flashing |= DISP_FLAG(LCDBit_Temp_One); s_digits_flashing |= (DISP_FLAG(LCDDigitOffset_Temp_1) | DISP_FLAG(LCDDigitOffset_Temp_2)); }
            }
            else if(s_screen_main.sensors.temp_motor < 0 && s_screen_main.sensors.temp_motor >= -99) { _temp = -s_screen_main.sensors.temp_motor; _negative = true; _is_below_10 = _temp < 10; }
            if(_temp != UINT16_MAX) {
                disp_fill_digits16(&_temp, LCDDigitOffset_Temp_2, false, 2, 1);
                if(_negative) {
                    if(_is_below_10) s_digits[LCDDigitOffset_Temp_1] = LCDDigit_Minus;
                    else s_bits |= DISP_FLAG(LCDBit_Temp_Minus);
                }
                if(_temp) s_bits |= DISP_FLAG(LCDBit_Temp_One);
                s_bits |= DISP_FLAG(s_screen_main.measure_unit == DispMainMeasureUnit_Imperic ? LCDBit_Temp_F : LCDBit_Temp_C);
            } else s_digits[LCDDigitOffset_Temp_1] = s_digits[LCDDigitOffset_Temp_2] = LCDDigit_o;
        }

        // process distance
        {
            bool _is_odo_flashing = s_screen_main.flashing == DispMainFlashing_ODO;
            if(s_screen_main.flashing == DispMainFlashing_None && s_screen_main.state == DispMainState_Statistic1) {
                // system voltage
                uint16_t _voltage = (s_screen_main.sensors.voltage + 25) / 50;
                disp_fill_digits16(&_voltage, LCDDigitOffset_ODOVolts_5, false, 5, 2);
                s_bits |= DISP_FLAG(LCDBit_ODOVolts_Dot);
                s_bits |= DISP_FLAG(LCDBit_ODOVolts_Vol);
            } else if(_is_odo_flashing || s_screen_main.flashing == DispMainFlashing_None) {
                // distance
                uint32_t  _dist;
                if(_is_odo_flashing || s_screen_main.state == DispMainState_Statistic2) { _dist = s_screen_main.stat.distance.odometer; s_bits |= DISP_FLAG(LCDBit_ODOVolts_ODO); }
                else { _dist = s_screen_main.stat.distance.session; s_bits |= DISP_FLAG(LCDBit_ODOVolts_DST); }
                _dist = (_dist + 5) / 10;
                if(_dist < 100000L) {
                    disp_fill_digits32(&_dist, LCDDigitOffset_ODOVolts_5, false, 5, 2);
                    s_bits |= DISP_FLAG(LCDBit_ODOVolts_Dot);
                    s_bits |= DISP_FLAG(s_screen_main.measure_unit == DispMainMeasureUnit_Imperic ? LCDBit_ODOVolts_Mil : LCDBit_ODOVolts_Km);
                } else s_digits[LCDDigitOffset_ODOVolts_3] = s_digits[LCDDigitOffset_ODOVolts_4] = LCDDigit_o;
                if(_is_odo_flashing) {
                    s_digits_flashing |= (DISP_FLAG(LCDDigitOffset_ODOVolts_1) | DISP_FLAG(LCDDigitOffset_ODOVolts_2) | DISP_FLAG(LCDDigitOffset_ODOVolts_3) | DISP_FLAG(LCDDigitOffset_ODOVolts_4) | DISP_FLAG(LCDDigitOffset_ODOVolts_5));
                    s_bits_flashing |= (DISP_FLAG(LCDBit_ODOVolts_Dot) | DISP_FLAG(LCDBit_ODOVolts_ODO) | DISP_FLAG(LCDBit_ODOVolts_Km) | DISP_FLAG(LCDBit_ODOVolts_Mil));
                }
            }
        }

        // process traveling time
        {
            bool _is_ttm_flashing = s_screen_main.flashing == DispMainFlashing_TTM;
            if(_is_ttm_flashing || s_screen_main.flashing == DispMainFlashing_None) {
                uint16_t _time;
                bool _is_minutes;
                if(!_is_ttm_flashing && s_screen_main.state == DispMainState_Drive) { _time = s_screen_main.stat.time.session; s_bits |= DISP_FLAG(LCDBit_TravelTime_TM); }
                else { _time = s_screen_main.stat.time.total; s_bits |= DISP_FLAG(LCDBit_TravelTime_TTM); }
                _is_minutes = _time >= 3600;
                if(_is_minutes) _time /= 60;
                if(_time < 60000) {
                    uint16_t _time_high = divu16(&_time, 60);
                    disp_fill_digits16(&_time_high, LCDDigitOffset_TravelTime_5, true, 2, 2);
                    disp_fill_digits16(&_time, LCDDigitOffset_TravelTime_3, true, 3, 1);
                    s_bits |= DISP_FLAG(LCDBit_TravelTime_Colon);
                    if(_is_minutes) s_bits_flashing |= DISP_FLAG(LCDBit_TravelTime_Colon);
                } else s_digits[LCDDigitOffset_TravelTime_4] = s_digits[LCDDigitOffset_TravelTime_5] = LCDDigit_o;
                if(_is_ttm_flashing) {
                    s_digits_flashing |= (DISP_FLAG(LCDDigitOffset_TravelTime_1) | DISP_FLAG(LCDDigitOffset_TravelTime_2) | DISP_FLAG(LCDDigitOffset_TravelTime_3) | DISP_FLAG(LCDDigitOffset_TravelTime_4) | DISP_FLAG(LCDDigitOffset_TravelTime_5));
                    s_bits |= (DISP_FLAG(LCDBit_TravelTime_Colon) | DISP_FLAG(LCDBit_TravelTime_TTM));
                    s_bits_flashing |= (DISP_FLAG(LCDBit_TravelTime_Colon) | DISP_FLAG(LCDBit_TravelTime_TTM));
                }
            }
        }
    }

    return DispState_Main;
}

// ===========================
// Settings screen processing
// ===========================
static sDispScreenSettings *s_screen_settings_ptr;
static sDispScreenSettings s_screen_settings;
static eDispState disp_cycle_settings() {
    // TODO: mutex (not needed now because FW is single-threaded)
    if(s_screen_settings_ptr) memcpy(&s_screen_settings, s_screen_settings_ptr, sizeof(sDispScreenSettings));
    else memset(&s_screen_settings, 0, sizeof(sDispScreenSettings));
    // TODO: mutex (not needed now because FW is single-threaded)

    return DispState_Settings;
}

eDispState disp_cycle(eDispState state) {
    static uint16_t _s_timer_prev;
    uint16_t _timer_current;

    _timer_current = periph_get_timer();
    if((_timer_current - _s_timer_prev) >= TIMER_MS2TT(50)) {
        static eDispState _s_state_prev;
        _s_timer_prev = _timer_current;

        // process display states
        if(_s_state_prev != state) {
            if(state == DispState_AnimateWithDigits) s_disp_animation_with_digits = 0;
        }

        s_battery_soc_bits = LCDBatterySocBit__None;
        s_bits = 0;
        s_bits_flashing = 0;
        memset(s_digits, LCDDigit_None, LCDDigitOffset__Max);
        s_digits_flashing = 0;

        if(state == DispState_AnimateWithDigits) state = disp_cycle_animate_with_digits();
        else if(state == DispState_Main) state = disp_cycle_main();
        else if(state == DispState_Settings) state = disp_cycle_settings();
        _s_state_prev = state;

        // update lcd
        lcd_set_battery_soc_bits(s_battery_soc_bits);
        {
            bool _is_blink = !buttons_is_repeating() && periph_get_mseconds() >= 500;
            uint8_t _cnt;
            uint32_t _msk = 1;
            eLCDDigit const *_digits_ptr = s_digits;
            for (_cnt = 0; _cnt < LCDBit__Max; _cnt++) {
                bool _bit = s_bits & _msk;
                if(_is_blink && (s_bits_flashing & _msk)) _bit = false;
                lcd_set_bit((eLCDBit)_cnt, _bit);
                _msk <<= 1;
            }
            _msk = 1;
            for (_cnt = 0; _cnt < LCDDigitOffset__Max; _cnt++) {
                eLCDDigit _digit = *_digits_ptr++;
                if(_is_blink && (s_digits_flashing & _msk)) _digit = LCDDigit_None;
                lcd_set_digit((eLCDDigitOffset)_cnt, _digit);
                _msk <<= 1;
            }
        }
        lcd_flush();

        // increment animations
        {
            if(s_disp_animation_main & 1) s_disp_animation_with_digits++;
            if((s_disp_animation_main & 3) == 3) s_disp_animation_battery_soc3++;
            s_disp_animation_main++;
            s_disp_animation_battery_soc0++;
        }
    }

    return state;
}

void disp_set_screens(sDispScreenMain *screen_main, sDispScreenSettings *screen_settings) {
    s_screen_main_ptr = screen_main; s_screen_settings_ptr = screen_settings;
}
