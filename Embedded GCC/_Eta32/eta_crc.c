/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_crc.c
 ***********************************************************************/

#include "stdperiph.h"
#include "eta_crc.h"

uint8_t crc8_update(uint8_t crc8, uint8_t data) {
    crc8 ^= data;
    for(uint8_t _i = 0; _i < 8; _i++) {
        if((crc8 & 0x80)) { crc8 = (crc8 << 1) ^ 0x07; }
        else { crc8 <<= 1; }
    }
    return crc8;
}
uint8_t crc8(uint8_t crc8, void const *src, size_t size) {
	uint8_t const *_src = (const uint8_t*)src;
	while(size--) crc8 = crc8_update(crc8, *_src++);
	return crc8;
}

uint16_t crc16_update(uint16_t crc16, uint8_t data) {
    data ^= (uint8_t)(crc16 & 0xFF);
    data ^= data << 4;
    return ((((uint16_t)data << 8) | (uint8_t)((crc16 >> 8) & 0xFF)) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
}
uint16_t crc16(uint16_t crc16, void const *src, size_t size) {
	uint8_t const *_src = (const uint8_t*)src;
	while(size--) crc16 = crc16_update(crc16, *_src++);
	return crc16;
}

uint32_t crc32_update(uint32_t crc32, uint8_t data) {
	crc32 ^= data;
    for(uint8_t _i = 0; _i < 8; _i++) {
        if((crc32 & 0x01)) { crc32 = (crc32 >> 1) ^ 0xEDB88320; }
        else { crc32 >>= 1; }
    }
	return crc32;
}
uint32_t crc32(uint32_t crc32, void const *src, size_t size) {
	uint8_t const *_src = (const uint8_t*)src;
	while(size--) crc32 = crc32_update(crc32, *_src++);
	return crc32;
}

uint32_t crc32_fast(uint32_t crc32, void const *src, size_t size) {
    CRC_SetInitRegister(crc32);
    CRC_ResetDR();
    size_t _sz4 = size >> 2;
    if (_sz4 && !(((uint32_t)src) & 3)) {
        size -= _sz4 << 2;
        uint32_t const *_ptr = src;
        while (_sz4--) CRC_CalcCRC(*_ptr++);
        src = _ptr;
    }
    size_t _sz2 = size >> 1;
    if (_sz2 && !(((uint32_t)src) & 1)) {
        size -= _sz2 << 1;
        uint16_t const *_ptr = src;
        while (_sz2--) CRC_CalcCRC16bits(*_ptr++);
        src = _ptr;
    }
    if (size) {
        uint8_t const *_ptr = src;
        while (size--) CRC_CalcCRC8bits(*_ptr++);
    }
    return CRC_GetCRC();
}
