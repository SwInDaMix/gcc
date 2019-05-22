#pragma once

#include <stdint.h>

#define swap16(value) ((uint16_t)(((value) & 0xFF) << 8) | (uint16_t)(((value) >> 8) & 0xFF))

uint8_t crc8_ccitt_update(uint8_t crc, uint8_t byte);

uint8_t divu16(uint16_t *numer, uint8_t denom);
uint8_t divu32(uint32_t *numer, uint8_t denom);

uint8_t map8(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max);
uint16_t map16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);
