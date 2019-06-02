#include <display.h>
#include <network.h>
#include "logic.h"
#include "buttons.h"
#include "display.h"
#include "sensors.h"
#include "utils.h"

typedef enum {
    LogicQuickSettingsState_AdjustMaxSpeed,
    LogicQuickSettingsState_ResetTTM,
    LogicQuickSettingsState_ResetODO,
    LogicQuickSettingsState_AdjustMeasureUnit,
    LogicQuickSettingsState__Max,
} eLogicQuickSettingsState;

static sDispScreenMain s_disp_screen_main;
static sDispScreenSettings s_disp_screen_settings;

static sNetworkPayload_Controller s_logic_network_payload_controller;
static sNetworkPayload_LCD s_logic_network_payload_lcd;

#ifdef DEBUG
static void uar_cb(uint8_t byte) {
    if(byte == 'q') { s_disp_screen_main.test.battery_soc++; if(s_disp_screen_main.test.battery_soc >= DispBatterySoc__Max) s_disp_screen_main.test.battery_soc = 0; }
    if(byte == 'a') { if(s_disp_screen_main.test.battery_soc == DispBatterySoc_None) s_disp_screen_main.test.battery_soc = DispBatterySoc__Max; s_disp_screen_main.test.battery_soc--; }
    if(byte == 'w') { s_disp_screen_main.test.digit++; if(s_disp_screen_main.test.digit >= LCDDigit__Max) s_disp_screen_main.test.digit = 0; }
    if(byte == 's') { if(s_disp_screen_main.test.digit == LCDDigit_None) s_disp_screen_main.test.digit = LCDDigit__Max; s_disp_screen_main.test.digit--; }
    if(byte == 'e') { s_disp_screen_main.test.bit++; if(s_disp_screen_main.test.bit >= LCDBit__Max) s_disp_screen_main.test.bit = 0; }
    if(byte == 'd') { if(s_disp_screen_main.test.bit == LCDBit_Gear_Cruise) s_disp_screen_main.test.bit = LCDBit__Max; s_disp_screen_main.test.bit--; }
    if(byte == 'r') { s_disp_screen_main.sensors.temp_motor++; s_disp_screen_main.sensors.temp_system++; }
    if(byte == 'f') { s_disp_screen_main.sensors.temp_motor--; s_disp_screen_main.sensors.temp_system--; }
    if(byte == 't') s_disp_screen_main.speed.current += 50;
    if(byte == 'g') s_disp_screen_main.speed.current -= 50;
    if(byte == 'y') { s_disp_screen_main.flashing++; if(s_disp_screen_main.flashing >= DispMainFlashing__Max) s_disp_screen_main.flashing = 0; }
    if(byte == 'c') { s_disp_screen_main.state++; if(s_disp_screen_main.state >= DispMainState__Max) s_disp_screen_main.state = 0; }
    if(byte == 'v') { s_disp_screen_main.measure_unit ^= 1; }
    if(byte == 'b') { s_disp_screen_main.flags ^= (DispMainFlags_Accelerating); }
    if(byte == 'n') { s_disp_screen_main.flags ^= (DispMainFlags_Brake); }
    if(byte == 'm') { s_disp_screen_main.flags ^= (DispMainFlags_Light); periph_set_backlight_pwm_duty_cycle(s_disp_screen_main.flags & (DispMainFlags_Light) ? 128 : 0); }
    if(byte == 'z') periph_set_onoff_power(false);
}
#endif

static void logic_shutdown(sSettings *settings) {
    settings->odometer += s_disp_screen_main.stat.distance.session;
    settings->ttm += s_disp_screen_main.stat.time.session;
    settings_flush();
    periph_shutdown();
}

void logic_init() {
    periph_uart_set_on_received_callback(uar_cb); // TODO: remove

    disp_set_screens(&s_disp_screen_main, &s_disp_screen_settings);

    s_logic_network_payload_lcd.drive.max_gear = 3;
    s_logic_network_payload_lcd.sensors.temp_motor = 25;
    s_logic_network_payload_lcd.sensors.voltage = 23456;
    s_logic_network_payload_lcd.sensors.wattage = 455;
    s_logic_network_payload_lcd.sensors.erps = 150;

    s_logic_network_payload_lcd.stat.total_erotations = 150000;
    s_logic_network_payload_lcd.stat.total_ride_time = 360;
    s_logic_network_payload_lcd.stat.wattage_consumed = 1234;

    s_logic_network_payload_controller.control.is_light = true;
}

void logic_cycle(sSensors const *sensors, sSettings *settings) {
    static eLogicState _s_logic_state;
    static eLogicQuickSettingsState _s_logic_qsettings_state;
    static ePeriphButton _s_buttons_repeat = PeriphButton__None;
    static uint8_t _s_disp_drive_timeout;

    static uint32_t _s_reset_erotations;

    eDispMainFlags _flags = DispMainFlags_None;
    eDispState _state = DispState_Main;

    // Buttons cycle
    {
        eButton _button;
        eButtonState _button_state;

        if(_s_logic_state == LogicState_Drive) _s_buttons_repeat = PeriphButton__None;
        else if(_s_logic_state == LogicState_QuickSettings) _s_buttons_repeat = PeriphButton__UpDown;
        else if(_s_logic_state == LogicState_Settings) _s_buttons_repeat = PeriphButton__UpDown;

        if(buttons_process(_s_buttons_repeat, &_button, &_button_state)) {
            DBGF("bs:%d,%d\n", _button, _button_state);
            if(_button == Button_OnOff && _button_state == ButtonState_LongClick) logic_shutdown(settings);
            // State "Drive"
            if(_s_logic_state == LogicState_Drive) {
                if(_button == Button_Up && _button_state == ButtonState_LongClick) { s_logic_network_payload_controller.control.is_light = !s_logic_network_payload_controller.control.is_light; }
                if(_button == Button_Down && _button_state == ButtonState_LongClick) { s_logic_network_payload_controller.control.is_cruise_control = true; }
                if(_button == Button_Up && _button_state == ButtonState_Click) { if(s_logic_network_payload_controller.control.gear < s_logic_network_payload_lcd.drive.max_gear) s_logic_network_payload_controller.control.gear++; }
                if(_button == Button_Up && _button_state == ButtonState_DoubleClick) { s_logic_network_payload_controller.control.gear = s_logic_network_payload_lcd.drive.max_gear; }
                if(_button == Button_Down && _button_state == ButtonState_Click) { if(s_logic_network_payload_controller.control.gear) s_logic_network_payload_controller.control.gear--; }
                if(_button == Button_Down && _button_state == ButtonState_DoubleClick) { s_logic_network_payload_controller.control.gear = 0; }
                if(_button == Button_OnOff && _button_state == ButtonState_Click) { if(s_disp_screen_main.state < (DispMainState__Max - 1)) s_disp_screen_main.state++; else s_disp_screen_main.state = 0; }
                if(_button == Button_OnOff && _button_state == ButtonState_DoubleClick) { _s_logic_state = LogicState_QuickSettings; }
            // State "Quick Settings"
            } else if(_s_logic_state == LogicState_QuickSettings) {
                if(_button == Button_OnOff && _button_state == ButtonState_Click) { if(_s_logic_qsettings_state < (LogicQuickSettingsState__Max - 1)) _s_logic_qsettings_state++; else _s_logic_qsettings_state = 0; }
                if(_button == Button_Up && _button_state == ButtonState_Click) {
                    if(_s_logic_qsettings_state == LogicQuickSettingsState_AdjustMaxSpeed) {
                        if(settings->measure_unit == DispMainMeasureUnit_Imperic) {
                            if(settings->max_mph < 990) { settings->max_mph = ((settings->max_mph / 10) + 1) * 10; settings->max_kmh = conv_distanceMil2Km(settings->max_mph); }
                            if(settings->max_kmh > 999) settings->max_kmh = 999;
                        } else {
                            if(settings->max_kmh < 990) { settings->max_kmh = ((settings->max_kmh / 10) + 1) * 10; settings->max_mph = conv_distanceKm2Mil(settings->max_kmh); }
                        }
                    }
                    if(_s_logic_qsettings_state == LogicQuickSettingsState_AdjustMeasureUnit) { settings->measure_unit = DispMainMeasureUnit_Metric; }
                }
                if(_button == Button_Down && _button_state == ButtonState_Click) {
                    if(_s_logic_qsettings_state == LogicQuickSettingsState_AdjustMaxSpeed) {
                        if(settings->measure_unit == DispMainMeasureUnit_Imperic) {
                            if(settings->max_mph) { if(settings->max_mph >= 10) settings->max_mph = ((settings->max_mph / 10) - 1) * 10; else settings->max_mph = 0; settings->max_kmh = conv_distanceMil2Km(settings->max_mph); }
                            if(settings->max_kmh > 999) settings->max_kmh = 999;
                        } else {
                            if(settings->max_kmh) { if(settings->max_kmh >= 10) settings->max_kmh = ((settings->max_kmh / 10) - 1) * 10; else settings->max_kmh = 0; settings->max_mph = conv_distanceKm2Mil(settings->max_kmh); }
                        }
                    }
                    if(_s_logic_qsettings_state == LogicQuickSettingsState_AdjustMeasureUnit) { settings->measure_unit = DispMainMeasureUnit_Imperic; }
                }
                if(_button == Button_OnOff && _button_state == ButtonState_LongDoubleClick) {
                    if(_s_logic_qsettings_state == LogicQuickSettingsState_ResetTTM) { settings->ttm = 0; periph_reset_seconds(); }
                    if(_s_logic_qsettings_state == LogicQuickSettingsState_ResetODO) { settings->odometer = 0; _s_reset_erotations = s_logic_network_payload_lcd.stat.total_erotations; }
                }
                if(_button == Button_OnOff && _button_state == ButtonState_DoubleClick) { _s_logic_state = LogicState_Settings; }
            } else if(_s_logic_state == LogicState_Settings) {
                if(_button == Button_OnOff && _button_state == ButtonState_DoubleClick) { _s_logic_state = LogicState_Drive; }
            }
            _s_disp_drive_timeout = 0;
        }
    }

    // timeouts
    if(_s_logic_state == LogicState_Drive && s_disp_screen_main.state != DispMainState_Drive && _s_disp_drive_timeout >= 5) s_disp_screen_main.state = DispMainState_Drive;
    if((_s_logic_state == LogicState_QuickSettings || _s_logic_state == LogicState_Settings) && _s_disp_drive_timeout >= 60) _s_logic_state = LogicState_Drive;

    // State "Drive"
    if(_s_logic_state == LogicState_Drive) {
        _state = DispState_Main;
        s_disp_screen_main.flashing = DispMainFlashing_None;
        s_disp_screen_main.gear = s_logic_network_payload_controller.control.gear;
        periph_set_backlight_pwm_duty_cycle(s_logic_network_payload_controller.control.is_light ? settings->backlight_brightness : 0);
        if(s_logic_network_payload_controller.control.is_light) _flags |= DispMainFlags_Light;
        if(s_logic_network_payload_controller.control.is_cruise_control) _flags |= DispMainFlags_Cruise;
    // State "Quick Settings"
    } else if(_s_logic_state == LogicState_QuickSettings) {
        _state = DispState_Main;
        if(_s_logic_qsettings_state == LogicQuickSettingsState_ResetTTM) s_disp_screen_main.flashing = DispMainFlashing_TTM;
        else if(_s_logic_qsettings_state == LogicQuickSettingsState_ResetODO) s_disp_screen_main.flashing = DispMainFlashing_ODO;
        else if(_s_logic_qsettings_state == LogicQuickSettingsState_AdjustMaxSpeed) s_disp_screen_main.flashing = DispMainFlashing_MaxSpeed;
        else if(_s_logic_qsettings_state == LogicQuickSettingsState_AdjustMeasureUnit) s_disp_screen_main.flashing = settings->measure_unit == DispMainMeasureUnit_Imperic ? DispMainFlashing_MeasureUnitImperic : DispMainFlashing_MeasureUnitMetric;
    // State "Settings"
    } else if(_s_logic_state == LogicState_Settings) {
        _state = DispState_Settings;
    }

    s_disp_screen_main.measure_unit = settings->measure_unit;
    s_logic_network_payload_controller.sensors.system_temp = sensors->system_temp;
    s_disp_screen_main.sensors.temp_system = s_disp_screen_main.measure_unit == DispMainMeasureUnit_Imperic ? conv_tempC2F(sensors->system_temp) : sensors->system_temp;
    s_disp_screen_main.sensors.temp_motor = s_disp_screen_main.measure_unit == DispMainMeasureUnit_Imperic ? conv_tempC2F(s_logic_network_payload_lcd.sensors.temp_motor) : s_logic_network_payload_lcd.sensors.temp_motor;
    s_disp_screen_main.sensors.wattage = s_logic_network_payload_lcd.sensors.wattage;
    s_disp_screen_main.stat.wattage_consumed = s_logic_network_payload_lcd.stat.wattage_consumed;
    s_disp_screen_main.stat.distance.session = (uint64_t)(s_logic_network_payload_lcd.stat.total_erotations - _s_reset_erotations) * settings->wheel_circumference / settings->motor_pole_pairs / 10000;
    s_disp_screen_main.stat.distance.odometer = settings->odometer + s_disp_screen_main.stat.distance.session;

    s_disp_screen_main.speed.max = (settings->measure_unit == DispMainMeasureUnit_Imperic ? settings->max_mph : settings->max_kmh) * 50;
    s_disp_screen_main.speed.avg = s_logic_network_payload_lcd.stat.total_ride_time ? ((uint64_t)s_disp_screen_main.stat.distance.session * 18000 / s_logic_network_payload_lcd.stat.total_ride_time) : 0;
    s_disp_screen_main.speed.current = conv_erps2mh(s_logic_network_payload_lcd.sensors.erps, settings->wheel_circumference, settings->motor_pole_pairs) / 2;
    if(settings->measure_unit == DispMainMeasureUnit_Imperic) {
        s_disp_screen_main.stat.distance.session = conv_distanceKm2Mil(s_disp_screen_main.stat.distance.session);
        s_disp_screen_main.stat.distance.odometer = conv_distanceKm2Mil(s_disp_screen_main.stat.distance.odometer);
        s_disp_screen_main.speed.avg = conv_distanceKm2Mil(s_disp_screen_main.speed.avg);
        s_disp_screen_main.speed.current = conv_distanceKm2Mil(s_disp_screen_main.speed.current);
    }

    // update session time
    {
        static uint32_t _s_seconds_prev;
        uint32_t _seconds = periph_get_seconds();
        if(_s_seconds_prev != _seconds) {
            _s_seconds_prev = _seconds;
            _s_disp_drive_timeout++;
            s_disp_screen_main.stat.time.session = (uint16_t)_s_seconds_prev;
            s_disp_screen_main.stat.time.total = settings->ttm + s_disp_screen_main.stat.time.session;
        }
    }

    s_disp_screen_main.flags = _flags;

    disp_cycle(_state);
}
