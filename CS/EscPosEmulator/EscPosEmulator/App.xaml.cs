using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace EscPosEmulator
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
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
}
