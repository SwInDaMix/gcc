using System;
using System.Text;
using System.Windows.Forms;

namespace ConsoleApplication1
{
    class Program
    {
        [STAThread]
        static void Main(string[] args) {
            StringBuilder _sb = new StringBuilder();

            int _max_value = 256;
            int _values_cnt = 128;
            int[] _value_asssinus = new int[_values_cnt * 2];
            int[] _value_sinus = new int[_values_cnt * 2];

            int _max_value_mid = _max_value / 2;

            // Assinus
            bool _is_null = false;
            for (int _i = 0; _i < _values_cnt * 2; _i++) {
                double _x = (Math.PI / 2) / _values_cnt * 2 * _i;
                if (_is_null) _value_asssinus[_i] = 0;
                else {
                    int _v = (int)(Math.Max(Math.Sin(_x), Math.Sin(_x - Math.PI / 3)) * (_max_value - 1) + 0.5);
                    if (_v <= 0 && _i > _values_cnt) { _v = 0; _is_null = true; }
                    _value_asssinus[_i] = _v;
                }
            }

            // Sinus
            for (int _i = 0; _i < _values_cnt; _i++) {
                double _x = (Math.PI / 2) / _values_cnt * 2 * _i;
                _value_sinus[_i] = (int)(Math.Sin(_x) * _max_value_mid + 0.5) + _max_value_mid - 1;
            }
            for (int _i = 0; _i < _values_cnt; _i++) {
                double _x = (Math.PI / 2) / _values_cnt * 2 * _i;
                _value_sinus[_i + _values_cnt] = _max_value_mid - (int)(Math.Sin(_x) * _max_value_mid + 0.5);
            }

            //int[] _value_output = _value_asssinus;
            int[] _value_output = _value_sinus;
            for (int _i = 0; _i < _values_cnt * 2; _i++) {
                _sb.AppendFormat($"{{0,{_max_value.ToString().Length}}}", _value_output[_i]); _sb.Append(", ");
                if ((_i & 0xF) == 0xF) _sb.AppendLine();
            }

            Clipboard.SetData(DataFormats.Text, (Object)_sb.ToString());
            Console.Write(_sb.ToString());
        }
    }
}
