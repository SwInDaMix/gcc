using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace EtaIntelHEX
{
    public class IntelHex
    {
        private CRecord[] _records;
        private byte[] _data;
        private uint _address_min, _address_max, _data_address_min, _data_address_max;

        public CRecord[] Records { get { return _records; } }
        public byte[] Data { get { return _data; } }
        public uint AddressMin { get { return _address_min; } }
        public uint AddressMax { get { return _address_max; } }
        public uint DataAddressMin { get { return _data_address_min; } }
        public uint DataAddressMax { get { return _data_address_max; } }

        public IntelHex(TextReader tr)
        {
            List<CRecord> _list_records = new List<CRecord>();
            string _record_string;
            uint _offset = 0; _address_min = uint.MaxValue;
            while ((_record_string = tr.ReadLine()) != null)
            {
                CRecord _record = new CRecord(_record_string);
                _list_records.Add(_record);

                if (_record.Type == EType.Data)
                {
                    uint _amin = _record.Offset, _amax = _amin + _record.Length;
                    if (_address_min > _amin) _address_min = _amin;
                    if (_address_max < _amax) _address_max = _amax;
                }
            }
            _records = _list_records.ToArray();
        }

        public byte[] BuildData(int alignment_shift)
        {
            _data_address_min = _address_min & ~(((uint)1 << alignment_shift) - 1);
            _data_address_max = _address_max + (((uint)1 << alignment_shift) - 1); _data_address_max &= ~(((uint)1 << alignment_shift) - 1);

            _data = new byte[_data_address_max - _data_address_min];
            foreach (IntelHex.CRecord _record in _records)
            {
                if (_record.Type == IntelHex.EType.Data)
                {
                    _record.Data.CopyTo(_data, _record.Offset - _data_address_min);
                }
            }

            return _data;
        }

        public class CRecord
        {
            private byte _length;
            private ushort _offset;
            private EType _type;
            private byte[] _data;

            public byte Length { get { return _length; } }
            public ushort Offset { get { return _offset; } }
            public EType Type { get { return _type; } }
            public byte[] Data { get { return _data; } }

            public CRecord(string record_string)
            {
                int _li = 0;
                if (record_string.Length < 1) throw new ArgumentException("Unexpected end of record", "record_string");
                if (record_string[_li++] != ':') { throw new ArgumentException("No record mark", "record_string"); }

                byte _crc = 0;
                _length = ReadByte(record_string, ref _li, ref _crc);
                _offset = ReadShort(record_string, ref _li, ref _crc);
                _type = (EType)ReadByte(record_string, ref _li, ref _crc);
                _data = new byte[_length];

                for (int _i = 0; _i < _length; _i++)
                {
                    _data[_i] = ReadByte(record_string, ref _li, ref _crc);
                }
                ReadByte(record_string, ref _li, ref _crc);

                if (_crc != 0) throw new ArgumentException("Wrong checksum of the record", "record_string");
            }

            private byte ReadByte(string record_string, ref int li, ref byte crc)
            {
                if (record_string.Length < li + 2) throw new ArgumentException("Unexpected end of record", "record_string");
                byte _res = byte.Parse(record_string.Substring(li, 2), System.Globalization.NumberStyles.HexNumber);
                li += 2; crc += _res; return _res;
            }
            private ushort ReadShort(string record_string, ref int li, ref byte crc)
            {
                ushort _res = (ushort)(ReadByte(record_string, ref li, ref crc) << 8);
                _res += ReadByte(record_string, ref li, ref crc);
                return _res;
            }
        }

        public enum EType : byte
        {
            Data = 0x00,
            EOF = 0x01,
            ExtendedSegmentAddress = 0x02,
            StartSegmentAddress = 0x03,
            ExtendedLinearAddress = 0x04,
            StartLinearAddress = 0x05
        }
    }
}
