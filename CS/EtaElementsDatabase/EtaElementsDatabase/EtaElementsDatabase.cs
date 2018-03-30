using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace EtaElementsDatabase
{
    //****************************************************************************************************************************************************
    public class CEtaElementsDatabase : CElementContainer
    {
        private static readonly Guid __GUID_TMP = Guid.NewGuid();
        private static readonly uint __CURRENT_VERSION = 1;
        private static readonly string __PATH_PACKAGES = "Packages";
        private static readonly string __PATH_DB = "DB";

        private static CElement[] GenerateChilds() {
            CElementItem.CParameterEnum _parameter_enum_diode_purpose = new CElementItem.CParameterEnum("Purpose", "[\\-_a-z0-9\\s]*", "Bridge rectifier", "Rectifier", "Schottky", "Switching", "Transient suppressor", "Tunnel", "Varicap", "Zener");
            CElementItem.CParameterIntegral _parameter_enum_diode_max_reverse_voltage = new CElementItem.CParameterIntegral("Max Reverse Voltage", 0, 10000, "V", true);
            CElementItem.CParameterIntegral _parameter_enum_diode_avg_forward_current = new CElementItem.CParameterIntegral("Average Forward Current", 0, 10000, "A", true);
            CElementItem.CParameterIntegral _parameter_enum_diode_power = new CElementItem.CParameterIntegral("Power", 0, 100000, "W", true);
            CElementItem.CParameterIntegral _parameter_enum_diode_avg_recovery_time = new CElementItem.CParameterIntegral("Average Recovery Time", 0, 0.100, "s", true);

            CElementItem.CParameterEnum _parameter_enum_transistor_bjt_junction = new CElementItem.CParameterEnum("Junction", null, "PNP", "NPN");
            CElementItem.CParameterEnum _parameter_enum_transistor_fet_junction = new CElementItem.CParameterEnum("Junction", null, "P", "N");
            CElementItem.CParameterIntegral _parameter_enum_transistor_off_voltage = new CElementItem.CParameterIntegral("Off Voltage", 0, 10000, "V", true);
            CElementItem.CParameterIntegral _parameter_enum_transistor_current = new CElementItem.CParameterIntegral("Current", 0, 10000, "A", true);
            CElementItem.CParameterIntegral _parameter_enum_transistor_power = new CElementItem.CParameterIntegral("Power", 0, 100000, "W", true);
            CElementItem.CParameterIntegral _parameter_enum_transistor_on_resistance = new CElementItem.CParameterIntegral("On Resistance", 0, 10, "Ω", true);
            CElementItem.CParameterIntegral _parameter_enum_transistor_bjt_hfe = new CElementItem.CParameterIntegral("hFE", 0, 10000, null, true);

            CElementItem.CParameterEnum _parameter_enum_ic_analog_purpose = new CElementItem.CParameterEnum("Purpose", "[\\-_a-z0-9\\s]*", "Amplifier", "Comparator", "Display Driver", "Filter", "Miscellaneous", "Multiplexer", "Regulator", "Timer", "Voltage Reference");
            CElementItem.CParameterEnum _parameter_enum_ic_logic_purpose = new CElementItem.CParameterEnum("Purpose", "[\\-_a-z0-9\\s]*", "Adder", "Buffers", "Driver", "Comparator", "Counter", "Decoder", "Encoder", "Flip-Flop", "Frequency Devider", "Latch", "Gate", "Inventer", "Miscellaneous", "Multiplexer", "Multivibrator", "PLL", "Register", "Signal Switches", "Timer", "Transceiver");
            CElementItem.CParameterEnum _parameter_enum_ic_memory_purpose = new CElementItem.CParameterEnum("Purpose", "[\\-_a-z0-9\\s]*", "DRAM", "SRAM", "EEPROM", "EPROM");
            CElementItem.CParameterEnum _parameter_enum_ic_memory_architecture = new CElementItem.CParameterEnum("Architecture", "[\\-_a-z0-9\\s]*", "ARM", "AVR", "x86", "x64");

            // Diodes
            CElementItemDiode _element_item_diodes = new CElementItemDiode("Diodes", _parameter_enum_diode_purpose, _parameter_enum_diode_max_reverse_voltage, _parameter_enum_diode_avg_forward_current, _parameter_enum_diode_power, _parameter_enum_diode_avg_recovery_time);
            // Transistors
            CElementItemTransistorBJT _element_item_transistor_bjt = new CElementItemTransistorBJT("BJT", _parameter_enum_transistor_bjt_junction, _parameter_enum_transistor_off_voltage, _parameter_enum_transistor_current, _parameter_enum_transistor_power, _parameter_enum_transistor_on_resistance, _parameter_enum_transistor_bjt_hfe);
            CElementItemTransistorMOSFET _element_item_transistor_mosfet = new CElementItemTransistorMOSFET("MOSFET", _parameter_enum_transistor_fet_junction, _parameter_enum_transistor_off_voltage, _parameter_enum_transistor_current, _parameter_enum_transistor_power, _parameter_enum_transistor_on_resistance);
            CElementItemTransistorIGBT _element_item_transistor_igbt = new CElementItemTransistorIGBT("IGBT", _parameter_enum_transistor_off_voltage, _parameter_enum_transistor_current, _parameter_enum_transistor_power, _parameter_enum_transistor_on_resistance);
            CElementItemTransistorJFET _element_item_transistor_jfet = new CElementItemTransistorJFET("JFET", _parameter_enum_transistor_fet_junction, _parameter_enum_transistor_off_voltage, _parameter_enum_transistor_current, _parameter_enum_transistor_power, _parameter_enum_transistor_on_resistance);
            CElementItemTransistorUnijunction _element_item_transistor_unijunction = new CElementItemTransistorUnijunction("Unijunction", _parameter_enum_transistor_fet_junction, _parameter_enum_transistor_off_voltage, _parameter_enum_transistor_current, _parameter_enum_transistor_power, _parameter_enum_transistor_on_resistance);
            CElementContainer _element_container_transistors = new CElementContainer("Transistors", _element_item_transistor_bjt, _element_item_transistor_mosfet, _element_item_transistor_igbt, _element_item_transistor_jfet, _element_item_transistor_unijunction);
            // ICs
            CElementItemICAnalog _element_item_ic_analog = new CElementItemICAnalog("Analog", _parameter_enum_ic_analog_purpose);
            CElementItemICLogic _element_item_ic_logic = new CElementItemICLogic("Logic", _parameter_enum_ic_logic_purpose);
            CElementItemICMemory _element_item_ic_memory = new CElementItemICMemory("Memory", _parameter_enum_ic_memory_purpose);
            CElementItemICMicroprocessor _element_item_ic_microprocessor = new CElementItemICMicroprocessor("Microprocessor", _parameter_enum_ic_memory_architecture);
            CElementContainer _element_container_ics = new CElementContainer("ICs", _element_item_ic_analog, _element_item_ic_memory, _element_item_ic_microprocessor, _element_item_ic_logic);
            // Roots
            CElement[] _element_roots = { _element_item_diodes, _element_container_transistors, _element_container_ics };
            return _element_roots;
        }

        private CEtaElementsDatabase() : base("Root", GenerateChilds()) { }

        public override void XMLParse(XElement xml_element_database) {
            if (xml_element_database == null) throw new ArgumentNullException("xml_element_database");
            XMLParseChilds(xml_element_database);
        }
        public override XElement XMLGenerate() {
            XElement _xml_elements_database = new XElement("EtaElementsDatabase"); _xml_elements_database.Add(new XAttribute("Version", __CURRENT_VERSION.ToString()));
            XMLGenerateChilds(_xml_elements_database);
            return _xml_elements_database;
        }

        public static CEtaElementsDatabase XMLLoad() {
            XDocument _xml_document = XDocument.Load("db.xml");
            XElement _xml_elements_database = _xml_document.Element("EtaElementsDatabase");
            if (_xml_elements_database == null) throw new ArgumentException("No EtaElementsDatabase element");

            XAttribute _attribute_version = _xml_elements_database.Attribute("Version");
            if (_attribute_version == null) throw new ArgumentException("No Version attribute");
            uint _version = uint.Parse(_attribute_version.Value);
            if (_version > __CURRENT_VERSION) throw new ArgumentException("EtaElementsDatabase version is higher than supported by current version of Software. Please Update software");
            if (_version < __CURRENT_VERSION) ConvertVersion(_xml_elements_database, _version);

            CEtaElementsDatabase _elements_database = new CEtaElementsDatabase();
            _elements_database.XMLParse(_xml_elements_database);
            return _elements_database;
        }
        public void XMLSave() {
            XDocument _xml_document = new XDocument(new XDeclaration("1.0", "windows-1251", null), XMLGenerate());
            try { File.Delete("db_backup10.xml"); }
            catch (Exception) { }
            for (int _i = 9; _i >= 0; _i--) {
                try { File.Move(string.Format("db_backup{0:00}.xml", _i), string.Format("db_backup{0:00}.xml", _i + 1)); }
                catch (Exception) { }
            }
            try { File.Move("db.xml", "db_backup00.xml"); }
            catch (Exception) { }
            _xml_document.Save("db.xml");
        }
        public void XMLSaveTemp() {
            XDocument _xml_document = new XDocument(new XDeclaration("1.0", "windows-1251", null), XMLGenerate());
            _xml_document.Save(string.Format("db_{0:N}.xml", __GUID_TMP));
        }
        public void XMLDeleteTemp() {
            try { File.Delete(string.Format("db_{0:N}.xml", __GUID_TMP)); }
            catch (Exception) { }
        }

        private static void ConvertVersion(XElement xml_element, uint old_version) {

        }

        //    **************
        public class CElementItemDiode : CElementItem
        {
            public CElementItemDiode(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityDiode(this); }

            //    **************
            public class CElementEntityDiode : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Purpose { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Purpose", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral MaxReverseVoltage { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Max Reverse Voltage", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral AverageForwardCurrent { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Average Forward Current", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Power { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Power", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral AverageRecoveryTime { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Average Recovery Time", false, null); } }

                public CElementEntityDiode(CElementItemDiode element_item_diode) : base(element_item_diode) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemTransistorBJT : CElementItem
        {
            public CElementItemTransistorBJT(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityTransistorBJT(this); }

            //    **************
            public class CElementEntityTransistorBJT : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Junction { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Junction", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OffVoltage { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Off Voltage", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Current { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Current", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Power { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Power", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OnResistance { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("On Resistance", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral hFE { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("hFE", false, null); } }

                public CElementEntityTransistorBJT(CElementItemTransistorBJT element_item_transistor_bjt) : base(element_item_transistor_bjt) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemTransistorMOSFET : CElementItem
        {
            public CElementItemTransistorMOSFET(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityTransistorMOSFET(this); }

            //    **************
            public class CElementEntityTransistorMOSFET : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Junction { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Junction", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OffVoltage { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Off Voltage", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Current { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Current", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Power { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Power", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OnResistance { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("On Resistance", false, null); } }

                public CElementEntityTransistorMOSFET(CElementItemTransistorMOSFET element_item_transistor_mosfet) : base(element_item_transistor_mosfet) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemTransistorIGBT : CElementItem
        {
            public CElementItemTransistorIGBT(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityTransistorIGBT(this); }

            //    **************
            public class CElementEntityTransistorIGBT : CElementEntity
            {
                public CParameterIntegral.CParameterEntityIntegral OffVoltage { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Off Voltage", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Current { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Current", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Power { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Power", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OnResistance { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("On Resistance", false, null); } }

                public CElementEntityTransistorIGBT(CElementItemTransistorIGBT element_item_transistor_igbt) : base(element_item_transistor_igbt) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemTransistorJFET : CElementItem
        {
            public CElementItemTransistorJFET(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityTransistorJFET(this); }

            //    **************
            public class CElementEntityTransistorJFET : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Junction { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Junction", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OffVoltage { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Off Voltage", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Current { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Current", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Power { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Power", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OnResistance { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("On Resistance", false, null); } }

                public CElementEntityTransistorJFET(CElementItemTransistorJFET element_item_transistor_jfet) : base(element_item_transistor_jfet) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemTransistorUnijunction : CElementItem
        {
            public CElementItemTransistorUnijunction(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityTransistorUnijunction(this); }

            //    **************
            public class CElementEntityTransistorUnijunction : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Junction { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Junction", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OffVoltage { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Off Voltage", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Current { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Current", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral Power { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("Power", false, null); } }
                public CParameterIntegral.CParameterEntityIntegral OnResistance { get { return (CParameterIntegral.CParameterEntityIntegral)ParameterGet("On Resistance", false, null); } }

                public CElementEntityTransistorUnijunction(CElementItemTransistorUnijunction element_item_transistor_unijunction) : base(element_item_transistor_unijunction) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemICAnalog : CElementItem
        {
            public CElementItemICAnalog(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityICAnalog(this); }

            //    **************
            public class CElementEntityICAnalog : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Purpose { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Purpose", false, null); } }

                public CElementEntityICAnalog(CElementItemICAnalog element_item_ic_analog) : base(element_item_ic_analog) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemICLogic : CElementItem
        {
            public CElementItemICLogic(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityICLogic(this); }

            //    **************
            public class CElementEntityICLogic : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Purpose { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Purpose", false, null); } }

                public CElementEntityICLogic(CElementItemICLogic element_item_ic_logic) : base(element_item_ic_logic) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemICMemory : CElementItem
        {
            public CElementItemICMemory(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityICMemory(this); }

            //    **************
            public class CElementEntityICMemory : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Purpose { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Purpose", false, null); } }

                public CElementEntityICMemory(CElementItemICMemory element_item_ic_memory) : base(element_item_ic_memory) { }
            }
            //    **************
        }
        //    **************
        public class CElementItemICMicroprocessor : CElementItem
        {
            public CElementItemICMicroprocessor(string name, params CParameter[] parameters) : base(name, parameters) { }

            protected override CElementEntity EntityFactory() { return new CElementEntityICMicroprocessor(this); }

            //    **************
            public class CElementEntityICMicroprocessor : CElementEntity
            {
                public CParameterEnum.CParameterEntityEnum Architecture { get { return (CParameterEnum.CParameterEntityEnum)ParameterGet("Architecture", false, null); } }

                public CElementEntityICMicroprocessor(CElementItemICMicroprocessor element_item_ic_microprocessor) : base(element_item_ic_microprocessor) { }
            }
            //    **************
        }
    }
    //****************************************************************************************************************************************************
}
