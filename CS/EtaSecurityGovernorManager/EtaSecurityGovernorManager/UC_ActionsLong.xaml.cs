using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace EtaSecurityGovernorManager
{
    /// <summary>
    /// Interaction logic for UC_ActionsMain.xaml
    /// </summary>
    public partial class UC_ActionsLong : UserControl
    {
        public static readonly DependencyProperty LabelProperty = DependencyProperty.Register("Label", typeof(string), typeof(UC_ActionsLong));
        public static readonly DependencyProperty ActionProperty = DependencyProperty.Register("Action", typeof(EtaSecurityGovernorManagerControl.EAction), typeof(UC_ActionsLong), new FrameworkPropertyMetadata() { BindsTwoWayByDefault = true });

        public string Label { get { return (string)GetValue(LabelProperty); } set { SetValue(LabelProperty, value); } }
        public EtaSecurityGovernorManagerControl.EAction Action { get { return (EtaSecurityGovernorManagerControl.EAction)GetValue(ActionProperty); } set { SetValue(ActionProperty, value); } }

        public UC_ActionsLong() {
            InitializeComponent();
        }
    }
}
