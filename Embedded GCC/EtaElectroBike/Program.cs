using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args) {
            int _values = 64;
            for (int _i = 0; _i < _values; _i++) {
                double _x = (Math.PI / 1.5) / _values * _i;
                Console.Write("0x"); Console.Write(((ushort)(Math.Max(Math.Sin(_x), Math.Sin(_x - (Math.PI / 3))) * 0x3FF)).ToString("X4")); Console.Write(", ");
                if ((_i & 0xF) == 0xF) { Console.WriteLine(); }
            }
        }
    }
}
