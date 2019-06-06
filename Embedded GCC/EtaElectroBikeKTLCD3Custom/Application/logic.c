#include <string.h>
#include <display.h>
#include <network.h>
#include "logic.h"
#include "buttons.h"
#include "display.h"
#include "sensors.h"
#include "network.h"
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

static void logic_shutdown(sSettings *settings) {
    settings->odometer += s_disp_screen_main.stat.distance.session;
    settings->total_ride_time += s_disp_screen_main.stat.time.ride;
    settings_flush();
    periph_shutdown();
}

void logic_init() {
    disp_set_screens(&s_disp_screen_main, &s_disp_screen_settings);

    FLAG_SET(s_logic_network_payload_controller.control.flags, NetworkControllerControlFlag_Light);
}

void logic_cycle(sSensors const *sensors, sSettings *settings) {
    static eLogicState _s_logic_state;
    static eLogicQuickSettingsState _s_logic_qsettings_state;
    static ePeriphButton _s_buttons_repeat = PeriphButton__None;
    static uint8_t _s_disp_drive_timeout;

    static uint32_t _s_reset_erotations;

    eDispMainFlags _flags = DispMainFlags_None;
    eDispState _state = DispState_Main;

    memcpy(&s_logic_network_payload_lcd, network_get_payload_lcd(), sizeof(sNetworkPayload_LCD));

    // Buttons cycle
    {
        eButton _button;
        eButtonState _button_state;

        if(_s_logic_state == LogicState_Drive) _s_buttons_repeat = PeriphButton__None;
        else if(_s_logic_state == LogicState_QuickSettings) _s_buttons_repeat = _s_logic_qsettings_state == LogicQuickSettingsState_AdjustMeasureUnit ? PeriphButton__None : PeriphButton__UpDown;
        else if(_s_logic_state == LogicState_Settings) _s_buttons_repeat = PeriphButton__UpDown;

        if(buttons_process(_s_buttons_repeat, &_button, &_button_state)) {
            DBGF("bs:%d,%d\n", _button, _button_state);
            if(_button == Button_OnOff && _button_state == ButtonState_LongClick) logic_shutdown(settings);
            // State "Drive"
            if(_s_logic_state == LogicState_Drive) {
                if(_button == Button_Up && _button_state == ButtonState_LongClick) { FLAG_TOGGLE(s_logic_network_payload_controller.control.flags, NetworkControllerControlFlag_Light); }
                if(_button == Button_Down && _button_state == ButtonState_LongClick) { FLAG_SET(s_logic_network_payload_controller.control.flags, NetworkControllerControlFlag_CruiseControl); }
                if(_button == Button_Up && _button_state == ButtonState_Click) { if(s_logic_network_payload_controller.control.gear < s_logic_network_payload_lcd.control.max_gear) s_logic_network_payload_controller.control.gear++; }
                if(_button == Button_Up && _button_state == ButtonState_DoubleClick) { s_logic_network_payload_controller.control.gear = s_logic_network_payload_lcd.control.max_gear; }
                if(_button == Button_Down && _button_state == ButtonState_Click) { if(s_logic_network_payload_controller.control.gear) s_logic_network_payload_controller.control.gear--; }
                if(_button == Button_Down && _button_state == ButtonState_DoubleClick) { s_logic_network_payload_controller.control.gear = 0; }
                if(_button == Button_OnOff && _button_state == ButtonState_Click) { if(s_disp_screen_main.state < (DispMainState__Max - 1)) s_disp_screen_main.state++; else s_disp_screen_main.state = 0; }
                if(_button == Button_OnOff && _button_state == ButtonState_DoubleClick) { _s_logic_state = LogicState_QuickSettings; }
            // State "Quick Settings"
            } else if(_s_logic_state == LogicState_QuickSettings) {
                if(_button == Button_OnOff && _button_state == ButtonState_Click) { if(_s_logic_qsettings_state < (LogicQuickSettingsState__Max - 1)) _s_logic_qsettings_state++; else _s_logic_qsettings_state = 0; }
                if(_s_logic_qsettings_state == LogicQuickSettingsState_AdjustMaxSpeed) {
                    uint32_t _value = conv_erps_to_10mh(settings->motor_settings.max_erps, &settings->motor_settings);
                    if(settings->measure_unit == DispMainMeasureUnit_Imperic) _value = conv_distanceKm_to_Mil(_value);
                    if(_button == Button_Up) _value = inc32(round32(_value, 50), 100, _button_state - ButtonState_Click, 9990);
                    if(_button == Button_Down) _value = dec32(round32(_value, 50), 100, _button_state - ButtonState_Click);
                    if(settings->measure_unit == DispMainMeasureUnit_Imperic) _value = conv_distanceMil_to_Km(_value);
                    settings->motor_settings.max_erps = conv_10mh_to_erps(_value, &settings->motor_settings);
                    DBGF("after : %lu, %u\n", _value, settings->motor_settings.max_erps);
                }

                if(_button == Button_OnOff && _button_state == ButtonState_LongDoubleClick) {
                    if(_s_logic_qsettings_state == LogicQuickSettingsState_ResetTTM) { settings->total_ride_time = 0; periph_reset_seconds(); }
                    if(_s_logic_qsettings_state == LogicQuickSettingsState_ResetODO) { settings->odometer = 0; _s_reset_erotations = s_logic_network_payload_lcd.stat.erotations; }
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
        if(FLAG_IS_SET(s_logic_network_payload_controller.control.flags, NetworkControllerControlFlag_Light)) _flags |= DispMainFlags_Light;
        if(FLAG_IS_SET(s_logic_network_payload_controller.control.flags, NetworkControllerControlFlag_CruiseControl) || FLAG_IS_SET(s_logic_network_payload_lcd.control.flags, NetworkLCDControlFlag_CruiseControl)) _flags |= DispMainFlags_Cruise;
        if(FLAG_IS_SET(s_logic_network_payload_lcd.control.flags, NetworkLCDControlFlag_Accelerating)) _flags |= DispMainFlags_Accelerating;
        if(FLAG_IS_SET(s_logic_network_payload_lcd.control.flags, NetworkLCDControlFlag_Braking)) _flags |= DispMainFlags_Braking;
        if(FLAG_IS_SET(s_logic_network_payload_lcd.control.flags, NetworkLCDControlFlag_Assist)) _flags |= DispMainFlags_Assist;
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

    periph_set_backlight_pwm_duty_cycle((FLAG_IS_SET(settings->flags, SettingsFlag_AlwaysBacklight) || FLAG_IS_SET(s_logic_network_payload_controller.control.flags, NetworkControllerControlFlag_Light)) ? settings->backlight_brightness : 0);

    s_logic_network_payload_controller.sensors.voltage = sensors->voltage;
    s_logic_network_payload_controller.sensors.system_temp = sensors->system_temp;

    s_disp_screen_main.measure_unit = settings->measure_unit;
    s_disp_screen_main.battery_soc = s_logic_network_payload_lcd.control.battery_soc;
    s_disp_screen_main.sensors.voltage = s_logic_network_payload_lcd.sensors.voltage;
    s_disp_screen_main.sensors.wattage = s_logic_network_payload_lcd.sensors.wattage;
    s_disp_screen_main.sensors.temp_system = sensors->system_temp;
    s_disp_screen_main.sensors.temp_motor = s_logic_network_payload_lcd.sensors.temp_motor;
    s_disp_screen_main.stat.wattage_consumed = s_logic_network_payload_lcd.stat.wattage_consumed;
    s_disp_screen_main.stat.distance.session = conv_erps_to_10m(s_logic_network_payload_lcd.stat.erotations - _s_reset_erotations, &settings->motor_settings);
    s_disp_screen_main.stat.distance.odometer = settings->odometer + s_disp_screen_main.stat.distance.session;

    s_disp_screen_main.speed.max = conv_erps_to_10mh(settings->motor_settings.max_erps, &settings->motor_settings);
    s_disp_screen_main.speed.avg = s_logic_network_payload_lcd.stat.ride_time ? ((uint64_t)s_disp_screen_main.stat.distance.session * 3600U / s_logic_network_payload_lcd.stat.ride_time) : 0;
    s_disp_screen_main.speed.current = conv_erps_to_10mh(s_logic_network_payload_lcd.sensors.erps, &settings->motor_settings);
    if(settings->measure_unit == DispMainMeasureUnit_Imperic) {
        s_disp_screen_main.sensors.temp_system = conv_tempC_to_F(s_disp_screen_main.sensors.temp_system);
        s_disp_screen_main.sensors.temp_motor = conv_tempC_to_F(s_disp_screen_main.sensors.temp_motor);
        s_disp_screen_main.stat.distance.session = conv_distanceKm_to_Mil(s_disp_screen_main.stat.distance.session);
        s_disp_screen_main.stat.distance.odometer = conv_distanceKm_to_Mil(s_disp_screen_main.stat.distance.odometer);
        s_disp_screen_main.speed.avg = conv_distanceKm_to_Mil(s_disp_screen_main.speed.avg);
        s_disp_screen_main.speed.current = conv_distanceKm_to_Mil(s_disp_screen_main.speed.current);
    }

    // update session time
    {
        static uint32_t _s_seconds_prev;
        uint32_t _seconds = periph_get_seconds();

        s_disp_screen_main.stat.time.session = _seconds;
        s_disp_screen_main.stat.time.ride = s_logic_network_payload_lcd.stat.ride_time;
        s_disp_screen_main.stat.time.total_ride = settings->total_ride_time + s_disp_screen_main.stat.time.ride;

        _s_disp_drive_timeout += (_seconds - _s_seconds_prev);
        _s_seconds_prev = _seconds;
    }

    network_update_payload_controller(&s_logic_network_payload_controller);

    s_disp_screen_main.flags = _flags;
    disp_cycle(_state);
}
