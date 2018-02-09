/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : interrupt.h
***********************************************************************/

#pragma once

#include "config.h"
#include "stm32f10x.h"

void HardFaultException() __attribute__((noreturn));
void USB_HP_CAN_TX_IRQHandler();
void USB_LP_CAN_RX0_IRQHandler();
void TIM3_IRQHandler();
void USART1_IRQHandler();
