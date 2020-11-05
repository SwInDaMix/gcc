/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : sensors.h
 ***********************************************************************/

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PeriphButton_None = 0,
    PeriphButton_Escape = (1 << 0),
    PeriphButton_Enter = (1 << 1),
    PeriphButton__Count = 2,
} ePeriphButton;

typedef enum {
    PeriphInput_None = 0,
    PeriphInput_Escape = (1 << 0),
    PeriphInput_Enter = (1 << 1),
    PeriphInput_Forward = (1 << 2),
    PeriphInput_Backward = (1 << 3)
} ePeriphInput;

void Periph_Periodic();
void Periph_Init();
ePeriphInput Periph_GetInput();
uint32_t Periph_GetGlobalTicks();
uint32_t Periph_GetGlobalMs10Ticks();

uint32_t Periph_Random();

void Periph_LCDBacklight_SetParams(uint8_t backlight_wakeup, uint8_t backlight_background, uint8_t backlight_standby, uint8_t timeout);
void Periph_LCDBacklight_SetBackgroundMode(bool is_background);