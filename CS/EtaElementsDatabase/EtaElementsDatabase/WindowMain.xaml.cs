using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Runtime.Remoting.Messaging;
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
using System.Windows.Threading;
using EtaElementsDatabase;

namespace EtaElementsDatabase
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class WindowMain
    {
        private CEtaElementsDatabase _elements_database;
        private DispatcherTimer _timer;

        public WindowMain() {
            InitializeComponent();
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e) {
            CElementItem.CPackage.XMLLoad();
            _elements_database = CEtaElementsDatabase.XMLLoad();
            TV_Containers.DataContext = new object[] { _elements_database };
            _timer = new DispatcherTimer(new TimeSpan(0, 0, 5), DispatcherPriority.ApplicationIdle, (sender2, args) => { if (_elements_database != null) { CElementItem.CPackage.XMLSaveTemp(); _elements_database.XMLSaveTemp(); } }, Dispatcher.CurrentDispatcher);
        }
        private void WindowMain_Closed(object sender, EventArgs e) {
            CElementItem.CPackage.XMLDeleteTemp(); _elements_database.XMLDeleteTemp();
            CElementItem.CPackage.XMLSave(); _elements_database.XMLSave();
        }

        private void LB_Items_OnMouseDoubleClick(object sender, MouseButtonEventArgs e) {
            CElementItem.CElementEntity _element_entity = LB_Items.SelectedItem as CElementItem.CElementEntity;
            CElementItem.CElementEntity.CName _element_entity_name = LB_Items.SelectedItem as CElementItem.CElementEntity.CName;
            if (_element_entity_name != null) _element_entity = _element_entity_name.ElementEntity;
            if (_element_entity != null) {
                WindowElementEntity _window_element_entity = new WindowElementEntity(_element_entity); _window_element_entity.Owner = this;
                _window_element_entity.ShowDialog();
            }
        }

        private void MI_File_Save_OnClick(object sender, RoutedEventArgs e) { CElementItem.CPackage.XMLSave(); _elements_database.XMLSave(); }
        private void MI_File_Exit_OnClick(object sender, RoutedEventArgs e) { Close(); }

        private void MI_Packages_OnClick(object sender, RoutedEventArgs e) {
            WindowPackages _window_packages = new WindowPackages(_elements_database); _window_packages.Owner = this;
            _window_packages.ShowDialog();
        }

        private bool AskToRemove(IEnumerable<string> items) {
            int _count = 0;
            StringBuilder _sb = new StringBuilder();
            foreach (string _item in items) {
                if (_count < 10) _sb.AppendLine(_item);
                else if (_count == 10) _sb.AppendLine(".....");
                _count++;
            }
            if (_count == 0) return false;
            if (_count == 1) _sb.Insert(0, "Do you wish to remove selected item:\n");
            else _sb.Insert(0, string.Format("Do you wish to remove {0} selected items:\n", _count));
            return MessageBox.Show(this, _sb.ToString(), "Confirm Removal", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes;
        }

        private void ElementEntityRemove_Distinct_OnClick(object sender, RoutedEventArgs e) { CElementItem.CElementEntity _element_entity = (CElementItem.CElementEntity)((Button)sender).DataContext; if (AskToRemove(new[] { _element_entity.NameTotal })) _element_entity.RemoveThis(); }
        private void ElementEntityRemove_ByName_OnClick(object sender, RoutedEventArgs e) { CElementItem.CElementEntity.CName _element_entity_name = (CElementItem.CElementEntity.CName)((Button)sender).DataContext; if (AskToRemove(new[] { _element_entity_name.Name })) _element_entity_name.RemoveThis(); }
        private void ElementEntityRemoveSelected_OnClick(object sender, RoutedEventArgs e) {
            if (MI_DistinctEntities.IsChecked) { CElementItem.CElementEntity[] _element_entities = LB_Items.SelectedItems.Cast<CElementItem.CElementEntity>().ToArray(); if (AskToRemove(_element_entities.Select(x => x.NameTotal))) foreach (CElementItem.CElementEntity _element_entity in _element_entities) { _element_entity.RemoveThis(); } }
            else { CElementItem.CElementEntity.CName[] _element_entity_names = LB_Items.SelectedItems.Cast<CElementItem.CElementEntity.CName>().ToArray(); if (AskToRemove(_element_entity_names.Select(x => x.Name)))foreach (CElementItem.CElementEntity.CName _name in _element_entity_names) { _name.RemoveThis(); } }
        }
        private void ElementEntityAdd(object sender, RoutedEventArgs e) {
            CElementItem.CElementEntity _element_entity = ((CElementItem)TV_Containers.SelectedItem).ElementEntityAdd();
            WindowElementEntity _window_element_entity = new WindowElementEntity(_element_entity); _window_element_entity.Owner = this;
            _window_element_entity.ShowDialog();
        }
    }

    public class svc_test : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return value; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_BooleanInvertor : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { bool _value = (bool)value; return !_value; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class mvc_Entities : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture) {
            try {
                CElement _element = (CElement)values[0]; bool _is_distinct = (bool)values[1];
                return _is_distinct ? (object)_element.EntitiesDistinct : (object)_element.EntitiesByName;
            }
            catch (Exception) { return null; }
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_CanAddNewEntity : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return value is CElementItem; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_CanRemoveSelectedEntities : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { return (int)value > 0; }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
}
