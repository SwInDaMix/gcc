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

namespace EtaSecurityGovernorManager
{
    /// <summary>
    /// Interaction logic for WindowMain.xaml
    /// </summary>
    public partial class WindowMain : Window
    {
        public EtaSecurityGovernorManagerControl GovernorManagerControl { get; } = new EtaSecurityGovernorManagerControl();

        public WindowMain() { InitializeComponent(); }
        private void Window_Closed(object sender, EventArgs e) { GovernorManagerControl.Disconnect(); }

        public async Task ConnectDisconnect() {
            if (GovernorManagerControl.IsConnected) GovernorManagerControl.Disconnect();
            else { GovernorManagerControl.Connect(); await GovernorManagerControl.ReloadAsync(); }
        }

        public async Task AccessorRegisterToggleActive(EtaSecurityGovernorManagerControl.CAccessorRegister accessor_register) { await accessor_register.ToggleActiveAsync(); }
        public async Task AccessorRegisterClobber(EtaSecurityGovernorManagerControl.CAccessorRegister accessor_register) { await accessor_register.AccessorRegisterRegistryOwner.AccessorRegisterClobberAsync(accessor_register.ID); }

        private void ButtonAllowRegistration_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.AllowRegistration(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to allow Registration", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestRFInfo_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestRFInfo(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test RF Info", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestEEPROMPagesWrite_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestEEPROMPagesWrite(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test EEPROM Pages Write", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestEEPROMPagesRead_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestEEPROMPagesRead(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test EEPROM Pages Read", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestEEPROMPagesErase_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestEEPROMPagesErase(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test EEPROM Pages Erase", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestGetADC_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestGetADC(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test Get ADC", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestSwitchSink_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestSwitchSink(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test Switch Sink", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestLock_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestLock(false); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test Lock", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestUnlock_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestUnlock(false); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test Unlock", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestLockForced_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestLock(true); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test Lock", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestUnlockForced_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestUnlock(true); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test Unlock", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonTestGetLockStatus_OnClick(object sender, RoutedEventArgs e) {
            try { GovernorManagerControl.TestGetLockStatus(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Test Get Lock Status", MessageBoxButton.OK, MessageBoxImage.Error); }
        }

        private async void ButtonAccessorRegisterToggleActive_OnClick(object sender, RoutedEventArgs e) {
            Button _btn = sender as Button;
            EtaSecurityGovernorManagerControl.CAccessorRegister _accessor_register = _btn?.DataContext as EtaSecurityGovernorManagerControl.CAccessorRegister;
            if (_accessor_register != null) {
                try { await AccessorRegisterToggleActive(_accessor_register); }
                catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Toggle Active state", MessageBoxButton.OK, MessageBoxImage.Error); }
            }
        }
        private async void ButtonAccessorRegisterClobber_OnClick(object sender, RoutedEventArgs e) {
            Button _btn = sender as Button;
            EtaSecurityGovernorManagerControl.CAccessorRegister _accessor_register = _btn?.DataContext as EtaSecurityGovernorManagerControl.CAccessorRegister;
            if (_accessor_register != null) {
                try { await AccessorRegisterClobber(_accessor_register); }
                catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Clobber", MessageBoxButton.OK, MessageBoxImage.Error); }
            }
        }

        private async void ButtonConnectDisconnect_OnClick(object sender, RoutedEventArgs e) {
            try { await ConnectDisconnect(); }
            catch (Exception ex) { MessageBox.Show(ex.Message); }
        }

        private void ComPorts_OnDropDownOpened(object sender, EventArgs e) {
            ComboBox _combo_box = (ComboBox)sender;
            _combo_box.ItemsSource = SerialPort.GetPortNames();
        }
        private void LB_AccessorRegisters_OnMouseDoubleClick(object sender, MouseButtonEventArgs e) {
            EtaSecurityGovernorManagerControl.CAccessorRegister _accessor_register = LB_AccessorRegisters.SelectedItem as EtaSecurityGovernorManagerControl.CAccessorRegister;
            if (_accessor_register != null) {
                WindowAccessorRegister _window_element_entity = new WindowAccessorRegister(GovernorManagerControl, _accessor_register) { Owner = this };
                _window_element_entity.ShowDialog();
            }
        }
    }

    public class svc_IsConnectedToText : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? "Disconnect" : "Connect"; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
}
