using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
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
using Microsoft.Win32;
using System.Management;

namespace EtaLoggingCharger
{
    /// <summary>
    /// Interaction logic for WindowMain.xaml
    /// </summary>
    public partial class WindowMain : Window
    {
        private EtaLoggingChargerControl _electro_bike_control = new EtaLoggingChargerControl();

        public EtaLoggingChargerControl ElectroBikeControl { get { return _electro_bike_control; } }

        public WindowMain() { InitializeComponent(); }
        private void Window_Closed(object sender, EventArgs e) { }

        private void ComPorts_OnDropDownOpened(object sender, EventArgs e) {
            ComboBox _combo_box = (ComboBox)sender;
            _combo_box.ItemsSource = EtaLoggingChargerControl.GetConnectionDevices;
        }

        private void ButtonReloadList_OnClick(object sender, RoutedEventArgs e) { Task.Run(() => _electro_bike_control.ReloadList()); }
        private void ButtonClearLogList_OnClick(object sender, RoutedEventArgs e) { Task.Run(() => _electro_bike_control.ClearList()); }
        private void LB_LogEntries_OnSelectionChanged(object sender, SelectionChangedEventArgs e) {
            IEnumerable<EtaLoggingChargerControl.CLogEntry> _selected_log_entries = LB_LogEntries.SelectedItems.OfType<EtaLoggingChargerControl.CLogEntry>();
            List<EtaCtlGraph.CGroup> _list_groups = new List<EtaCtlGraph.CGroup>(4);
            IEnumerable<EtaLoggingChargerControl.CLogEntry> _log_entries_discharge = _selected_log_entries.Where(log_entry => log_entry.IsDischarging);
            EtaCtlGraph.CGraph[] _graph_discharge_vit = _log_entries_discharge.Select(log_entry => log_entry.GraphvoltageInTime).ToArray();
            EtaCtlGraph.CGraph[] _graph_discharge_vmp = _log_entries_discharge.Select(log_entry => log_entry.GraphVoltageMidpoint).ToArray();
            IEnumerable<EtaLoggingChargerControl.CLogEntry> _log_entries_charge = _selected_log_entries.Where(log_entry => !log_entry.IsDischarging);
            EtaCtlGraph.CGraph[] _graph_charge_vit = _log_entries_charge.Select(log_entry => log_entry.GraphvoltageInTime).ToArray();
            EtaCtlGraph.CGraph[] _graph_charge_vmp = _log_entries_charge.Select(log_entry => log_entry.GraphVoltageMidpoint).ToArray();
            if (_graph_discharge_vit.Length > 0) _list_groups.Add(new EtaCtlGraph.CGroup(_graph_discharge_vit, "Discharge chart", EtaCtlGraph.CGroup.EDescriptionCorner.LeftBottom, new SolidColorBrush(Color.FromRgb(0x1C, 0x10, 0x10)), "Ah", "V"));
            if (_graph_discharge_vmp.Length > 0) _list_groups.Add(new EtaCtlGraph.CGroup(_graph_discharge_vmp, "Discharge voltage midpoint", EtaCtlGraph.CGroup.EDescriptionCorner.LeftTop, new SolidColorBrush(Color.FromRgb(0x1C, 0x10, 0x10)), "V", "e"));
            if (_graph_charge_vit.Length > 0) _list_groups.Add(new EtaCtlGraph.CGroup(_graph_charge_vit, "Charge chart", EtaCtlGraph.CGroup.EDescriptionCorner.RightBottom, new SolidColorBrush(Color.FromRgb(0x10, 0x1C, 0x10)), "Ah", "V"));
            if (_graph_charge_vmp.Length > 0) _list_groups.Add(new EtaCtlGraph.CGroup(_graph_charge_vmp, "Charge voltage midpoint", EtaCtlGraph.CGroup.EDescriptionCorner.LeftTop, new SolidColorBrush(Color.FromRgb(0x10, 0x1C, 0x10)), "V", "e"));
            GRF_LogEntries.Groups = _list_groups.ToArray();
        }

        private void ButtonDownload_OnClick(object sender, RoutedEventArgs e) {
            SaveFileDialog _sfd = new SaveFileDialog();
            _sfd.OverwritePrompt = true;
            _sfd.CheckPathExists = true;
            _sfd.DefaultExt = ".bin";
            if (_sfd.ShowDialog(this).Value) { _electro_bike_control.DownloadToFile(_sfd.FileName); }
        }

        private void ButtonApplyName_OnClick(object sender, RoutedEventArgs e) {
            Button _btn = sender as Button;
            EtaLoggingChargerControl.CLogEntry _log_entry = _btn?.DataContext as EtaLoggingChargerControl.CLogEntry;
            _log_entry?.ApplyName();
        }
    }

    public class svc_LogEntriesOptionedToLogEntriesFiltered : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture) {
            try {
                EtaLoggingChargerControl.CLogEntry[] _log_entries = (EtaLoggingChargerControl.CLogEntry[])value[0];
                bool _is_all = (bool)value[1], _is_discharge = (bool)value[2], _is_charge = (bool)value[3];
                if (_is_all) return _log_entries;
                else if (_is_discharge) return _log_entries.Where(log_entry => log_entry.IsDischarging).ToArray();
                else if (_is_charge) return _log_entries.Where(log_entry => !log_entry.IsDischarging).ToArray();
                else return null;
            }
            catch (Exception) { return null; }
        }
        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }

    public class svc_ChargeDischargeToColor : IValueConverter
    {
        private readonly SolidColorBrush _brush_discharge = new SolidColorBrush(Color.FromRgb(0xFF, 0xC0, 0xC0));
        private readonly SolidColorBrush _brush_charge = new SolidColorBrush(Color.FromRgb(0xC0, 0xFF, 0xC0));

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? _brush_discharge : _brush_charge; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_CellTypeToText : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return EtaLoggingChargerControl.CLogEntry.CellTypeToText((EtaLoggingChargerControl.CLogEntry.ECellType)value); }
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
