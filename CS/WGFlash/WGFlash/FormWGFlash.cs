using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using EtaIntelHEX;

namespace WindowsFormsApplication1
{
    public partial class FormWGFlash : Form
    {
        private string _filename = string.Empty;
        private IntelHex _intelhex;
        private const int _pagewidthshift = 6;
        private const int _pagesize = (1 << (_pagewidthshift + 1));

        public FormWGFlash()
        {
            InitializeComponent();
            _cb_port.Items.AddRange(SerialPort.GetPortNames()); _cb_port.SelectedIndex = 0;
        }

        private void _btn_browse_Click(object sender, EventArgs e)
        {
            DialogResult _dr = _ofd_firmware.ShowDialog();
            if (_dr == DialogResult.OK)
            {
                _filename = _ofd_firmware.FileName;
                _tb_firmware.Text = _filename;
                _fsw_firmware.Path = Path.GetDirectoryName(_filename);
                _fsw_firmware.EnableRaisingEvents = true;
                _ReloadFile();
            }
            else
            {
                _tb_firmware.Text = string.Empty;
                _fsw_firmware.EnableRaisingEvents = false;
            }
        }
        private void _fsw_firmware_event(object sender, FileSystemEventArgs e)
        {
            if (string.Compare(Path.GetFileName(e.FullPath), Path.GetFileName(_filename), true) == 0) { _ReloadFile(); }
        }
        private void _ReloadFile()
        {
            StreamReader _sr = null;
            string _tbv = string.Empty, _tbf = null;
            Color _tbfb = Color.Black;
            bool _bte = false;
            try
            {
                _sr = new StreamReader(_filename); _intelhex = null;
                IntelHex _intelhex_new = new IntelHex(_sr);
                _intelhex_new.BuildData(_pagewidthshift + 1);
                byte[] _firmware_data = _intelhex_new.Data;
                if (_intelhex_new.DataAddressMin == 0 && _intelhex_new.Data[84] == 0x47 && _intelhex_new.Data[85] == 0x57)
                {
                    StringBuilder _sb = new StringBuilder();
                    for (int _i = 86; _i < _intelhex_new.DataAddressMax; _i++) { byte _b = _intelhex_new.Data[_i]; if (_b == 0) break; _sb.Append((char)(_b)); }
                    
                    _tbv = _sb.ToString();
                    _intelhex = _intelhex_new; _bte = true;
                }
                else _tbv = "<unrecognized>";
                //for (int _i = 0; _i < _firmware_data.Length; _i += 2) { byte _t = _firmware_data[_i]; _firmware_data[_i] = _firmware_data[_i + 1]; _firmware_data[_i + 1] = _t; }
                _tbfb = SystemColors.Control;
            }
            catch (IOException ex) { _tbv = ex.Message; _tbfb = Color.Red; }
            finally { if (_sr != null) _sr.Close(); }
            Invoke((Action)(() =>
            {
                _tb_version.Text = _tbv; _tb_firmware.BackColor = _tbfb; _btn_program.Enabled = _bte;
                _tb_firmware.Text = _tbf != null ? _tbf : _filename;
            }));
        }
        private void _btn_program_Click(object sender, EventArgs e)
        {
            Thread _thread_program = new Thread(() =>
            {
                IntelHex _ihex = _intelhex;
                if (_ihex != null)
                {
                    uint _amin = _ihex.DataAddressMin, _amax = _ihex.DataAddressMax;
                    unchecked
                    {
                        byte[] _firmware_data = _ihex.Data;
                        Invoke((Action)(() => { _comport.PortName = (string)_cb_port.SelectedItem; }));
                        try
                        {
                            Invoke((Action)(() => { _pb_program.Minimum = (int)_amin; _pb_program.Maximum = (int)_amax; }));
                            _comport.Open();
                            _comport.Write("reset\r");
                            Thread.Sleep(300);
                            _comport.Write(new byte[] { 9 }, 0, 1);
                            Thread.Sleep(100);

                            WGSPResult _res = 0;
                            for (uint _addr = _amin; _addr < _amax; _addr += _pagesize)
                            {
                                Invoke((Action)(() => { _pb_program.Value = (int)_addr + (_pagesize >> 1); }));
                                ushort _page = (ushort)(_addr >> (_pagewidthshift + 1));
                                ushort _chksum = 0;

                                for (int _i = 0; _i < (1 << _pagewidthshift); _i++)
                                {
                                    ushort _v = (ushort)(_firmware_data[_addr + (_i << 1) + 1] << 8);
                                    _v |= (ushort)(_firmware_data[_addr + (_i << 1)]);
                                    _chksum = (ushort)((_chksum ^ _v) * 40503);
                                }

                                string _str = string.Format("flash x{0:X},x{1:X}\r", _page, _chksum);
                                _comport.Write(_str); _comport.ReadTo(_str + "\n");

                                _comport.Write(_firmware_data, (int)_addr, _pagesize);

                                _res = (WGSPResult)_comport.ReadByte();
                                if (_res != 0) { MessageBox.Show(string.Format("Error '{0}' when writing page {1} {2:X}", _res, _page, _chksum)); break; }
                            }
                            bool _b_wgmode = false;
                            Invoke((Action)(() => { _b_wgmode = _cb_wgmode.Checked; }));
                            if (_b_wgmode) { _comport.ReadTo("bl>\\"); _comport.Write("wg\r"); }
                            _comport.Close();
                        }
                        catch (Exception ex) { MessageBox.Show(ex.Message); }
                        finally
                        {
                            Invoke((Action)(() => { _pb_program.Value = 0; _btn_program.Enabled = true; }));
                            if (_comport.IsOpen) _comport.Close();
                        }
                    }
                }
            });
            _btn_program.Enabled = false; _thread_program.Start(); 
        }
    }

    public enum WGSPResult { OK, PageOverflow, ChecksumMissmatch, HardwareMulfunction }
}
