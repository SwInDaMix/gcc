using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Drawing;
using System.IO;

namespace image_to_h
{
    class Program
    {
        private const string __HEADER_HEAD =
@"/// Generated with image_to_h

#define {1}_IMAGE_WIDTH {3}
#define {1}_IMAGE_HEIGHT {4}
#define {1}_IMAGE_CONTAINER {5}

const {2} {0}_imgbmp[] = {{";
        private const string __HEADER_TAIL =
@"
};
";
        private static bool _is_use_pixel_aproximation = false;
        private static Random _rnd = new Random();

        static int Main(string[] args) {
            if (args.Length < 2 || args.Length > 3) return Usage(null);
            try { return Perform(args[0], args[1], int.Parse(args[2]), args.Length == 4 ? args[3] : Path.GetFileNameWithoutExtension(args[0])); }
            catch (Exception ex) { return Usage(string.Format("Unhandled exception: {0}", ex.Message)); }
        }

        static int Perform(string filename_image, string filename_header, int pixelbits, string header_var_name) {
            header_var_name = header_var_name.ToLower();
            StringBuilder _sb_header = new StringBuilder();
            List<string> _list_imgbmp_values = new List<string>();
            string _imgbmp_value_type, _img_container;
            _is_use_pixel_aproximation = header_var_name.EndsWith("_upa");

            if (pixelbits == 1) { _imgbmp_value_type = "unsigned char"; _img_container = "CImage1"; }
            else if (pixelbits == 8) { _imgbmp_value_type = "lcd_color8"; _img_container = "CImage8"; }
            else if (pixelbits == 16) { _imgbmp_value_type = "lcd_color16"; _img_container = "CImage16"; }
            else throw new ArgumentException("Invalid pixel bits width (ca be only 16, 8 or 1)", "pixelbits");

            try {
                using (Bitmap _bitmap = new Bitmap(Image.FromFile(filename_image))) {
                    int _img_width = _bitmap.Width, _img_height = _bitmap.Height;

                    _sb_header.Append(string.Format(__HEADER_HEAD, header_var_name, header_var_name.ToUpper(), _imgbmp_value_type, _img_width, _img_height, _img_container));

                    // generate string imgbmp values
                    byte _byte_1b_value = 0; byte _byte_1b_bit = 0x01;
                    for (int _y = 0; _y < _img_height; _y++) {
                        for (int _x = 0; _x < _img_width; _x++) {
                            Color _color = _bitmap.GetPixel(_x, _y);
                            if (pixelbits == 1) {
                                _byte_1b_value |= (((_color.ToArgb() & 0xFFFFFF) != 0) ? _byte_1b_bit : (byte)0x00);
                                if (_byte_1b_bit == 0x80) {
                                    _list_imgbmp_values.Add(_byte_1b_value.ToString("X02"));
                                    _byte_1b_value = 0; _byte_1b_bit = 0x01;
                                }
                                else _byte_1b_bit <<= 1;
                            }
                            else if (pixelbits == 8) {
                                _list_imgbmp_values.Add((PixelComponentAproximation(_color.R, 3) | (PixelComponentAproximation(_color.G, 3) << 3) | (PixelComponentAproximation(_color.B, 2) << 6)).ToString("X02"));
                            }
                            else if (pixelbits == 16) {
                                _list_imgbmp_values.Add((PixelComponentAproximation(_color.R, 5) | (PixelComponentAproximation(_color.G, 6) << 5) | (PixelComponentAproximation(_color.B, 5) << 11)).ToString("X04"));
                            }
                        }
                    }
                    if (pixelbits == 1 && _byte_1b_bit != 0x01) _list_imgbmp_values.Add(_byte_1b_value.ToString("X02"));

                    // append string imgbmp values
                    bool is_not_first = false;
                    for (int _i = 0; _i < _list_imgbmp_values.Count; _i++) {
                        if (is_not_first) _sb_header.Append(','); is_not_first = true;
                        _sb_header.Append((_i & 0xF) == 0 ? "\n\t" : " ");
                        _sb_header.Append("0x" + _list_imgbmp_values[_i]);
                    }

                    _sb_header.Append(__HEADER_TAIL);
                }
            }
            catch (IOException ex) { return Usage(string.Format("Can't open image for reading: {0}", ex.Message)); }

            try {
                using (StreamWriter _stream_writer = new StreamWriter(filename_header)) {
                    _stream_writer.Write(_sb_header.ToString());
                }
            }
            catch (IOException ex) { return Usage(string.Format("Can't open header file for writing: {0}", ex.Message)); }
            return 0;
        }
        static byte PixelComponentAproximation(byte pixel_component, int bits) {
            byte _mask = (byte)((1 << (8 - bits)) - 1);
            byte _res = (byte)(pixel_component >> (8 - bits)), _rem = (byte)(pixel_component & _mask);
            if (_is_use_pixel_aproximation && _res != (byte)(255 >> (8 - bits)) && _rnd.Next(0, _mask + 1) < _rem) _res++;
            return _res;
        }

        static int Usage(string comment) {
            if (comment != null) Console.WriteLine(comment);
            Console.WriteLine(@"image_to_h by SW (c) 2014

Usage:
    image_to_h imgfile.png output.imgheader pixelbits [VAR_NAME]
        imgfile.png         - inpuit image filename (.png .bmp .jpg)
        output.imgheader    - output filename of C compatible header with image data
        pixelbits           - image pixel bits (16, 8, 1)
        VAR_NAME            - optional var name used in header file (by default is a imgfile.png without extention eg. 'imgfile'");
            return 1;
        }
    }
}
