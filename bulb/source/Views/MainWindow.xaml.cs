using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace Bulb {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {
        private EditorSessionViewModel ViewModel => DataContext as EditorSessionViewModel;

        public MainWindow() {
            InitializeComponent();

            EntityListBox.SelectionChanged += EntityListBox_SelectionChanged;
        }

        private void EntityListBox_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            var listBox = sender as ListBox;
            if (listBox.SelectedItem is EntityViewModel entityVm) {
                entityVm.SelectedCommand.Execute(null);
            }
        }

        private void EntityListBox_KeyUp(object sender, KeyEventArgs e) {
            if (e.Key == Key.Delete) {
                var listBox = sender as ListBox;
                if (listBox.SelectedItem is EntityViewModel entityVm) {
                    (DataContext as EditorSessionViewModel).Scene.DeleteEntityCommand.Execute(entityVm.EntityId);
                }
            }
        }

        private void EntityListItem_MouseDown(object sender, MouseButtonEventArgs e) {
            if (e.ClickCount != 2) {
                return;
            }

            var grid = sender as Grid;

            int childCount = VisualTreeHelper.GetChildrenCount(grid);
            for (int i = 0; i < childCount; i++) {
                var child = VisualTreeHelper.GetChild(grid, i) as FrameworkElement;
                if (child.Name == "EditTextBox") {
                    child.Visibility = Visibility.Visible;
                    child.LostFocus += EditTextBoxLostFocus;
                    _ = child.Focus();
                }
            }

            e.Handled = true;
        }

        private void EditTextBoxLostFocus(object sender, RoutedEventArgs e) {
            var textBox = sender as FrameworkElement;
            textBox.Visibility = Visibility.Collapsed;
            textBox.LostFocus -= EditTextBoxLostFocus;
            e.Handled = true;
        }

        private void Window_KeyDown(object sender, KeyEventArgs e) {
            if(e.Key == Key.Escape) {
                ViewModel.Stop();
            }
        }
    }
}
