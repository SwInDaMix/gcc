using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ACPowerMetter
{
    /// <summary>
    /// Interaction logic for WindowMain.xaml
    /// </summary>
    public partial class WindowMain : Window
    {
        private EtaACPowerMetterControl d_ac_power_metter_control = new EtaACPowerMetterControl();

        public EtaACPowerMetterControl ElectroBikeControl { get { return d_ac_power_metter_control; } }

        public WindowMain() { InitializeComponent(); }
        private void Window_Closed(object sender, EventArgs e) { d_ac_power_metter_control.Disconnect(); }

        private void ButtonConnection_OnClick(object sender, RoutedEventArgs e) {
            try { if (d_ac_power_metter_control.IsConnected) d_ac_power_metter_control.Disconnect(); else d_ac_power_metter_control.Connect(); }
            catch (Exception ex) { MessageBox.Show(ex.Message); }
        }
        private void ButtonSaveSettings_OnClick(object sender, RoutedEventArgs e) { }
        private void ButtonTest_OnClick(object sender, RoutedEventArgs e) { }

        private void TextBox_GotFocus(object sender, RoutedEventArgs e) { ((TextBox)sender).SelectAll(); }
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

        private void ButtonReadCalibration_OnClick(object sender, RoutedEventArgs e) { d_ac_power_metter_control.Request_ReadCalibration(); }
        private void ButtonWriteCalibration_OnClick(object sender, RoutedEventArgs e) { d_ac_power_metter_control.Request_WriteCalibration(); }
        private void ButtonSaveCalibration_OnClick(object sender, RoutedEventArgs e) { d_ac_power_metter_control.Request_SaveCalibration(); }
    }

    public class svc_IsConnectedToText : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? "Disconnect" : "Connect"; }
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
}
