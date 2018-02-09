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
#define F_LSI 40000
#define V_CPU 3300
#define GLOBAL_TICKS_PER_SECOND 500

#define USART_DBG USART1
#define USART_DBG_TX_PORT GPIOB
#define USART_DBG_TX_PIN GPIO_PinSource6
#define USART_DBG_TX_AF GPIO_AF_0
#define USART_DBG_RX_PORT GPIOB
#define USART_DBG_RX_PIN GPIO_PinSource7
#define USART_DBG_RX_AF GPIO_AF_0
#define USART_DBG_PINCFG_B GPIO_Pin_6 | GPIO_Pin_7
#define USART_DBG_IRQn USART1_IRQn
#define USART_DBG_IRQHandler USART1_IRQHandler

#define LED_TIM1 TIM1
#define LED_TIM2 TIM3
#define LED_SEGMENT_A_PORT GPIOA
#define LED_SEGMENT_A_PIN GPIO_PinSource9
#define LED_SEGMENT_A_AF GPIO_AF_2
#define LED_SEGMENT_A_SetCompare(ccr) TIM_SetCompare2(LED_TIM1, ccr)
#define LED_SEGMENT_B_PORT GPIOA
#define LED_SEGMENT_B_PIN GPIO_PinSource8
#define LED_SEGMENT_B_AF GPIO_AF_2
#define LED_SEGMENT_B_SetCompare(ccr) TIM_SetCompare1(LED_TIM1, ccr)
#define LED_SEGMENT_C_PORT GPIOB
#define LED_SEGMENT_C_PIN GPIO_PinSource0
#define LED_SEGMENT_C_AF GPIO_AF_1
#define LED_SEGMENT_C_SetCompare(ccr) TIM_SetCompare3(LED_TIM2, ccr)
#define LED_SEGMENT_D_PORT GPIOA
#define LED_SEGMENT_D_PIN GPIO_PinSource6
#define LED_SEGMENT_D_AF GPIO_AF_1
#define LED_SEGMENT_D_SetCompare(ccr) TIM_SetCompare1(LED_TIM2, ccr)
#define LED_SEGMENT_E_PORT GPIOA
#define LED_SEGMENT_E_PIN GPIO_PinSource7
#define LED_SEGMENT_E_AF GPIO_AF_1
#define LED_SEGMENT_E_SetCompare(ccr) TIM_SetCompare2(LED_TIM2, ccr)
#define LED_SEGMENT_F_PORT GPIOA
#define LED_SEGMENT_F_PIN GPIO_PinSource11
#define LED_SEGMENT_F_AF GPIO_AF_2
#define LED_SEGMENT_F_SetCompare(ccr) TIM_SetCompare4(LED_TIM1, ccr)
#define LED_SEGMENT_G_PORT GPIOA
#define LED_SEGMENT_G_PIN GPIO_PinSource10
#define LED_SEGMENT_G_AF GPIO_AF_2
#define LED_SEGMENT_G_SetCompare(ccr) TIM_SetCompare3(LED_TIM1, ccr)
#define LED_SEGMENT_DP_PORT GPIOB
#define LED_SEGMENT_DP_PIN GPIO_PinSource1
#define LED_SEGMENT_DP_AF GPIO_AF_1
#define LED_SEGMENT_DP_SetCompare(ccr) TIM_SetCompare4(LED_TIM2, ccr)
#define LED_SEGMENT_PINCFG_A GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11
#define LED_SEGMENT_PINCFG_B GPIO_Pin_0 | GPIO_Pin_1
#define LED_DIG1_PORT GPIOF
#define LED_DIG1_PIN GPIO_PinSource0
#define LED_DIG2_PORT GPIOF
#define LED_DIG2_PIN GPIO_PinSource1
#define LED_DIG3_PORT GPIOA
#define LED_DIG3_PIN GPIO_PinSource3
#define LED_DIG4_PORT GPIOA
#define LED_DIG4_PIN GPIO_PinSource5
#define LED_DIGLEDS_PORT GPIOA
#define LED_DIGLEDS_PIN GPIO_PinSource2
#define LED_LED_WaterInlet_SetCompare(ccr) LED_SEGMENT_A_SetCompare(ccr)
#define LED_LED_CleanMembrane_SetCompare(ccr) LED_SEGMENT_B_SetCompare(ccr)
#define LED_LED_LowTank_SetCompare(ccr) LED_SEGMENT_DP_SetCompare(ccr)
#define LED_LED_NoWaterSource_SetCompare(ccr) LED_SEGMENT_C_SetCompare(ccr)
#define LED_DIG_PINCFG_A GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5
#define LED_DIG_PINCFG_F GPIO_Pin_0 | GPIO_Pin_1
#define LED_DIGITS 4
#define LED_DIGS (LED_DIGITS + 1)
#define LED_LEDS 4
#define LED_PRESCALER 2
#define LED_PERIOD (F_CPU / LED_PRESCALER / GLOBAL_TICKS_PER_SECOND)
#define LED_IRQn TIM1_BRK_UP_TRG_COM_IRQn
#define LED_IRQHandler TIM1_BRK_UP_TRG_COM_IRQHandler

#define BUZZER_TIM TIM14
#define BUZZER_PORT GPIOA
#define BUZZER_PIN GPIO_PinSource4
#define BUZZER_AF GPIO_AF_4
#define BUZZER_PINCFG_A GPIO_Pin_4

#define VALVE_WATER_INLET_PORT GPIOA
#define VALVE_WATER_INLET_PIN GPIO_PinSource12
#define VALVE_CLEAN_MEMBRANE_PORT GPIOA
#define VALVE_CLEAN_MEMBRANE_PIN GPIO_PinSource15
#define VALVE_PINCFG_A GPIO_Pin_12 | GPIO_Pin_15

#define SENS_LOWINLET_PRESSURE_PORT GPIOA
#define SENS_LOWINLET_PRESSURE_PIN GPIO_PinSource0
#define SENS_HIGHTANK_PRESSURE_PORT GPIOA
#define SENS_HIGHTANK_PRESSURE_PIN GPIO_PinSource1
#define SENS_PINCFG_A GPIO_Pin_0 | GPIO_Pin_1

#define BTN_PREV_PORT GPIOB
#define BTN_PREV_PORTSOURCE EXTI_PortSourceGPIOB
#define BTN_PREV_PIN GPIO_PinSource5
#define BTN_NEXT_PORT GPIOB
#define BTN_NEXT_PORTSOURCE EXTI_PortSourceGPIOB
#define BTN_NEXT_PIN GPIO_PinSource3
#define BTN_ENTER_PORT GPIOB
#define BTN_ENTER_PORTSOURCE EXTI_PortSourceGPIOB
#define BTN_ENTER_PIN GPIO_PinSource4
#define BTN_PINCFG_B GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5

#ifdef __cplusplus
}
#endif
