/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_crc.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include <avr/io.h>

uint8_t crc8_update(uint8_t crc8, uint8_t data);
uint8_t crc8(uint8_t crc8, void const *src, size_t size);

uint16_t crc16_update(uint16_t crc16, uint8_t data);
uint16_t crc16(uint16_t crc16, void const *src, size_t size);
