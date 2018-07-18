#include "drive.h"

static int16_t s_i_term;



void drv_pi_reset() { s_i_term = 0; }
uint8_t drv_pi_calc(uint16_t current, uint16_t target) {
    int16_t _res;
    int16_t _error;
    int16_t _p_term;
    _error = (int16_t)(target - current);
    _p_term = (int16_t)((_error * (uint8_t)P_FACTOR_DIVIDENT) >> (uint8_t)P_FACTOR_DIVISOR);
    s_i_term += (int16_t)((_error * (uint8_t)I_FACTOR_DIVIDENT) >> (uint8_t)I_FACTOR_DIVISOR);
    if (s_i_term > 255) s_i_term = 255;
    else if (s_i_term < 0) s_i_term = 0;
    _res = _p_term + s_i_term;
    if (_res > 255) _res = 255;
    else if (_res < 5) _res = 0;
    return (uint8_t)_res;
}
