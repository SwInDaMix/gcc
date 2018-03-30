using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;

namespace EtaSecurityGovernorManager
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        
    }

    public class svc_BoolInverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return !(bool)value; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_BoolToVisible : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? Visibility.Visible : Visibility.Collapsed; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_BoolToVisibleInverted : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? Visibility.Collapsed : Visibility.Visible; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_EnumValueToEnumDescription : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try {
                Enum _enum_obj = (Enum)value;
                object[] _descriptions = _enum_obj.GetType().GetField(_enum_obj.ToString())?.GetCustomAttributes(typeof(DescriptionAttribute), false);
                return _descriptions == null || _descriptions.Length == 0 ? _enum_obj.ToString() : ((DescriptionAttribute)_descriptions[0]).Description;
            }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_IsActionToBrush : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try {
                EtaSecurityGovernorManagerControl.EAction _action = value as EtaSecurityGovernorManagerControl.EAction? ?? EtaSecurityGovernorManagerControl.EAction.None;
                return _action != EtaSecurityGovernorManagerControl.EAction.None ? Brushes.MediumVioletRed : Brushes.DarkGray;
            }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }

    public class svc_IsActiveToButtonActionText : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (bool)value ? "Deactivate" : "Activate"; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
}
