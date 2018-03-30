using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Media;

namespace Injector
{
    public class CConnectionFrames
    {
        private readonly byte[] __SYNC_SIGNATURE = { 0x72, 0xC5, 0xD9, 0xAC, 0x86, 0xE3, 0xBA, 0x9E };
        private readonly byte __SYNC_STUB = 0xAA;

        internal Action _act_disconnect;
        internal Action<CFrame> _act_frame_processor;
        internal SerialPort _serial_port;
        internal Color _color_of_connection;
        internal Thread _thread;
        internal Queue<CFrame> _queue_frame_outgoing = new Queue<CFrame>();
        internal CFrame _frame_singleton;
        internal bool _is_frame_collision;

        private EReceiverStep _rx_step = EReceiverStep.STOP;
        private int _rx_signature_index, _rx_stub_index;
        private byte _rx_frame_size_left, _rx_frame_command;
        private byte[] _rx_frame_buffer;

        public Color ColorOfConnection { get { return _color_of_connection; } }

        public CConnectionFrames(Action act_disconnect, string port, int baudrate, Action<CFrame> act_frame_processor) {
            _act_disconnect = act_disconnect;
            _serial_port = new SerialPort(port, baudrate);
            _serial_port.ReadTimeout = SerialPort.InfiniteTimeout; _serial_port.WriteTimeout = SerialPort.InfiniteTimeout;
            _serial_port.ReadBufferSize = 32768; _serial_port.WriteBufferSize = 32768;
            _serial_port.Open();
            _act_frame_processor = act_frame_processor;
            _color_of_connection = Colors.Gray; _thread = new Thread(_ConnectionThread); _thread.Start();
        }

        public void PushFrame(CFrame frame) { lock (_queue_frame_outgoing) { _queue_frame_outgoing.Enqueue(frame); } }
        public void PushSingleton(CFrame frame_singleton) { lock (_queue_frame_outgoing) { _frame_singleton = frame_singleton; } }
        public void Terminate() { _thread.Abort(); _thread.Join(); }

        private byte _CRC8_Update(byte crc8, byte data) {
            data ^= crc8;
            for (int _i = 0; _i < 8; _i++) {
                if ((data & 0x80) != 0) { data <<= 1; data ^= 0x07; }
                else { data <<= 1; }
            }
            return data;
        }
        private byte _CRC8(byte[] buffer) { return _CRC8(buffer, 0, buffer.Length); }
        private byte _CRC8(byte[] buffer, int offset, int count) {
            byte _crc8 = 0;
            while (count-- > 0) { _crc8 = _CRC8_Update(_crc8, buffer[offset++]); }
            return _crc8;
        }
        private void _Write(byte[] buffer) { _Write(buffer, 0, buffer.Length); }
        private void _Write(byte[] buffer, int offset, int count) { _serial_port.Write(buffer, offset, count); }
        private void _DiscardIncomming() {
            if (_rx_step != EReceiverStep.STOP) {
                _rx_frame_buffer = null;
                _rx_step = EReceiverStep.STOP;
            }
        }
        private void _ProccessIncomingByte(byte data) {
            if (_rx_step == EReceiverStep.COMMAND) {
                _rx_frame_command = data;
                _rx_step = EReceiverStep.SIZE;
            }
            else if (_rx_step == EReceiverStep.SIZE) {
                if (data > 0) {
                    _rx_frame_size_left = data; _rx_frame_buffer = new byte[data];
                    _rx_step = EReceiverStep.STUB;
                }
                else { _is_frame_collision = true; _DiscardIncomming(); }
            }
            else if (_rx_step == EReceiverStep.STUB) {
                if (data == __SYNC_STUB) {
                    _rx_stub_index = 0;
                    _rx_step = _rx_frame_size_left > 0 ? EReceiverStep.DATA : EReceiverStep.CRC;
                }
                else { _is_frame_collision = true; _DiscardIncomming(); }
            }
            else if (_rx_step == EReceiverStep.DATA) {
                _rx_frame_buffer[_rx_frame_buffer.Length - _rx_frame_size_left--] = data;
                if (++_rx_stub_index >= __SYNC_SIGNATURE.Length - 1) _rx_step = EReceiverStep.STUB;
                else if (_rx_frame_size_left == 0) _rx_step = EReceiverStep.CRC;
            }
            else if (_rx_step == EReceiverStep.CRC) {
                if (data == _CRC8(_rx_frame_buffer)) {
                    _rx_step = EReceiverStep.STOP;
                    _act_frame_processor(new CFrame(_rx_frame_command, _rx_frame_buffer));
                }
                else { _is_frame_collision = true; _DiscardIncomming(); }
            }
        }
        private void _ConnectionThread() {
            try {
                while (true) {
                    CFrame _frame_outgoing = null;
                    if (_serial_port.BytesToWrite == 0) {
                        lock (_queue_frame_outgoing) {
                            if (_frame_singleton != null) { _frame_outgoing = _frame_singleton; _frame_singleton = null; }
                            else if (_queue_frame_outgoing.Count > 0) { _frame_outgoing = _queue_frame_outgoing.Dequeue(); }
                        }
                    }
                    if (_frame_outgoing != null) {
                        byte[] _buffer = _frame_outgoing._bytes_frame;
                        int _offset = 0; byte _command = _frame_outgoing._command; int _count = _frame_outgoing.BytesFrame.Length;
                        if (_count <= byte.MaxValue) {
                            _Write(__SYNC_SIGNATURE); _Write(new[] { _command, (byte)_count, __SYNC_STUB });
                            while (_count > 0) {
                                int _block_count = Math.Min(_count, __SYNC_SIGNATURE.Length - 1);
                                _Write(_buffer, _offset, _block_count);
                                if (_block_count == __SYNC_SIGNATURE.Length - 1) _Write(new[] { __SYNC_STUB });
                                _offset += _block_count; _count -= _block_count;
                            }
                            _Write(new[] { _CRC8(_buffer) });
                        }
                    }
                    else if (_serial_port.BytesToRead > 0) {
                        while (_serial_port.BytesToRead > 0) {
                            byte _data = (byte)_serial_port.ReadByte();
                            if (_data == __SYNC_SIGNATURE[_rx_signature_index]) {
                                _rx_signature_index++;
                                if (_rx_signature_index >= __SYNC_SIGNATURE.Length) {
                                    _rx_signature_index = 0;
                                    if (_rx_step != EReceiverStep.STOP) _DiscardIncomming();
                                    _rx_step = EReceiverStep.COMMAND;
                                }
                            }
                            else {
                                if (_rx_step != EReceiverStep.STOP) {
                                    if (_rx_signature_index > 0) { for (int _i = 0; _rx_signature_index > 0 && _rx_step != EReceiverStep.STOP; _rx_signature_index--) { _ProccessIncomingByte(__SYNC_SIGNATURE[_i++]); } }
                                    _ProccessIncomingByte(_data);
                                }
                                else _is_frame_collision = true;
                                _rx_signature_index = 0;
                            }
                        }
                    }
                    else Thread.Sleep(1);
                }
            }
            catch (Exception) {
                try { if (_serial_port.IsOpen) _serial_port.Close(); }
                catch (Exception) { }
                if (_act_disconnect != null) _act_disconnect();
            }
        }

        public class CFrame
        {
            internal byte _command;
            internal byte[] _bytes_frame;

            public byte Command { get { return _command; } }
            public byte[] BytesFrame { get { return _bytes_frame; } }

            public CFrame(byte command, byte[] bytes_frame) { _command = command; _bytes_frame = bytes_frame; }
        }
        private enum EReceiverStep
        {
            STOP,
            COMMAND,
            SIZE,
            STUB,
            DATA,
            CRC,
        }
    }
}
