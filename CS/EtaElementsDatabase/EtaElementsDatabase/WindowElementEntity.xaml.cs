using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
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

namespace EtaElementsDatabase
{
    /// <summary>
    /// Interaction logic for WindowElementEntity.xaml
    /// </summary>
    public partial class WindowElementEntity
    {
        private readonly CElementItem.CElementEntity _element_entity;

        public CElementItem.CElementEntity ElementEntity { get { return _element_entity; } }

        public WindowElementEntity(CElementItem.CElementEntity element_entity) {
            _element_entity = element_entity;
            InitializeComponent();
        }

        private void RemoveName_OnClick(object sender, RoutedEventArgs e) { _element_entity.NameRemove((string)((Button)sender).Tag); }
        private void AddName_OnClick(object sender, RoutedEventArgs e) { _element_entity.NameAdd(TB_NewName.Text); }

        private void AddOrRemoveParameter_OnClick(object sender, RoutedEventArgs e) {
            DependencyObject _dependency_object = ((Button)sender).Parent;
            CElementItem.CParameter _parameter = (CElementItem.CParameter)_dependency_object.GetValue(DataContextProperty);
            CElementItem.CParameter.CParameterEntity _parameter_entity = (CElementItem.CParameter.CParameterEntity)_dependency_object.GetValue(TagProperty);
            if (_parameter_entity != null) _element_entity.ParameterRemove(_parameter.ParameterName);
            else _element_entity.ParameterGet(_parameter.ParameterName, true, null);
        }

        private void RemovePackage_OnClick(object sender, RoutedEventArgs e) { _element_entity.PackageRemove((string)((Button)sender).Tag); }

        private void AddPackage_OnClick(object sender, RoutedEventArgs e) { _element_entity.PackageGet(((CElementItem.CPackage)CB_AvailablePackages.SelectedItem).PackageName, true); }

        private void RemoveDatasheet_OnClick(object sender, RoutedEventArgs e) { _element_entity.DatasheetRemove((CElementItem.CElementEntity.CDatasheet)((Button)sender).DataContext); }
        private void ViewDatasheet_OnClick(object sender, RoutedEventArgs e) {
            try {
                Process _process = new Process();
                _process.StartInfo.FileName = ((CElementItem.CElementEntity.CDatasheet)((Button)sender).DataContext).Filename;
                _process.Start();
            }
            catch (Exception ex) { MessageBox.Show(this, string.Format("Can't view datasheet: {0}", ex.Message), "Error", MessageBoxButton.OK, MessageBoxImage.Error); }
        }
        private void AddDatasheet_OnClick(object sender, RoutedEventArgs e) { _element_entity.DatasheetAdd(TB_DatasheetFilename.Text, 0); }
    }

    public class mvc_ElementNameCanAdd : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture) {
            try {
                IList<CElementItem.CElementEntity.CName> _list_names = (IList<CElementItem.CElementEntity.CName>)value[0]; string _new_name = ((string)value[1]).ToUpper();
                return _list_names.All(x => x.Name != _new_name);
            }
            catch (Exception) { return null; }
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class mvc_ParameterEntities : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture) {
            try {
                IEnumerable<CElementItem.CParameter> _parameters = (IEnumerable<CElementItem.CParameter>)value[0]; IList<CElementItem.CParameter.CParameterEntity> _list_parameter_entities = (IList<CElementItem.CParameter.CParameterEntity>)value[1];
                return _parameters.ToList();
            }
            catch (Exception) { return null; }
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class mvc_ParameterEntity : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture) {
            try {
                CElementItem.CElementEntity _element_entity = (CElementItem.CElementEntity)value[0]; string _parameter_name = (string)value[1];
                CElementItem.CParameter.CParameterEntity _parameter_entity = _element_entity.ParameterGet(_parameter_name, false, null);
                return _parameter_entity;
            }
            catch (Exception) { return null; }
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_ParameterEntityTextBoxVisibility : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { CElementItem.CParameter.CParameterEntity _parameter_entity = (CElementItem.CParameter.CParameterEntity)value; return _parameter_entity != null && (_parameter_entity is CElementItem.CParameterIntegral.CParameterEntityIntegral || _parameter_entity is CElementItem.CParameterString.CParameterEntityString) ? Visibility.Visible : Visibility.Collapsed; }
            catch (Exception) { return Visibility.Collapsed; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_ParameterEntityComboBoxVisibility : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { CElementItem.CParameter.CParameterEntity _parameter_entity = (CElementItem.CParameter.CParameterEntity)value; return _parameter_entity != null && (_parameter_entity is CElementItem.CParameterEnum.CParameterEntityEnum) ? Visibility.Visible : Visibility.Collapsed; }
            catch (Exception) { return Visibility.Collapsed; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_ParameterEntityComboBoxIsEditable : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { string _regular_expression_for_other = (string)value; return _regular_expression_for_other != null; }
            catch (Exception) { return false; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_ParameterEntityToButtonText : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { CElementItem.CParameter.CParameterEntity _parameter_entity = (CElementItem.CParameter.CParameterEntity)value; return new Image { Source = (_parameter_entity != null ? (BitmapImage)App.Current.FindResource("png_Delete") : (BitmapImage)App.Current.FindResource("png_Add")) }; }
            catch (Exception) { return false; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class mvc_ElementAvailablePackages : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture) {
            try {
                IList<CElementItem.CPackage> _list_packages = (IList<CElementItem.CPackage>)value[0]; IList<CElementItem.CPackage.CPackageEntity> _list_available_packages = (IList<CElementItem.CPackage.CPackageEntity>)value[1];
                return _list_packages.Except(_list_available_packages.Select(x => x.Package));
            }
            catch (Exception) { return null; }
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_ElementPackageAddVisibility : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { IEnumerable<CElementItem.CPackage> _available_packages = (IEnumerable<CElementItem.CPackage>)value; return _available_packages.Any() ? Visibility.Visible : Visibility.Collapsed; }
            catch (Exception) { return Visibility.Collapsed; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_ElementPackageCanAdd : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { CElementItem.CPackage _packages = (CElementItem.CPackage)value; return _packages != null; }
            catch (Exception) { return false; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class mvc_PlacementRowColumn : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture) {
            try {
                uint _row = (uint)value[0]; uint _column = (uint)value[1];
                return string.Format("{0}:{1}", _row, _column);
            }
            catch (Exception) { return null; }
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) {
            try {
                string _row_column_string = (string)value;
                string[] _splitted_strings = _row_column_string.Split(new[] { ':' }, StringSplitOptions.RemoveEmptyEntries);
                if (_splitted_strings.Length != 2) throw new ArgumentException("Only row and column value should be in source string", "value");
                uint _row = uint.Parse(_splitted_strings[0]), _column = uint.Parse(_splitted_strings[1]);
                return new[] { (object)_row, (object)_column };
            }
            catch (Exception) { return null; }
        }
    }
}
