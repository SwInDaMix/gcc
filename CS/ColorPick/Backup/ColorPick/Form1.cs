using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        KeysConverter _kk = new KeysConverter();
        Dictionary<int, KnownColor> _dict_nks = new Dictionary<int, KnownColor>();
        List<Color> _list_colors = new List<Color>();
        Color _curcolor = Color.Black;

        public Form1()
        {
            InitializeComponent();
            Array _a = Enum.GetValues(typeof(KnownColor));

            foreach (KnownColor _nk in _a)
            {
                Color _clr = Color.FromKnownColor(_nk);
                _list_colors.Add(_clr);
                _dict_nks[_clr.ToArgb() & 0xFFFFFF] = _nk;
            }
            _list_colors.Sort(delegate(Color x, Color y)
            {

                double _ix = x.B * 0.11 + x.R * 0.3 * x.G * 0.59, _iy = y.B * 0.11 + y.R * 0.3 * y.G * 0.59;
                int _res = _ix.CompareTo(_iy);
                if (_res == 0) { _res = x.GetHue().CompareTo(y.GetHue()); }
                return _res;
            });
            //_list_colors.Sort(delegate(Color x, Color y) { return (x.GetHue() * x.GetBrightness() * x.GetSaturation()).CompareTo(y.GetHue() * y.GetBrightness() * y.GetSaturation()); });
            //_list_colors.Sort(delegate(Color x, Color y) { int _res = x.GetHue().CompareTo(y.GetHue()); if (_res == 0) { _res = x.GetBrightness().CompareTo(y.GetBrightness()); } return _res; });
            //_list_colors.Sort(delegate(Color x, Color y) { int _res = x.GetHue().CompareTo(y.GetHue()); if (_res == 0) { _res = (x.GetBrightness() * x.GetSaturation()).CompareTo(y.GetBrightness() * y.GetSaturation()); } return _res; });

            _lv_kcs.BeginUpdate();
            foreach (Color _color in _list_colors)
            {
                int _ic = _color.B + (_color.R << 1) + (_color.G << 2);
                ListViewItem _lvi = _lv_kcs.Items.Add(_color.Name);
                _lvi.BackColor = _color; if (_color.GetBrightness() < 0.5f) { _lvi.ForeColor = Color.White; }
                _lvi.SubItems.Add(_color.R.ToString());
                _lvi.SubItems.Add(_color.G.ToString());
                _lvi.SubItems.Add(_color.B.ToString());
                _lvi.SubItems.Add(_color.GetBrightness().ToString());
                _lvi.SubItems.Add(_color.GetHue().ToString());
                _lvi.SubItems.Add(_color.GetSaturation().ToString());
                _lvi.SubItems.Add(_ic.ToString());
            }
            _lv_kcs.AutoResizeColumns(ColumnHeaderAutoResizeStyle.HeaderSize);
            _lv_kcs.EndUpdate();

        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            base.OnKeyPress(e);
        }

        private void button1_Click(object sender, EventArgs e)
        {
        }

        private void _tb_TextChanged(object sender, EventArgs e)
        {
            try
            {
                _curcolor = Color.FromArgb(255, int.Parse(_tbR.Text), int.Parse(_tbG.Text), int.Parse(_tbB.Text));
                _update_color();
            }
            catch (Exception _ex) { lab_color.Text = _ex.Message; lab_color.BackColor = SystemColors.Control; lab_color.ForeColor = SystemColors.ControlText; }
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            Color _clr = lab_color.BackColor;
            long _mindiff = long.MaxValue;

            foreach (Color _nclr in _list_colors)
            {
                long _diff = Math.Abs(_clr.R - _nclr.R);
                _diff *= Math.Abs(_clr.G - _nclr.G);
                _diff *= Math.Abs(_clr.B - _nclr.B);
                if (_diff < _mindiff) { _mindiff = _diff; _curcolor = _nclr; }
            }

            _update_color();
        }

        private void _update_color()
        {
            KnownColor _nk; _dict_nks.TryGetValue(_curcolor.ToArgb() & 0xFFFFFF, out _nk);
            lab_color.Text = _nk.ToString(); lab_color.BackColor = _curcolor; lab_color.ForeColor = _curcolor.GetBrightness() > 0.5f ? Color.Black : Color.White;
        }
    }
}
