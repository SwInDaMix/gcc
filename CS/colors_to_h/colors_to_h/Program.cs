using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Reflection;
using System.IO;

namespace colors_to_h
{
    class Program
    {
        static void Main(string[] args) {
            StringBuilder _sb_header = new StringBuilder();
            _sb_header.Append("enum class EColors {\n");
            foreach (PropertyInfo _property_info in typeof(Color).GetProperties().Where(info => info.PropertyType == typeof(Color))) {
                Color _color = (Color)_property_info.GetGetMethod().Invoke(null, null);
                if (_color.A == 255) {
                    string _name = _color.Name; string _tabs = new string('\t', (23 - _name.Length) / 4);
                    _sb_header.AppendFormat("\t{0}{1} = LCD_COLOR_FROM_R_G_B(0x{2:X2}, 0x{3:X2}, 0x{4:X2}),\n", _name, _tabs, _color.R, _color.G, _color.B);
                }
            }
            _sb_header.Append("};\n");
            using (StreamWriter _sw = new StreamWriter("header.h")) { _sw.Write(_sb_header.ToString()); }
        }
    }
}
