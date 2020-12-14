using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Garlic.Bulb
{
    /// <summary>
    /// A UserControl for displaying 3 component vectors
    /// </summary>
    public partial class Vector3Box : UserControl
    {
        public string LeftFieldText
        {
            get { return (string)GetValue(LeftTextProperty); }
            set { SetValue(LeftTextProperty, value); }
        }
        public string LeftFieldValue
        {
            get { return (string)GetValue(LeftValueProperty); }
            set { SetValue(LeftValueProperty, value); }
        }

        public string CenterFieldText
        {
            get { return (string)GetValue(CenterTextProperty); }
            set { SetValue(CenterTextProperty, value); }
        }
        public string CenterFieldValue
        {
            get { return (string)GetValue(CenterValueProperty); }
            set { SetValue(CenterValueProperty, value); }
        }

        public string RightFieldText
        {
            get { return (string)GetValue(RightTextProperty); }
            set { SetValue(RightTextProperty, value); }
        }
        public string RightFieldValue
        {
            get { return (string)GetValue(RightValueProperty); }
            set { SetValue(RightValueProperty, value); }
        }

        public static DependencyProperty LeftTextProperty = DependencyProperty.Register(nameof(LeftFieldText), typeof(string), typeof(Vector3Box), new PropertyMetadata(null));
        public static DependencyProperty LeftValueProperty = DependencyProperty.Register(nameof(LeftFieldValue), typeof(string), typeof(Vector3Box), new PropertyMetadata(null));

        public static DependencyProperty CenterTextProperty = DependencyProperty.Register(nameof(CenterFieldText), typeof(string), typeof(Vector3Box), new PropertyMetadata(null));
        public static DependencyProperty CenterValueProperty = DependencyProperty.Register(nameof(CenterFieldValue), typeof(string), typeof(Vector3Box), new PropertyMetadata(null));

        public static DependencyProperty RightTextProperty = DependencyProperty.Register(nameof(RightFieldText), typeof(string), typeof(Vector3Box), new PropertyMetadata(null));
        public static DependencyProperty RightValueProperty = DependencyProperty.Register(nameof(RightFieldValue), typeof(string), typeof(Vector3Box), new PropertyMetadata(null));

        public Vector3Box()
        {
            InitializeComponent();
        }
    }
}