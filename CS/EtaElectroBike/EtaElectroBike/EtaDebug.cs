using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace EtaSecurityGovernorManager
{
    public class EtaDebug
    {
        private static readonly Queue<Tuple<ConsoleColor, bool, string>> __QUEUE_DEBUG_MESSAGE = new Queue<Tuple<ConsoleColor, bool, string>>();
        private static readonly Thread __THREAD_CONSOLE = new Thread(_ThreadProcessing);

        public static void DebugWrite(ConsoleColor color, bool is_nextline, string format, params object[] args) { lock (__QUEUE_DEBUG_MESSAGE) { __QUEUE_DEBUG_MESSAGE.Enqueue(new Tuple<ConsoleColor, bool, string>(color, is_nextline, string.Format(format, args))); } }
        public static void DebugWrite() { DebugWrite(ConsoleColor.Black, true, ""); }

        static EtaDebug() { __THREAD_CONSOLE.Start(); }

        private static void _ThreadProcessing() {
            while (true) {
                Tuple<ConsoleColor, bool, string> _tuple;
                lock (__QUEUE_DEBUG_MESSAGE) { _tuple = __QUEUE_DEBUG_MESSAGE.Count > 0 ? __QUEUE_DEBUG_MESSAGE.Dequeue() : null; }
                if (_tuple != null) {
                    Console.ForegroundColor = _tuple.Item1;
                    if (_tuple.Item2) Console.WriteLine(_tuple.Item3); else Console.Write(_tuple.Item3);
                }
                else Thread.Sleep(1);
            }
        }
    }
}
