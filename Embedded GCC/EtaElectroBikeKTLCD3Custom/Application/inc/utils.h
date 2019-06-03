#pragma once

#include <stdint.h>
#include "config.h"

#define    MIN(a,b)    (((a)<(b))?(a):(b))
#define    MAX(a,b)    (((a)>(b))?(a):(b))

#define FLAG_IS_SET(flags, bit) (flags & (bit))
#define FLAG_SET(flags, bit) flags |= (bit)
#define FLAG_RESET(flags, bit) flags &= ~(bit)
#define FLAG_TOGGLE(flags, bit) flags ^= (bit)
#define FLAG_PUT(flags, bit, value) if(value) { flags |= (bit); } else { flags &= ~(bit); }

#define swap16(value) ((uint16_t)(((value) & 0xFF) << 8) | (uint16_t)(((value) >> 8) & 0xFF))

uint8_t crc8_ccitt_update(uint8_t crc, uint8_t byte);

uint8_t divu16(uint16_t *numer, uint8_t denom);
uint8_t divu32(uint32_t *numer, uint8_t denom);

uint8_t map8(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max);
uint16_t map16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

int16_t conv_tempC2F(int16_t temp_c);
uint32_t conv_distanceKm2Mil(uint32_t distance_km);
uint32_t conv_distanceMil2Km(uint32_t distance_mil);

uint16_t conv_mh2erps(uint32_t mh, uint16_t wheel_circumference, uint8_t motor_pole_pairs);
uint32_t conv_erps2mh(uint16_t erps, uint16_t wheel_circumference, uint8_t motor_pole_pairs);
