#include <string.h>
#include "settings.h"
#include "ktlcd3.h"
#include "utils.h"
#include "periph.h"

typedef struct {
    sSettings settings;
    uint8_t crc;
} sSettingsInternal;

static sSettingsInternal s_settings;

static uint8_t setting_calc_crc() {
    uint8_t _i;
    uint8_t _crc = 0;
    uint8_t const *_src = (uint8_t const *)&s_settings.settings;

    for (_i = 0; _i < sizeof(sSettings); _i++) _crc = crc8_ccitt_update(_crc, *_src++);
    return _crc;
}

static void setting_eeprom_default() {
    s_settings.settings.motor_settings.phase = NetworkMotorPhase_0;
    s_settings.settings.motor_settings.correction_angle = 12;
    s_settings.settings.motor_settings.wheel_circumference = ktlcd3_get_wheel_size_circumference(KTLCD3_WheelSize_10);
    s_settings.settings.motor_settings.pole_pairs = 15;
    s_settings.settings.motor_settings.max_erps = 600;
    s_settings.settings.max_current_scale = 255;
    s_settings.settings.min_voltage_correct = 0;
    s_settings.settings.throttle_when_moving = false;
    s_settings.settings.scale_max_current_by_gear = true;
}

void settings_init() {
    DBG("Reading EEPROM settings\n");
    periph_eeprom_read(&s_settings, 0, sizeof(sSettingsInternal));
    if(setting_calc_crc() != s_settings.crc) {
        DBG("EEPROM settings bad CRC. Applying defaults\n");
        setting_eeprom_default();
        s_settings.crc = setting_calc_crc();
    }
}

sSettings const *settings_get_current() {
    if(ktlcd3_was_updated()) {
        uint8_t _crc_new;

        s_settings.settings.motor_settings.phase = ktlcd3_get_motor_phase();
        s_settings.settings.motor_settings.correction_angle = BLDC_CORRECTION_ANGLE;
        s_settings.settings.motor_settings.wheel_circumference = ktlcd3_get_wheel_size_circumference(ktlcd3_get_wheel_size());
        s_settings.settings.motor_settings.pole_pairs = ktlcd3_get_motor_poluses() >> 1;
        s_settings.settings.motor_settings.max_erps = (uint16_t)((ktlcd3_get_max_speed() * s_settings.settings.motor_settings.pole_pairs / 3.6 / s_settings.settings.motor_settings.wheel_circumference) + 0.5f);
        s_settings.settings.max_current_scale = ktlcd3_get_max_current_scale(ktlcd3_get_max_current());
        s_settings.settings.min_voltage_correct = ktlcd3_get_min_voltage_correct(ktlcd3_get_min_voltage());
        s_settings.settings.throttle_when_moving = ktlcd3_get_throttle_when_moving();
        s_settings.settings.scale_max_current_by_gear = ktlcd3_get_ignore_pas_gear_ratio();

        _crc_new = setting_calc_crc();
        if(s_settings.crc != _crc_new) {
            DBG("Settings changed. Writing to EEPROM\n");
            s_settings.crc = _crc_new;
            periph_eeprom_write(&s_settings, 0, sizeof(sSettingsInternal));
        }
    }

    return &s_settings.settings;
}