using System;
using System.ComponentModel;
using System.Text;
using System.Windows.Threading;

namespace ACPowerMetter
{
    public class EtaACPowerMetterControl : INotifyPropertyChanged
    {
        public const double __SIN45 = 0.70710678118654752440084436210485;

        public const uint __ACPM_ADE_F_CPU = 3579545;
        public const uint __ADE7753_FULLSACLE_I_SAMPLING = 0x2851EC;
        public const uint __ADE7753_FULLSACLE_V_SAMPLING = 0x2852;
        public const uint __ADE7753_FULLSACLE_IRMS = 0x1C82B3; // 1868467
        public const uint __ADE7753_FULLSACLE_VRMS = 0x17D338; // 1561400

        public const uint __ACPM_CURRENT_GAIN = 2;
        public const uint __ACPM_CURRENT_RATIO = 600;
        public const uint __ACPM_CURRENT_RESISTOR = 5;

        public const uint __ACPM_VOLTAGE_RATIO = 1000;

        public const double __ACPM_I_INPUT = 500D / __ACPM_CURRENT_GAIN / __ACPM_CURRENT_RESISTOR * __ACPM_CURRENT_RATIO;
        public const double __ACPM_V_INPUT = 500D * __ACPM_VOLTAGE_RATIO / 1000;
        public const double __ACPM_ADE_I_DIVIDOR = __ADE7753_FULLSACLE_I_SAMPLING / __ACPM_I_INPUT;
        public const double __ACPM_ADE_V_DIVIDOR = __ADE7753_FULLSACLE_V_SAMPLING / __ACPM_V_INPUT;
        public const double __ACPM_ADE_IRMS_DIVIDOR = __ADE7753_FULLSACLE_IRMS / __ACPM_I_INPUT / __SIN45;
        public const double __ACPM_ADE_VRMS_DIVIDOR = __ADE7753_FULLSACLE_VRMS / __ACPM_V_INPUT / __SIN45;

        public const uint __ACPM_ADE_PERIOD_NOMINATOR = __ACPM_ADE_F_CPU >> 3;

        public const int __ACPM_MAX_WAVEFORM_SAMPLES_PAGES_SHIFT = 2;
        public const int __ACPM_MAX_WAVEFORM_SAMPLES_PAGES = (1 << __ACPM_MAX_WAVEFORM_SAMPLES_PAGES_SHIFT);
        public const int __ACPM_MAX_WAVEFORM_SAMPLES_SHIFT = 9;
        public const int __ACPM_MAX_WAVEFORM_SAMPLES = (1 << __ACPM_MAX_WAVEFORM_SAMPLES_SHIFT);
        public const int __ACPM_MAX_WAVEFORM_SAMPLES_PART_SHIFT = (__ACPM_MAX_WAVEFORM_SAMPLES_SHIFT - __ACPM_MAX_WAVEFORM_SAMPLES_PAGES_SHIFT);
        public const int __ACPM_MAX_WAVEFORM_SAMPLES_PART = (1 << __ACPM_MAX_WAVEFORM_SAMPLES_PART_SHIFT);

        internal string d_connection_port;
        internal int d_connection_port_baudrate;
        internal DispatcherTimer d_timer_settings_reload;
        internal EtaConnectionFrames d_connection_frames;

        internal int d_ADERegLAENERGY;
        internal uint d_ADERegLVAENERGY;
        internal int d_ADERegLVARENERGY;
        internal byte d_ADERegCH1OS;
        internal byte d_ADERegCH2OS;
        internal byte d_ADERegPHCAL;
        internal short d_ADERegAPOS;
        internal short d_ADERegWGAIN;
        internal byte d_ADERegWDIV;
        internal ushort d_ADERegCFNUM;
        internal ushort d_ADERegCFDEN;
        internal uint d_ADERegIRMS;
        internal uint d_ADERegVRMS;
        internal short d_ADERegIRMSOS;
        internal short d_ADERegVRMSOS;
        internal short d_ADERegVAGAIN;
        internal byte d_ADERegVADIV;
        internal byte d_ADERegSAGLVL;
        internal byte d_ADERegIPKLVL;
        internal byte d_ADERegVPKLVL;
        internal uint d_ADERegIPEAK;
        internal uint d_ADERegVPEAK;
        internal sbyte d_ADERegTEMP;
        internal ushort d_ADERegPERIOD;

        internal TimeSpan d_ts_working_time;
        internal double d_energy_active;
        internal double d_energy_apparent;
        internal double d_line_power_active;
        internal double d_line_power_apparent;
        internal double d_line_power_reactive;
        internal double d_current_rms;
        internal double d_voltage_rms;
        internal double d_current_peak;
        internal double d_voltage_peak;
        internal float d_frequency;
        internal float d_temp;
        internal float d_pfc;

        internal EWavewormSamplingFreq d_waveworm_sampling_freq;
        internal EWavewormChannel d_waveworm_channel;
        internal EWavewormScale d_waveworm_scale;
        internal float[] d_waveform_buffer = new float[__ACPM_MAX_WAVEFORM_SAMPLES];

        public string ConnectionPort { get { return d_connection_port; } set { d_connection_port = value; EtaSettings.DeviceSerialPort = d_connection_port; OnPropertyChanged(nameof(ConnectionPort)); } }
        public int ConnectionPortBaudrate { get { return d_connection_port_baudrate; } set { d_connection_port_baudrate = value; EtaSettings.DeviceSerialPortBaudRate = d_connection_port_baudrate; OnPropertyChanged(nameof(ConnectionPortBaudrate)); } }
        public bool IsConnected { get { return d_connection_frames != null; } }

        public int ADERegLAENERGY { get { return d_ADERegLAENERGY; } private set { d_ADERegLAENERGY = value; OnPropertyChanged(nameof(ADERegLAENERGY)); } }
        public uint ADERegLVAENERGY { get { return d_ADERegLVAENERGY; } private set { d_ADERegLVAENERGY = value; OnPropertyChanged(nameof(ADERegLVAENERGY)); } }
        public int ADERegLVARENERGY { get { return d_ADERegLVARENERGY; } private set { d_ADERegLVARENERGY = value; OnPropertyChanged(nameof(ADERegLVARENERGY)); } }
        public byte ADERegCH1OS { get { return d_ADERegCH1OS; } set { d_ADERegCH1OS = value; OnPropertyChanged(nameof(ADERegCH1OS)); } }
        public byte ADERegCH2OS { get { return d_ADERegCH2OS; } set { d_ADERegCH2OS = value; OnPropertyChanged(nameof(ADERegCH2OS)); } }
        public byte ADERegPHCAL { get { return d_ADERegPHCAL; } set { d_ADERegPHCAL = value; OnPropertyChanged(nameof(ADERegPHCAL)); } }
        public short ADERegAPOS { get { return d_ADERegAPOS; } set { d_ADERegAPOS = value; OnPropertyChanged(nameof(ADERegAPOS)); } }
        public short ADERegWGAIN { get { return d_ADERegWGAIN; } set { d_ADERegWGAIN = value; OnPropertyChanged(nameof(ADERegWGAIN)); } }
        public byte ADERegWDIV { get { return d_ADERegWDIV; } set { d_ADERegWDIV = value; OnPropertyChanged(nameof(ADERegWDIV)); } }
        public ushort ADERegCFNUM { get { return d_ADERegCFNUM; } set { d_ADERegCFNUM = value; OnPropertyChanged(nameof(ADERegCFNUM)); } }
        public ushort ADERegCFDEN { get { return d_ADERegCFDEN; } set { d_ADERegCFDEN = value; OnPropertyChanged(nameof(ADERegCFDEN)); } }
        public uint ADERegIRMS { get { return d_ADERegIRMS; } private set { d_ADERegIRMS = value; OnPropertyChanged(nameof(ADERegIRMS)); } }
        public uint ADERegVRMS { get { return d_ADERegVRMS; } private set { d_ADERegVRMS = value; OnPropertyChanged(nameof(ADERegVRMS)); } }
        public short ADERegIRMSOS { get { return d_ADERegIRMSOS; } set { d_ADERegIRMSOS = value; OnPropertyChanged(nameof(ADERegIRMSOS)); } }
        public short ADERegVRMSOS { get { return d_ADERegVRMSOS; } set { d_ADERegVRMSOS = value; OnPropertyChanged(nameof(ADERegVRMSOS)); } }
        public short ADERegVAGAIN { get { return d_ADERegVAGAIN; } set { d_ADERegVAGAIN = value; OnPropertyChanged(nameof(ADERegVAGAIN)); } }
        public byte ADERegVADIV { get { return d_ADERegVADIV; } set { d_ADERegVADIV = value; OnPropertyChanged(nameof(ADERegVADIV)); } }
        public byte ADERegSAGLVL { get { return d_ADERegSAGLVL; } set { d_ADERegSAGLVL = value; OnPropertyChanged(nameof(ADERegSAGLVL)); } }
        public byte ADERegIPKLVL { get { return d_ADERegIPKLVL; } set { d_ADERegIPKLVL = value; OnPropertyChanged(nameof(ADERegIPKLVL)); } }
        public byte ADERegVPKLVL { get { return d_ADERegVPKLVL; } set { d_ADERegVPKLVL = value; OnPropertyChanged(nameof(ADERegVPKLVL)); } }
        public uint ADERegIPEAK { get { return d_ADERegIPEAK; } private set { d_ADERegIPEAK = value; OnPropertyChanged(nameof(ADERegIPEAK)); } }
        public uint ADERegVPEAK { get { return d_ADERegVPEAK; } private set { d_ADERegVPEAK = value; OnPropertyChanged(nameof(ADERegVPEAK)); } }
        public sbyte ADERegTEMP { get { return d_ADERegTEMP; } private set { d_ADERegTEMP = value; OnPropertyChanged(nameof(ADERegTEMP)); } }
        public ushort ADERegPERIOD { get { return d_ADERegPERIOD; } private set { d_ADERegPERIOD = value; OnPropertyChanged(nameof(ADERegPERIOD)); } }

        public TimeSpan TSWorkingTime { get { return d_ts_working_time; } private set { d_ts_working_time = value; OnPropertyChanged(nameof(TSWorkingTime)); } }
        public double EnergyActive { get { return d_energy_active; } private set { d_energy_active = value; OnPropertyChanged(nameof(EnergyActive)); } }
        public double EnergyApparent { get { return d_energy_apparent; } private set { d_energy_apparent = value; OnPropertyChanged(nameof(EnergyApparent)); } }
        public double LinePowerActive { get { return d_line_power_active; } private set { d_line_power_active = value; OnPropertyChanged(nameof(LinePowerActive)); } }
        public double LinePowerApparent { get { return d_line_power_apparent; } private set { d_line_power_apparent = value; OnPropertyChanged(nameof(LinePowerApparent)); } }
        public double LinePowerReactive { get { return d_line_power_reactive; } private set { d_line_power_reactive = value; OnPropertyChanged(nameof(LinePowerReactive)); } }
        public double CurrentRMS { get { return d_current_rms; } private set { d_current_rms = value; OnPropertyChanged(nameof(CurrentRMS)); } }
        public double VoltageRMS { get { return d_voltage_rms; } private set { d_voltage_rms = value; OnPropertyChanged(nameof(VoltageRMS)); } }
        public double CurrentPeak { get { return d_current_peak; } private set { d_current_peak = value; OnPropertyChanged(nameof(CurrentPeak)); } }
        public double VoltagePeak { get { return d_voltage_peak; } private set { d_voltage_peak = value; OnPropertyChanged(nameof(VoltagePeak)); } }
        public float Frequency { get { return d_frequency; } private set { d_frequency = value; OnPropertyChanged(nameof(Frequency)); } }
        public float Temp { get { return d_temp; } private set { d_temp = value; OnPropertyChanged(nameof(Temp)); } }
        public float PFC { get { return d_pfc; } private set { d_pfc = value; OnPropertyChanged(nameof(PFC)); } }

        public EWavewormSamplingFreq WavewormSamplingFreq { get { return d_waveworm_sampling_freq; } set { d_waveworm_sampling_freq = value; OnPropertyChanged(nameof(WavewormSamplingFreq)); } }
        public EWavewormChannel WavewormChannel { get { return d_waveworm_channel; } set { d_waveworm_channel = value; OnPropertyChanged(nameof(WavewormChannel)); } }
        public EWavewormScale WavewormScale { get { return d_waveworm_scale; } set { d_waveworm_scale = value; OnPropertyChanged(nameof(WavewormScale)); } }
        public float[] WaveformBuffer { get { return d_waveform_buffer; } private set { d_waveform_buffer = value; OnPropertyChanged(nameof(WaveformBuffer)); } }

        public EtaACPowerMetterControl() {
            d_connection_port = EtaSettings.DeviceSerialPort;
            d_connection_port_baudrate = EtaSettings.DeviceSerialPortBaudRate;
            d_timer_settings_reload = new DispatcherTimer(new TimeSpan(0, 0, 0, 0, 500), DispatcherPriority.ApplicationIdle, (sender, e) => { }, Dispatcher.CurrentDispatcher);
        }

        public void Connect() { Disconnect(); d_connection_frames = new EtaConnectionFrames(() => { d_connection_frames = null; OnPropertyChanged(nameof(IsConnected)); }, d_connection_port, d_connection_port_baudrate, _AsyncFrameProcessor); OnPropertyChanged(nameof(IsConnected)); }
        public void Disconnect() { EtaConnectionFrames _connection_frames_temp = d_connection_frames; _connection_frames_temp?.Terminate(); }

        public void PushFrame(EFrameCommand frame_command, byte[] frame_data) { EtaConnectionFrames _connection_frames_temp = d_connection_frames; _connection_frames_temp?.PushAsyncFrame((byte)frame_command, frame_data); }
        public byte[] RequestFrame(EFrameCommand frame_command, byte[] frame_data, int timeout_ms) { EtaConnectionFrames _connection_frames_temp = d_connection_frames; return _connection_frames_temp?.RequestFrame((byte)frame_command, frame_data, timeout_ms); }

        public bool Request_ValueRead() {
            byte[] _frame_data_resp = RequestFrame(EFrameCommand.ReadValues, null, 1000);
            if (_frame_data_resp != null) return _ParseADEValues(_frame_data_resp);
            return false;
        }
        public bool Request_Waveform() {
            if (Request_WaveformRequest()) {
                byte[][] _frame_datas = new byte[__ACPM_MAX_WAVEFORM_SAMPLES_PAGES][];
                for (byte _page = 0; _page < __ACPM_MAX_WAVEFORM_SAMPLES_PAGES; _page++) {
                    if (!Request_WaveformPage(_frame_datas, _page)) break;
                }
                return _ParseWaveformPages(_frame_datas);
            }
            return false;
        }
        public bool Request_WaveformRequest() {
            byte[] _frame_data_resp = RequestFrame(EFrameCommand.WaveformRequest, _GenerateWaveformRequest(), 1000);
            return _frame_data_resp?.Length == 0;
        }
        public bool Request_WaveformPage(byte[][] frame_datas, byte page) {
            byte[] _frame_data_resp = RequestFrame(EFrameCommand.WaveformPage, new[] { page }, 1000);
            if (_frame_data_resp != null) { frame_datas[page] = _frame_data_resp; return true; }
            return false;
        }
        public bool Request_ReadCalibration() {
            byte[] _frame_data_resp = RequestFrame(EFrameCommand.ReadCalibration, null, 1000);
            if (_frame_data_resp != null) return _ParseADECalibration(_frame_data_resp);
            return false;
        }
        public bool Request_WriteCalibration() {
            byte[] _frame_data_resp = RequestFrame(EFrameCommand.WriteCalibration, _GenerateADECalibration(), 1000);
            return _frame_data_resp?.Length == 0;
        }
        public bool Request_SaveCalibration() {
            byte[] _frame_data_resp = RequestFrame(EFrameCommand.SaveCalibration, null, 1000);
            return _frame_data_resp?.Length == 0;
        }
        public bool Request_Reset() {
            byte[] _frame_data_resp = RequestFrame(EFrameCommand.Reset, null, 1000);
            return _frame_data_resp?.Length == 0;
        }

        private void _AsyncFrameProcessor(byte frame_command, byte[] frame_data) {
            EFrameCommand _command = (EFrameCommand)frame_command; int _frame_data_length = frame_data.Length, _offset = 0;
            //Console.WriteLine(BitConverter.ToString(_bytes_buffer));
            if (_command == EFrameCommand.DebugString) { Console.Write(Encoding.GetEncoding(1251).GetString(frame_data)); }
            else if (_command == EFrameCommand.ValuesUpdated) { _ParseADEValues(frame_data); }
        }

        private bool _ParseADEValues(byte[] frame_data) {
            if (frame_data.Length == 36) {
                int _offset = 0;
                TSWorkingTime = new TimeSpan(BitConverter.ToUInt32(frame_data, _offset) / 32L * 10000000); _offset += 4;
                EnergyActive = BitConverter.ToInt32(frame_data, _offset); _offset += 4;
                EnergyApparent = BitConverter.ToUInt32(frame_data, _offset); _offset += 4;
                ADERegLAENERGY = _GetSigned24(frame_data, _offset); _offset += 3; LinePowerActive = d_ADERegLAENERGY;
                ADERegLVAENERGY = _GetUnsigned24(frame_data, _offset); _offset += 3; LinePowerApparent = d_ADERegLVAENERGY;
                ADERegLVARENERGY = _GetSigned24(frame_data, _offset); _offset += 3; LinePowerReactive = d_ADERegLVARENERGY;
                ADERegIRMS = _GetUnsigned24(frame_data, _offset); _offset += 3; CurrentRMS = d_ADERegIRMS / __ACPM_ADE_IRMS_DIVIDOR;
                ADERegVRMS = _GetUnsigned24(frame_data, _offset); _offset += 3; VoltageRMS = d_ADERegVRMS / __ACPM_ADE_VRMS_DIVIDOR;
                ADERegIPEAK = _GetUnsigned24(frame_data, _offset); _offset += 3; CurrentPeak = d_ADERegIPEAK / __ACPM_ADE_I_DIVIDOR;
                ADERegVPEAK = _GetUnsigned24(frame_data, _offset); _offset += 3; VoltagePeak = d_ADERegVPEAK / __ACPM_ADE_V_DIVIDOR;
                ADERegPERIOD = BitConverter.ToUInt16(frame_data, _offset); _offset += 2; Frequency = (d_ADERegPERIOD > 5000 && d_ADERegPERIOD < 10000) ? (float)__ACPM_ADE_PERIOD_NOMINATOR / d_ADERegPERIOD : 0;
                ADERegTEMP = (sbyte)frame_data[_offset++]; Temp = (((d_ADERegTEMP) << 1) / 3) - 25;
                PFC = ADERegLVAENERGY != 0 ? ((float)Math.Abs(d_ADERegLAENERGY) * 100 / ADERegLVAENERGY) : 0;

                Console.WriteLine("--------------------------ADEValues--------------------------");
                Console.WriteLine("GlobalTicks:       {0}", d_ts_working_time);
                Console.WriteLine("EnergyActive:      {0}", d_energy_active);
                Console.WriteLine("EnergyApparent:    {0}", d_energy_apparent);
                Console.WriteLine("LinePowerActive:   {0} {1}", d_line_power_active, d_ADERegLAENERGY);
                Console.WriteLine("LinePowerApparent: {0} {1}", d_line_power_apparent, d_ADERegLVAENERGY);
                Console.WriteLine("LinePowerReactive: {0} {1}", d_line_power_reactive, d_ADERegLVARENERGY);
                Console.WriteLine("CurrentRMS:        {0} {1}", d_current_rms, d_ADERegIRMS);
                Console.WriteLine("VoltageRMS:        {0} {1}", d_voltage_rms, d_ADERegVRMS);
                Console.WriteLine("CurrentPeak:       {0} {1}", d_current_peak, d_ADERegIPEAK);
                Console.WriteLine("VoltagePeak:       {0} {1}", d_voltage_peak, d_ADERegVPEAK);
                Console.WriteLine("Frequency:         {0} {1}", d_frequency, d_ADERegPERIOD);
                Console.WriteLine("Temp:              {0} {1}", d_temp, d_ADERegTEMP);
                Console.WriteLine("-------------------------------------------------------------");
                return true;
            }
            Console.WriteLine("Error: Wrong size of ADEValues (expecting 36, received {0})", frame_data.Length);
            return false;
        }
        private bool _ParseADECalibration(byte[] frame_data) {
            if (frame_data.Length == 22) {
                int _offset = 0;
                ADERegCH1OS = frame_data[_offset++];
                ADERegCH2OS = frame_data[_offset++];
                ADERegPHCAL = frame_data[_offset++];
                ADERegAPOS = BitConverter.ToInt16(frame_data, _offset); _offset += 2;
                ADERegWGAIN = BitConverter.ToInt16(frame_data, _offset); _offset += 2;
                ADERegWDIV = frame_data[_offset++];
                ADERegCFNUM = BitConverter.ToUInt16(frame_data, _offset); _offset += 2;
                ADERegCFDEN = BitConverter.ToUInt16(frame_data, _offset); _offset += 2;
                ADERegIRMSOS = BitConverter.ToInt16(frame_data, _offset); _offset += 2;
                ADERegVRMSOS = BitConverter.ToInt16(frame_data, _offset); _offset += 2;
                ADERegVAGAIN = BitConverter.ToInt16(frame_data, _offset); _offset += 2;
                ADERegVADIV = frame_data[_offset++];
                ADERegSAGLVL = frame_data[_offset++];
                ADERegIPKLVL = frame_data[_offset++];
                ADERegVPKLVL = frame_data[_offset++];

                Console.WriteLine("--------------------------ADECalibration---------------------");
                Console.WriteLine("ADERegCH1OS:    {0}", ADERegCH1OS);
                Console.WriteLine("ADERegCH2OS:    {0}", ADERegCH2OS);
                Console.WriteLine("ADERegPHCAL:    {0}", ADERegPHCAL);
                Console.WriteLine("ADERegAPOS:     {0}", ADERegAPOS);
                Console.WriteLine("ADERegWGAIN:    {0}", ADERegWGAIN);
                Console.WriteLine("ADERegWDIV:     {0}", ADERegWDIV);
                Console.WriteLine("ADERegCFNUM:    {0}", ADERegCFNUM);
                Console.WriteLine("ADERegCFDEN:    {0}", ADERegCFDEN);
                Console.WriteLine("ADERegIRMSOS:   {0}", ADERegIRMSOS);
                Console.WriteLine("ADERegVRMSOS:   {0}", ADERegVRMSOS);
                Console.WriteLine("ADERegVAGAIN:   {0}", ADERegVAGAIN);
                Console.WriteLine("ADERegVADIV:    {0}", ADERegVADIV);
                Console.WriteLine("ADERegSAGLVL:   {0}", ADERegSAGLVL);
                Console.WriteLine("ADERegIPKLVL:   {0}", ADERegIPKLVL);
                Console.WriteLine("ADERegVPKLVL:   {0}", ADERegVPKLVL);
                Console.WriteLine("-------------------------------------------------------------");
            }
            Console.WriteLine("Error: Wrong size of ADECalibration (expecting 22, received {0})", frame_data.Length);
            return false;
        }
        private bool _ParseWaveformPages(byte[][] frame_datas) {
            float[] _waveform_buffer = new float[__ACPM_MAX_WAVEFORM_SAMPLES_SHIFT];
            for (byte _page = 0; _page < __ACPM_MAX_WAVEFORM_SAMPLES_PAGES; _page++) {
                byte[] _frame_data = frame_datas[_page];
                if (_frame_data.Length == __ACPM_MAX_WAVEFORM_SAMPLES_PART << 1) {
                    int _offset = 0, _page_offset = (__ACPM_MAX_WAVEFORM_SAMPLES_PART << 1) * _page;
                    for (int _i = 0; _i < __ACPM_MAX_WAVEFORM_SAMPLES_PART; _i++) {
                        short _value = BitConverter.ToInt16(_frame_data, _offset); _offset += 2;
                        _waveform_buffer[_page_offset + _i] = ((float)_value) / (_value >= 0 ? 32767 : 32768);
                    }
                }
                else { Console.WriteLine("Error: Wrong size of WaveformPage"); return false; }
            }
            WaveformBuffer = _waveform_buffer;
            return true;
        }

        private byte[] _GenerateADECalibration() {
            byte[] _bytes = new byte[22]; int _offset = 0;
            _bytes[_offset++] = d_ADERegCH1OS;
            _bytes[_offset++] = d_ADERegCH2OS;
            _bytes[_offset++] = d_ADERegPHCAL;
            BitConverter.GetBytes(d_ADERegAPOS).CopyTo(_bytes, _offset); _offset += 2;
            BitConverter.GetBytes(d_ADERegWGAIN).CopyTo(_bytes, _offset); _offset += 2;
            _bytes[_offset++] = d_ADERegWDIV;
            BitConverter.GetBytes(d_ADERegCFNUM).CopyTo(_bytes, _offset); _offset += 2;
            BitConverter.GetBytes(d_ADERegCFDEN).CopyTo(_bytes, _offset); _offset += 2;
            BitConverter.GetBytes(d_ADERegIRMSOS).CopyTo(_bytes, _offset); _offset += 2;
            BitConverter.GetBytes(d_ADERegVRMSOS).CopyTo(_bytes, _offset); _offset += 2;
            BitConverter.GetBytes(d_ADERegVAGAIN).CopyTo(_bytes, _offset); _offset += 2;
            _bytes[_offset++] = d_ADERegVADIV;
            _bytes[_offset++] = d_ADERegSAGLVL;
            _bytes[_offset++] = d_ADERegIPKLVL;
            _bytes[_offset++] = d_ADERegVPKLVL;
            return _bytes;
        }
        private byte[] _GenerateWaveformRequest() {
            byte[] _bytes = new byte[32]; int _offset = 0;
            _bytes[_offset++] = (byte)d_waveworm_sampling_freq;
            _bytes[_offset++] = (byte)d_waveworm_channel;
            _bytes[_offset++] = (byte)d_waveworm_scale;
            return _bytes;
        }

        private static uint _GetUnsigned24(byte[] data, int offset) { return data[offset++] | (((uint)data[offset++]) << 8) | (((uint)data[offset]) << 16); }
        private static int _GetSigned24(byte[] data, int offset) { return (int)(_GetUnsigned24(data, offset) | ((data[offset + 2] & 0x80) != 0 ? 0xFF000000 : 0)); ; }

        public event PropertyChangedEventHandler PropertyChanged;

        protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; _handler?.Invoke(this, new PropertyChangedEventArgs(propery_name)); }

        public enum EWavewormSamplingFreq
        {
            SPC27965 = 1,   // SPS (CLKIN/128)
            SPC13983 = 2,   // SPS (CLKIN/256)
            SPC6991 = 3,    // SPS (CLKIN/512)
            SPC3495 = 4,    // SPS (CLKIN/1024)
        }
        public enum EWavewormChannel
        {
            ActivePower = 0,
            Current = 2,
            Voltage = 3,
        }
        public enum EWavewormScale
        {
            Div64 = -6,
            Div32 = -5,
            Div16 = -4,
            Div8 = -3,
            Div4 = -2,
            Div2 = -1,
            Org = 0,
            Mul2 = 1,
            Mul4 = 2,
            Mul8 = 3,
            Mul16 = 4,
            Mul32 = 5,
            Mul64 = 6,
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
    }
}
