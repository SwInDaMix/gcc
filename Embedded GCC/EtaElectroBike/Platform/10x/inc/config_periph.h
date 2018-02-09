/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config_periph.h
 ***********************************************************************/

#pragma once

#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define F_CPU 48000000UL
#define V_CPU 3300

#define USART_DBG USART1

#define TIM_3PHASE TIM1
#define TIM_HALL TIM3
#define TIM_DMA1 TIM6
#define TIM_DMA2 TIM7
#define TIM_RECUPERATOR_BOOSTER TIM17
#define DMA_Channel_Sin1 DMA1_Channel3
#define DMA_Channel_Sin2 DMA1_Channel4

#define USART_DBG_IRQn USART1_IRQn
#define USART_DBG_IRQHandler USART1_IRQHandler
#define TIM_3PHASE_BRK_UP_TRG_COM_IRQn TIM1_BRK_UP_TRG_COM_IRQn
#define TIM_3PHASE_BRK_UP_TRG_COM_IRQHandler TIM1_BRK_UP_TRG_COM_IRQHandler
#define TIM_HALL_IRQn TIM3_IRQn
#define TIM_HALL_IRQHandler TIM3_IRQHandler

#ifdef __cplusplus
}
#endif
