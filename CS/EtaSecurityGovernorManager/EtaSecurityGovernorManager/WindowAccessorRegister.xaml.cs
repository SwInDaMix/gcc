using System;
using System.Collections.Generic;
using System.Globalization;
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
using System.Windows.Shapes;

namespace EtaSecurityGovernorManager
{
    /// <summary>
    /// Interaction logic for WindowAccessorRegister.xaml
    /// </summary>
    public partial class WindowAccessorRegister : Window
    {
        public EtaSecurityGovernorManagerControl GovernorManagerControl { get; }
        public EtaSecurityGovernorManagerControl.CAccessorRegister AccessorRegister { get; }

        public WindowAccessorRegister(EtaSecurityGovernorManagerControl governor_manager_control, EtaSecurityGovernorManagerControl.CAccessorRegister accessor_register) {
            GovernorManagerControl = governor_manager_control; AccessorRegister = accessor_register;
            InitializeComponent();
        }

        public async Task Apply() { await AccessorRegister.ApplyAsync(); }
        public async Task ToggleActive() { await AccessorRegister.ToggleActiveAsync(); }
        public void Restore() { AccessorRegister.Restore(); }
        public void AllowRegistration() { AccessorRegister.AllowRegistration(); }

        private async void ButtonApply_OnClick(object sender, RoutedEventArgs e) {
            try { await Apply(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Apply", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private async void ButtonOK_OnClick(object sender, RoutedEventArgs e) {
            try { await Apply(); Close(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Apply", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private async void ButtonToggleActive_OnClick(object sender, RoutedEventArgs e) {
            try { await ToggleActive(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Toggle Active state", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonRestore_OnClick(object sender, RoutedEventArgs e) {
            try { AccessorRegister.Restore(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Restore", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void ButtonAllowRegistration_OnClick(object sender, RoutedEventArgs e) {
            try { AllowRegistration(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "Failed to Allow Registration", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
    }
}
