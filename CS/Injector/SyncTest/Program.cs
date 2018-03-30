using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Injector;

namespace SyncTest
{
    class Program
    {
        static readonly byte[] _TEST_BYTES = new byte[] { 0xdf, 0x2f, 0x6d, 0xc3, 0x45, 0x89 };

        static void Main(string[] args) {
            bool _is_came = false;

            CConnectionFrames _connection_frames = new CConnectionFrames(() => { }, "COM7", 115200, (frame) => {
                //Console.Write(BitConverter.ToString(frame.BytesFrame)); Console.Write(" - ");
                if (frame.BytesFrame.Length != _TEST_BYTES.Length) Console.WriteLine("{0} Wrong incomming frame length!", BitConverter.ToString(frame.BytesFrame));
                else {
                    for (int _i = 0; _i < _TEST_BYTES.Length; _i++) {
                        if (_TEST_BYTES[_i] != frame.BytesFrame[_i]) {
                            Console.WriteLine("{0} Wrong frame data!", BitConverter.ToString(frame.BytesFrame));
                            break;
                        }
                    }
                }
                _is_came = true;
                //Console.WriteLine();
            });

            int _cnt = 200;
            while (_cnt-- > 0) { _connection_frames.PushFrame(new CConnectionFrames.CFrame(0, _TEST_BYTES)); }

            while (true) {
                _is_came = false;
                DateTime _time_in = DateTime.Now;
                _connection_frames.PushSingleton(new CConnectionFrames.CFrame(0, _TEST_BYTES));
                while (!_is_came && (DateTime.Now - _time_in) < new TimeSpan(0, 0, 0, 0, 100)) Thread.Sleep(1);
            }
        }
    }
}
