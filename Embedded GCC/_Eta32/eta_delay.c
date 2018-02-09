/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_delay.c
 ***********************************************************************/

#include "stdperiph.h"
#include "eta_delay.h"

#ifdef TIM_DELAY
void _delay_ms (uint16_t delay) {
	TIM_DELAY->PSC = F_CPU / 1000 - 1;
	TIM_DELAY->ARR = delay - 1;
	TIM_DELAY->EGR = TIM_EGR_UG;
	TIM_DELAY->CR1 = TIM_CR1_OPM;
	TIM_DELAY->CR1 |= TIM_CR1_CEN;
	while ((TIM_DELAY->CR1 & TIM_CR1_CEN));
}
void _delay_us (uint16_t delay) {
	TIM_DELAY->PSC = F_CPU / 1000000 - 1;
	TIM_DELAY->ARR = delay - 1;
	TIM_DELAY->EGR = TIM_EGR_UG;
	TIM_DELAY->CR1 = TIM_CR1_OPM;
	TIM_DELAY->CR1 |= TIM_CR1_CEN;
	while ((TIM_DELAY->CR1 & TIM_CR1_CEN));
}
#endif
