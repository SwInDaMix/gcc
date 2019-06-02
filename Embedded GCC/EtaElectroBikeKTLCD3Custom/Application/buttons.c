#include "buttons.h"

#define BUTTON_TICKS_CLICK 5
#define BUTTON_TICKS_LONGCLICK 20
#define BUTTON_TICKS_REPEAT 2

static bool s_buttons_is_repeating;

bool buttons_process(ePeriphButton periph_button_repeat_click, eButton *button, eButtonState *button_state) {
    static uint16_t _s_timer_prev;

    static ePeriphButton _s_periph_button_repeat_click_prev;
    static eButton _s_button_pressed;
    static eButton _s_button_prev;
    static uint8_t _s_clock;
    static uint8_t _s_clicks;
    static uint8_t _s_repeat_clicks;
    static bool _s_error, _is_initiated;

    uint16_t _timer_current;
    bool _res = false;

    if(!_is_initiated) { _s_error = true; _is_initiated = true; }

    _timer_current = periph_get_timer();
    if((_timer_current - _s_timer_prev) >= TIMER_MS2TT(50)) {
        ePeriphButton _periph_buttons = periph_get_buttons();
        eButton _button = UINT8_MAX;
        bool _is_repeat = _s_periph_button_repeat_click_prev & _periph_buttons;
        _s_timer_prev = _timer_current;

        if(_s_periph_button_repeat_click_prev != periph_button_repeat_click) _s_error = true;

        if(!_s_error) {

            if(_periph_buttons == PeriphButton_Up) _button = Button_Up;
            else if(_periph_buttons == PeriphButton_OnOff) _button = Button_OnOff;
            else if(_periph_buttons == PeriphButton_Down) _button = Button_Down;
            else if(_periph_buttons != PeriphButton__None) _s_error = true;

            if(_s_button_pressed != UINT8_MAX && _button != UINT8_MAX && _s_button_pressed != _button) _s_error = true;

            if(!_s_error) {
                if(_s_repeat_clicks && _button == UINT8_MAX) { _s_repeat_clicks = 0; _s_button_pressed = UINT8_MAX; }
                if(_s_button_prev == UINT8_MAX && _button != UINT8_MAX) { _s_button_pressed = _button; _s_clicks++; _s_clock = 0; }

                // short tap
                if(_s_clicks && _s_clock == BUTTON_TICKS_CLICK) {
                    if(_is_repeat || _button == UINT8_MAX) { *button = _s_button_pressed; *button_state = ButtonState_Click + _s_clicks - 1; _res = true; }
                    if(_button == UINT8_MAX) { _s_button_pressed = UINT8_MAX; _s_clicks = 0; }
                }
                // long tap
                if(_s_clicks && _s_clock == BUTTON_TICKS_LONGCLICK && _s_button_pressed != UINT8_MAX) {
                    if(_is_repeat) { *button = _s_button_pressed; *button_state = ButtonState_Click + _s_clicks - 1; _s_repeat_clicks = _s_clicks; _s_clicks = 0; _res = true; }
                    else { *button = _s_button_pressed; *button_state = ButtonState_LongClick + _s_clicks - 1; _s_clicks = 0; _s_button_pressed = UINT8_MAX; _res = true; }
                }
                // repeat after long tap
                if(_is_repeat && _s_repeat_clicks && _s_clock >= (BUTTON_TICKS_LONGCLICK + BUTTON_TICKS_REPEAT) && _s_button_pressed != UINT8_MAX) {
                    *button = _s_button_pressed; *button_state = ButtonState_Click + _s_repeat_clicks - 1; _s_clicks = 0; _s_clock -= BUTTON_TICKS_REPEAT; _res = true;
                }

                if(_s_clock < UINT8_MAX) _s_clock++;
            }
        }

        _s_button_prev = _button;
        if(_periph_buttons == PeriphButton__None && _s_error) { _s_button_pressed = UINT8_MAX; s_buttons_is_repeating = false; _s_clock = 0; _s_clicks = 0; _s_repeat_clicks = 0; _s_periph_button_repeat_click_prev = periph_button_repeat_click; _s_error = false; }
        s_buttons_is_repeating = _is_repeat && !_s_error;
        //DBGF("%d = %d:%d : %d, %d.%d, %d:%d #%d\n", _s_error, _s_button_pressed, _button, _s_clock, _s_clicks, _s_repeat_clicks, _s_periph_button_repeat_click_prev, periph_button_repeat_click, s_buttons_is_repeating);
    }

    return _res;
}

bool buttons_is_repeating() { return s_buttons_is_repeating; }