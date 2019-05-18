#include "display.h"
#include "lcd.h"
#include "lcd.h"
#include "periph.h"

static bool s_animate_with_digits = true;

static eDispBatterySoC s_battery_soc = DispBatterySoc_None;
static uint8_t s_soc_animation = UINT8_MAX;

void disp_cycle() {
    static uint16_t _s_timer_prev;
    uint16_t _timer_current;

    eLCDBatterySocBits battery_soc_bits = 0;

    _timer_current = periph_get_timer();
    if ((_timer_current - _s_timer_prev) > TIMER_TP_MS(50)) {
        _s_timer_prev = _timer_current;

        // Battery SoC
        {
            #define FLASHING_SHIFT 3
            #define CHARGING_SHIFT 3
            #define RECUPERATING_SHIFT 0

            uint8_t _level = 0;
            bool _is_rlevel = false;
            if(s_battery_soc == DispBatterySoc_Flashing) {
                if(s_soc_animation >= (2 << FLASHING_SHIFT)) s_soc_animation = 0;
                if(s_soc_animation & (1 << FLASHING_SHIFT)) battery_soc_bits = LCDBatterySoC__All;
            } else if(s_battery_soc >= DispBatterySoc_Charging0 && s_battery_soc <= DispBatterySoc_Charging3) {
                battery_soc_bits |= LCDBatterySoC_Body;
                if(s_soc_animation >= (5 << CHARGING_SHIFT)) s_soc_animation = (s_battery_soc - DispBatterySoc_Charging0) << CHARGING_SHIFT;
                _level = s_soc_animation >> CHARGING_SHIFT;
            } else if(s_battery_soc == DispBatterySoc_Recuperating) {
                battery_soc_bits |= LCDBatterySoC_Body;
                if(s_soc_animation >= (17 << RECUPERATING_SHIFT)) s_soc_animation = 0;
                _level = s_soc_animation >> RECUPERATING_SHIFT;
                if(_level >= 8) {
                    _level -= 8;
                    _is_rlevel = true;
                }
                if(_level >= 4) {
                    _is_rlevel = !_is_rlevel;
                    _level = 8 - _level;
                }
            } else if(s_battery_soc >= DispBatterySoc_Empty && s_battery_soc <= DispBatterySoc_Full) {
                battery_soc_bits |= LCDBatterySoC_Body;
                _level = s_battery_soc - DispBatterySoc_Empty;
            }
            s_soc_animation++;

            if(_is_rlevel) {
                if(_level > 0) battery_soc_bits |= LCDBatterySoC_SoC_4;
                if(_level > 1) battery_soc_bits |= LCDBatterySoC_SoC_3;
                if(_level > 2) battery_soc_bits |= LCDBatterySoC_SoC_2;
                if(_level > 3) battery_soc_bits |= LCDBatterySoC_SoC_1;
            } else {
                if(_level > 0) battery_soc_bits |= LCDBatterySoC_SoC_1;
                if(_level > 1) battery_soc_bits |= LCDBatterySoC_SoC_2;
                if(_level > 2) battery_soc_bits |= LCDBatterySoC_SoC_3;
                if(_level > 3) battery_soc_bits |= LCDBatterySoC_SoC_4;
            }
        }

        lcd_set_battery_soc_bits(battery_soc_bits);
        lcd_flush();
    }
}

void disp_set_battery_soc(eDispBatterySoC battery_soc) {
    s_battery_soc = battery_soc;
    s_soc_animation = UINT8_MAX;
}
