#include "settings.h"
#include "periph.h"
#include "utils.h"

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

static void setting_eeprom_read() { periph_eeprom_read(&s_settings, 0, sizeof(sSettingsInternal)); }
static void setting_eeprom_write() { periph_eeprom_write(&s_settings, 0, sizeof(sSettingsInternal)); }

static void setting_eeprom_default() {
    s_settings.settings.measure_unit = DispMainMeasureUnit_Metric;
    s_settings.settings.odometer = 0;
    s_settings.settings.ttm = 0;
    s_settings.settings.wheel_circumference = 798;
    s_settings.settings.motor_pole_pairs = 15;
    s_settings.settings.max_kmh = 300;
    s_settings.settings.max_mph = conv_distanceKm2Mil(s_settings.settings.max_kmh);
    s_settings.settings.backlight_brightness = 128;
}

void settings_init() {
    uint8_t _crc_eeprom;
    DBG("Reading EEPROM settings\n");
    setting_eeprom_read();
    _crc_eeprom = setting_calc_crc();
    if(s_settings.crc != _crc_eeprom) {
        DBGF("EEPROM settings bad CRC. Applying defaults %d = %d\n", _crc_eeprom, s_settings.crc);
        setting_eeprom_default();
        setting_eeprom_write();
        //s_settings.crc = _crc_eeprom + 1; // corrupt crc to force write later
    }
}

void settings_flush() {
    uint8_t _crc_new = setting_calc_crc();
    if(s_settings.crc != _crc_new) {
        DBGF("Settings changed. Writing to EEPROM %d = %d\n", s_settings.crc, _crc_new);
        s_settings.crc = _crc_new;
        setting_eeprom_write();
    }
}

sSettings *settings_get_current() { return &s_settings.settings; }