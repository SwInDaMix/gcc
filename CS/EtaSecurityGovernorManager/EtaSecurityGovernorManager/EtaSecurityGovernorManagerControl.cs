using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace EtaSecurityGovernorManager
{
    public class EtaSecurityGovernorManagerControl : INotifyPropertyChanged
    {
        private const byte __FRAME_ADRESS = 0;

        internal string d_connection_port;
        internal int d_connection_port_baudrate;
        internal EtaConnectionFrames d_connection_frames;
        internal CAccessorRegisterRegistry d_accessor_register_registry;

        public string ConnectionPort { get { return d_connection_port; } set { d_connection_port = value; EtaSettings.DeviceSerialPort = d_connection_port; OnPropertyChanged(nameof(ConnectionPort)); } }
        public int ConnectionPortBaudrate { get { return d_connection_port_baudrate; } set { d_connection_port_baudrate = value; EtaSettings.DeviceSerialPortBaudRate = d_connection_port_baudrate; OnPropertyChanged(nameof(ConnectionPortBaudrate)); } }
        public bool IsConnected => d_connection_frames != null;
        public CAccessorRegisterRegistry AccessorRegisterRegistry => d_accessor_register_registry;
        public bool IsValid => d_accessor_register_registry != null;

        public EtaSecurityGovernorManagerControl() {
            d_connection_port = EtaSettings.DeviceSerialPort;
            d_connection_port_baudrate = EtaSettings.DeviceSerialPortBaudRate;
        }

        public void Connect() { Disconnect(); d_connection_frames = new EtaConnectionFrames(() => { d_connection_frames = null; d_accessor_register_registry = null; OnPropertyChanged(nameof(IsConnected)); OnPropertyChanged(nameof(AccessorRegisterRegistry)); }, d_connection_port, d_connection_port_baudrate, _AsyncFrameProcessor); OnPropertyChanged(nameof(IsConnected)); }
        public void Disconnect() { EtaConnectionFrames _connection_frames_temp = d_connection_frames; _connection_frames_temp?.Terminate(); }

        public void Reload() { Task.Run(async () => await ReloadAsync()).Wait(); }
        public async Task ReloadAsync() {
            d_accessor_register_registry = new CAccessorRegisterRegistry(this);
            await d_accessor_register_registry?.UpdateAsync();
            OnPropertyChanged(nameof(AccessorRegisterRegistry)); OnPropertyChanged(nameof(IsValid));
        }

        public void AllowRegistration() { PushFrame(EFrameCommand.AllowRegistration, new CID().ToBytes()); }
        public void TestRFInfo() { PushFrame(EFrameCommand.TestRFInfo, null); }

        public void TestEEPROMPagesWrite() { PushFrame(EFrameCommand.TestEEPROMPagesWrite, null); }
        public void TestEEPROMPagesRead() { PushFrame(EFrameCommand.TestEEPROMPagesRead, null); }
        public void TestEEPROMPagesErase() { PushFrame(EFrameCommand.TestEEPROMPagesErase, null); }
        public void TestGetADC() { PushFrame(EFrameCommand.TestGetADC, null); }
        public void TestSwitchSink() { PushFrame(EFrameCommand.TestSwitchSink, null); }

        public void TestLock(bool is_forced) { PushFrame(EFrameCommand.TestLock, BitConverter.GetBytes(is_forced)); }
        public void TestUnlock(bool is_forced) { PushFrame(EFrameCommand.TestUnlock, BitConverter.GetBytes(is_forced)); }
        public void TestGetLockStatus() { PushFrame(EFrameCommand.TestGetLockStatus, null); }

        public void PushFrame(EFrameCommand frame_command, byte[] frame_data) { EtaConnectionFrames _connection_frames_temp = d_connection_frames; _connection_frames_temp?.PushAsyncFrame(__FRAME_ADRESS, (byte)frame_command, frame_data); }
        public byte[] RequestFrame(EFrameCommand frame_command, byte[] frame_data, int timeout_ms) { EtaConnectionFrames _connection_frames_temp = d_connection_frames; return _connection_frames_temp?.RequestFrame(__FRAME_ADRESS, (byte)frame_command, frame_data, timeout_ms); }
        public Task<byte[]> RequestFrameAsync(EFrameCommand frame_command, byte[] frame_data, int timeout_ms) { EtaConnectionFrames _connection_frames_temp = d_connection_frames; return _connection_frames_temp?.RequestFrameAsync(__FRAME_ADRESS, (byte)frame_command, frame_data, timeout_ms); }

        private void _AsyncFrameProcessor(byte frame_address, byte frame_command, byte[] frame_data) {
            EFrameCommand _command = (EFrameCommand)frame_command; int _frame_data_length = frame_data.Length, _offset = 0;
            if (_command == EFrameCommand.DebugString) {
                EtaDebug.DebugWrite(ConsoleColor.White, false, Encoding.GetEncoding(1251).GetString(frame_data));
            }

            else if (frame_address == __FRAME_ADRESS) {
                // Ordinal Events
                if (_command == EFrameCommand.OrdinalEvent_ChallengeInitiated && _frame_data_length == CID.StructureSize + 1) {
                    CID _id = new CID(frame_data, ref _offset);
                    EAction _action = (EAction)frame_data[_offset++];
                    EtaDebug.DebugWrite(ConsoleColor.Cyan, true, "Accessor {0} initiates a new challenge with action {1}", _id, _action);
                }
                else if (_command == EFrameCommand.OrdinalEvent_ChallengeWonWithPrimaryKey && _frame_data_length == CID.StructureSize + 1) {
                    CID _id = new CID(frame_data, ref _offset);
                    EAction _action = (EAction)frame_data[_offset++];
                    EtaDebug.DebugWrite(ConsoleColor.Cyan, true, "Accessor {0} won the challenge with primary key and action {1}", _id, _action);
                }
                else if (_command == EFrameCommand.OrdinalEvent_ChallengeWonWithSecondaryKey && _frame_data_length == CID.StructureSize + 1) {
                    CID _id = new CID(frame_data, ref _offset);
                    EAction _action = (EAction)frame_data[_offset++];
                    d_accessor_register_registry?.AccessorRegisterUpdate(_id);
                    EtaDebug.DebugWrite(ConsoleColor.Cyan, true, "Accessor {0} won the challenge with secondary key and action {1}, secondary key will be set as primary", _id, _action);
                }
                else if (_command == EFrameCommand.OrdinalEvent_ChallengeKeyReplacement && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    d_accessor_register_registry?.AccessorRegisterUpdate(_id);
                    EtaDebug.DebugWrite(ConsoleColor.Cyan, true, "Governor initiates key replacement for accessor {0}", _id);
                }
                else if (_command == EFrameCommand.OrdinalEvent_ChallengeAnotherTryKeyReplacement && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    d_accessor_register_registry?.AccessorRegisterUpdate(_id);
                    EtaDebug.DebugWrite(ConsoleColor.Cyan, true, "Governor initiates another try of key replacement for accessor {0}", _id);
                }
                else if (_command == EFrameCommand.OrdinalEvent_RegistrationInitiated && _frame_data_length == CID.StructureSize + 4) {
                    CID _id = new CID(frame_data, ref _offset);
                    uint _registration_sequence = BitConverter.ToUInt32(frame_data, _offset);
                    _offset += 4;
                    CAccessorRegisterRegistry.CRegistration _registration = d_accessor_register_registry?.RegistrationInitiate(_id, _registration_sequence);
                    EtaDebug.DebugWrite(ConsoleColor.Cyan, true, "Accessor {0} initiates a new registration with sequence {1}", _registration.ID, _registration.RegistrationSequenceString);
                }
                else if (_command == EFrameCommand.OrdinalEvent_RegistrationWonKeyReplacement && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    CAccessorRegisterRegistry.CRegistration _registration = d_accessor_register_registry?.RegistrationWonKeyReplacement(_id);
                    EtaDebug.DebugWrite(ConsoleColor.Cyan, true, "Accessor {0} won the registration challenge with sequence key and key replacement initiated", _id);
                }
                else if (_command == EFrameCommand.OrdinalEvent_RegistrationConfirmed && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    CAccessorRegisterRegistry.CRegistration _registration = d_accessor_register_registry?.RegistrationConfirmed(_id);
                    EtaDebug.DebugWrite(ConsoleColor.Cyan, true, "Accessor {0} confirmed the registration challenge with re-encoded key", _id);
                }

                // Security Fails
                else if (_command == EFrameCommand.SecurityFail_PipeAlreadyRequested && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Pipe already requested by accessor {0}", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_CommunicationFailed && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Communication with accessor {0} failed", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_SessionIsNotInitiated && _frame_data_length == 1) {
                    byte _pipe = frame_data[_offset++];
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Session from pipe {0} is not initiated", _pipe);
                }
                else if (_command == EFrameCommand.SecurityFail_SessionWrongAccessor && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Session from wrong accessor {0}", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_SessionAlreadyFailed && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Session from accessor {0} already failed", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_SessionTimeout && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Session initiated by accessor {0} timeout", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_UnexpectedResponseForSession && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Unexpected response for session initiated by accessor {0}", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_NoRegistrationForAccessor && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Accessor {0} doesn't have registration on this governor", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_NoProperActionForButtons && _frame_data_length == CID.StructureSize + 1) {
                    CID _id = new CID(frame_data, ref _offset);
                    EActionPressedButtons _action_pressed_buttons = (EActionPressedButtons)frame_data[_offset++];
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Accessor {0} doesn't have proper action for buttons {1}", _id, _action_pressed_buttons);
                }
                else if (_command == EFrameCommand.SecurityFail_AccessorIsNotActive && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Accessor {0} found, but is not active", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_ChallengeLost && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Accessor {0} has lost the challenge", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_RegistrationNotAllowed && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Accessor {0} tries to register, but registration is not allowed", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_RegistrationFromWrongAccessor && _frame_data_length == CID.StructureSize * 2) {
                    CID _id_allowed = new CID(frame_data, ref _offset);
                    CID _id_from = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Registration allowed for accessor {0}, but received from accessor {1}", _id_allowed, _id_from);
                }
                else if (_command == EFrameCommand.SecurityFail_RegistrationAlreadyExists && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Registration for accessor {0} already exists", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_RegistrationLost && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    CAccessorRegisterRegistry.CRegistration _registration = d_accessor_register_registry?.RegistrationLost(_id);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Accessor {0} has lost the registration challenge", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_RegistrationDenied && _frame_data_length == CID.StructureSize) {
                    CID _id = new CID(frame_data, ref _offset);
                    CAccessorRegisterRegistry.CRegistration _registration = d_accessor_register_registry?.RegistrationDenied(_id);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Wrong confirmation from accessor {0} for registration challenge with re-encoded key", _id);
                }
                else if (_command == EFrameCommand.SecurityFail_RegistrationAllowanceTimeout && _frame_data_length == 0) {
                    d_accessor_register_registry?.RegistrationAllowTimeout();
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Registration allowance timeout");
                }
                else if (_command == EFrameCommand.SecurityFail_UnexpectedCommand && _frame_data_length == CID.StructureSize + 1) {
                    CID _id = new CID(frame_data, ref _offset);
                    EFrameCommand _unexpected_command_sent = (EFrameCommand)frame_data[_offset++];
                    d_accessor_register_registry?.RegistrationTimeout(_id);
                    EtaDebug.DebugWrite(ConsoleColor.Yellow, true, "Unexpected command {1} from accessor {0}", _id, _unexpected_command_sent);
                }

                // Not recognized command
                else {
                    EtaDebug.DebugWrite(ConsoleColor.DarkGray, true, "Unknown package: CMD - {0} (0x{0:X})", frame_command);
                    while (_frame_data_length > 0) {
                        int _this_len = Math.Min(16, _frame_data_length);
                        EtaDebug.DebugWrite(ConsoleColor.DarkGray, true, " {0}", BitConverter.ToString(frame_data, _offset, _this_len));
                        _offset += _this_len;
                        _frame_data_length -= _this_len;
                    }
                }
            }
            else {
                EtaDebug.DebugWrite(ConsoleColor.DarkGray, true, "Unknown package address: ADDR - {0}, CMD - {1} (0x{1:X})", frame_address, frame_command);
                while (_frame_data_length > 0) {
                    int _this_len = Math.Min(16, _frame_data_length);
                    EtaDebug.DebugWrite(ConsoleColor.DarkGray, true, " {0}", BitConverter.ToString(frame_data, _offset, _this_len));
                    _offset += _this_len;
                    _frame_data_length -= _this_len;
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; _handler?.Invoke(this, new PropertyChangedEventArgs(propery_name)); }

        [Flags]
        public enum EActionButtons
        {
            Button_0 = 0x01,
            Button_1 = 0x02,
            Button_2 = 0x04,
            Button_3 = 0x08,
            Button_Long = 0x10,
        }

        public enum EActionPressedButtons
        {
            Buttons_1,
            Buttons_2,
            Buttons_3,
            Buttons_4,
            Buttons_Long1,
            Buttons_Long2,
            Buttons_Long12,
            Buttons_Long3,
            Buttons_Long13,
            Buttons_Long23,
            Buttons_Long123,
            Buttons_Long4,
            Buttons_Long14,
            Buttons_Long24,
            Buttons_Long124,
            Buttons_Long34,
            Buttons_Long134,
            Buttons_Long234,
            Buttons_Long1234,
            Buttons__Last,
        }

        public enum EAction
        {
            [Description("No action")]
            None,
            [Description("Close entrance door")]
            CloseDoor,
            [Description("Open entrance door")]
            OpenDoor,
            [Description("Close entrance door (forced)")]
            CloseDoorForced,
            [Description("Open entrance door (forced)")]
            OpenDoorForced,
            [Description("Domophone auto opener")]
            DomophoneAutoOpener,
        }

        public enum ELog
        {
            OrdinalEvent,
            SecurityAlert,
            SystemError,
            HardwareFailure,
        }

        public abstract class CSecurityObject : INotifyPropertyChanged
        {
            protected CSecurityObject() { }
            protected CSecurityObject(byte[] data) { Parse(data); }
            protected CSecurityObject(byte[] data, ref int offset) { Parse(data, ref offset); }

            public void Parse(byte[] data) { int _offset = 0; Parse(data, ref _offset); }
            public abstract void Parse(byte[] data, ref int offset);
            public abstract byte[] ToBytes();

            public event PropertyChangedEventHandler PropertyChanged;

            protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; _handler?.Invoke(this, new PropertyChangedEventArgs(propery_name)); }
        }

        public class CID : CSecurityObject, IEquatable<CID>
        {
            public byte[] ID { get; } = new byte[16];

            public bool IsClobbered => ID.All(b => b == 0);
            public bool IsFree => ID.All(b => b == 0xFF);
            public bool IsValid => !IsClobbered && !IsFree;

            public static int StructureSize => 16;

            public CID() { }
            public CID(byte[] data) : base(data) { }
            public CID(byte[] data, ref int offset) : base(data, ref offset) { }

            public override void Parse(byte[] data, ref int offset) { Array.Copy(data, offset, ID, 0, 16); offset += 16; OnPropertyChanged(nameof(ID)); OnPropertyChanged(nameof(IsClobbered)); OnPropertyChanged(nameof(IsFree)); OnPropertyChanged(nameof(IsValid)); }
            public override byte[] ToBytes() { return ID; }

            public bool Equals(CID other) {
                if (other == null) return false;
                for (int _i = 0; _i < 16; _i++) { if (ID[_i] != other.ID[_i]) return false; }
                return true;
            }

            public override string ToString() { return BitConverter.ToString(ID).Replace("-", string.Empty); }

            public override int GetHashCode() {
                int _crc32 = -1;
                foreach (byte _byte in ID) {
                    _crc32 ^= _byte;
                    for (int _i = 0; _i < 8; _i++) {
                        if ((_crc32 & 0x01) != 0) { _crc32 = (_crc32 >> 1) ^ unchecked((int)0xEDB88320); }
                        else { _crc32 >>= 1; }
                    }
                }
                return _crc32;
            }
        }

        public class CPayload : CSecurityObject
        {
            public byte[] Data { get; } = new byte[32];
            public bool IsFree { get { return Data.All(b => b == 0xFF); } }
            public bool IsValid => !IsFree;

            public static int StructureSize => 32;

            public CPayload() { }
            public CPayload(byte[] data) : base(data) { }
            public CPayload(byte[] data, ref int offset) : base(data, ref offset) { }

            public override void Parse(byte[] data, ref int offset) { Array.Copy(data, offset, Data, 0, 32); offset += 32; OnPropertyChanged(nameof(Data)); }
            public override byte[] ToBytes() { return Data; }

            public override string ToString() { return BitConverter.ToString(Data); }
        }

        public class CAccessorRegister : CSecurityObject
        {
            private const int SECURITY_ACCESSOR_ACTION_ENTRIES_COUNT = (int)EActionPressedButtons.Buttons__Last;
            private const int SECURITY_ACCESSOR_DESCRIPTION_COUNT = (44 - (int)EActionPressedButtons.Buttons__Last);

            private static readonly string[] s_action_entries_properties = new string[SECURITY_ACCESSOR_ACTION_ENTRIES_COUNT] { nameof(ActionButton1), nameof(ActionButton2), nameof(ActionButton3), nameof(ActionButton4), nameof(ActionButtonLong1), nameof(ActionButtonLong2), nameof(ActionButtonLong12), nameof(ActionButtonLong3), nameof(ActionButtonLong13), nameof(ActionButtonLong23), nameof(ActionButtonLong123), nameof(ActionButtonLong4), nameof(ActionButtonLong14), nameof(ActionButtonLong24), nameof(ActionButtonLong124), nameof(ActionButtonLong34), nameof(ActionButtonLong134), nameof(ActionButtonLong234), nameof(ActionButtonLong1234) };

            private byte[] d_original_binary;
            private string d_description;
            private readonly EAction[] d_action_entries = new EAction[SECURITY_ACCESSOR_ACTION_ENTRIES_COUNT];

            public CAccessorRegisterRegistry AccessorRegisterRegistryOwner { get; }
            public CID ID { get; } = new CID();
            public CPayload PrimaryKey { get; } = new CPayload();
            public CPayload SecondaryKey { get; } = new CPayload();
            public bool IsActive { get; private set; }
            public string Description { get { return d_description; } set { if (string.IsNullOrWhiteSpace(value)) d_description = null; else { d_description = value.TrimStart(); if (d_description.Length > SECURITY_ACCESSOR_DESCRIPTION_COUNT - 1) d_description = d_description.Substring(0, SECURITY_ACCESSOR_DESCRIPTION_COUNT - 1); } OnPropertyChanged(nameof(Description)); IsModified = true; OnPropertyChanged(nameof(IsModified)); } }
            public bool IsModified { get; private set; }

            public EAction ActionButton1 { get { return this[EActionPressedButtons.Buttons_1]; } set { this[EActionPressedButtons.Buttons_1] = value; } }
            public EAction ActionButton2 { get { return this[EActionPressedButtons.Buttons_2]; } set { this[EActionPressedButtons.Buttons_2] = value; } }
            public EAction ActionButton3 { get { return this[EActionPressedButtons.Buttons_3]; } set { this[EActionPressedButtons.Buttons_3] = value; } }
            public EAction ActionButton4 { get { return this[EActionPressedButtons.Buttons_4]; } set { this[EActionPressedButtons.Buttons_4] = value; } }
            public EAction ActionButtonLong1 { get { return this[EActionPressedButtons.Buttons_Long1]; } set { this[EActionPressedButtons.Buttons_Long1] = value; } }
            public EAction ActionButtonLong2 { get { return this[EActionPressedButtons.Buttons_Long2]; } set { this[EActionPressedButtons.Buttons_Long2] = value; } }
            public EAction ActionButtonLong12 { get { return this[EActionPressedButtons.Buttons_Long12]; } set { this[EActionPressedButtons.Buttons_Long12] = value; } }
            public EAction ActionButtonLong3 { get { return this[EActionPressedButtons.Buttons_Long3]; } set { this[EActionPressedButtons.Buttons_Long3] = value; } }
            public EAction ActionButtonLong13 { get { return this[EActionPressedButtons.Buttons_Long13]; } set { this[EActionPressedButtons.Buttons_Long13] = value; } }
            public EAction ActionButtonLong23 { get { return this[EActionPressedButtons.Buttons_Long23]; } set { this[EActionPressedButtons.Buttons_Long23] = value; } }
            public EAction ActionButtonLong123 { get { return this[EActionPressedButtons.Buttons_Long123]; } set { this[EActionPressedButtons.Buttons_Long123] = value; } }
            public EAction ActionButtonLong4 { get { return this[EActionPressedButtons.Buttons_Long4]; } set { this[EActionPressedButtons.Buttons_Long4] = value; } }
            public EAction ActionButtonLong14 { get { return this[EActionPressedButtons.Buttons_Long14]; } set { this[EActionPressedButtons.Buttons_Long14] = value; } }
            public EAction ActionButtonLong24 { get { return this[EActionPressedButtons.Buttons_Long24]; } set { this[EActionPressedButtons.Buttons_Long24] = value; } }
            public EAction ActionButtonLong124 { get { return this[EActionPressedButtons.Buttons_Long124]; } set { this[EActionPressedButtons.Buttons_Long124] = value; } }
            public EAction ActionButtonLong34 { get { return this[EActionPressedButtons.Buttons_Long34]; } set { this[EActionPressedButtons.Buttons_Long34] = value; } }
            public EAction ActionButtonLong134 { get { return this[EActionPressedButtons.Buttons_Long134]; } set { this[EActionPressedButtons.Buttons_Long134] = value; } }
            public EAction ActionButtonLong234 { get { return this[EActionPressedButtons.Buttons_Long234]; } set { this[EActionPressedButtons.Buttons_Long234] = value; } }
            public EAction ActionButtonLong1234 { get { return this[EActionPressedButtons.Buttons_Long1234]; } set { this[EActionPressedButtons.Buttons_Long1234] = value; } }
            public EAction this[EActionPressedButtons pressed_buttons] { get { return d_action_entries[(int)pressed_buttons]; } set { d_action_entries[(int)pressed_buttons] = value; IsModified = true; OnPropertyChanged(s_action_entries_properties[(int)pressed_buttons]); OnPropertyChanged(nameof(IsModified)); } }

            public static int StructureSize => CID.StructureSize + (CPayload.StructureSize * 2) + 4 + SECURITY_ACCESSOR_ACTION_ENTRIES_COUNT + SECURITY_ACCESSOR_DESCRIPTION_COUNT;

            public CAccessorRegister(CAccessorRegisterRegistry accessor_register_registry_owner, byte[] data) : base(data) { AccessorRegisterRegistryOwner = accessor_register_registry_owner; }
            public CAccessorRegister(CAccessorRegisterRegistry accessor_register_registry_owner, byte[] data, ref int offset) : base(data, ref offset) { AccessorRegisterRegistryOwner = accessor_register_registry_owner; }
            public CAccessorRegister(CAccessorRegisterRegistry accessor_register_registry_owner, CID id) { AccessorRegisterRegistryOwner = accessor_register_registry_owner; ID.Parse(id.ToBytes()); Update(); }

            public override void Parse(byte[] data, ref int offset) {
                d_original_binary = data;
                ID.Parse(data, ref offset);
                PrimaryKey.Parse(data, ref offset);
                SecondaryKey.Parse(data, ref offset);
                uint _active_bits = BitConverter.ToUInt32(data, offset); offset += 4;
                int _parity = 0; for (int _i = 0; _i < 32; _i++) { if ((_active_bits & (1 << _i)) != 0) _parity++; }
                IsActive = (_parity & 1) == 0;
                for (int _index = 0; _index < d_action_entries.Length; _index++) {
                    byte _action = data[offset++]; if (_action == 0xFF) _action = 0;
                    d_action_entries[_index] = (EAction)_action;
                }
                Encoding _enc = Encoding.ASCII;
                int _description_end_idx1 = Array.IndexOf<byte>(data, 0, offset, SECURITY_ACCESSOR_DESCRIPTION_COUNT), _description_end_idx2 = Array.IndexOf<byte>(data, 0xFF, offset, SECURITY_ACCESSOR_DESCRIPTION_COUNT);
                if (_description_end_idx1 == -1) _description_end_idx1 = offset + SECURITY_ACCESSOR_DESCRIPTION_COUNT; if (_description_end_idx2 == -1) _description_end_idx2 = offset + SECURITY_ACCESSOR_DESCRIPTION_COUNT;
                d_description = _enc.GetString(data, offset, Math.Min(_description_end_idx1, _description_end_idx2) - offset); offset += SECURITY_ACCESSOR_DESCRIPTION_COUNT;
                IsModified = false;
                OnPropertyChanged(nameof(ID)); OnPropertyChanged(nameof(PrimaryKey)); OnPropertyChanged(nameof(SecondaryKey)); OnPropertyChanged(nameof(IsActive)); OnPropertyChanged(nameof(Description)); OnPropertyChanged(nameof(IsModified));
                foreach (string _action_entries_property in s_action_entries_properties) { OnPropertyChanged(_action_entries_property); }
            }
            public override byte[] ToBytes() {
                byte[] _bytes = new byte[StructureSize]; int _offset = 0;
                ID.ToBytes().CopyTo(_bytes, _offset); _offset += CID.StructureSize;
                PrimaryKey.ToBytes().CopyTo(_bytes, _offset); _offset += CPayload.StructureSize;
                SecondaryKey.ToBytes().CopyTo(_bytes, _offset); _offset += CPayload.StructureSize;
                foreach (EAction _action_entry in d_action_entries) { _bytes[_offset++] = (byte)_action_entry; }
                if (d_description != null) { byte[] _bytes_name = Encoding.ASCII.GetBytes(d_description); _bytes_name.CopyTo(_bytes, _offset); _bytes[_offset + _bytes_name.Length] = 0; } else { _bytes[_offset] = 0; }
                _offset += SECURITY_ACCESSOR_DESCRIPTION_COUNT;
                return _bytes;
            }

            public void Update() { Task.Run(async () => await UpdateAsync()).Wait(); }
            public async Task UpdateAsync() {
                if (!ID.IsValid) throw new InvalidOperationException("Cannot update AccessorRegister with invalid ID");
                byte[] _get_accessor_register_by_index_response = await AccessorRegisterRegistryOwner.GovernorManagerControlOwner.RequestFrameAsync(EFrameCommand.GetAccessorRegister, ID.ToBytes(), 500);
                if (_get_accessor_register_by_index_response == null || _get_accessor_register_by_index_response.Length != CAccessorRegister.StructureSize) throw new InvalidOperationException("No or invalid response length for command GetAccessorRegister");
                Parse(_get_accessor_register_by_index_response);
            }

            public void Apply() { Task.Run(async () => await ApplyAsync()).Wait(); }
            public async Task ApplyAsync() {
                if (!ID.IsValid) throw new InvalidOperationException("Cannot save AccessorRegister with invalid ID");
                byte[] _frame_data_bytes = new byte[CID.StructureSize + SECURITY_ACCESSOR_ACTION_ENTRIES_COUNT + SECURITY_ACCESSOR_DESCRIPTION_COUNT]; int _offset = 0;
                ID.ToBytes().CopyTo(_frame_data_bytes, _offset); _offset += CID.StructureSize;
                foreach (EAction _action_entry in d_action_entries) { _frame_data_bytes[_offset++] = (byte)_action_entry; }
                if (d_description != null) { byte[] _bytes_name = Encoding.ASCII.GetBytes(d_description); _bytes_name.CopyTo(_frame_data_bytes, _offset); _frame_data_bytes[_offset + _bytes_name.Length] = 0; } else { _frame_data_bytes[_offset] = 0; }
                byte[] _get_accessor_register_by_index_response = await AccessorRegisterRegistryOwner.GovernorManagerControlOwner.RequestFrameAsync(EFrameCommand.SetAccessorRegister, _frame_data_bytes, 500);
                if (_get_accessor_register_by_index_response == null || _get_accessor_register_by_index_response.Length != 1) throw new InvalidOperationException("No or invalid response length for command SetAccessorRegister");
                if (!BitConverter.ToBoolean(_get_accessor_register_by_index_response, 0)) throw new InvalidOperationException("SetAccessorRegister failed on governor");
                IsModified = false; OnPropertyChanged(nameof(IsModified));
            }

            public void Restore() { int _offset = 0; Parse(d_original_binary, ref _offset); }

            private async Task _SetActiveAsync(bool is_active) {
                if (!ID.IsValid) throw new InvalidOperationException("Cannot change active state of AccessorRegister with invalid ID");
                if (IsActive != is_active) {
                    byte[] _frame_data_bytes = new byte[CID.StructureSize + 1]; int _offset = 0;
                    ID.ToBytes().CopyTo(_frame_data_bytes, _offset); _offset += CID.StructureSize;
                    BitConverter.GetBytes(is_active).CopyTo(_frame_data_bytes, _offset); _offset++;
                    byte[] _get_accessor_register_by_index_response = await AccessorRegisterRegistryOwner.GovernorManagerControlOwner.RequestFrameAsync(EFrameCommand.SetAccessorRegisterActive, _frame_data_bytes, 500);
                    if (_get_accessor_register_by_index_response == null || _get_accessor_register_by_index_response.Length != 1) throw new InvalidOperationException("No or invalid response length for command SetAccessorRegisterActive");
                    if (!BitConverter.ToBoolean(_get_accessor_register_by_index_response, 0)) throw new InvalidOperationException("SetAccessorRegisterActive failed on governor");
                    IsActive = is_active; OnPropertyChanged(nameof(IsActive));
                }
            }
            public void SetActive(bool is_active) { Task.Run(async () => await SetActiveAsync(is_active)).Wait(); }
            public async Task SetActiveAsync(bool is_active) { await _SetActiveAsync(is_active); }
            public void Activate() { Task.Run(async () => await ActivateAsync()).Wait(); }
            public async Task ActivateAsync() { await _SetActiveAsync(true); }
            public void Deactivate() { Task.Run(async () => await DeactivateAsync()).Wait(); }
            public async Task DeactivateAsync() { await _SetActiveAsync(false); }
            public void ToggleActive() { Task.Run(async () => await ToggleActiveAsync()).Wait(); }
            public async Task ToggleActiveAsync() { await _SetActiveAsync(!IsActive); }

            public void AllowRegistration() { AccessorRegisterRegistryOwner.GovernorManagerControlOwner.PushFrame(EFrameCommand.AllowRegistration, ID.ToBytes()); }

            public override string ToString() { return $"{ID} - {PrimaryKey} {SecondaryKey}"; }
        }

        public class CAccessorRegisterRegistry : INotifyPropertyChanged
        {
            private readonly Dictionary<CID, CRegistration> d_dict_registration = new Dictionary<CID, CRegistration>();
            private readonly Dictionary<CID, CAccessorRegister> d_dict_accessor_registers = new Dictionary<CID, CAccessorRegister>();

            public EtaSecurityGovernorManagerControl GovernorManagerControlOwner { get; }
            public uint MaxAccessorRegistersCount { get; private set; }

            public CAccessorRegister[] AccessorRegisters { get; private set; } = new CAccessorRegister[0];
            public CRegistration[] Registrations { get; private set; } = new CRegistration[0];
            public bool IsRegistering => Registrations.Length > 0;

            public CAccessorRegister this[CID id] => d_dict_accessor_registers[id];

            public CAccessorRegisterRegistry(EtaSecurityGovernorManagerControl governor_manager_control) {
                GovernorManagerControlOwner = governor_manager_control;
                byte[] _get_governor_info_response = GovernorManagerControlOwner.RequestFrame(EFrameCommand.GetGovernorInfo, null, 500);
                if (_get_governor_info_response == null || _get_governor_info_response.Length != 4) throw new InvalidOperationException("No or invalid response length for command GetGovernorInfo");
                MaxAccessorRegistersCount = BitConverter.ToUInt32(_get_governor_info_response, 0);
            }

            public void AccessorRegisterUpdate(CID id) { Task.Run(async () => await AccessorRegisterUpdateAsync(id)).Wait(); }
            public async Task AccessorRegisterUpdateAsync(CID id) {
                CAccessorRegister _accessor_register;
                if (d_dict_accessor_registers.TryGetValue(id, out _accessor_register)) { await _accessor_register.UpdateAsync(); }
            }
            public void AccessorRegisterClobber(CID id) { Task.Run(async () => await AccessorRegisterClobberAsync(id)).Wait(); }
            public async Task AccessorRegisterClobberAsync(CID id) {
                CAccessorRegister _accessor_register;
                if (d_dict_accessor_registers.TryGetValue(id, out _accessor_register)) {
                    byte[] _get_accessor_register_by_index_response = await GovernorManagerControlOwner.RequestFrameAsync(EFrameCommand.ClobberAccessorRegister, _accessor_register.ID.ToBytes(), 500);
                    if (_get_accessor_register_by_index_response == null || _get_accessor_register_by_index_response.Length != 1) throw new InvalidOperationException("No or invalid response length for command ClobberAccessorRegister");
                    if (!BitConverter.ToBoolean(_get_accessor_register_by_index_response, 0)) throw new InvalidOperationException("ClobberAccessorRegister failed on governor");
                    d_dict_accessor_registers.Remove(_accessor_register.ID); AccessorRegisters = d_dict_accessor_registers.Values.ToArray(); OnPropertyChanged(nameof(AccessorRegisters));
                    _accessor_register.ID.Parse(new CID().ToBytes());
                }
            }

            public CRegistration RegistrationInitiate(CID id, uint registration_sequence) { CRegistration _registration = d_dict_registration[id] = new CRegistration(id, registration_sequence); Registrations = d_dict_registration.Values.ToArray(); OnPropertyChanged(nameof(Registrations)); OnPropertyChanged(nameof(IsRegistering)); return _registration; }
            public CRegistration RegistrationByID(CID id) { CRegistration _registration; d_dict_registration.TryGetValue(id, out _registration); return _registration; }
            public void RegistrationRemove(CID id) { if (d_dict_registration.Remove(id)) { Registrations = d_dict_registration.Values.ToArray(); OnPropertyChanged(nameof(Registrations)); OnPropertyChanged(nameof(IsRegistering)); } }
            public CRegistration RegistrationWonKeyReplacement(CID id) { CRegistration _registration; if (d_dict_registration.TryGetValue(id, out _registration)) { _registration.State = CRegistration.EState.WonKeyReplacement; OnPropertyChanged(nameof(Registrations)); OnPropertyChanged(nameof(IsRegistering)); } return _registration; }

            public CRegistration RegistrationConfirmed(CID id) {
                CRegistration _registration; if (d_dict_registration.TryGetValue(id, out _registration)) { _registration.State = CRegistration.EState.Confirmed; RegistrationRemove(id); }
                CAccessorRegister _accessor_register = new CAccessorRegister(this, id);
                d_dict_accessor_registers[_accessor_register.ID] = _accessor_register; AccessorRegisters = d_dict_accessor_registers.Values.ToArray(); OnPropertyChanged(nameof(AccessorRegisters));
                return _registration;
            }
            public CRegistration RegistrationLost(CID id) { CRegistration _registration; if (d_dict_registration.TryGetValue(id, out _registration)) { _registration.State = CRegistration.EState.Lost; RegistrationRemove(id); } return _registration; }
            public CRegistration RegistrationDenied(CID id) { CRegistration _registration; if (d_dict_registration.TryGetValue(id, out _registration)) { _registration.State = CRegistration.EState.Denied; RegistrationRemove(id); } return _registration; }
            public CRegistration RegistrationTimeout(CID id) { CRegistration _registration; if (d_dict_registration.TryGetValue(id, out _registration)) { _registration.State = CRegistration.EState.Timeout; RegistrationRemove(id); } return _registration; }
            public void RegistrationAllowTimeout() { }

            public void Update() { Task.Run(async () => await UpdateAsync()).Wait(); }
            public async Task UpdateAsync() {
                HashSet<CID> _hash_updated_ids = new HashSet<CID>();
                bool _is_first = true;
                while (true) {
                    byte[] _get_next_accessor_register_response = await GovernorManagerControlOwner.RequestFrameAsync(EFrameCommand.GetNextAccessorRegister, new[] { _is_first ? (byte)1 : (byte)0 }, 500);
                    if (_get_next_accessor_register_response == null || (_get_next_accessor_register_response.Length != 0 && _get_next_accessor_register_response.Length != CAccessorRegister.StructureSize)) throw new InvalidOperationException("No or invalid response length for command GetNextAccessorRegister");
                    if (_get_next_accessor_register_response.Length == 0) break;
                    CAccessorRegister _accessor_register;
                    if (d_dict_accessor_registers.TryGetValue(new CID(_get_next_accessor_register_response), out _accessor_register)) { _accessor_register.Parse(_get_next_accessor_register_response); }
                    else { _accessor_register = new CAccessorRegister(this, _get_next_accessor_register_response); d_dict_accessor_registers[_accessor_register.ID] = _accessor_register; }
                    if (!_hash_updated_ids.Add(_accessor_register.ID)) EtaDebug.DebugWrite(ConsoleColor.DarkMagenta, true, "Duplicated accessor {0}", _accessor_register.ID);
                    _is_first = false;
                }
                CID[] _ids_to_delete = d_dict_accessor_registers.Keys.Except(_hash_updated_ids).ToArray();
                foreach (CID _id in _ids_to_delete) { d_dict_accessor_registers.Remove(_id); }
                AccessorRegisters = d_dict_accessor_registers.Values.ToArray(); OnPropertyChanged(nameof(AccessorRegisters));
            }

            public event PropertyChangedEventHandler PropertyChanged;

            protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; _handler?.Invoke(this, new PropertyChangedEventArgs(propery_name)); }

            public class CRegistration : INotifyPropertyChanged
            {
                private EState d_state;

                public CID ID { get; }
                public uint RegistrationSequence { get; }
                public string RegistrationSequenceString { get; }
                public EState State { get { return d_state; } set { d_state = value; OnPropertyChanged(nameof(State)); } }

                public CRegistration(CID id, uint registration_sequence) {
                    ID = id; RegistrationSequence = registration_sequence;
                    char[] _chars = new char[19];
                    for (int _i = 0, _ci = 0; _i < 16; _i++, _ci++) {
                        if (_i > 0 && (_i & 0x03) == 0) _chars[_ci++] = '-';
                        _chars[_ci] = (char)('1' + ((registration_sequence >> ((15 - _i) << 1)) & 0x03));
                    }
                    RegistrationSequenceString = new string(_chars);
                }

                public event PropertyChangedEventHandler PropertyChanged;

                protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; _handler?.Invoke(this, new PropertyChangedEventArgs(propery_name)); }

                public enum EState
                {
                    Initiated,
                    WonKeyReplacement,
                    Confirmed,
                    Lost,
                    Denied,
                    Timeout,
                }
            }
        }

        public enum EFrameCommand
        {
            DebugString,
            AllowRegistration,
            TestRFInfo,
            TestEEPROMPagesWrite,
            TestEEPROMPagesRead,
            TestEEPROMPagesErase,
            TestGetADC,
            TestSwitchSink,
            TestLock,
            TestUnlock,
            TestGetLockStatus,

            OrdinalEvent_ChallengeInitiated = 0x20,
            OrdinalEvent_ChallengeWonWithPrimaryKey,
            OrdinalEvent_ChallengeWonWithSecondaryKey,
            OrdinalEvent_ChallengeKeyReplacement,
            OrdinalEvent_ChallengeAnotherTryKeyReplacement,
            OrdinalEvent_RegistrationInitiated,
            OrdinalEvent_RegistrationWonKeyReplacement,
            OrdinalEvent_RegistrationConfirmed,

            SecurityFail_PipeAlreadyRequested = 0x40,
            SecurityFail_CommunicationFailed,
            SecurityFail_SessionIsNotInitiated,
            SecurityFail_SessionWrongAccessor,
            SecurityFail_SessionAlreadyFailed,
            SecurityFail_SessionTimeout,
            SecurityFail_UnexpectedResponseForSession,
            SecurityFail_NoRegistrationForAccessor,
            SecurityFail_NoProperActionForButtons,
            SecurityFail_AccessorIsNotActive,
            SecurityFail_ChallengeLost,
            SecurityFail_RegistrationNotAllowed,
            SecurityFail_RegistrationFromWrongAccessor,
            SecurityFail_RegistrationAlreadyExists,
            SecurityFail_RegistrationLost,
            SecurityFail_RegistrationDenied,
            SecurityFail_RegistrationAllowanceTimeout,
            SecurityFail_UnexpectedCommand,

            GetGovernorInfo = 0x80,
            GetNextAccessorRegister,
            GetAccessorRegister,
            SetAccessorRegister,
            ClobberAccessorRegister,
            SetAccessorRegisterActive
        }
    }
}






















































