/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_crc.c
 ***********************************************************************/

#include "util/crc16.h"
#include "eta_crc.h"

uint8_t crc8_update(uint8_t crc8, uint8_t data) { return _crc8_ccitt_update(crc8, data); }
uint8_t crc8(uint8_t crc8, void const *src, size_t size) {
	uint8_t const *_src = (const uint8_t*)src;
	while(size--) crc8 = crc8_update(crc8, *_src++);
	return crc8;
}

uint16_t crc16_update(uint16_t crc16, uint8_t data) { return _crc_ccitt_update(crc16, data); }
uint16_t crc16(uint16_t crc16, void const *src, size_t size) {
	uint8_t const *_src = (const uint8_t*)src;
	while(size--) crc16 = crc16_update(crc16, *_src++);
	return crc16;
}
