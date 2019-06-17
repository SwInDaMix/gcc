#include "sensors.h"
#include "ktlcd3.h"
#include "utils.h"

#define HUD_TEMP_LOOKUP1_START 200
#define HUD_TEMP_LOOKUP2_START 986
#define HUD_TEMP_LOOKUP_END 1024
static int8_t const s_temp_for_ktlcd3_lookup1[(HUD_TEMP_LOOKUP2_START - HUD_TEMP_LOOKUP1_START) / 2] = {
        127, 126, 126, 125, 124, 124, 123, 123, 122, 121, 121, 120, 119, 119, 118, 118,
        117, 116, 116, 115, 115, 114, 114, 113, 112, 112, 111, 111, 110, 110, 109, 109,
        108, 108, 107, 107, 106, 106, 105, 105, 104, 104, 103, 103, 102, 102, 101, 101,
        100, 100, 99, 99, 98, 98, 98, 97, 97, 96, 96, 95, 95, 94, 94, 94,
        93, 93, 92, 92, 92, 91, 91, 90, 90, 90, 89, 89, 88, 88, 88, 87,
        87, 86, 86, 86, 85, 85, 84, 84, 84, 83, 83, 83, 82, 82, 81, 81,
        81, 80, 80, 80, 79, 79, 79, 78, 78, 78, 77, 77, 76, 76, 76, 75,
        75, 75, 74, 74, 74, 73, 73, 73, 72, 72, 72, 71, 71, 71, 70, 70,
        70, 69, 69, 69, 68, 68, 68, 67, 67, 67, 66, 66, 66, 66, 65, 65,
        65, 64, 64, 64, 63, 63, 63, 62, 62, 62, 61, 61, 61, 61, 60, 60,
        60, 59, 59, 59, 58, 58, 58, 57, 57, 57, 57, 56, 56, 56, 55, 55,
        55, 54, 54, 54, 54, 53, 53, 53, 52, 52, 52, 52, 51, 51, 51, 50,
        50, 50, 49, 49, 49, 49, 48, 48, 48, 47, 47, 47, 47, 46, 46, 46,
        45, 45, 45, 44, 44, 44, 44, 43, 43, 43, 42, 42, 42, 42, 41, 41,
        41, 40, 40, 40, 39, 39, 39, 39, 38, 38, 38, 37, 37, 37, 36, 36,
        36, 36, 35, 35, 35, 34, 34, 34, 34, 33, 33, 33, 32, 32, 32, 31,
        31, 31, 30, 30, 30, 30, 29, 29, 29, 28, 28, 28, 27, 27, 27, 26,
        26, 26, 26, 25, 25, 25, 24, 24, 24, 23, 23, 23, 22, 22, 22, 21,
        21, 21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16,
        16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 10, 10,
        10, 9, 9, 9, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 4, 4,
        3, 3, 2, 2, 2, 1, 1, 0, 0, -1, -1, -2, -2, -3, -3, -4,
        -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, -10, -10, -11, -12, -12,
        -13, -13, -14, -15, -15, -16, -17, -17, -18, -19, -20, -20, -21, -22, -23, -24,
        -24, -25, -26, -27, -28, -29, -30, -31, -32
};
static int8_t const s_temp_for_ktlcd3_lookup2[HUD_TEMP_LOOKUP_END - HUD_TEMP_LOOKUP2_START] = {
        -33, -34, -35, -35, -36, -37, -37, -38, -39, -39, -40, -41, -42, -42, -43, -44,
        -45, -46, -47, -48, -49, -50, -51, -53, -54, -55, -57, -59, -60, -62, -65, -67,
        -70, -73, -78, -83, -92, -128
};

static sSensors s_sensors;

void sensors_init() { }

sSensors const *sensors_get_current() {
    uint16_t _motor_current_calibration = periph_get_adc_motor_current_calibration();
    uint16_t _motor_current = periph_get_adc_motor_current();
    uint16_t _motor_current_filtered = periph_get_adc_motor_current_filtered();
    uint16_t _sens_temp_adc = periph_get_adc_motor_temperature();

    s_sensors.battery_volts = (periph_get_adc_battery_voltage() >> 1) * (uint8_t)HUD_BATTERY_VOLTAGE_K;
    s_sensors.motor_current = _motor_current > _motor_current_calibration ? _motor_current - _motor_current_calibration : (uint16_t)0;
    s_sensors.motor_current_filtered = _motor_current_filtered > _motor_current_calibration ? _motor_current_filtered - _motor_current_calibration : (uint16_t)0;

    // calc motor temperature
    // not used due to low performance of calculation
    //_sens_temp_k = (uint8_t)roundf((1.0f / ((logf(HUD_TEMP_OPP_RESISTANCE / ((1023.0f / (float)periph_get_adc_motor_temperature()) - 1.0f) / HUD_TEMP_NOMINAL_RESISTANCE) / HUD_TEMP_BCOEFFICIENT) + (1.0f / HUD_TEMP_NOMINAL_TEMP))) - 288.15f);
    // fast method for ktlcd3
    if(_sens_temp_adc < HUD_TEMP_LOOKUP1_START) s_sensors.motor_temperature = s_temp_for_ktlcd3_lookup1[0];
    else if(_sens_temp_adc < HUD_TEMP_LOOKUP2_START) s_sensors.motor_temperature = s_temp_for_ktlcd3_lookup1[(_sens_temp_adc - HUD_TEMP_LOOKUP1_START) >> 1];
    else s_sensors.motor_temperature = s_temp_for_ktlcd3_lookup2[_sens_temp_adc - HUD_TEMP_LOOKUP2_START];

    return &s_sensors;
}
