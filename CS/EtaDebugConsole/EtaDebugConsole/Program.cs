using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EtaDebugConsole
{
    public class Program
    {
        static internal EtaConnectionFrames d_connection_frames;
        static internal StreamWriter d_stream_writer;

        static public bool IsConnected => d_connection_frames != null;

        static void Main(string[] args) {
            EtaDebug.DebugWrite(ConsoleColor.Green, true, "EtaDebugConsole");
            try {
                EtaDebug.DebugWrite(ConsoleColor.Cyan, false, "Log filename: "); EtaDebug.DebugWrite(ConsoleColor.White, false, ""); string _filename = Console.ReadLine();
                if (!string.IsNullOrWhiteSpace(_filename)) {
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, false, "Opening log file.....");
                    d_stream_writer = new StreamWriter(_filename);
                    EtaDebug.DebugWrite(ConsoleColor.Green, true, "OK");
                }
            }
            catch (Exception) { EtaDebug.DebugWrite(ConsoleColor.Red, true, "Failed"); }
            EtaDebug.DebugWrite(ConsoleColor.Cyan, false, "COM port number: "); EtaDebug.DebugWrite(ConsoleColor.White, false, ""); string _comport = "COM" + Console.ReadLine();
            EtaDebug.DebugWrite(ConsoleColor.Cyan, false, "COM port baudrate: "); EtaDebug.DebugWrite(ConsoleColor.White, false, ""); int _baudrate = int.Parse(Console.ReadLine());
            try {
                EtaDebug.DebugWrite(ConsoleColor.Yellow, false, "Connecting to {0} at {1}.....", _comport, _baudrate);
                d_connection_frames = new EtaConnectionFrames(() => { d_connection_frames = null; }, _comport, _baudrate, _AsyncFrameProcessor);
                EtaDebug.DebugWrite(ConsoleColor.Green, true, "OK");
            }
            catch (Exception) { EtaDebug.DebugWrite(ConsoleColor.Red, true, "Failed"); }
            Console.WriteLine();
        }

        static private void _AsyncFrameProcessor(byte frame_address, byte frame_command, byte[] frame_data) {
            int _frame_data_length = frame_data.Length, _offset = 0;
            if (frame_command == 0) {
                string _msg = Encoding.GetEncoding(1251).GetString(frame_data);
                d_stream_writer?.Write(_msg); EtaDebug.DebugWrite(ConsoleColor.White, false, _msg);
            }
            else {
                string _msg = string.Format("Unknown package: ADDR - {0}, CMD - {1} (0x{1:X})", frame_address, frame_command);
                d_stream_writer?.WriteLine(_msg); EtaDebug.DebugWrite(ConsoleColor.DarkGray, true, _msg);
                while (_frame_data_length > 0) {
                    int _this_len = Math.Min(16, _frame_data_length);
                    _msg = $" {BitConverter.ToString(frame_data, _offset, _this_len)}";
                    d_stream_writer?.WriteLine(_msg); EtaDebug.DebugWrite(ConsoleColor.DarkGray, true, _msg);
                    _offset += _this_len; _frame_data_length -= _this_len;
                }
            }
        }
    }
}
