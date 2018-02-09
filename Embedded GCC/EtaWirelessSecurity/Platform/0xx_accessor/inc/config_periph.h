/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : config_periph.h
 ***********************************************************************/

#pragma once

#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define F_CPU 8000000UL
#define F_LSI 40000
#define V_CPU 3300
#define PERIODIC_FREQ 1000

#define TIM_DELAY TIM14
#define TIM_PERIODIC TIM16
#define TIM_PERIODIC_IRQn TIM16_IRQn
#define TIM_PERIODIC_IRQHandler TIM16_IRQHandler

#define USART_DBG USART1
#define USART_DBG_TX_PORT GPIOA
#define USART_DBG_TX_PIN GPIO_PinSource9
#define USART_DBG_TX_AF GPIO_AF_1
#define USART_DBG_RX_PORT GPIOA
#define USART_DBG_RX_PIN GPIO_PinSource10
#define USART_DBG_RX_AF GPIO_AF_1
#define USART_DBG_PINCFG_A GPIO_Pin_9 | GPIO_Pin_10
#define USART_DBG_IRQn USART1_IRQn
#define USART_DBG_IRQHandler USART1_IRQHandler
#define FRAMES_8B
#define FRAMES0_PERIPH USART_DBG
#define FRAMES0_IRQ USART_DBG_IRQn
#define FRAMES0_RX_BUFFER_SIZESHIFT 8
#define FRAMES0_TX_BUFFER_SIZESHIFT 9

#define SPI_RF_CLIENT SPI1
#define SPI_RF_CLIENT_IRQ_PORT GPIOB
#define SPI_RF_CLIENT_IRQ_PIN GPIO_PinSource0
#define SPI_RF_CLIENT_CE_PORT GPIOB
#define SPI_RF_CLIENT_CE_PIN GPIO_PinSource1
#define SPI_RF_CLIENT_NSS_PORT GPIOA
#define SPI_RF_CLIENT_NSS_PIN GPIO_PinSource4
#define SPI_RF_CLIENT_SCK_PORT GPIOA
#define SPI_RF_CLIENT_SCK_PIN GPIO_PinSource5
#define SPI_RF_CLIENT_SCK_AF GPIO_AF_0
#define SPI_RF_CLIENT_MISO_PORT GPIOA
#define SPI_RF_CLIENT_MISO_PIN GPIO_PinSource6
#define SPI_RF_CLIENT_MISO_AF GPIO_AF_0
#define SPI_RF_CLIENT_MOSI_PORT GPIOA
#define SPI_RF_CLIENT_MOSI_PIN GPIO_PinSource7
#define SPI_RF_CLIENT_MOSI_AF GPIO_AF_0
#define SPI_RF_PINCFG_A GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7

#define BUTTON_A_PORT GPIOB
#define BUTTON_A_PIN GPIO_PinSource6
#define BUTTON_B_PORT GPIOB
#define BUTTON_B_PIN GPIO_PinSource5
#define BUTTON_C_PORT GPIOB
#define BUTTON_C_PIN GPIO_PinSource4
#define BUTTON_D_PORT GPIOB
#define BUTTON_D_PIN GPIO_PinSource3

#define POWERLOCK_PORT GPIOA
#define POWERLOCK_PIN GPIO_PinSource3

#define LED_RED_PORT GPIOA
#define LED_RED_PIN GPIO_PinSource1
#define LED_GREEN_PORT GPIOA
#define LED_GREEN_PIN GPIO_PinSource2

#define ADC_VOLTAGE_PORT GPIOA
#define ADC_VOLTAGE_PIN GPIO_PinSource0
#define ADC_VOLTAGE_CHANNEL (1 << GPIO_PinSource0)

#define GPIO_OUTPUT_PP_PIN_A GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4
#define GPIO_OUTPUT_PP_PIN_B GPIO_Pin_1
#define GPIO_INPUT_UP_PIN_B GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6

#ifdef __cplusplus
}
#endif
