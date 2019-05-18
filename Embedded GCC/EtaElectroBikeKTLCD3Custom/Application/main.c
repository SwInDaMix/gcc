#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "periph.h"
#include "ht1622.h"
#include "display.h"
#include "utils.h"

uint8_t cnt = 0;
static void uar_cb(uint8_t byte) {
    if(byte == 'a') cnt--;
    if(byte == 's') cnt++;
}

int main(void) {
    sHT1622Frame frame;
    static uint16_t _s_timer_prev;
    static uint8_t _s_cnt_prev;
    uint16_t _timer_current;

    periph_init();
    ht1622_init();
    periph_set_onoff_power(true);
    //periph_set_power_up(true);
    periph_set_enable_backlight(true);

    periph_uart_set_on_received_callback(uar_cb);

    periph_wdt_enable();

    while (true) {
        periph_wdt_reset();

        disp_cycle();

        if(_s_cnt_prev != cnt) {
            if(cnt > DispBatterySoc_Full) cnt = 0;
            _s_cnt_prev = cnt;
            DBGF("cnt: %d\n", cnt);
            disp_set_battery_soc(cnt);
        }

//        if(_s_cnt_prev != cnt) {
//            uint8_t _i = cnt >> 3;
//            uint8_t _b = cnt & 7;
//
//            _s_cnt_prev = cnt;
//            DBGF("cnt: %d (%d:%d)\n", cnt, _i, _b);
//
//            memset(&frame.buffer, 0, sizeof(frame.buffer));
//            frame.buffer[_i] = 1 << _b;
//            ht1622_send_frame(&frame);
//        }
//
//        _timer_current = periph_get_timer();
//        if ((_timer_current - _s_timer_prev) > TIMER_TP_MS(50)) {
//
//            _s_timer_prev = _timer_current;
//        }
    }
}
