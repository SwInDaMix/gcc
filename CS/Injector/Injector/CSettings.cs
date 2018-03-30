using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;

namespace Injector
{
    public class CSettings : INotifyPropertyChanged
    {
        private const ushort __DELAY_MAX = 65535;
        private const double __DELAY_MS_MAX = (double)(__DELAY_MAX * 1000) / __PERIOD_1S;
        private const ushort __PERIOD_1S = 28800;
        private const ushort __PERIOD_BASE = (ushort)(__PERIOD_1S / __FREQUENCY_MAX);
        private const ushort __PERIOD_MAX = (ushort)(__PERIOD_1S / __FREQUENCY_MIN);
        private const double __FREQUENCY_MIN = 1;
        private const double __FREQUENCY_MAX = 150;

        public static ushort DELAY_MAX { get { return __DELAY_MAX; } }
        public static double DELAY_MS_MAX { get { return __DELAY_MS_MAX; } }
        public static ushort PERIOD_1S { get { return __PERIOD_1S; } }
        public static ushort PERIOD_BASE { get { return __PERIOD_BASE; } }
        public static ushort PERIOD_MAX { get { return __PERIOD_MAX; } }
        public static double FREQUENCY_MIN { get { return __FREQUENCY_MIN; } }
        public static double FREQUENCY_MAX { get { return __FREQUENCY_MAX; } }

        private bool _is_hold_settings_changed_event = false;

        internal bool _is_valid;
        internal CInjectorControl _injector_control;
        internal ushort _period_offset;
        internal double _frequency;
        internal ushort _delay;
        internal double _delay_ms;
        internal CInjectorFirstSettings _injector_first_settings1;
        internal CInjectorSettings _injector_settings2;
        internal CInjectorSettings _injector_settings3;

        public bool IsValid { get { return _is_valid; } }
        public CInjectorControl InjectorControl { get { return _injector_control; } }
        public ushort Delay { get { return _delay; } set { _NewDelay(value); } }
        public double DelayMs { get { return _delay_ms; } set { _NewDelayMs(value); } }
        public ushort Period { get { return (ushort)(_period_offset + __PERIOD_BASE); } set { _NewPeriod(value); } }
        public double Frequency { get { return _frequency; } set { _NewFrequency(value); } }
        public CInjectorFirstSettings InjectorFirstSettings1 { get { return _injector_first_settings1; } }
        public CInjectorSettings InjectorSettings2 { get { return _injector_settings2; } }
        public CInjectorSettings InjectorSettings3 { get { return _injector_settings3; } }

        public CSettings(CInjectorControl injector_control) {
            _injector_control = injector_control;
            _period_offset = 0;
            _frequency = (double)__PERIOD_1S / __PERIOD_BASE;
            _injector_first_settings1 = new CInjectorFirstSettings(this); _injector_settings2 = new CInjectorSettings(this); _injector_settings3 = new CInjectorSettings(this);
        }

        public void Reset() { _NewIsValid(false); }
        public void SetParams(ushort delay, ushort period_offset, byte duty_cycle1, byte duty_cycle2, byte phase2, byte duty_cycle3, byte phase3) {
            _is_hold_settings_changed_event = true;
            Delay = delay;
            Period = (ushort)(period_offset + __PERIOD_BASE);
            _injector_first_settings1.DutyCycle = duty_cycle1;
            _injector_settings2.Phase = phase2; _injector_settings2.DutyCycle = duty_cycle2;
            _injector_settings3.Phase = phase3; _injector_settings3.DutyCycle = duty_cycle3;
            _NewIsValid(true);
            _is_valid = true;
            _is_hold_settings_changed_event = false;
        }
        public virtual byte[] GetBinary() {
            byte[] _injector_binary1 = _injector_first_settings1.GetBinary();
            byte[] _injector_binary2 = _injector_settings2.GetBinary();
            byte[] _injector_binary3 = _injector_settings3.GetBinary();
            byte[] _binary_new = new byte[2 + 2 + _injector_binary1.Length + _injector_binary2.Length + _injector_binary3.Length]; int _offset = 0;
            BitConverter.GetBytes(_delay).CopyTo(_binary_new, _offset); _offset += 2;
            BitConverter.GetBytes(_period_offset).CopyTo(_binary_new, _offset); _offset += 2;
            _injector_binary1.CopyTo(_binary_new, _offset); _offset += _injector_binary1.Length;
            _injector_binary2.CopyTo(_binary_new, _offset); _offset += _injector_binary2.Length;
            _injector_binary3.CopyTo(_binary_new, _offset); _offset += _injector_binary3.Length;
            return _binary_new;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public event Action SettingsChanged;

        private void _NewIsValid(bool value) { if (_is_valid != value) { _is_valid = value; OnPropertyChanged(nameof(IsValid)); } }
        private void _NewPeriod(ushort value) {
            if (value < __PERIOD_BASE || value > __PERIOD_MAX) throw new ArgumentException(string.Format("Допустимые значение периода от {0} до {1} тактов (одна секунда = {2} тактов)", __PERIOD_BASE, __PERIOD_MAX, __PERIOD_1S), "value");
            ushort _period_offset_new = (ushort)(value - __PERIOD_BASE); double _frequency_new = (double)__PERIOD_1S / value;
            bool _is_changed = false;
            if (_period_offset != _period_offset_new) { _period_offset = _period_offset_new; _is_changed = true; OnPropertyChanged(nameof(Period)); }
            if (_frequency != _frequency_new) { _frequency = _frequency_new; _is_changed = true; OnPropertyChanged(nameof(Frequency)); }
            if (_is_changed) OnSettingsChanged();
        }
        private void _NewFrequency(double value) {
            if (value < __FREQUENCY_MIN || value > __FREQUENCY_MAX) throw new ArgumentException(string.Format("Допустимые значение частоты от {0} до {1} Hz", __FREQUENCY_MIN, __FREQUENCY_MAX), "value");
            _NewPeriod((ushort)(__PERIOD_1S / value));
        }
        private void _NewDelay(ushort value) {
            if (value > __DELAY_MAX) throw new ArgumentException(string.Format("Допустимые значение задержки от 0 до {0} тактов (одна секунда = {1} тактов)", __DELAY_MAX, __PERIOD_1S), "value");
            ushort _delay_new = value; double _delay_ms_new = (double)value * 1000 / PERIOD_1S;
            bool _is_changed = false;
            if (_delay != _delay_new) { _delay = _delay_new; _is_changed = true; OnPropertyChanged(nameof(Delay)); }
            if (_delay_ms != _delay_ms_new) { _delay_ms = _delay_ms_new; _is_changed = true; OnPropertyChanged(nameof(DelayMs)); }
            if (_is_changed) OnSettingsChanged();
        }
        private void _NewDelayMs(double value) {
            if (value > __DELAY_MS_MAX) throw new ArgumentException(string.Format("Допустимые значение задержки от 0 до {0} мс", __DELAY_MS_MAX), "value");
            _NewDelay((ushort)(__PERIOD_1S * value / 1000));
        }

        protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(propery_name)); }
        protected internal void OnSettingsChanged() { if (!_is_hold_settings_changed_event) { Action _handler = SettingsChanged; if (_handler != null) _handler(); } }

        public class CInjectorFirstSettings : INotifyPropertyChanged
        {
            internal CSettings _settings;
            internal byte _duty_cycle;

            public CSettings Settings { get { return _settings; } }
            public byte DutyCycle { get { return _duty_cycle; } set { if (_duty_cycle != value) { _duty_cycle = value; OnPropertyChanged(nameof(DutyCycle)); _settings.OnSettingsChanged(); } } }

            public CInjectorFirstSettings(CSettings settings) { _settings = settings; }

            public virtual byte[] GetBinary() { return new[] { _duty_cycle }; }

            public event PropertyChangedEventHandler PropertyChanged;

            protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(propery_name)); }
        }

        public class CInjectorSettings : CInjectorFirstSettings
        {
            internal byte _phase;

            public byte Phase { get { return _phase; } set { if (_phase != value) { _phase = value; OnPropertyChanged(nameof(Phase)); _settings.OnSettingsChanged(); } } }

            public CInjectorSettings(CSettings settings) : base(settings) { }

            public override byte[] GetBinary() {
                byte[] _binary_base = base.GetBinary();
                byte[] _binary_new = new byte[_binary_base.Length + 1]; int _offset = 0;
                _binary_base.CopyTo(_binary_new, 0); _offset += _binary_base.Length;
                _binary_new[_offset] = _phase; _offset++;
                return _binary_new;
            }
        }
    }
}
