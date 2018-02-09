/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_crc.h
 ***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t crc8_update(uint8_t crc8, uint8_t data);
uint8_t crc8(uint8_t crc8, void const *src, size_t size);

uint16_t crc16_update(uint16_t crc16, uint8_t data);
uint16_t crc16(uint16_t crc16, void const *src, size_t size);

uint32_t crc32_update(uint32_t crc32, uint8_t data);
uint32_t crc32(uint32_t crc32, void const *src, size_t size);

#ifdef __cplusplus
}
#endif
