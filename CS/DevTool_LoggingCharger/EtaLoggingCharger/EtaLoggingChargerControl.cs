using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics.Eventing.Reader;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;

namespace EtaLoggingCharger
{
    public class EtaLoggingChargerControl : INotifyPropertyChanged
    {
        internal CDevice d_connection_device;
        internal DispatcherTimer d_timer_settings_reload;

        internal bool d_is_ignore_last_sector_value;
        internal CLogEntry[] d_log_entries;

        public static CDevice[] GetConnectionDevices {
            get {
                List<CDevice> _array_list = SerialPort.GetPortNames().Select(_port_name => new CDeviceCOMPort(_port_name)).Cast<CDevice>().ToList();
                _array_list.AddRange(CDeviceDisk.GetPhysicalMedias().Where(disk => disk.Tag.StartsWith("\\\\.\\PHYSICALDRIVE")));
                _array_list.Add(new CDeviceFile());
                return _array_list.ToArray();
            }
        }

        public CDevice ConnectionDevice { get { return d_connection_device; } set { d_connection_device = value; OnPropertyChanged(nameof(ConnectionPort)); OnPropertyChanged(nameof(IsCanConnect)); OnPropertyChanged(nameof(IsConnected)); ReloadList(); } }
        public bool IsValid => d_log_entries != null;

        public bool IsIgnoreLastSectorValue { get { return d_is_ignore_last_sector_value; } set { d_is_ignore_last_sector_value = value; OnPropertyChanged(nameof(IsIgnoreLastSectorValue)); ReloadList(); } }
        public CLogEntry[] LogEntries => d_log_entries;

        public EtaLoggingChargerControl() {
            d_timer_settings_reload = new DispatcherTimer(new TimeSpan(0, 0, 0, 0, 500), DispatcherPriority.ApplicationIdle, (sender, e) => { }, Dispatcher.CurrentDispatcher);
        }

        public void ReloadList() {
            CDevice _device = d_connection_device;
            if (_device != null && _device.Connect()) {
                byte[] _bytes_sector_zero = _device.RequestSector(0);
                if (_bytes_sector_zero != null) {
                    if (BitConverter.ToUInt32(_bytes_sector_zero, 0) == 0x3CF18EDA) {
                        List<CLogEntry> _list_log_entries = new List<CLogEntry>();
                        uint _last_sector = BitConverter.ToUInt32(_bytes_sector_zero, 4);
                        uint _current_sector = 0;
                        while (_current_sector < _last_sector) {
                            uint _entry_start_sector = _current_sector + 1;
                            byte[] _bytes_sector_header = _device.RequestSector(_entry_start_sector); _current_sector++;
                            if (_bytes_sector_header == null) break;
                            uint _entry_signature = BitConverter.ToUInt32(_bytes_sector_header, 0);
                            uint _entries_count = BitConverter.ToUInt32(_bytes_sector_header, 4);
                            CLogEntry.ECellType _cell_type = (CLogEntry.ECellType)_bytes_sector_header[8];
                            bool _is_discharging = BitConverter.ToBoolean(_bytes_sector_header, 9);
                            Encoding _enc = Encoding.ASCII;
                            string _name = _enc.GetString(_bytes_sector_header, 32, Array.IndexOf<byte>(_bytes_sector_header, 0, 32, 32) - 32);
                            byte[] _bytes_sector = null; int _offset = 0;

                            CLogEntry.CSample[] _samples = new CLogEntry.CSample[_entries_count]; int _sample_idx = 0;
                            while (_entries_count > 0) {
                                double _cell_current, _cell_voltage, _cell_sensed_voltage;
                                if (_entry_signature == 0xD236FAC4) {
                                    // old 24 bit voltage value version
                                    int _v_cell_current = _ReloadList_Get24(_device, ref _current_sector, ref _bytes_sector, ref _offset); if (_v_cell_current == -1) break;
                                    int _v_cell_voltage = _ReloadList_Get24(_device, ref _current_sector, ref _bytes_sector, ref _offset); if (_v_cell_voltage == -1) break;
                                    int _v_cell_sensed_voltage = _ReloadList_Get24(_device, ref _current_sector, ref _bytes_sector, ref _offset); if (_v_cell_sensed_voltage == -1) break;
                                    _cell_current = (double)_v_cell_current / 10000;
                                    _cell_voltage = (double)_v_cell_voltage / 10000;
                                    _cell_sensed_voltage = (double)_v_cell_sensed_voltage / 10000;
                                }
                                else if (_entry_signature == 0x2C483AC7) {
                                    // new 16 bit voltage value version
                                    int _v_cell_current = _ReloadList_Get24(_device, ref _current_sector, ref _bytes_sector, ref _offset); if (_v_cell_current == -1) break;
                                    int _v_cell_voltage = _ReloadList_Get16(_device, ref _current_sector, ref _bytes_sector, ref _offset); if (_v_cell_voltage == -1) break;
                                    int _v_cell_sensed_voltage = _ReloadList_Get16(_device, ref _current_sector, ref _bytes_sector, ref _offset); if (_v_cell_sensed_voltage == -1) break;
                                    _cell_current = (double)_v_cell_current / 10000;
                                    _cell_voltage = (double)_v_cell_voltage / 1000;
                                    _cell_sensed_voltage = (double)_v_cell_sensed_voltage / 1000;
                                }
                                else break;
                                _samples[_sample_idx++] = new CLogEntry.CSample(_cell_current, _cell_voltage, _cell_sensed_voltage);
                                _entries_count--;
                            }
                            if (_entries_count > 0) break;
                            _list_log_entries.Add(new CLogEntry(this, _device, _entry_start_sector, _name, _samples, _cell_type, _is_discharging));
                        }
                        d_log_entries = _list_log_entries.ToArray();
                        OnPropertyChanged(nameof(LogEntries)); OnPropertyChanged(nameof(IsValid));
                    }
                }
                _device.Disconnect();
            }
        }
        public void ClearList() { }
        public void DownloadToFile(string filename) {
            CDevice _device = d_connection_device;
            if (_device != null && _device.Connect()) {
                using (FileStream _fs = new FileStream(filename, FileMode.Create)) {
                    byte[] _bytes_sector_zero = _device.RequestSector(0);
                    if (_bytes_sector_zero != null) {
                        if (BitConverter.ToUInt32(_bytes_sector_zero, 0) == 0x3CF18EDA) {
                            _fs.Write(_bytes_sector_zero, 0, 512);
                            uint _last_sector = BitConverter.ToUInt32(_bytes_sector_zero, 4);
                            uint _current_sector = 0;
                            while (_current_sector < _last_sector) {
                                byte[] _bytes_sector = _device.RequestSector(_current_sector + 1); _current_sector++;
                                if (_bytes_sector != null) _fs.Write(_bytes_sector, 0, 512);
                            }
                        }
                    }
                }
            }
        }

        private int _ReloadList_Get24(CDevice device, ref uint current_sector, ref byte[] bytes_sector, ref int offset) {
            int _v1 = _ReloadList_Get8(device, ref current_sector, ref bytes_sector, ref offset); if (_v1 == -1) return -1;
            int _v2 = _ReloadList_Get8(device, ref current_sector, ref bytes_sector, ref offset); if (_v2 == -1) return -1;
            int _v3 = _ReloadList_Get8(device, ref current_sector, ref bytes_sector, ref offset); if (_v3 == -1) return -1;
            return (_v3 << 16) | (_v2 << 8) | _v1;
        }
        private int _ReloadList_Get16(CDevice device, ref uint current_sector, ref byte[] bytes_sector, ref int offset) {
            int _v1 = _ReloadList_Get8(device, ref current_sector, ref bytes_sector, ref offset); if (_v1 == -1) return -1;
            int _v2 = _ReloadList_Get8(device, ref current_sector, ref bytes_sector, ref offset); if (_v2 == -1) return -1;
            return (_v2 << 8) | _v1;
        }
        private int _ReloadList_Get8(CDevice device, ref uint current_sector, ref byte[] bytes_sector, ref int offset) {
            if (offset == 0) {
                bytes_sector = device.RequestSector(current_sector + 1); current_sector++;
                if (bytes_sector == null) return -1;
            }
            byte _byte = bytes_sector[offset++];
            if (offset >= 512) offset = 0;
            return _byte;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; _handler?.Invoke(this, new PropertyChangedEventArgs(propery_name)); }

        public class CLogEntry : INotifyPropertyChanged
        {
            private bool d_is_display_on_graph;
            private EtaCtlGraph.CGraph d_graph_voltage_in_time;
            private EtaCtlGraph.CGraph d_graph_voltage_midpoint;
            private string d_name;

            public EtaLoggingChargerControl LoggingChargerControl { get; }
            public CDevice Device { get; }
            public uint StartSector { get; }
            public CSample[] Samples { get; }
            public string Name { get { return d_name; } set { if (string.IsNullOrWhiteSpace(value)) d_name = null; else { d_name = value.Trim(); if (d_name.Length > 32) d_name = d_name.Substring(0, 32); } OnPropertyChanged(nameof(Name)); } }
            public ECellType CellType { get; }
            public bool IsDischarging { get; }
            public TimeSpan ProcessingTime { get; }
            public double AvarageCurrent { get; }
            public double MidpointVoltage { get; }
            public double AmpHour { get; }
            public double Wattage { get; }
            public bool IsDisplayOnGraph { get { return d_is_display_on_graph; } set { d_is_display_on_graph = value; OnPropertyChanged(nameof(IsDisplayOnGraph)); } }
            public string Description => $"{CellTypeToText(CellType)} \"{Name}\" - Avarage Current: {AvarageCurrent:F3} A, Midpoint Voltage: {MidpointVoltage:F3}, Wattage: {Wattage:F3}";

            public EtaCtlGraph.CGraph GraphvoltageInTime { get { if (d_graph_voltage_in_time == null) { d_graph_voltage_in_time = new EtaCtlGraph.CGraphTimeSpan(Samples.Select(sample => sample.CellVoltage), Description, 0, Samples.Length / AmpHour); } return d_graph_voltage_in_time; } }
            public EtaCtlGraph.CGraph GraphVoltageMidpoint {
                get {
                    if (d_graph_voltage_midpoint == null) {
                        EtaCtlGraph.CGraph _graph_voltage_in_time = GraphvoltageInTime;
                        int __cnt = 128;
                        double[] _entries = new double[__cnt]; double _diff = _graph_voltage_in_time.MaxYValue - _graph_voltage_in_time.MinYValue;
                        foreach (CSample _sample in Samples) {
                            double _dot = (_sample.CellVoltage - _graph_voltage_in_time.MinYValue) / _diff * (__cnt - 1);
                            double _idx = Math.Floor(_dot), _fract = _dot - _idx; int _int_idx = (int)_idx;
                            _entries[_int_idx] += (1.0 - _fract); if (_fract > 0) _entries[_int_idx + 1] += _fract;
                        }
                        d_graph_voltage_midpoint = new EtaCtlGraph.CGraphTimeSpan(_entries, Description, _graph_voltage_in_time.MinYValue, __cnt / _diff);
                    }
                    return d_graph_voltage_midpoint;
                }
            }

            public CLogEntry(EtaLoggingChargerControl logging_charger_control, CDevice device, uint start_sector, string name, CSample[] samples, ECellType cell_type, bool is_discharging) {
                LoggingChargerControl = logging_charger_control; Device = device; StartSector = start_sector; Name = name; Samples = samples; IsDischarging = is_discharging; CellType = cell_type;
                ProcessingTime = new TimeSpan(0, 0, Samples.Length / 2);
                double _avarage_current = 0, _midpoint_voltage = 0, _amp_hour = 0, _watt = 0;
                foreach (CSample _sample in Samples) {
                    _amp_hour += _sample.CellCurrent;
                    _midpoint_voltage += _sample.CellVoltage;
                    _watt += (_sample.CellVoltage * _sample.CellCurrent);
                }
                AvarageCurrent = _amp_hour / Samples.Length;
                MidpointVoltage = _midpoint_voltage / Samples.Length;
                AmpHour = _amp_hour / 7200;
                Wattage = _watt / 7200;
            }

            public void ApplyName() {
                if (Device.Connect()) {
                    byte[] _bytes_sector_header = Device.RequestSector(StartSector);
                    if (_bytes_sector_header != null) {
                        if (d_name != null) { byte[] _bytes_name = Encoding.ASCII.GetBytes(d_name); _bytes_name.CopyTo(_bytes_sector_header, 32); _bytes_sector_header[32 + _bytes_name.Length] = 0; }
                        else _bytes_sector_header[32] = 0;
                        Device.WriteSector(StartSector, _bytes_sector_header);
                    }
                    Device.Disconnect();
                    d_graph_voltage_in_time = d_graph_voltage_midpoint = null;
                    LoggingChargerControl.OnPropertyChanged(nameof(LogEntries));
                }
            }

            public event PropertyChangedEventHandler PropertyChanged;

            protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; _handler?.Invoke(this, new PropertyChangedEventArgs(propery_name)); }

            public static string CellTypeToText(ECellType cell_type) {
                if (cell_type == ECellType.LiFePo4) return "LiFePo4";
                if (cell_type == ECellType.LiFePo4_Full) return "LiFePo4 Full";
                if (cell_type == ECellType.LiIon) return "LiIon";
                if (cell_type == ECellType.LiPo) return "LiPo";
                if (cell_type == ECellType.PbAcid3) return "Pb Acid 3 cells";
                if (cell_type == ECellType.PbAcid6) return "Pb Acid 6 cells";
                return "<unknown>";
            }

            public enum ECellType
            {
                LiFePo4,
                LiFePo4_Full,
                LiIon,
                LiPo,
                PbAcid3,
                PbAcid6,
            }

            public class CSample
            {
                public double CellCurrent { get; }
                public double CellVoltage { get; }
                public double CellSensedVoltage { get; }

                public CSample(double cell_current, double cell_voltage, double cell_sensed_voltage) { CellCurrent = cell_current; CellVoltage = cell_voltage; CellSensedVoltage = cell_sensed_voltage; }
            }
        }

        public abstract class CDevice
        {
            public abstract bool Connect();
            public abstract bool Disconnect();
            public abstract byte[] RequestSector(uint sector);
            public abstract bool WriteSector(uint sector, byte[] bytes_sector);

            ~CDevice() { Disconnect(); }
        }
        public class CDeviceCOMPort : CDevice
        {
            private EtaConnectionFrames d_connection_frames;

            public string COMPort { get; }

            public CDeviceCOMPort(string comport) { COMPort = comport; }

            public override bool Connect() { Disconnect(); d_connection_frames = new EtaConnectionFrames(() => { d_connection_frames = null; }, COMPort, 125000, _AsyncFrameProcessor); return true; }
            public override bool Disconnect() { EtaConnectionFrames _connection_frames_temp = d_connection_frames; _connection_frames_temp?.Terminate(); return true; }
            public override byte[] RequestSector(uint sector) { throw new NotImplementedException(); }
            public override bool WriteSector(uint sector, byte[] bytes_sector) { throw new NotImplementedException(); }

            public void PushFrame(EFrameCommand frame_command, byte[] frame_data) { EtaConnectionFrames _connection_frames_temp = d_connection_frames; _connection_frames_temp?.PushAsyncFrame((byte)frame_command, frame_data); }
            public byte[] RequestFrame(EFrameCommand frame_command, byte[] frame_data, int timeout_ms) { EtaConnectionFrames _connection_frames_temp = d_connection_frames; return _connection_frames_temp?.RequestFrame((byte)frame_command, frame_data, timeout_ms); }

            private void _AsyncFrameProcessor(byte frame_command, byte[] frame_data) {
                EFrameCommand _command = (EFrameCommand)frame_command; int _frame_data_length = frame_data.Length, _offset = 0;
                //Console.WriteLine(BitConverter.ToString(_bytes_buffer));
                if (_command == EFrameCommand.DebugString) { Console.Write(Encoding.GetEncoding(1251).GetString(frame_data)); }
            }

            public override string ToString() { return COMPort; }
        }
        public class CDeviceFile : CDevice
        {
            private string d_filename;
            private FileStream d_fs;

            public string Filename {
                get {
                    if (d_filename == null) {
                        OpenFileDialog _ofd = new OpenFileDialog();
                        _ofd.CheckFileExists = true;
                        if (_ofd.ShowDialog().Value) { d_filename = _ofd.FileName; }
                    }
                    return d_filename;
                }
            }

            public CDeviceFile() { }
            public CDeviceFile(string filename) { d_filename = filename; }

            public override bool Connect() {
                string _filename = Filename;
                if (_filename != null) {
                    try {
                        d_fs = new FileStream(_filename, FileMode.Open);
                        return true;
                    }
                    catch (Exception) { }
                }
                return false;
            }
            public override bool Disconnect() {
                if (d_fs != null) { d_fs.Close(); d_fs.Dispose(); d_fs = null; }
                return true;
            }
            public override byte[] RequestSector(uint sector) {
                d_fs.Seek(sector * 512, SeekOrigin.Begin);
                byte[] _bytes_sector = new byte[512];
                d_fs.Read(_bytes_sector, 0, 512);
                return _bytes_sector;
            }
            public override bool WriteSector(uint sector, byte[] bytes_sector) {
                d_fs.Seek(sector * 512, SeekOrigin.Begin);
                d_fs.Write(bytes_sector, 0, 512);
                return true;
            }

            public override string ToString() { return d_filename != null ? $"{d_filename}" : "<from file> "; }
        }
        public class CDeviceDisk : CDevice
        {
            const uint __GENERIC_WRITE = 0x40000000;
            const uint __GENERIC_READ = 0x80000000;
            const uint __OPEN_EXISTING = 3;

            private SafeFileHandle d_handle_value;

            public string Caption { get; }
            public string Description { get; }
            public DateTime? InstallDate { get; }
            public string Name { get; }
            public string Status { get; }
            public string CreationClassName { get; }
            public string Manufacturer { get; }
            public string Model { get; }
            public string SKU { get; }
            public string SerialNumber { get; }
            public string Tag { get; }
            public string Version { get; }
            public string PartNumber { get; }
            public string OtherIdentifyingInfo { get; }
            public bool? PoweredOn { get; }
            public bool? Removable { get; }
            public bool? Replaceable { get; }
            public bool? HotSwappable { get; }
            public ulong? Capacity { get; }
            public ushort? MediaType { get; }
            public string MediaDescription { get; }
            public bool? WriteProtectOn { get; }
            public bool? CleanerMedia { get; }

            public static IEnumerable<CDeviceDisk> GetPhysicalMedias() { ManagementObjectSearcher _searcher = new ManagementObjectSearcher("SELECT * FROM Win32_PhysicalMedia"); return _searcher.Get().OfType<ManagementObject>().Select(obj => new CDeviceDisk(obj)); }

            private CDeviceDisk(ManagementObject obj) {
                Caption = (string)obj["Caption"];
                Description = (string)obj["Description"];
                InstallDate = (DateTime?)obj["InstallDate"];
                Name = (string)obj["Name"];
                Status = (string)obj["Status"];
                CreationClassName = (string)obj["CreationClassName"];
                Manufacturer = (string)obj["Manufacturer"];
                Model = (string)obj["Model"];
                SKU = (string)obj["SKU"];
                SerialNumber = (string)obj["SerialNumber"];
                Tag = (string)obj["Tag"];
                Version = (string)obj["Version"];
                PartNumber = (string)obj["PartNumber"];
                OtherIdentifyingInfo = (string)obj["OtherIdentifyingInfo"];
                PoweredOn = (bool?)obj["PoweredOn"];
                Removable = (bool?)obj["Removable"];
                Replaceable = (bool?)obj["Replaceable"];
                HotSwappable = (bool?)obj["HotSwappable"];
                Capacity = (ulong?)obj["Capacity"];
                MediaType = (ushort?)obj["MediaType"];
                MediaDescription = (string)obj["MediaDescription"];
                WriteProtectOn = (bool?)obj["WriteProtectOn"];
                CleanerMedia = (bool?)obj["CleanerMedia"];
            }

            public override bool Connect() {
                d_handle_value = CreateFile(Tag, __GENERIC_READ | __GENERIC_WRITE, 0, IntPtr.Zero, __OPEN_EXISTING, 0, IntPtr.Zero);
                return !d_handle_value.IsInvalid;
            }
            public override bool Disconnect() {
                if (d_handle_value != null && !d_handle_value.IsInvalid && !d_handle_value.IsClosed) {
                    d_handle_value.Close(); d_handle_value.Dispose(); d_handle_value = null;
                    return true;
                }
                return false;
            }
            public override byte[] RequestSector(uint sector) {
                byte[] _bytes_buf = new byte[512];
                int _read, _move_to_high;
                SetFilePointer(d_handle_value, (int)(sector * 512), out _move_to_high, EMoveMethod.Begin);
                ReadFile(d_handle_value, _bytes_buf, 512, out _read, IntPtr.Zero);
                return _bytes_buf;
            }

            public override bool WriteSector(uint sector, byte[] bytes_sector) {
                int _read, _move_to_high;
                SetFilePointer(d_handle_value, (int)(sector * 512), out _move_to_high, EMoveMethod.Begin);
                WriteFile(d_handle_value, bytes_sector, 512, out _read, IntPtr.Zero);
                return true;
            }

            public override string ToString() { return $"{Tag} - {SerialNumber}"; }
        }

        public enum EFrameCommand
        {
            DebugString,
            ValuesUpdated,

            ReadValues = 0x80,
            WaveformRequest,
            WaveformPage,
            ReadCalibration,
            WriteCalibration,
            SaveCalibration,
            Reset
        }

        public enum EMoveMethod : uint
        {
            Begin = 0,
            Current = 1,
            End = 2
        }

        [DllImport("Kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        static extern uint SetFilePointer([In] SafeFileHandle hFile, [In] int lDistanceToMove, [Out] out int lpDistanceToMoveHigh, [In] EMoveMethod dwMoveMethod);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern SafeFileHandle CreateFile(string lpFileName, uint dwDesiredAccess, uint dwShareMode, IntPtr lpSecurityAttributes, uint dwCreationDisposition, uint dwFlagsAndAttributes, IntPtr hTemplateFile);

        [DllImport("kernel32", SetLastError = true)]
        internal extern static int ReadFile(SafeFileHandle handle, byte[] bytes, int numBytesToRead, out int numBytesRead, IntPtr overlapped_MustBeZero);

        [DllImport("kernel32", SetLastError = true)]
        internal extern static int WriteFile(SafeFileHandle handle, byte[] bytes, int numBytesToRead, out int numBytesRead, IntPtr overlapped_MustBeZero);
    }
}
