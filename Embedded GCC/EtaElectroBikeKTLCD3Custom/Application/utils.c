#include "utils.h"

uint8_t crc8_ccitt_update(uint8_t crc, uint8_t byte) {
    uint8_t i;
    uint8_t data;

    data = crc ^ byte;
    for (i = 0; i < 8; i++) {
        if ((data & 0x80) != 0) {
            data <<= 1;
            data ^= 0x07;
        } else data <<= 1;
    }
    return data;
}

uint8_t divu16(uint16_t *numer, uint8_t denom) {
    uint8_t _rem = *numer % denom;
    *numer = *numer / denom;
    return _rem;
}

uint8_t divu32(uint32_t *numer, uint8_t denom) {
    uint8_t _rem = *numer % denom;
    *numer = *numer / denom;
    return _rem;
}

uint8_t map8(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max) {
    uint8_t _diff_in, _diff_out;

    if (x < in_min) return out_min;
    if (x > in_max) return out_max;

    _diff_in = in_max - in_min;
    _diff_out = out_max - out_min;

    //if (_diff_in > _diff_out) return (uint8_t)((uint8_t)((uint16_t)((uint8_t)(x - in_min) * (uint8_t)(_diff_out + 1U)) / (uint8_t)(_diff_in + 1U)) + (uint8_t)out_min);
    return (uint8_t)((uint8_t)((uint16_t)((uint8_t)(x - in_min) * _diff_out) / _diff_in) + out_min);
}
uint16_t map16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    uint16_t _diff_in, _diff_out;

    if (x < in_min) return out_min;
    if (x > in_max) return out_max;

    _diff_in = in_max - in_min;
    _diff_out = out_max - out_min;

    //if (_diff_in > _diff_out) return (uint16_t)((uint16_t)((uint32_t)((uint16_t)(x - in_min) * (uint16_t)(_diff_out + 1U)) / (uint16_t)(_diff_in + 1U)) + (uint16_t)out_min);
    return (uint16_t)((uint16_t)((uint32_t)((uint16_t)(x - in_min) * _diff_out) / _diff_in) + out_min);
}
