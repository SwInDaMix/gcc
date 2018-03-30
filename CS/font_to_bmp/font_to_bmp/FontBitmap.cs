using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace font_to_bmp
{
    public partial class FontBitmap : UserControl
    {
        private int d_fnt_width = 8;
        private int d_fnt_height = 10;

        public int FntWidth { get { return d_fnt_width; } set { if (value > 6 && value < 40) { d_fnt_width = value; Invalidate(); } } }
        public int FntHeight { get { return d_fnt_height; } set { if (value > 6 && value < 40) { d_fnt_height = value; Invalidate(); } } }

        public FontBitmap() {
            InitializeComponent();
        }

        private void FontBitmap_FontChanged(object sender, EventArgs e) {
            Invalidate();
        }

        private void FontBitmap_Paint(object sender, PaintEventArgs e) {
            Graphics _g = e.Graphics;

            using (Brush _brush = new SolidBrush(ForeColor)) {
                for (int _y = 0; _y < 6; _y++) {
                    for (int _x = 0; _x < 16; _x++) {
                        e.Graphics.DrawString(((char)(0x20 + _y * 16 + _x)).ToString(), Font, _brush, _x * d_fnt_width, _y * d_fnt_height);
                    }
                }
            }
        }
    }
}
