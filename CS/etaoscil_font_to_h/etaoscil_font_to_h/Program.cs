using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Drawing;
using System.IO;

namespace etaoscil_font_to_h
{
    class Program
    {
        private const int __CHAR_COLONS = 16, __CHAR_ROWS = 12, __CHAR_MIDDLE_ROWS = 6;

        private const string __HEADER_HEAD =
@"/// Generated with etaoscil_font_to_h

#define {1}_HEIGHT {2}
#define {1}_CHARS {3}

const unsigned char {0}_bmp_regular[] = {{";
        private const string __HEADER_MIDDLE =
@"
}};

const unsigned char {0}_bmp_bold[] = {{";
        private const string __HEADER_TAIL =
@"
}};
";
        private const string __HEADER_TAIL_NO_BOLD =
@"
}};

#define {0}_bmp_bold 0
";
        static int Main(string[] args) {
            if (args.Length < 2 || args.Length > 3) return Usage(null);
            try { return Perform(args[0], args[1], args.Length == 3 ? args[2] : Path.GetFileNameWithoutExtension(args[0])); }
            catch (Exception ex) { return Usage(string.Format("Unhandled exception: {0}", ex.Message)); }
        }

        static int Perform(string filename_font, string filename_header, string header_var_name) {
            header_var_name = header_var_name.ToLower();
            StringBuilder _sb_header = new StringBuilder();
            try {
                Image _image = Image.FromFile(filename_font); ImageFormat _image_format = _image.RawFormat;
                using (Bitmap _bitmap = new Bitmap(_image)) {
                    _image.Dispose();
                    int _char_width, _char_height, _remw, _remh;
                    _char_width = Math.DivRem(_bitmap.Width, __CHAR_COLONS, out _remw); _char_height = Math.DivRem(_bitmap.Height, __CHAR_ROWS, out _remh);
                    if (_remw != 0 || _remh != 0) return Usage("Invalid size of font image");
                    if (_char_width > 8) return Usage("Too big width of font image");

                    _sb_header.Append(string.Format(__HEADER_HEAD, header_var_name, header_var_name.ToUpper(), _char_height, __CHAR_COLONS * __CHAR_MIDDLE_ROWS));

                    bool _is_bold_diffrent = false;
                    for (int _y = 0; _y < __CHAR_MIDDLE_ROWS * _char_height; _y++) {
                        for (int _x = 0; _x < __CHAR_COLONS * _char_width; _x++) {
                            _is_bold_diffrent = ((_bitmap.GetPixel(_x, _y).ToArgb() & 0xFFFFFF) != 0) != ((_bitmap.GetPixel(_x, _y + __CHAR_MIDDLE_ROWS * _char_height).ToArgb() & 0xFFFFFF) != 0);
                            if (_is_bold_diffrent) break;
                        }
                        if (_is_bold_diffrent) break;
                    }

                    byte _char_code = 0x20; bool _do_break = false;
                    for (int _ch = 0; _ch < __CHAR_ROWS; _ch++) {
                        for (int _cw = 0; _cw < __CHAR_COLONS; _cw++, _char_code++) {
                            // scan char pixels from bitmap
                            byte[] _bytes_char = new byte[_char_height];
                            for (int _cch = 0; _cch < _char_height; _cch++) {
                                byte _byte_char_row = 0;
                                for (int _ccw = 0; _ccw < _char_width; _ccw++) {
                                    _byte_char_row <<= 1;
                                    if ((_bitmap.GetPixel(_cw * _char_width + _char_width - _ccw - 1, _ch * _char_height + _cch).ToArgb() & 0xFFFFFF) != 0) _byte_char_row |= 1;
                                }
                                _bytes_char[_cch] = _byte_char_row;
                            }

                            // optimize symbol pixels and calculate char actual width
                            int _caw_l = 0, _caw_r = 8, _caw = 2;
                            for (int _cch = 0; _cch < _char_height; _cch++) {
                                byte _byte_char_row = _bytes_char[_cch];
                                for (int _i = 0; _i < 8; _i++) {
                                    if ((_byte_char_row & 0x1) != 0) {
                                        if (_caw_l < _i) _caw_l = _i;
                                        if (_caw_r > _i) _caw_r = _i;
                                    }
                                    _byte_char_row >>= 1;
                                }
                            }
                            _caw = _caw_l - _caw_r; if (_caw < 0) { _caw_l = _caw_r = 0; _caw = _char_height >> 1; } else _caw++;
                            if (_caw_r > 0) { for (int _cch = 0; _cch < _char_height; _cch++) { _bytes_char[_cch] >>= _caw_r; } }

                            // Correct image file
                            for (int _cch = 0; _cch < _char_height; _cch++) {
                                byte _byte_char_row = _bytes_char[_cch];
                                for (int _ccw = 0; _ccw < _char_width; _ccw++) {
                                    _bitmap.SetPixel(_cw * _char_width + _ccw, _ch * _char_height + _cch, (_byte_char_row & 0x1) != 0 ? Color.White : Color.Black);
                                    _byte_char_row >>= 1;
                                }
                            }

                            // append to header file
                            if (_ch == __CHAR_MIDDLE_ROWS && _cw == 0) {
                                if (!_is_bold_diffrent) { _do_break = true; break; }
                                _sb_header.Append(string.Format(__HEADER_MIDDLE, header_var_name)); _char_code = 0x20;
                            }
                            for (int _cch = 0; _cch < _char_height; _cch++) {
                                _sb_header.Append(_cch > 0 ? " " : "\n\t");
                                _sb_header.AppendFormat("0x{0:X2},", _bytes_char[_cch]);
                            }
                            _sb_header.AppendFormat(" {0}{1}", _caw, (_ch != __CHAR_ROWS - 1 && _ch != __CHAR_MIDDLE_ROWS - 1) || _cw != __CHAR_COLONS - 1 ? "," : "");
                            _sb_header.AppendFormat("\t\t// char 0x{0:X2} '{1}'", _char_code, (char)_char_code);
                        }
                        if (_do_break) break;
                    }

                    _bitmap.Save(filename_font, _image_format);
                    _sb_header.Append(string.Format(_is_bold_diffrent ? __HEADER_TAIL : __HEADER_TAIL_NO_BOLD, header_var_name));
                }
            }
            catch (IOException ex) { return Usage(string.Format("Can't open font image for reading: {0}", ex.Message)); }

            try {
                using (StreamWriter _stream_writer = new StreamWriter(filename_header)) {
                    _stream_writer.Write(_sb_header.ToString());
                }
            }
            catch (IOException ex) { return Usage(string.Format("Can't open header file for writing: {0}", ex.Message)); }
            return 0;
        }

        static int Usage(string comment) {
            if (comment != null) Console.WriteLine(comment);
            Console.WriteLine(@"etaoscil_font_to_h by SW (c) 2014

Usage:
    etaoscil_font_to_h fontfile.png output.fontheader [VAR_NAME]
        fontfile.png        - inpuit font filename 16x6 charmap image (.png .bmp .jpg) where non-black dots is char dots
        output.fontheader   - output filename of C compatible header with font data
        VAR_NAME            - optional var name used in header file (by default is a fontfile.png without extention eg. 'fontfile'");
            return 1;
        }
    }
}
