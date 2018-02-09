/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_spi.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include <avr/io.h>

#ifdef SPI_TRANSMITTER

typedef enum {
	SPICfg_CPOL0_CPHA0 = 0,
	SPICfg_CPOL1_CPHA0 = (1 << CPOL),
	SPICfg_CPOL0_CPHA1 = (1 << CPHA),
	SPICfg_CPOL1_CPHA1 = (1 << CPOL) | (1 << CPHA),
} eSPI_PolarityCfg;

typedef enum {
	SPIPrescaler_4 = 0,
	SPIPrescaler_2 = SPIPrescaler_4 + 4,
	SPIPrescaler_16 = 1,
	SPIPrescaler_8 = SPIPrescaler_16 + 4,
	SPIPrescaler_64 = 2,
	SPIPrescaler_32 = SPIPrescaler_64 + 4,
	SPIPrescaler_128 = 3,
} eSPI_Prescaler;

void SPI_Init(bool is_master, bool is_lsb, eSPI_PolarityCfg polarity_cfg, eSPI_Prescaler prescaler);
uint8_t SPI_ReadWrite(uint8_t data);

#endif