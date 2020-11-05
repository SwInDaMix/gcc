/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_inputs.h
 ***********************************************************************/

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

typedef enum {
    EncoderState_Idle,
    EncoderState_1,
    EncoderState_2,
    EncoderState_3
} eInput_EncoderState;

typedef enum {
    EncoderAction_None,
    EncoderAction_Forward,
    EncoderAction_Backward
} eInput_EncoderAction;

typedef struct {
    uint8_t error:1;
    uint32_t prev;
    uint16_t stable_count;
} sInputInstance_Buttons;

typedef struct {
    uint8_t error:1;
    uint8_t dir:1;
    uint8_t prev_a:1;
    uint8_t prev_b:1;
    eInput_EncoderState state;
    uint16_t stable_count;
} sInputInstance_Encoder;

uint32_t Input_ButtonsProcess(uint32_t buttons, uint8_t buttons_count, sInputInstance_Buttons *input_instance_buttons);
eInput_EncoderAction Input_EncoderProcess(bool a, bool b, sInputInstance_Encoder *input_instance_encoder);
