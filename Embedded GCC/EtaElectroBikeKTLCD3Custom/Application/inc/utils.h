#pragma once

#include <stdint.h>
#include "config.h"
#include "network.h"

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

uint16_t round16(uint16_t value, uint16_t unit);
uint32_t round32(uint32_t value, uint16_t unit);

uint16_t dec16(uint16_t value, uint16_t unit, uint8_t weight);
uint32_t dec32(uint32_t value, uint16_t unit, uint8_t weight);

uint16_t inc16(uint16_t value, uint16_t unit, uint8_t weight, uint16_t max);
uint32_t inc32(uint32_t value, uint16_t unit, uint8_t weight, uint32_t max);

int16_t conv_tempC_to_F(int16_t temp_c);
uint32_t conv_distanceKm_to_Mil(uint32_t distance_km);
uint32_t conv_distanceMil_to_Km(uint32_t distance_mil);

uint16_t conv_10mh_to_erps(uint32_t mh, sNetworkMotorSettings const *motor_settings);
uint32_t conv_erps_to_10mh(uint16_t erps, sNetworkMotorSettings const *motor_settings);
uint32_t conv_erps_to_10m(uint32_t erps, sNetworkMotorSettings const *motor_settings);
