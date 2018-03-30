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
    /// Interaction logic for WindowReceipt.xaml
    /// </summary>
    public partial class WindowReceipt : Window
    {
        public string ReceiptTitle { get; set; }
        public BitmapSource ImageReceipt { get; set; }
        public DateTime ReceiptDateTime { get; set; } = DateTime.Now;
        public double UsingMaxHeight => SystemParameters.PrimaryScreenHeight * .9d;

        public WindowReceipt(String receipt_title, BitmapSource image_receipt) {
            ReceiptTitle = receipt_title; ImageReceipt = image_receipt;
            InitializeComponent();
            scrl.Focus();
        }

        protected override void OnPreviewKeyDown(KeyEventArgs e) {
            base.OnPreviewKeyDown(e);
            if (e.Key == Key.Escape) Close();
        }

        private void ImageReceipt_OnMouseDoubleClick(object sender, MouseButtonEventArgs e) {
            SaveFileDialog save_file_dialog = new SaveFileDialog();
            save_file_dialog.DefaultExt = "png";
            save_file_dialog.FileName = $"receipt from {ReceiptTitle}";
            if (save_file_dialog.ShowDialog() == true) {
                using (var fileStream = new FileStream(save_file_dialog.FileName, FileMode.Create)) {
                    BitmapEncoder encoder = new PngBitmapEncoder();
                    encoder.Frames.Add(BitmapFrame.Create(ImageReceipt));
                    encoder.Save(fileStream);
                }
            }
        }
    }
}
