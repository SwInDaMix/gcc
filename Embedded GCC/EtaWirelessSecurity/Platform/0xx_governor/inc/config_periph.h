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
#define TIM_PERIODIC TIM15
#define TIM_PERIODIC_IRQn TIM15_IRQn
#define TIM_PERIODIC_IRQHandler TIM15_IRQHandler

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
#define USART_DBG_portparamlast 0
#define USART_DBG_portparam USART_DBG_portparamlast,
#define FRAMES_8B
#define FRAMES0_PERIPH USART_DBG
#define FRAMES0_IRQ USART_DBG_IRQn
#define FRAMES0_RX_BUFFER_SIZESHIFT 8
#define FRAMES0_TX_BUFFER_SIZESHIFT 10

#define USART_BCELL USART2
#define USART_BCELL_TX_PORT GPIOA
#define USART_BCELL_TX_PIN GPIO_PinSource2
#define USART_BCELL_TX_AF GPIO_AF_1
#define USART_BCELL_RX_PORT GPIOA
#define USART_BCELL_RX_PIN GPIO_PinSource3
#define USART_BCELL_RX_AF GPIO_AF_1
#define USART_BCELL_PINCFG_A GPIO_Pin_2 | GPIO_Pin_3
#define USART_BCELL_IRQn USART2_IRQn
#define USART_BCELL_IRQHandler USART2_IRQHandler
#define USART_BCELL_portparamlast 1
#define USART_BCELL_portparam USART_BCELL_portparamlast,
#define FRAMES_9B
#define FRAMES1_PERIPH USART_BCELL
#define FRAMES1_IRQ USART_BCELL_IRQn
#define FRAMES1_RX_BUFFER_SIZESHIFT 8
#define FRAMES1_TX_BUFFER_SIZESHIFT 5

#define ENCODER_T1_PORT GPIOB
#define ENCODER_T1_PIN GPIO_PinSource0
#define ENCODER_T2_PORT GPIOB
#define ENCODER_T2_PIN GPIO_PinSource1
#define ENCODER_BUTTON_PORT GPIOA
#define ENCODER_BUTTON_PIN GPIO_PinSource12

#define SPI_RF_SERVER SPI1
#define SPI_RF_SERVER_IRQ_PORT GPIOA
#define SPI_RF_SERVER_IRQ_PIN GPIO_PinSource0
#define SPI_RF_SERVER_CE_PORT GPIOA
#define SPI_RF_SERVER_CE_PIN GPIO_PinSource1
#define SPI_RF_SERVER_NSS_PORT GPIOA
#define SPI_RF_SERVER_NSS_PIN GPIO_PinSource4
#define SPI_RF_SERVER_SCK_PORT GPIOA
#define SPI_RF_SERVER_SCK_PIN GPIO_PinSource5
#define SPI_RF_SERVER_SCK_AF GPIO_AF_0
#define SPI_RF_SERVER_MISO_PORT GPIOA
#define SPI_RF_SERVER_MISO_PIN GPIO_PinSource6
#define SPI_RF_SERVER_MISO_AF GPIO_AF_0
#define SPI_RF_SERVER_MOSI_PORT GPIOA
#define SPI_RF_SERVER_MOSI_PIN GPIO_PinSource7
#define SPI_RF_SERVER_MOSI_AF GPIO_AF_0
#define SPI_RF_PINCFG_A GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7

#define GPIO_OUTPUT_PP_PIN_A GPIO_Pin_1 | GPIO_Pin_4
#define GPIO_INPUT_UP_PIN_A GPIO_Pin_0 | GPIO_Pin_12
#define GPIO_INPUT_UP_PIN_B GPIO_Pin_0 | GPIO_Pin_1

#ifdef __cplusplus
}
#endif
