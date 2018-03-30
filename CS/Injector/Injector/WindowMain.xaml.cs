using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Injector
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        CInjectorControl _injector_control;

        public CInjectorControl InjectorControl { get { return _injector_control; } }

        public MainWindow() {
            _injector_control = new CInjectorControl(); _injector_control.Settings.SettingsChanged += () => _injector_control.PushSingleton(new CConnectionFrames.CFrame((byte)CInjectorControl.ECommand.SETTINGS_DATA, _injector_control.Settings.GetBinary()));
            InitializeComponent();
            EventManager.RegisterClassHandler(typeof(TextBox), TextBox.GotFocusEvent, new RoutedEventHandler(TextBox_GotFocus));
            EventManager.RegisterClassHandler(typeof(TextBox), TextBox.PreviewMouseDownEvent, new MouseButtonEventHandler(TextBox_PreviewMouseDown));
            EventManager.RegisterClassHandler(typeof(TextBox), TextBox.PreviewKeyUpEvent, new KeyEventHandler(TextBox_PreviewKeyUpEvent));
        }

        protected override void OnClosing(CancelEventArgs e) { base.OnClosing(e); InjectorControl.Disconnect(); }

        private void ButtonConnection_OnClick(object sender, RoutedEventArgs e) {
            try {
                if (InjectorControl.IsConnected) InjectorControl.Disconnect(); else InjectorControl.Connect();
            }
            catch (Exception ex) { MessageBox.Show(ex.Message); }
        }
        private void ButtonSaveSettings_OnClick(object sender, RoutedEventArgs e) { _injector_control.SaveSettingsToController(); }
        private void ButtonTest_OnClick(object sender, RoutedEventArgs e) { _injector_control._connection_frames._serial_port.Write(new byte[] { 0x72, 0xC5, 0xD9, 0xAC, 0x86, 0xE3, 0xBA, 0x9E, 0x10, 0xAA, 0x55, 0x7d }, 0, 12); }

        private void TextBox_GotFocus(object sender, RoutedEventArgs e) {
            ((TextBox)sender).SelectAll();
        }
        private void TextBox_PreviewMouseDown(object sender, MouseButtonEventArgs e) {
            TextBox _text_box = (TextBox)sender;
            if (!_text_box.IsKeyboardFocusWithin) {
                _text_box.Focus();
                e.Handled = true;
            }
        }
        private void TextBox_PreviewKeyUpEvent(object sender, KeyEventArgs e) {
            if (e.Key == Key.Enter) {
                TextBox _text_box = (TextBox)sender;
                _text_box.GetBindingExpression(TextBox.TextProperty).UpdateSource();
                _text_box.SelectAll();
            }
        }

        private void ComPorts_OnDropDownOpened(object sender, EventArgs e) {
            ComboBox _combo_box = (ComboBox)sender;
            _combo_box.ItemsSource = SerialPort.GetPortNames();
        }
    }

    public class svc_IsConnectedToText : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? "Отключиться" : "Подключиться"; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_IsConnectedToVisible : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? Visibility.Visible : Visibility.Collapsed; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_IsValidToVisible : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? Visibility.Visible : Visibility.Collapsed; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_IsValidToVisibleInverted : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? Visibility.Collapsed : Visibility.Visible; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_FrequencyLogarithmic : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try {
                double _diff = CSettings.FREQUENCY_MAX - CSettings.FREQUENCY_MIN, _coef = _diff / Math.Sqrt(_diff);
                return Math.Sqrt((double)value - CSettings.FREQUENCY_MIN) * _coef;
            }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) {
            try {
                double _diff = CSettings.FREQUENCY_MAX - CSettings.FREQUENCY_MIN, _coef = _diff / Math.Pow(_diff, 2);
                return (Math.Pow((double)value, 2) * _coef) + CSettings.FREQUENCY_MIN;
            }
            catch (Exception) { return null; }
        }
    }
    public class svc_DutyCycle : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (double)(byte)value / 255 * 100; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) {
            try { double _value = double.Parse((string)value, CultureInfo.InvariantCulture); return _value >= 0 && _value <= 100 ? ((object)(byte)(_value / 100 * 255)) : null; }
            catch (Exception) { return null; }
        }
    }
    public class svc_PhaseDegree : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (double)(byte)value / 256 * 360; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) {
            try { double _value = double.Parse((string)value, CultureInfo.InvariantCulture); return _value >= 0 && _value <= (360.0 - (360.0 / 256)) ? ((object)(byte)(_value / 360 * 256)) : null; }
            catch (Exception) { return null; }
        }
    }
    public class svc_PhasePercentage : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (double)(byte)value / 256 * 100; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) {
            try { double _value = double.Parse((string)value, CultureInfo.InvariantCulture); return _value >= 0 && _value <= (100.0 - (100.0 / 256)) ? ((object)(byte)(_value / 100 * 256)) : null; }
            catch (Exception) { return null; }
        }
    }
}
