/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_inputs.c
 ***********************************************************************/

#include "eta_inputs.h"

uint32_t Input_ButtonsProcess(uint32_t buttons, uint8_t buttons_count, sInputInstance_Buttons *input_instance_buttons) {
    uint8_t _buttons_pressed = 0;
    uint32_t _buttons_iterate = buttons;
    while (buttons_count--) {
        if (_buttons_iterate & 1) _buttons_pressed++;
        _buttons_iterate >>= 1;
    }

    if (!_buttons_pressed) input_instance_buttons->error = false;
    else if (_buttons_pressed != 1) input_instance_buttons->error = true;

    if (input_instance_buttons->prev != buttons || input_instance_buttons->error) input_instance_buttons->stable_count = 0;
    else if (input_instance_buttons->stable_count <= INPUT_BUTTON_STABLE_TICKS) input_instance_buttons->stable_count++;
    input_instance_buttons->prev = buttons;

    return (input_instance_buttons->stable_count == INPUT_BUTTON_STABLE_TICKS && !input_instance_buttons->error) ? buttons : 0;
}

eInput_EncoderAction Input_EncoderProcess(bool a, bool b, sInputInstance_Encoder *input_instance_encoder) {
    eInput_EncoderAction _encoder_action = EncoderAction_None;

    if (input_instance_encoder->prev_a != a || input_instance_encoder->prev_b != b) {
        input_instance_encoder->prev_a = a; input_instance_encoder->prev_b = b;
        input_instance_encoder->stable_count = 0;
    } else if (input_instance_encoder->stable_count <= INPUT_ENCODER_STABLE_TICKS) { input_instance_encoder->stable_count++; }

    if (!input_instance_encoder->error) {
        if (input_instance_encoder->stable_count == INPUT_ENCODER_STABLE_TICKS) {
            if (input_instance_encoder->state == EncoderState_Idle) {
                if (a && !b) { input_instance_encoder->dir = false; input_instance_encoder->state = EncoderState_1; }
                else if (!a && b) { input_instance_encoder->dir = true; input_instance_encoder->state = EncoderState_1; }
                else input_instance_encoder->error = true;
            } else if (input_instance_encoder->state == EncoderState_1) {
                if (a && b) { input_instance_encoder->state = EncoderState_2; }
                else input_instance_encoder->error = true;
            } else if (input_instance_encoder->state == EncoderState_2) {
                if (a == input_instance_encoder->dir && b != input_instance_encoder->dir) { input_instance_encoder->state = EncoderState_3; }
                else input_instance_encoder->error = true;
            } else if (input_instance_encoder->state == EncoderState_3) {
                if (!a && !b) {
                    if (input_instance_encoder->dir) { _encoder_action = EncoderAction_Backward; }
                    else { _encoder_action = EncoderAction_Forward; }
                    input_instance_encoder->state = EncoderState_Idle;
                } else input_instance_encoder->error = true;
            }
        }
    } else if (!a && !b) { input_instance_encoder->error = false; input_instance_encoder->state = EncoderState_Idle; }

    return _encoder_action;
}
