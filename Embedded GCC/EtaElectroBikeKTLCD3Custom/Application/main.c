#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <display.h>
#include "main.h"
#include "periph.h"
#include "ht1622.h"
#include "display.h"
#include "lcd.h"
#include "utils.h"

uint8_t cnt_soc = 0;
uint8_t cnt_dig = 0;
static sDispMain _s_disp_main;
bool anim = false;

static void uar_cb(uint8_t byte) {
    if(byte == 'q') cnt_soc++;
    if(byte == 'a') cnt_soc--;
    if(byte == 'w') cnt_dig++;
    if(byte == 's') cnt_dig--;
    if(byte == 'e') { _s_disp_main.sensors.temp_motor++; _s_disp_main.sensors.temp_system++; }
    if(byte == 'd') { _s_disp_main.sensors.temp_motor--; _s_disp_main.sensors.temp_system--; }
    if(byte == 'r') _s_disp_main.speed.current += 50;
    if(byte == 'f') _s_disp_main.speed.current -= 50;
    if(byte == 'c') { _s_disp_main.state++; if(_s_disp_main.state >= DispMainState__Max) _s_disp_main.state = 0; }
    if(byte == 'v') { _s_disp_main.measure_unit ^= 1; }
    if(byte == 'b') { _s_disp_main.flags ^= (DispMainFlags_Accelerating); }
    if(byte == 'n') { _s_disp_main.flags ^= (DispMainFlags_Brake); }
    if(byte == 'm') { _s_disp_main.flags ^= (DispMainFlags_Light); periph_set_backlight_pwm_duty_cycle(_s_disp_main.flags & (DispMainFlags_Light) ? 128 : 0); }
    if(byte == ' ') anim = true;
    if(byte == 'z') periph_set_onoff_power(false);
}

int main(void) {
    sHT1622Frame frame;
    static uint16_t _s_timer_prev;
    static uint8_t _s_cnt_soc_prev, _s_cnt_dig_prev;
    uint16_t _timer_current;

    periph_init();
    ht1622_init();

    periph_wdt_enable();

    periph_set_backlight_pwm_duty_cycle(128);
    disp_state_animate_digits();
    while (disp_get_state() == DispState_AnimateWithDigits) {
        periph_wdt_reset();
        periph_set_onoff_power(periph_get_buttons() & PeriphButton_OnOff);
        disp_cycle();
    }

    periph_uart_set_on_received_callback(uar_cb);
    periph_set_onoff_power(true);

    while (true) {
        periph_wdt_reset();

        if(anim) {
            disp_state_animate_digits();
            anim = false;
        }

        _s_disp_main.speed.avg = 22800;
        _s_disp_main.speed.max = 28350;
        _s_disp_main.distance.session = 123;
        _s_disp_main.distance.odometer = 72345;
        _s_disp_main.time.total = 1455;
        _s_disp_main.sensors.current = 15000;
        _s_disp_main.sensors.battery_voltage = 23400;
        _s_disp_main.sensors.wattage_consumed = 1599;

        disp_cycle();

        if(cnt_soc == UINT8_MAX) cnt_soc = DispBatterySoc_Full;
        if(cnt_soc > DispBatterySoc_Full) cnt_soc = 0;
        if(_s_cnt_soc_prev != cnt_soc) {
            _s_cnt_soc_prev = cnt_soc;
            DBGF("soc: %d\n", cnt_soc);
            _s_disp_main.battery_soc = cnt_soc;
        }

        if(cnt_dig == UINT8_MAX) cnt_dig = LCDDigit__Max -1;
        if(cnt_dig >= LCDDigit__Max) cnt_dig = 0;
        if(_s_cnt_dig_prev != cnt_dig) {
            _s_cnt_dig_prev = cnt_dig;
            DBGF("dig: %d\n", cnt_dig);
            _s_disp_main.test_dig = cnt_dig;
        }

        {
            uint32_t _hs = periph_get_halfseconds();
            _s_disp_main.time.session = (uint16_t)(_hs >> 1);
            _s_disp_main.time.half_second = !(_hs & 1);
        }
        disp_state_main(&_s_disp_main);

//        if(_s_cnt_prev != cnt_soc) {
//            uint8_t _i = cnt_soc >> 3;
//            uint8_t _b = cnt_soc & 7;
//
//            _s_cnt_prev = cnt_soc;
//            DBGF("cnt_soc: %d (%d:%d)\n", cnt_soc, _i, _b);
//
//            memset(&frame.buffer, 0, sizeof(frame.buffer));
//            frame.buffer[_i] = 1 << _b;
//            ht1622_send_frame(&frame);
//        }

        _timer_current = periph_get_timer();
        if ((_timer_current - _s_timer_prev) > TIMER_TP_MS(50)) {

            _s_timer_prev = _timer_current;
        }
    }
}
