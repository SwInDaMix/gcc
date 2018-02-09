/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_spi.c
 ***********************************************************************/

#include "eta_spi.h"

#ifdef SPI_TRANSMITTER

void SPI_Init(bool is_master, bool is_lsb, eSPI_PolarityCfg polarity_cfg, eSPI_Prescaler prescaler) {
	SPSR = (prescaler >> 2);
	SPCR = (1 << SPE) | (is_master ? (1 << MSTR) : 0) | (is_lsb ? (1 << DORD) : 0) | polarity_cfg | (prescaler & 0x3);
}

static void _spi_wait() { while(!(SPSR & (1 << SPIF))) SPI_YIELD(); }

uint8_t SPI_ReadWrite(uint8_t data) { SPDR = data; _spi_wait(); return SPDR; }

#endif
