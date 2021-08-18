using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Bulb {
    /// <summary>
    /// An idividual item inside of a directory tree.
    /// </summary>
    public partial class DirectoryTreeItem : UserControl {
        private bool isExpanded = false;

        public DirectoryTreeItem() {
            InitializeComponent();
            UpdateExpansionIcon();

            ExpandIcon.Click += ToggleExpansion;
        }

        private void ToggleExpansion(object sender, RoutedEventArgs e) {
            isExpanded = !isExpanded;

            if (isExpanded) {
                var viewModel = (FolderViewModel)DataContext;
                foreach (var subDir in viewModel.SubDirectories) {
                    var item = new DirectoryTreeItem();
                    item.DataContext = subDir;

                    ContentPanel.Children.Add(item);
                }
            } else {
                ContentPanel.Children.Clear();
            }

            UpdateExpansionIcon();
        }

        private void UpdateExpansionIcon() {
            ExpandIcon.Content = isExpanded ? "V" : ">";
        }
    }
}