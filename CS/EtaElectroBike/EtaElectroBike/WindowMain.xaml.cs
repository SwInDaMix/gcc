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

namespace EtaElectroBike
{
    /// <summary>
    /// Interaction logic for WindowMain.xaml
    /// </summary>
    public partial class WindowMain : Window
    {
        private EtaElectroBikeControl _electro_bike_control = new EtaElectroBikeControl();

        public EtaElectroBikeControl ElectroBikeControl { get { return _electro_bike_control; } }

        public WindowMain() { InitializeComponent(); }
        private void Window_Closed(object sender, EventArgs e) { _electro_bike_control.Disconnect(); }

        private void ButtonConnection_OnClick(object sender, RoutedEventArgs e) {
            try {
                if (_electro_bike_control.IsConnected) _electro_bike_control.Disconnect(); else _electro_bike_control.Connect();
            }
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

        private void Button_TIM_GenerateEvent_Click(object sender, RoutedEventArgs e) { _electro_bike_control.PushFrame(new EtaConnectionFrames.CFrame((byte)EtaElectroBikeControl.EFrameCommand.TEST_COMMAND, new byte[] { 0 })); }
        private void Button_BLDC_ProcessStep_Click(object sender, RoutedEventArgs e) { _electro_bike_control.PushFrame(new EtaConnectionFrames.CFrame((byte)EtaElectroBikeControl.EFrameCommand.TEST_COMMAND, new byte[] { 1 })); }
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
