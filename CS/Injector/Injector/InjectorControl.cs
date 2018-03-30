using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO.Ports;
using System.Linq;
using System.Net.Sockets;
using System.Security.RightsManagement;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Media;
using System.Windows.Threading;

namespace Injector
{
    public class CInjectorControl : INotifyPropertyChanged
    {
        internal CSettings _settings;
        internal string _connection_port;
        internal int _connection_baudrate;
        internal DispatcherTimer _timer_settings_reload;
        internal CConnectionFrames _connection_frames;

        public CSettings Settings { get { return _settings; } }
        public string ConnectionPort { get { return _connection_port; } set { _connection_port = value; OnPropertyChanged(nameof(ConnectionPort)); } }
        public int ConnectionBaudrate { get { return _connection_baudrate; } set { _connection_baudrate = value; OnPropertyChanged(nameof(ConnectionBaudrate)); } }
        public bool IsConnected { get { return _connection_frames != null; } }

        public CInjectorControl() {
            if (SerialPort.GetPortNames().Length > 0) _connection_port = SerialPort.GetPortNames()[0]; _connection_baudrate = 115200;
            _settings = new CSettings(this);
            _timer_settings_reload = new DispatcherTimer(new TimeSpan(0, 0, 0, 0, 500), DispatcherPriority.ApplicationIdle, (sender, e) => { RequestSettingsFromController(); }, Dispatcher.CurrentDispatcher);
        }

        public void Connect() { Disconnect(); _settings.Reset(); _connection_frames = new CConnectionFrames(() => { _connection_frames = null; OnPropertyChanged(nameof(IsConnected)); }, _connection_port, _connection_baudrate, _FrameProcessor); OnPropertyChanged(nameof(IsConnected)); }
        public void Disconnect() { CConnectionFrames _connection_frames_temp = _connection_frames; if (_connection_frames_temp != null) { _connection_frames_temp.Terminate(); } }

        public void PushFrame(CConnectionFrames.CFrame frame) { CConnectionFrames _connection_frames_temp = _connection_frames; if (_connection_frames_temp != null) _connection_frames_temp.PushFrame(frame); }
        public void PushSingleton(CConnectionFrames.CFrame frame_singleton) { CConnectionFrames _connection_frames_temp = _connection_frames; if (_connection_frames_temp != null) _connection_frames_temp.PushSingleton(frame_singleton); }

        public void SaveSettingsToController() { PushFrame(new CConnectionFrames.CFrame((byte)ECommand.SETTINGS_SAVE, new byte[] { })); }
        public void RequestSettingsFromController() { PushFrame(new CConnectionFrames.CFrame((byte)ECommand.SETTINGS_REQUEST, new byte[] { })); }

        private void _FrameProcessor(CConnectionFrames.CFrame frame) {
            ECommand _command = (ECommand)frame.Command; byte[] _bytes_buffer = frame.BytesFrame; int _buffer_length = _bytes_buffer.Length;
            Console.WriteLine(BitConverter.ToString(_bytes_buffer));
            if (_command == ECommand.DEBUG_STRING) { Console.Write(Encoding.GetEncoding(1251).GetString(_bytes_buffer)); }
            else if (_command == ECommand.SETTINGS_DATA && _buffer_length == (2 + 2 + 1 + (2 * 2))) {
                int _offset = 0;
                ushort _delay = BitConverter.ToUInt16(_bytes_buffer, _offset); _offset += 2;
                ushort _period_offset = BitConverter.ToUInt16(_bytes_buffer, _offset); _offset += 2;
                byte _duty_cycle1 = _bytes_buffer[_offset++];
                byte _duty_cycle2 = _bytes_buffer[_offset++], _phase2 = _bytes_buffer[_offset++];
                byte _duty_cycle3 = _bytes_buffer[_offset++], _phase3 = _bytes_buffer[_offset++];
                _settings.SetParams(_delay, _period_offset, _duty_cycle1, _duty_cycle2, _phase2, _duty_cycle3, _phase3);
            }
            else if (_command == ECommand.SETTINGS_SAVE && _buffer_length == 0) {
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(propery_name)); }

        public enum ECommand
        {
            DEBUG_STRING,
            SETTINGS_REQUEST,
            SETTINGS_SAVE,
            SETTINGS_DATA,
        }
    }
}
