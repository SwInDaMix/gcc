using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Microsoft.Win32;

namespace EscPosEmulator
{
    /// <summary>
    /// Interaction logic for WindowErrorMessage.xaml
    /// </summary>
    public partial class WindowErrorMessage : Window
    {
        public MainWindow.Printer Printer { get; set; }
        public string ErrorMessage { get; set; }

        public WindowErrorMessage(MainWindow.Printer printer, string error_message) {
            Printer = printer; ErrorMessage = error_message;
            InitializeComponent();
        }

        protected override void OnPreviewKeyDown(KeyEventArgs e) {
            base.OnPreviewKeyDown(e);
            if (e.Key == Key.Escape) Close();
        }
    }
}
