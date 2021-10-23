using System.Windows;
using System.Windows.Controls;

namespace Bulb {
    /// <summary>
    /// A UserControl that displays infomation about an entity (name, components, etc.)
    /// </summary>
    public partial class EntityInfo : UserControl {
        public EntityInfo() {
            InitializeComponent();

            DataContextChanged += OnDataContextChanged;
            InfoStackPanel.Visibility = Visibility.Hidden;
        }

        private void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs e) {
            if (e.NewValue == null) {
                InfoStackPanel.Visibility = Visibility.Hidden;
            } else {
                InfoStackPanel.Visibility = Visibility.Visible;
            }
        }

        private void TextBox_Drop(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(DataFormats.StringFormat)) {
                var textBox = (TextBlock)sender;
                string dataString = (string)e.Data.GetData(DataFormats.StringFormat);

                textBox.Text = dataString;

                e.Handled = true;
            }
        }

        private void TransformPropertyLostFocus(object sender, RoutedEventArgs e) {
            //var element = sender as FrameworkElement;
            //var viewmodel = element.DataContext as TransformComponentViewModel;

            //viewmodel.RefreshValues();
        }

        private void RigidBodyPropertyLostFocus(object sender, RoutedEventArgs e) {
            //var element = sender as FrameworkElement;
            //var viewmodel = element.DataContext as RigidBodyComponentViewModel;

            //viewmodel.RefreshValues();
        }

        private void CollisionShapePropertyLostFocus(object sender, RoutedEventArgs e) {
            //var element = sender as FrameworkElement;
            //var viewmodel = element.DataContext as CollisionShapeComponentViewModel;

            //viewmodel.RefreshValues();
        }
    }
}