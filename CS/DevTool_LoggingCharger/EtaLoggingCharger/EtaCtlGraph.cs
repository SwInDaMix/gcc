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
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace EtaLoggingCharger
{
    /// <summary>
    /// Follow steps 1a or 1b and then 2 to use this custom control in a XAML file.
    ///
    /// Step 1a) Using this custom control in a XAML file that exists in the current project.
    /// Add this XmlNamespace attribute to the root element of the markup file where it is 
    /// to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:EtaLoggingCharger"
    ///
    ///
    /// Step 1b) Using this custom control in a XAML file that exists in a different project.
    /// Add this XmlNamespace attribute to the root element of the markup file where it is 
    /// to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:EtaLoggingCharger;assembly=EtaLoggingCharger"
    ///
    /// You will also need to add a project reference from the project where the XAML file lives
    /// to this project and Rebuild to avoid compilation errors:
    ///
    ///     Right click on the target project in the Solution Explorer and
    ///     "Add Reference"->"Projects"->[Browse to and select this project]
    ///
    ///
    /// Step 2)
    /// Go ahead and use your control in the XAML file.
    ///
    ///     <MyNamespace:EtaCtlGraph/>
    ///
    /// </summary>
    public class EtaCtlGraph : Control
    {
        public static readonly DependencyProperty GroupsProperty;
        public static readonly DependencyProperty GridRectanglePenBrushProperty;
        public static readonly DependencyProperty GridLinesPenBrushProperty;

        public CGroup[] Groups { get { return (CGroup[])GetValue(GroupsProperty); } set { SetValue(GroupsProperty, value); } }
        public Brush GridRectanglePenBrush { get { return (Brush)GetValue(GridRectanglePenBrushProperty); } set { SetValue(GridRectanglePenBrushProperty, value); } }
        public Brush GridLinesPenBrush { get { return (Brush)GetValue(GridLinesPenBrushProperty); } set { SetValue(GridLinesPenBrushProperty, value); } }

        static EtaCtlGraph() {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(EtaCtlGraph), new FrameworkPropertyMetadata(typeof(EtaCtlGraph)));
            // Initialize dependency properties
            GroupsProperty = DependencyProperty.Register("Groups", typeof(CGroup[]), typeof(EtaCtlGraph), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.AffectsRender));
            GridRectanglePenBrushProperty = DependencyProperty.Register("GridRectanglePenBrush", typeof(Brush), typeof(EtaCtlGraph), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.AffectsRender));
            GridLinesPenBrushProperty = DependencyProperty.Register("GridLinesPenBrush", typeof(Brush), typeof(EtaCtlGraph), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.AffectsRender));
        }

        protected override void OnRenderSizeChanged(SizeChangedInfo si) { base.OnRenderSizeChanged(si); InvalidateVisual(); }
        protected override void OnRender(DrawingContext dc) {
            base.OnRender(dc);
            double _width = ActualWidth, _height = ActualHeight;
            dc.DrawRectangle(Background, null, new Rect(0, 0, ActualWidth, ActualHeight));
            Typeface _typeface = new Typeface(FontFamily, FontStyle, FontWeight, FontStretch);
            CGroup[] _groups = Groups;
            if (_groups != null) {
                int _groups_cnt = _groups.Length;
                double _y = 0; double _y_step = _height / _groups_cnt;
                foreach (CGroup _group in _groups) {
                    dc.PushClip(new RectangleGeometry(new Rect(2, _y + 2, _width - 4, _y_step - 4)));
                    _group.Render(this, dc, _y, _width, _y_step, _typeface);
                    dc.Pop(); _y += _y_step;
                }
            }
        }

        public class CGroup
        {
            public CGraph[] Graphs { get; }
            public string Description { get; }
            public EDescriptionCorner DescriptionCorner { get; }
            public Brush GroupBackground { get; }
            public string XUnit { get; }
            public string YUnit { get; }
            public double MinGroupXValue { get; }
            public double MaxGroupXValue { get; }
            public double MinGroupYValue { get; }
            public double MaxGroupYValue { get; }

            public CGroup(CGraph[] graphs, string description, EDescriptionCorner description_corner, Brush group_background, string x_unit, string y_unit) {
                Graphs = graphs; Description = description; DescriptionCorner = description_corner; GroupBackground = group_background; XUnit = x_unit; YUnit = y_unit;
                bool _is_first = true;
                foreach (CGraph _graph in Graphs) {
                    if (_is_first) { _is_first = false; MinGroupXValue = _graph.MinXValue; MaxGroupXValue = _graph.MaxXValue; MinGroupYValue = _graph.MinYValue; MaxGroupYValue = _graph.MaxYValue; }
                    if (MinGroupXValue > _graph.MinXValue) MinGroupXValue = _graph.MinXValue;
                    if (MaxGroupXValue < _graph.MaxXValue) MaxGroupXValue = _graph.MaxXValue;
                    if (MinGroupYValue > _graph.MinYValue) MinGroupYValue = _graph.MinYValue;
                    if (MaxGroupYValue < _graph.MaxYValue) MaxGroupYValue = _graph.MaxYValue;
                }
            }

            public void Render(EtaCtlGraph ctl_graph, DrawingContext dc, double y, double width, double height, Typeface typeface) {
                Action<double, double, double, double> _act_draw_graph = (cord_x, cord_y, cord_width, cord_height) => {
                    Brush[] _foregrounds_brushes = { Brushes.Aqua, Brushes.Chocolate, Brushes.Chartreuse, Brushes.MediumPurple, Brushes.Crimson, Brushes.BurlyWood, Brushes.Brown, Brushes.LightBlue, Brushes.PaleGreen, Brushes.Yellow, Brushes.Violet };
                    double _description_offset = 0; int _brush_idx = 0; Func<Brush> _act_get_brush = () => { if (_brush_idx >= _foregrounds_brushes.Length) _brush_idx = 0; return _foregrounds_brushes[_brush_idx++]; };
                    foreach (CGraph _graph in Graphs) { _graph.Render(ctl_graph, this, dc, ref _description_offset, cord_x, cord_y, cord_width, cord_height, _act_get_brush(), typeface); }
                };
                _RenderGrid(ctl_graph, dc, Description, y, width, height, MinGroupXValue, MaxGroupXValue - MinGroupXValue, MinGroupYValue, MaxGroupYValue - MinGroupYValue, typeface, _act_draw_graph);
            }

            private double _RenderGrid_GetStep(double length, double cord_length, double cord_min) {
                double _step = length / (cord_length / cord_min), _scale = Math.Pow(10, Math.Floor(Math.Log10(_step)));
                double _dec_step = _step / _scale;
                if (_dec_step > 5) _dec_step = 10;
                else if (_dec_step > 2) _dec_step = 5;
                else _dec_step = 2;
                return _dec_step * _scale;
            }

            private double _RenderGrid_Modulus(double dividend, double divisor) { return (Math.Abs(dividend) - (Math.Abs(divisor) * (Math.Floor(Math.Abs(dividend) / Math.Abs(divisor))))) * Math.Sign(dividend); }
            private void _RenderGrid(EtaCtlGraph ctl_graph, DrawingContext dc, string description, double y, double width, double height, double x_start, double x_length, double y_start, double y_length, Typeface typeface, Action<double, double, double, double> act_draw_graph) {
                FormattedText _ft_description = new FormattedText(description, CultureInfo.CurrentCulture, FlowDirection.LeftToRight, typeface, 16, Brushes.White);
                if (_ft_description.Height >= height) { y += (height - 1); height = 1; } else { height -= _ft_description.Height; y += _ft_description.Height; }
                double _x_end = x_start + x_length, _y_end = y_start + y_length;
                double _x_step = _RenderGrid_GetStep(x_length, width - 50, 50), _y_step = _RenderGrid_GetStep(y_length, height - 20, 20);
                double _x_start_rem = _RenderGrid_Modulus(x_start, _x_step), _y_start_rem = _RenderGrid_Modulus(y_start, _y_step);
                double _x_start_granular = x_start - _x_start_rem, _y_start_granular = y_start - _y_start_rem;
                if (_x_start_granular < x_start) { _x_start_granular += _x_step; _x_start_rem = _x_step - _x_start_rem; }
                if (_y_start_granular < y_start) { _y_start_granular += _y_step; _y_start_rem = _y_step - _y_start_rem; }
                List<FormattedText> _list_x_ft = new List<FormattedText>(12), _list_y_ft = new List<FormattedText>(12);
                string _x_fmt = string.IsNullOrWhiteSpace(XUnit) ? "{0}" : "{0} {1}", _y_fmt = string.IsNullOrWhiteSpace(YUnit) ? "{0}" : "{0} {1}";
                for (double _x_value = _x_start_granular; _x_value <= _x_end; _x_value += _x_step) _list_x_ft.Add(new FormattedText(string.Format(_x_fmt, _x_value, XUnit), CultureInfo.CurrentCulture, FlowDirection.LeftToRight, typeface, 12, Brushes.White));
                for (double _y_value = _y_start_granular; _y_value <= _y_end; _y_value += _y_step) _list_y_ft.Add(new FormattedText(string.Format(_y_fmt, _y_value, YUnit), CultureInfo.CurrentCulture, FlowDirection.LeftToRight, typeface, 12, Brushes.White));
                double _x_ruler_length = _list_y_ft.Count > 0 ? _list_y_ft.Select(ft => ft.Width).Max() : 0, _y_ruler_length = _list_x_ft.Count > 0 ? _list_x_ft.Select(ft => ft.Height).Max() : 0;
                if (width > (6 + 4 + _x_ruler_length) && height >= (6 + 4 + _y_ruler_length)) {
                    double _cord_x = 3 + 4 + _x_ruler_length, _cord_y = y + 3, _cord_width = width - (6 + 4 + _x_ruler_length), _cord_height = height - (6 + 4 + _y_ruler_length);
                    double _cord_step_x = _cord_width * _x_step / x_length, _cord_step_y = _cord_height * _y_step / y_length;
                    Pen _pen_grid_lines = new Pen(ctl_graph.GridLinesPenBrush, 1); _pen_grid_lines.DashStyle = DashStyles.Dash;
                    dc.DrawRectangle(GroupBackground, null, new Rect(_cord_x, _cord_y, _cord_width, _cord_height));
                    double _x_curr = _cord_x + (_x_start_rem / _x_step * _cord_step_x);
                    foreach (FormattedText _ft in _list_x_ft) {
                        dc.DrawText(_ft, new Point(_x_curr, _cord_y + 2 + _cord_height));
                        dc.DrawLine(_pen_grid_lines, new Point(_x_curr, _cord_y), new Point(_x_curr, _cord_y + _cord_height));
                        _x_curr += _cord_step_x;
                    }
                    double _y_curr = _cord_y + _cord_height - (_y_start_rem / _y_step * _cord_step_y);
                    foreach (FormattedText _ft in _list_y_ft) {
                        //dc.DrawRectangle(Brushes.Navy, new Pen(Brushes.LightSalmon, 1), new Rect(_cord_x - 2 - _ft.Width, _y_curr - _ft.Height, _ft.Width, _ft.Height));
                        dc.DrawText(_ft, new Point(_cord_x - 2 - _ft.Width, _y_curr - _ft.Height - 3));
                        dc.DrawLine(_pen_grid_lines, new Point(_cord_x, _y_curr), new Point(_cord_x + _cord_width, _y_curr));
                        _y_curr -= _cord_step_y;
                    }
                    double _ft_description_x = _ft_description.Width > _cord_width ? 0 : _cord_x + ((_cord_width - _ft_description.Width) / 2);
                    dc.DrawText(_ft_description, new Point(_ft_description_x, _cord_y - _ft_description.Height - 3));
                    act_draw_graph(_cord_x, _cord_y, _cord_width, _cord_height);
                    dc.DrawRectangle(null, new Pen(ctl_graph.GridRectanglePenBrush, 2), new Rect(_cord_x, _cord_y, _cord_width, _cord_height));
                }
            }

            public enum EDescriptionCorner
            {
                LeftTop,
                RightTop,
                LeftBottom,
                RightBottom
            }
        }

        public abstract class CGraph
        {
            public IEnumerable<double> Samples { get; protected set; }
            public int SamplesCount { get; protected set; }
            public string Description { get; protected set; }
            public double MinXValue { get; protected set; }
            public double MaxXValue { get; protected set; }
            public double MinYValue { get; protected set; }
            public double MaxYValue { get; protected set; }

            protected CGraph(IEnumerable<double> samples, string description) { Samples = samples; Description = description; }

            public virtual void Render(EtaCtlGraph ctl_graph, CGroup group, DrawingContext dc, ref double description_offset, double x, double y, double width, double height, Brush foreground, Typeface typeface) {
                FormattedText _ft_description = new FormattedText(Description, CultureInfo.CurrentCulture, FlowDirection.LeftToRight, typeface, 10, foreground);
                double _ftd_x = group.DescriptionCorner == CGroup.EDescriptionCorner.LeftTop || group.DescriptionCorner == CGroup.EDescriptionCorner.LeftBottom ? x : x + width - _ft_description.Width;
                double _ftd_y = group.DescriptionCorner == CGroup.EDescriptionCorner.LeftTop || group.DescriptionCorner == CGroup.EDescriptionCorner.RightTop ? y + description_offset : y + height - description_offset - _ft_description.Height;
                dc.DrawText(_ft_description, new Point(_ftd_x, _ftd_y)); description_offset += _ft_description.Height;
            }
        }

        public class CGraphTimeSpan : CGraph
        {
            public double SamplesPerUnit { get; }

            public CGraphTimeSpan(IEnumerable<double> samples, string description, double x_start, double samples_per_unit) : base(samples, description) {
                SamplesPerUnit = samples_per_unit;
                bool _is_first = true; int _cnt = 0;
                foreach (double _sample in Samples) {
                    _cnt++;
                    if (_is_first) { _is_first = false; MinYValue = MaxYValue = _sample; }
                    if (MinYValue > _sample) MinYValue = _sample;
                    if (MaxYValue < _sample) MaxYValue = _sample;
                }
                SamplesCount = _cnt;
                MinXValue = x_start; MaxXValue = x_start + (_cnt / samples_per_unit);
            }

            public override void Render(EtaCtlGraph ctl_graph, CGroup group, DrawingContext dc, ref double description_offset, double x, double y, double width, double height, Brush foreground, Typeface typeface) {
                base.Render(ctl_graph, @group, dc, ref description_offset, x, y, width, height, foreground, typeface); Pen _pen_graph = new Pen(foreground, 1);
                double _y_diff = group.MaxGroupYValue - group.MinGroupYValue; int _samples_cnt = SamplesCount; double _group_cnt = (group.MaxGroupXValue - group.MinGroupXValue) * SamplesPerUnit;
                x += (MinXValue - group.MinGroupXValue) / (group.MaxGroupXValue - group.MinGroupXValue) * width;
                StreamGeometry _sg = new StreamGeometry();
                List<Point> _list_points = new List<Point>(128);
                using (StreamGeometryContext _sgc = _sg.Open()) {
                    if (_samples_cnt <= 128) {
                        double _x_step = width / (_group_cnt - 1); bool _is_first = true;
                        foreach (double _sample in Samples) {
                            double _y_curr = y + ((group.MaxGroupYValue - _sample) / _y_diff * height);
                            if (_is_first) { _is_first = false; _sgc.BeginFigure(new Point(x, _y_curr), true, false); }
                            else _list_points.Add(new Point(x, _y_curr));
                            x += _x_step;
                        }
                    }
                    else {
                        double _ovf_step = ((double)_samples_cnt) / 128;
                        double _x_step = width / _group_cnt; bool _is_first = true;
                        double _sample_trace = 0, _trace_ovf = 0; int _trace_cnt = 0;
                        IEnumerator<double> _samples_enumerator = Samples.GetEnumerator();
                        bool _is_value = _samples_enumerator.MoveNext();
                        while (_is_value) {
                            double _sample = _samples_enumerator.Current;
                            if (_is_first) { _is_first = false; _sgc.BeginFigure(new Point(x, y + ((group.MaxGroupYValue - _sample) / _y_diff * height)), true, false); }
                            _sample_trace += _sample; _trace_ovf += 1; _trace_cnt++;
                            _is_value = _samples_enumerator.MoveNext();
                            if (_trace_ovf >= _ovf_step || (!_is_value && _trace_cnt > 0)) {
                                _trace_ovf -= _ovf_step; _sample_trace /= _trace_cnt;
                                double _y_curr = y + ((group.MaxGroupYValue - (_is_value ? _sample_trace : _sample)) / _y_diff * height), _x_off = _x_step * _trace_cnt;
                                _list_points.Add(new Point(x + _x_off, _y_curr)); x += _x_off;
                                _sample_trace = 0; _trace_cnt = 0;
                            }
                        }
                    }
                    _sgc.PolyLineTo(_list_points, true, false);
                }
                _sg.Freeze();
                dc.DrawGeometry(null, _pen_graph, _sg);
            }
        }
    }
}
