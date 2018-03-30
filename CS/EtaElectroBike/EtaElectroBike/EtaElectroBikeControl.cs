using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace EtaElectroBike
{
    public class EtaElectroBikeControl : INotifyPropertyChanged
    {
        internal string _connection_port;
        internal int _connection_port_baudrate;
        internal DispatcherTimer _timer_settings_reload;
        internal EtaConnectionFrames _connection_frames;

        internal byte _hall_position;
        internal byte _hall_prescaler;
        internal ushort _hall_period;
        internal ushort _pwm_power;

        public string ConnectionPort { get { return _connection_port; } set { _connection_port = value; EtaSettings.DeviceSerialPort = _connection_port; OnPropertyChanged(nameof(ConnectionPort)); } }
        public int ConnectionPortBaudrate { get { return _connection_port_baudrate; } set { _connection_port_baudrate = value; EtaSettings.DeviceSerialPortBaudRate = _connection_port_baudrate; OnPropertyChanged(nameof(ConnectionPortBaudrate)); } }
        public bool IsConnected { get { return _connection_frames != null; } }

        public byte HallPosition { get { return _hall_position; } set { if (_hall_position != value) _hall_position = value; OnPropertyChanged(nameof(HallPosition)); } }
        public byte HallPrescaler { get { return _hall_prescaler; } set { if (_hall_prescaler != value) _hall_prescaler = value; OnPropertyChanged(nameof(HallPrescaler)); } }
        public ushort HallPeriod { get { return _hall_period; } set { if (_hall_period != value) _hall_period = value; OnPropertyChanged(nameof(HallPeriod)); } }
        public ushort PWMPower { get { return _pwm_power; } set { if (_pwm_power != value) _pwm_power = value; OnPropertyChanged(nameof(PWMPower)); } }

        public ushort NewParamHall {
            get { return (ushort)(_hall_period >> _hall_prescaler); }
            set { _SendNewParams(new SNewParams(_hall_prescaler, (_hall_period = (ushort)(value << _hall_prescaler)), _pwm_power)); }
        }
        public ushort NewParamPWM { get { return _pwm_power; } set { _SendNewParams(new SNewParams(_hall_prescaler, (ushort)(_hall_period << _hall_prescaler), (_pwm_power = value))); } }

        public EtaElectroBikeControl() {
            _connection_port = EtaSettings.DeviceSerialPort;
            _connection_port_baudrate = EtaSettings.DeviceSerialPortBaudRate;

            _timer_settings_reload = new DispatcherTimer(new TimeSpan(0, 0, 0, 0, 500), DispatcherPriority.ApplicationIdle, (sender, e) => { }, Dispatcher.CurrentDispatcher);
        }

        public void Connect() { Disconnect(); _connection_frames = new EtaConnectionFrames(() => { _connection_frames = null; OnPropertyChanged(nameof(IsConnected)); }, _connection_port, _connection_port_baudrate, _FrameProcessor); OnPropertyChanged(nameof(IsConnected)); }
        public void Disconnect() { EtaConnectionFrames _connection_frames_temp = _connection_frames; if (_connection_frames_temp != null) { _connection_frames_temp.Terminate(); } }

        public void PushFrame(EtaConnectionFrames.CFrame frame) { EtaConnectionFrames _connection_frames_temp = _connection_frames; if (_connection_frames_temp != null) _connection_frames_temp.PushFrame(frame); }
        public void PushSingleton(EtaConnectionFrames.CFrame frame_singleton, EFrameSignletonPriority priority) { EtaConnectionFrames _connection_frames_temp = _connection_frames; if (_connection_frames_temp != null) _connection_frames_temp.PushSingleton(frame_singleton, (uint)priority); }

        private void _SendNewParams(SNewParams new_params) {
            PushSingleton(new EtaConnectionFrames.CFrame((byte)EFrameCommand.NEW_PARAMS, new_params.ToBytes()), EFrameSignletonPriority.NEW_PARAMS);
        }
        private void _FrameProcessor(EtaConnectionFrames.CFrame frame) {
            EFrameCommand _command = (EFrameCommand)frame.Command; byte[] _bytes_buffer = frame.BytesFrame; int _buffer_length = _bytes_buffer.Length;
            //Console.WriteLine(BitConverter.ToString(_bytes_buffer));
            if (_command == EFrameCommand.DEBUG_STRING) { Console.Write(Encoding.GetEncoding(1251).GetString(_bytes_buffer)); }
            else if (_command == EFrameCommand.STATUS) {
                int _offset = 0; bool _is_new_hall = false, _is_new_pwm = false;
                HallPosition = _bytes_buffer[_offset++];
                byte _new_hall_prescaler = _bytes_buffer[_offset++];
                ushort _new_hall_period = BitConverter.ToUInt16(_bytes_buffer, _offset); _offset += 2;
                ushort _new_pwm_power = BitConverter.ToUInt16(_bytes_buffer, _offset); _offset += 2;
                _is_new_hall = _new_hall_prescaler != _hall_prescaler || _new_hall_period != _hall_period;
                _is_new_pwm = _new_pwm_power != _pwm_power;
                HallPrescaler = _new_hall_prescaler; HallPeriod = _new_hall_period; PWMPower = _new_pwm_power;
                if (_is_new_hall) OnPropertyChanged(nameof(NewParamHall));
                if (_is_new_pwm) OnPropertyChanged(nameof(NewParamPWM));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(propery_name)); }

        public struct SStatus
        {
            public byte HallPosition;
            public byte HallPrescaler;
            public ushort HallPeriod;

            public SStatus(byte[] data, ref int offset) {
                HallPosition = data[offset++];
                HallPrescaler = data[offset++];
                HallPeriod = BitConverter.ToUInt16(data, offset); offset += 2;
            }
        }
        public struct SNewParams
        {
            public byte HallPrescaler;
            public ushort HallPeriod;
            public ushort PWMPower;

            public SNewParams(byte hall_prescaler, ushort hall_period, ushort pwm_power) {
                HallPrescaler = hall_prescaler;
                HallPeriod = hall_period;
                PWMPower = pwm_power;
            }

            public byte[] ToBytes() {
                byte[] _bytes = new byte[5]; int _offset = 0;
                _bytes[_offset++] = HallPrescaler;
                BitConverter.GetBytes(HallPeriod).CopyTo(_bytes, _offset); _offset += 2;
                BitConverter.GetBytes(PWMPower).CopyTo(_bytes, _offset); _offset += 2;
                return _bytes;
            }
        }

        public enum EFrameCommand
        {
            DEBUG_STRING,
            STATUS,
            NEW_PARAMS,
            TEST_COMMAND
        }

        public enum EFrameSignletonPriority
        {
            NEW_PARAMS
        }
    }
}
