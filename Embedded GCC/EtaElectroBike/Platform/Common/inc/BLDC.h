/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : BLDC.h
***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t BLDC_HallPositionLast;
extern uint8_t BLDC_HallPrescaler;
extern uint16_t BLDC_HallPeriod;
extern uint16_t BLDC_PWMPeriod;

void BLDC_HallFault();
void BLDC_UpdateHallPeriod(uint16_t hall_period);
void BLDC_RebuildSinusTable(uint16_t pwm_period);
void BLDC_ProcessStep(uint8_t step);

#ifdef __cplusplus
}
#endif
