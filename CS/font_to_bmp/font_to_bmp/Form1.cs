using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace font_to_bmp
{
    public partial class Form1 : Form
    {
        public Form1() {
            InitializeComponent();
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e) { fontBitmap1.FntWidth = (int)numericUpDown1.Value; numericUpDown1.Value = fontBitmap1.FntWidth; }

        private void numericUpDown2_ValueChanged(object sender, EventArgs e) { fontBitmap1.FntHeight = (int)numericUpDown2.Value; numericUpDown2.Value = fontBitmap1.FntHeight; }

        private void Form1_Load(object sender, EventArgs e) {
            numericUpDown1.Value = fontBitmap1.FntWidth;
            numericUpDown2.Value = fontBitmap1.FntHeight;
            textBox1.Text = fontBitmap1.Font.ToString();
        }

        private void button1_Click(object sender, EventArgs e) {
            fontDialog1.Font = fontBitmap1.Font;
            if (fontDialog1.ShowDialog() == DialogResult.OK) {
                fontBitmap1.Font = fontDialog1.Font;
                textBox1.Text = fontBitmap1.Font.ToString();
            }
        }
    }
}
