#include "utils.h"
#include "periph.h"

void delay_ms(uint16_t ms) {
    uint16_t _st = periph_get_timer();
    while((periph_get_timer() - _st) < TIMER_MS2TT(ms)) periph_wdt_reset();
}

uint8_t crc8_ccitt_update(uint8_t crc, uint8_t byte) {
    uint8_t i;
    uint8_t data;

    data = crc ^ byte;
    for (i = 0; i < 8; i++) {
        if((data & 0x80)) {
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

    if(x < in_min) return out_min;
    if(x > in_max) return out_max;

    _diff_in = in_max - in_min;
    _diff_out = out_max - out_min;

    return (uint8_t)((uint8_t)(((uint16_t)((uint8_t)(x - in_min) * _diff_out) + (_diff_in >> 1)) / _diff_in) + out_min);
}
uint16_t map16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    uint16_t _diff_in, _diff_out;

    if(x < in_min) return out_min;
    if(x > in_max) return out_max;

    _diff_in = in_max - in_min;
    _diff_out = out_max - out_min;

    return (uint16_t)((uint16_t)(((uint32_t)((uint16_t)(x - in_min) * _diff_out) + (_diff_in >> 1)) / _diff_in) + out_min);
}

uint16_t round16(uint16_t value, uint16_t unit) { return (value + (unit >> 1)) / unit * unit; }
uint32_t round32(uint32_t value, uint16_t unit) { return (value + (unit >> 1)) / unit * unit; }

static uint16_t pow(uint16_t unit, uint8_t weight) {
    if(weight == 0) return unit;
    if(weight == 1) return unit * 10;
    if(weight == 2) return unit * 100;
    if(weight == 3) return unit * 1000;
    return unit * 10000;
}
uint16_t dec16(uint16_t value, uint16_t unit, uint8_t weight, uint16_t min) {
    uint16_t _unit = pow(unit, weight);
    uint16_t _value = round16(value, _unit);

    if(_value >= _unit) _value -= _unit; else _value = 0;
    if(_value < min) _value = min;
    return _value;
}
uint32_t dec32(uint32_t value, uint16_t unit, uint8_t weight, uint16_t min) {
    uint16_t _unit = pow(unit, weight);
    uint32_t _value = round32(value, _unit);

    if(_value >= _unit) _value -= _unit; else _value = 0;
    if(_value < min) _value = min;
    return _value;
}

uint16_t inc16(uint16_t value, uint16_t unit, uint8_t weight, uint16_t max) {
    uint16_t _unit = pow(unit, weight);
    uint16_t _value = round16(value, _unit);
    _value += _unit;
    if(_value > max) return max;
    return _value;
}
uint32_t inc32(uint32_t value, uint16_t unit, uint8_t weight, uint32_t max) {
    uint16_t _unit = pow(unit, weight);
    uint32_t _value = round32(value, _unit);
    _value += _unit;
    if(_value > max) return max;
    return _value;
}

int16_t conv_tempC_to_F(int16_t temp_c) { return (temp_c * 9U / 5U) + 32U; }
uint32_t conv_distanceKm_to_Mil(uint32_t distance_km) { return (distance_km * 636U) >> 10; }
uint32_t conv_distanceMil_to_Km(uint32_t distance_mil) { return ((distance_mil << 10) + 318) / 636U; }

uint16_t conv_10mh_to_erps(uint32_t mh, sNetworkMotorSettings const *motor_settings) { return (uint16_t)((((uint64_t)mh * 100U * motor_settings->pole_pairs) / 36U + (motor_settings->wheel_circumference >> 1)) / motor_settings->wheel_circumference);  }
uint32_t conv_erps_to_10mh(uint16_t erps, sNetworkMotorSettings const *motor_settings) { return (uint32_t)(((((uint64_t)erps) * motor_settings->wheel_circumference * 36U) / motor_settings->pole_pairs + 50U) / 100U); }
uint32_t conv_erps_to_10m(uint32_t erps, sNetworkMotorSettings const *motor_settings) { return (uint32_t)(((((uint64_t)erps) * motor_settings->wheel_circumference) / motor_settings->pole_pairs + 5000U) / 10000U); }
