using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Media;

namespace EtaSecurityGovernorManager
{
    public class EtaConnectionFrames
    {
        internal Action d_act_disconnect;
        internal Action<byte, byte, byte[]> d_act_async_frame_processor;
        internal Dictionary<ushort, List<Action<byte[]>>> d_dict_sync_frame_processors = new Dictionary<ushort, List<Action<byte[]>>>();
        internal SerialPort d_serial_port;
        internal Color d_color_of_connection;
        internal Thread d_thread;
        internal Queue<CFrame> d_queue_frame_outgoing = new Queue<CFrame>();
        internal SortedDictionary<uint, CFrame> d_sorted_dict_singletons = new SortedDictionary<uint, CFrame>();
        internal bool d_is_frame_collision;

        private EReceiverStep _rx_step = EReceiverStep.Stop;
        private byte d_rx_rotation_index, d_rx_rotation_byte;
        private byte d_rx_frame_address, d_rx_frame_command, d_rx_frame_size_left;
        private ushort d_rx_frame_addrcmd;
        private byte[] d_rx_frame_buffer;

        public Color ColorOfConnection => d_color_of_connection;

        public EtaConnectionFrames(Action act_disconnect, string port, int baudrate, Action<byte, byte, byte[]> act_async_frame_processor) {
            d_act_disconnect = act_disconnect;
            d_serial_port = new SerialPort(port, baudrate) { ReadTimeout = SerialPort.InfiniteTimeout, WriteTimeout = SerialPort.InfiniteTimeout, ReadBufferSize = 32768, WriteBufferSize = 32768 };
            d_serial_port.Open();
            d_act_async_frame_processor = act_async_frame_processor;
            d_color_of_connection = Colors.Gray; d_thread = new Thread(_ConnectionThread); d_thread.Start();
        }

        public void PushAsyncFrame(byte frame_address, byte frame_command, byte[] frame_data) { lock (d_queue_frame_outgoing) { d_queue_frame_outgoing.Enqueue(new CFrame(frame_address, (byte)(frame_command & 0x7F), frame_data)); } }
        public void PushAsyncSingleton(byte frame_address, byte frame_command, byte[] frame_data, uint priority) { lock (d_sorted_dict_singletons) { d_sorted_dict_singletons[priority] = new CFrame(frame_address, (byte)(frame_command & 0x7F), frame_data); } }
        public byte[] RequestFrame(byte frame_address, byte frame_command, byte[] frame_data, int timeout_ms) { return Task.Run(async () => await RequestFrameAsync(frame_address, frame_command, frame_data, timeout_ms)).Result; }
        public async Task<byte[]> RequestFrameAsync(byte frame_address, byte frame_command, byte[] frame_data, int timeout_ms) {
            frame_command = (byte)(frame_command | 0x80);
            byte[] _frame_data_resp = null;
            ManualResetEvent _event = new ManualResetEvent(false);
            List<Action<byte[]>> _list_of_commands;
            Action<byte[]> _act_resp;
            ushort _addrcmd = (ushort)((frame_address << 8) | frame_command);
            lock (d_dict_sync_frame_processors) {
                if (!d_dict_sync_frame_processors.TryGetValue(_addrcmd, out _list_of_commands)) d_dict_sync_frame_processors[_addrcmd] = _list_of_commands = new List<Action<byte[]>>();
                _act_resp = frame_data_resp => {
                    _frame_data_resp = frame_data_resp;
                    _event.Set();
                };
                _list_of_commands.Add(_act_resp);
            }
            lock (d_queue_frame_outgoing) { d_queue_frame_outgoing.Enqueue(new CFrame(frame_address, frame_command, frame_data)); }
            await Task.Run(() => _event.WaitOne(timeout_ms));
            lock (d_dict_sync_frame_processors) {
                _list_of_commands.Remove(_act_resp);
                if (_list_of_commands.Count == 0) d_dict_sync_frame_processors.Remove(_addrcmd);
            }
            return _frame_data_resp;
        }
        public bool IsSingletonInQueue(uint priority) { lock (d_sorted_dict_singletons) { return d_sorted_dict_singletons.ContainsKey(priority); } }
        public void Terminate() { d_thread.Abort(); d_thread.Join(); }

        private byte _CRC8_Update(byte crc8, byte data) {
            data ^= crc8;
            for (int _i = 0; _i < 8; _i++) {
                if ((data & 0x80) != 0) { data <<= 1; data ^= 0x07; }
                else { data <<= 1; }
            }
            return data;
        }
        private byte _CRC8(byte cmd, byte[] buffer) { return _CRC8(cmd, buffer, 0, buffer.Length); }
        private byte _CRC8(byte cmd, byte[] buffer, int offset, int count) {
            byte _crc8 = 0;
            _crc8 = _CRC8_Update(_crc8, cmd); _crc8 = _CRC8_Update(_crc8, (byte)count);
            while (count-- > 0) { _crc8 = _CRC8_Update(_crc8, buffer[offset++]); }
            return _crc8;
        }
        private void _Write(byte[] buffer) { _Write(buffer, 0, buffer.Length); }
        private void _Write(byte[] buffer, int offset, int count) { d_serial_port.Write(buffer, offset, count); }
        private void _DiscardIncomming() {
            if (_rx_step != EReceiverStep.Stop) {
                if (_rx_step < EReceiverStep.Address) EtaDebug.DebugWrite(ConsoleColor.Red, true, "DiscardIncomming");
                if (_rx_step < EReceiverStep.Command) EtaDebug.DebugWrite(ConsoleColor.Red, true, "DiscardIncomming addr: {0}", d_rx_frame_address);
                else if (_rx_step < EReceiverStep.Size) EtaDebug.DebugWrite(ConsoleColor.Red, true, "DiscardIncomming addr: {0}, cmd: {1}", d_rx_frame_address, d_rx_frame_command);
                else EtaDebug.DebugWrite(ConsoleColor.Red, true, "DiscardIncomming addr: {0}, cmd: {1} {2}", d_rx_frame_address, d_rx_frame_command, BitConverter.ToString(d_rx_frame_buffer));
                d_rx_frame_command = 0; d_rx_frame_buffer = null;
                _rx_step = EReceiverStep.Stop;
            }
        }
        private void _ProccessIncomingByte(byte data) {
            if (_rx_step == EReceiverStep.Address) {
                d_rx_frame_address = data;
                _rx_step = EReceiverStep.Command;
            }
            else if (_rx_step == EReceiverStep.Command) {
                d_rx_frame_command = data;
                d_rx_frame_addrcmd = (ushort)((d_rx_frame_address << 8) | d_rx_frame_command);
                _rx_step = EReceiverStep.Size;
            }
            else if (_rx_step == EReceiverStep.Size) {
                d_rx_frame_size_left = data; d_rx_frame_buffer = new byte[data];
                _rx_step = d_rx_frame_size_left > 0 ? EReceiverStep.Data : EReceiverStep.CRC;
            }
            else if (_rx_step == EReceiverStep.Data) {
                d_rx_frame_buffer[d_rx_frame_buffer.Length - d_rx_frame_size_left--] = data;
                if (d_rx_frame_size_left == 0) _rx_step = EReceiverStep.CRC;
            }
            else if (_rx_step == EReceiverStep.CRC) {
                if (data == _CRC8(d_rx_frame_command, d_rx_frame_buffer)) {
                    _rx_step = EReceiverStep.Stop;
                    try {
                        if ((d_rx_frame_command & 0x80) != 0) {
                            lock (d_dict_sync_frame_processors) {
                                List<Action<byte[]>> _list_of_commands;
                                if (!d_dict_sync_frame_processors.TryGetValue(d_rx_frame_addrcmd, out _list_of_commands)) d_dict_sync_frame_processors[d_rx_frame_addrcmd] = _list_of_commands = new List<Action<byte[]>>();
                                _list_of_commands[0](d_rx_frame_buffer);
                                _list_of_commands.RemoveAt(0);
                            }
                        }
                        else { byte _addr = d_rx_frame_address; byte _cmd = d_rx_frame_command; byte[] _bytes_data = d_rx_frame_buffer; Task.Run(() => d_act_async_frame_processor(_addr, _cmd, _bytes_data)); }
                    }
                    catch { }
                    d_rx_frame_command = 0; d_rx_frame_buffer = null;
                }
                else { d_is_frame_collision = true; _DiscardIncomming(); }
            }
        }
        private void _ConnectionThread() {
            try {
                while (true) {
                    CFrame _frame_outgoing = null;
                    if (d_serial_port.BytesToWrite == 0) {
                        lock (d_queue_frame_outgoing) {
                            if (d_queue_frame_outgoing.Count > 0) { _frame_outgoing = d_queue_frame_outgoing.Dequeue(); }
                        }
                        if (_frame_outgoing == null) {
                            lock (d_sorted_dict_singletons) {
                                if (d_sorted_dict_singletons.Count > 0) {
                                    KeyValuePair<uint, CFrame> _kvp_frame_singleton = d_sorted_dict_singletons.First();
                                    d_sorted_dict_singletons.Remove(_kvp_frame_singleton.Key);
                                    _frame_outgoing = _kvp_frame_singleton.Value;
                                }
                            }
                        }
                    }
                    if (_frame_outgoing != null) {
                        byte _address = _frame_outgoing.Address;
                        byte _command = _frame_outgoing.Command;
                        byte[] _buffer = _frame_outgoing.BytesFrame ?? new byte[0]; int _count = _buffer.Length;

                        byte _rotation_index = 0; ushort _rotation_byte = 0; byte[] _tx_buffer = new byte[((_count * 8) / 7) + 5]; int _tx_buffer_idx = 0; byte _crc = 0;
                        Action<byte> _act_send = data => {
                            if (_rotation_index >= 7) { _tx_buffer[_tx_buffer_idx++] = (byte)_rotation_byte; _rotation_index = 0; _rotation_byte = 0; }
                            _rotation_byte |= (ushort)(data << _rotation_index);
                            _tx_buffer[_tx_buffer_idx] = (byte)((_rotation_byte & 0x7F) | ((_tx_buffer_idx == 0) ? 0x80 : 0)); _tx_buffer_idx++; _rotation_index++; _rotation_byte >>= 7;
                            _crc = _CRC8_Update(_crc, data);
                        };

                        _act_send(_address); _act_send(_command); _act_send((byte)_count);
                        foreach (byte _data in _buffer) { _act_send(_data); }
                        _act_send(_crc); if (_rotation_index > 0) _tx_buffer[_tx_buffer_idx++] = (byte)_rotation_byte;
                        _Write(_tx_buffer, 0, _tx_buffer_idx);
                    }
                    else if (d_serial_port.BytesToRead > 0) {
                        while (d_serial_port.BytesToRead > 0) {
                            byte _data = (byte)d_serial_port.ReadByte();
                            if ((_data & 0x80) != 0) {
                                if (_rx_step != EReceiverStep.Stop) _DiscardIncomming();
                                _rx_step = EReceiverStep.Address;
                                d_rx_rotation_index = 7;
                                d_rx_rotation_byte = (byte)(_data & 0x7F);
                            }
                            else if (_rx_step != EReceiverStep.Stop) {
                                ushort _data16 = (ushort)(_data << d_rx_rotation_index);
                                d_rx_rotation_byte |= (byte)_data16; d_rx_rotation_index += 7;
                                if (d_rx_rotation_index >= 8) {
                                    _ProccessIncomingByte(d_rx_rotation_byte);
                                    d_rx_rotation_byte = (byte)(_data16 >> 8); d_rx_rotation_index -= 8;
                                }
                            }
                        }
                    }
                    else Thread.Sleep(1);
                }
            }
            catch (Exception) {
                try { if (d_serial_port.IsOpen) d_serial_port.Close(); }
                catch (Exception) { }
                d_act_disconnect?.Invoke();
            }
        }

        internal class CFrame
        {
            public byte Address { get; }
            public byte Command { get; }
            public byte[] BytesFrame { get; }

            public CFrame(byte address, byte command, byte[] bytes_frame) { Address = address; Command = command; BytesFrame = bytes_frame; }
        }
        private enum EReceiverStep
        {
            Stop,
            Address,
            Command,
            Size,
            Data,
            CRC,
        }
    }
}
