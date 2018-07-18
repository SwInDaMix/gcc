#include <string.h>
#include "settings.h"
#include "ktlcd3.h"
#include "stm8s_flash.h"

typedef struct {
    sSettings settings;
    uint8_t crc;
} sSettingsInternal;

static sSettingsInternal s_settings;

uint8_t _crc8_ccitt_update(uint8_t crc, uint8_t byte) {
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

static uint8_t setting_calc_crc() {
    uint8_t _i;
    uint8_t _crc = 0;
    uint8_t const *_src = (uint8_t const *)&s_settings.settings;

    for (_i = 0; _i < sizeof(sSettings); _i++) _crc = _crc8_ccitt_update(_crc, *_src++);
    return _crc;
}

static void setting_eeprom_read() {
    memcpy(&s_settings, (void *)FLASH_DATA_START_PHYSICAL_ADDRESS, sizeof(sSettingsInternal));
}

static void setting_eeprom_write() {
    uint8_t _i;
    uint32_t *_src = (uint32_t *)&s_settings;
    uint32_t *_dst = (uint32_t *)FLASH_DATA_START_PHYSICAL_ADDRESS;

    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    while (!FLASH_GetFlagStatus(FLASH_FLAG_DUL));
    for (_i = 0; _i < (sizeof(sSettingsInternal) + 3) >> 2; _i++) {
        FLASH_ProgramWord((uint32_t)_dst++, *_src++);
        while (!FLASH_GetFlagStatus(FLASH_FLAG_EOP));
    }
    FLASH_Lock(FLASH_MEMTYPE_DATA);
}

static void setting_eeprom_default() {
    s_settings.settings.motor_polus_pairs = 15;
    s_settings.settings.wheel_circumference = ktlcd3_get_wheel_size_circumference(KTLCD3_WheelSize_10);
    s_settings.settings.erps_limit = 600;
    s_settings.settings.max_current_scale = 255;
    s_settings.settings.min_voltage_correct = 0;
    s_settings.settings.throttle_when_moving = false;
}

void settings_init() {
    DBG("Reading EEPROM settings\n");
    setting_eeprom_read();
    if(setting_calc_crc() != s_settings.crc) {
        DBG("EEPROM settings bad CRC. Applying defaults\n");
        setting_eeprom_default();
        s_settings.crc = setting_calc_crc();
    }
}

sSettings const *settings_get_current() {
    if(ktlcd3_was_updated()) {
        uint8_t _crc_new;

        s_settings.settings.motor_polus_pairs = ktlcd3_get_motor_poluses() >> 1;
        s_settings.settings.wheel_circumference = ktlcd3_get_wheel_size_circumference(ktlcd3_get_wheel_size());
        s_settings.settings.erps_limit = (uint16_t)((ktlcd3_get_max_speed() * s_settings.settings.motor_polus_pairs / 3.6 / s_settings.settings.wheel_circumference) + 0.5f);
        s_settings.settings.max_current_scale = ktlcd3_get_max_current_scale(ktlcd3_get_max_current());
        s_settings.settings.min_voltage_correct = ktlcd3_get_min_voltage_correct(ktlcd3_get_min_voltage());
        s_settings.settings.throttle_when_moving = ktlcd3_get_throttle_when_moving();

        _crc_new = setting_calc_crc();
        if(s_settings.crc != _crc_new) {
            DBG("Settings changed. Writing to EEPROM\n");
            s_settings.crc = _crc_new;
            setting_eeprom_write();
        }
    }

    return &s_settings.settings;
}