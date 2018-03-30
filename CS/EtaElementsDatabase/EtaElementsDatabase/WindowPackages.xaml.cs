using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
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
using Microsoft.Win32;

namespace EtaElementsDatabase
{
    /// <summary>
    /// Interaction logic for WindowPackages.xaml
    /// </summary>
    public partial class WindowPackages : Window
    {
        private readonly CEtaElementsDatabase _elements_database;

        public CEtaElementsDatabase ElementsDatabase { get { return _elements_database; } }

        public WindowPackages(CEtaElementsDatabase elements_database) {
            _elements_database = elements_database;
            InitializeComponent();
        }

        private void RemovePackage_OnClick(object sender, RoutedEventArgs e) {
            CElementItem.CPackage.PackageRemove(((CElementItem.CPackage)((Button)sender).DataContext).PackageName, _elements_database, list => {
                return null;
            });
        }
        private void AddPackage_OnClick(object sender, RoutedEventArgs e) { CElementItem.CPackage.PackageAdd(TB_NewPackageName.Text); }

        private void RemovePackageImage_OnClick(object sender, RoutedEventArgs e) {
            CElementItem.CPackage.CPackageImage _package_image = ((CElementItem.CPackage.CPackageImage)((Button)sender).DataContext);
            _package_image.Package.ImageRemove(_package_image);
        }
        private void AddPackageImage_OnClick(object sender, RoutedEventArgs e) {
            OpenFileDialog _ofd = new OpenFileDialog();
            _ofd.Filter = "JPEG Files|*.jpeg;*.jpg|PNG Files|*.png|BMP Files|*.bmp|All Files|*.*";
            _ofd.CheckFileExists = true;
            if (_ofd.ShowDialog() == true) {
                string _fullfn_source = _ofd.FileName.ToLower();
                string _filename = System.IO.Path.GetFileName(_fullfn_source);
                string _fullfn_destination = System.IO.Path.GetFullPath(System.IO.Path.Combine("Packages", _filename)).ToLower();
                if (_fullfn_destination != _fullfn_source) {
                    bool _b_overwrite = false;
                    while (true) {
                        try {
                            File.Copy(_fullfn_source, _fullfn_destination, _b_overwrite);
                            break;
                        }
                        catch (IOException ex) {
                            MessageBoxResult _mbr = MessageBox.Show(this, string.Format("{0}\n{1}", ex.Message, "Do you wish to overwrite existing file"), "Cannot copy image file to Packages directory", MessageBoxButton.OKCancel, MessageBoxImage.Error);
                            if (_mbr == MessageBoxResult.OK) _b_overwrite = true;
                            else { _filename = null; break; }
                        }
                    }
                }

                if (_filename != null) ((CElementItem.CPackage)((Button)sender).DataContext).ImageAdd(_filename);
            }
        }
    }

    public class svc_PackageCanDelete : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try { IList<CElementItem.CPackage> _packages = (IList<CElementItem.CPackage>)value; return _packages.Count > 1; }
            catch (Exception) { return false; }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
    public class svc_PackageCanAdd : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            try {
                string _new_name = ((string)value).ToUpper();
                return CElementItem.CPackage.Packages.All(x => x.PackageName != _new_name);
            }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type targetTypes, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
}
