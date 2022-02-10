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
    }
}