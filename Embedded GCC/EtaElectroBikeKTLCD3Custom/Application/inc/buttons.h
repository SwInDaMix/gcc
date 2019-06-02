#pragma once

#include <stdint.h>
#include "config.h"
#include "periph.h"

typedef enum {
    Button_Up,
    Button_OnOff,
    Button_Down
} eButton;

typedef enum {
    ButtonState_Click,
    ButtonState_DoubleClick,
    ButtonState_TrippleClick,
    ButtonState_QuadClick,
    ButtonState_LongClick = 0x80,
    ButtonState_LongDoubleClick,
    ButtonState_LongTrippleClick,
    ButtonState_LongQuadClick
} eButtonState;

#define buttons_is_clicks(state, clicks) (state == (ButtonState_Click + ((num) - 1)))

bool buttons_process(ePeriphButton periph_button_repeat_click, eButton *button, eButtonState *button_state);
bool buttons_is_repeating();
