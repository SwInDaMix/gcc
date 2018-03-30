using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows;
using Microsoft.Win32;

namespace EtaLoggingCharger
{
    public static class EtaSettings
    {
        private static readonly string SettingRegistryPath = @"Software\Eta\UartLogger";
        private static RegistryKey _registry_key_software;

        public static readonly string DefaultLanguage = "English";
        public static readonly string DefaultDeviceSerialPort = "COM1";
        public static readonly uint DefaultDeviceQuartzFrequency = 8000000;

        private static string _language;
        private static string _device_serial_port;
        private static uint? s_device_quartz_frequency;
//        private static IEnumerable<string> _device_host_addresses;
//        private static Color? _remote_control_back_color;

        public static string Language { get { return _language ?? (_language = _GetSetting("Language", DefaultLanguage)); } set { _SetSetting("Language", value); _language = value; } }
        public static string DeviceSerialPort { get { return _device_serial_port ?? (_device_serial_port = _GetSetting("DeviceSerialPort", DefaultDeviceSerialPort)); } set { _SetSetting("DeviceSerialPort", value); _device_serial_port = value; } }
        public static uint DeviceQuartzFrequency { get { if (!s_device_quartz_frequency.HasValue) s_device_quartz_frequency = _GetSetting("DeviceQuartzFrequency", DefaultDeviceQuartzFrequency); return s_device_quartz_frequency.Value; } set { _SetSetting("DeviceQuartzFrequency", value); s_device_quartz_frequency = value; } }
//        public static IEnumerable<string> DeviceHostAddresses { get { return _device_host_addresses ?? (_device_host_addresses = _GetSettingArray("DeviceHostAddresses", 10, DefaultDeviceHostAddress)); } set { _SetSettingArray("DeviceHostAddresses", 10, value); _device_host_addresses = value; } }
//        public static Color RemoteControlBackColor { get { if (!_remote_control_back_color.HasValue) _remote_control_back_color = Color.FromArgb(255, Color.FromArgb(_GetSetting("RemoteControlBackColor", DefaultRemoteControlBackColor.ToArgb()))); return _remote_control_back_color.Value; } set { _SetSetting("RemoteControlBackColor", value.ToArgb()); _remote_control_back_color = value; } }

        public static Point PointGet(string name, Point pt_default) { return new Point(_GetSetting(name + "_X", pt_default.X), _GetSetting(name + "_Y", pt_default.Y)); }
        public static Size SizeGet(string name, Size sz_default) { return new Size(_GetSetting(name + "_Width", sz_default.Width), _GetSetting(name + "_Height", sz_default.Height)); }
        public static Rect RectGet(string name, Rect rc_default) { return new Rect(PointGet(name, rc_default.Location), SizeGet(name, rc_default.Size)); }
        public static void PointSet(string name, Point rc) { _SetSetting(name + "_X", rc.X); _SetSetting(name + "_Y", rc.Y); }
        public static void SizeSet(string name, Size rc) { _SetSetting(name + "_Width", rc.Width); _SetSetting(name + "_Height", rc.Height); }
        public static void RectSet(string name, Rect rc) { PointSet(name, rc.Location); SizeSet(name, rc.Size); }

        static EtaSettings() { _registry_key_software = Registry.CurrentUser.CreateSubKey(SettingRegistryPath); }

        private static IEnumerable<TItem> _GetSettingArray<TItem>(string setting_name, int max_items, TItem default_value) { return _GetSettingArray(_registry_key_software, setting_name, max_items, default_value); }
        private static IEnumerable<TItem> _GetSettingArray<TItem>(RegistryKey _registry_key, string setting_name, int max_items, TItem default_value) {
            List<TItem> _list_items = new List<TItem>();
            RegistryKey _registry_key_array = _registry_key.CreateSubKey(setting_name + "_Array");
            HashSet<string> _hs_value_names = new HashSet<string>(_registry_key_array.GetValueNames());
            for (int _i = 0; _i < max_items; _i++) {
                string _value_name = _i.ToString();
                if (_hs_value_names.Contains(_value_name)) _list_items.Add(_GetSetting(_registry_key_array, _value_name, default_value));
            }
            return _list_items;
        }
        private static TEnum _GetSettingEnum<TEnum>(string setting_name, TEnum default_value) { return _GetSettingEnum(_registry_key_software, setting_name, default_value); }
        private static TEnum _GetSettingEnum<TEnum>(RegistryKey _registry_key, string setting_name, TEnum default_value) {
            try { return (TEnum)Enum.Parse(typeof(TEnum), (string)_registry_key.GetValue(setting_name, default_value.ToString()), true); }
            catch (Exception) { _SetSetting(setting_name, default_value.ToString()); return default_value; }
        }
        private static TType _GetSetting<TType>(string setting_name, TType default_value) { return _GetSetting(_registry_key_software, setting_name, default_value); }
        private static TType _GetSetting<TType>(RegistryKey _registry_key, string setting_name, TType default_value) {
            try { return (TType)_registry_key.GetValue(setting_name, default_value); }
            catch (Exception) { _SetSetting<TType>(setting_name, default_value); return default_value; }
        }

        private static void _SetSettingArray<TItem>(string setting_name, int max_items, IEnumerable<TItem> values) { _SetSettingArray(_registry_key_software, setting_name, max_items, values); }
        private static void _SetSettingArray<TItem>(RegistryKey _registry_key, string setting_name, int max_items, IEnumerable<TItem> values) {
            RegistryKey _registry_key_array = _registry_key.CreateSubKey(setting_name + "_Array");
            IEnumerator<TItem> _values_enumerator = values.GetEnumerator();
            for (int _i = 0; _i < max_items; _i++) {
                if (!_values_enumerator.MoveNext()) break;
                _SetSetting(_registry_key_array, _i.ToString(), _values_enumerator.Current);
            }
        }
        private static void _SetSetting<TType>(string setting_name, TType value) { _SetSetting(_registry_key_software, setting_name, value); }
        private static void _SetSetting<TType>(RegistryKey _registry_key, string setting_name, TType value) {
            _registry_key.SetValue(setting_name, value);
        }
    }
}
