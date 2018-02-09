/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CPeriph.h
***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

void Periph_Init();
void Periph_TIM_20ms();
void Periph_KBD_20ms();

#ifdef __cplusplus
}
#endif
