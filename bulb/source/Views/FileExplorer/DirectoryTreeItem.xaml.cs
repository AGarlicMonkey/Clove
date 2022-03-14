using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Bulb {
    /// <summary>
    /// An idividual item inside of a directory tree.
    /// </summary>
    public partial class DirectoryTreeItem : UserControl {
        private FolderViewModel ViewModel => DataContext as FolderViewModel;
        private bool isExpanded = false;

        public DirectoryTreeItem() {
            InitializeComponent();
            UpdateExpansionIcon();

            ExpandIcon.Click += ToggleExpansion;
        }

        private void ToggleExpansion(object sender, RoutedEventArgs e) {
            isExpanded = !isExpanded;

            if (isExpanded) {
                foreach (var subDir in ViewModel.SubDirectories) {
                    var item = new DirectoryTreeItem {
                        DataContext = subDir
                    };

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

        private void Button_DragEnter(object sender, DragEventArgs e) {
            DragDropBorder.BorderThickness = new Thickness(3);
            DragDropBorder.Padding = new Thickness(0);
            e.Handled = true;
        }

        private void Button_DragLeave(object sender, DragEventArgs e) {
            DragDropBorder.BorderThickness = new Thickness(0);
            DragDropBorder.Padding = new Thickness(3);
            e.Handled = true;
        }

        private void Button_DragOver(object sender, DragEventArgs e) {
            e.Effects = DragDropEffects.None;

            if (CanDrop(e.Data)) {
                e.Effects = DragDropEffects.Move;
            }

            e.Handled = true;
        }

        private void Button_Drop(object sender, DragEventArgs e) {
            e.Effects = DragDropEffects.None;

            if (CanDrop(e.Data)) {
                var data = ((DragDropData)e.Data.GetData(typeof(DragDropData)));
                ViewModel.OnFileDropped(data.assetViewModel);
                e.Effects = DragDropEffects.Move;
            }

            DragDropBorder.BorderThickness = new Thickness(0);
            DragDropBorder.Padding = new Thickness(3);

            e.Handled = true;
        }

        private bool CanDrop(IDataObject dataObject) {
            if (dataObject.GetDataPresent(typeof(DragDropData))) {
                var data = ((DragDropData)dataObject.GetData(typeof(DragDropData)));
                return data.assetViewModel != ViewModel && ViewModel.CanDropFile(data.assetViewModel.FullPath);
            } else {
                return false;
            }
        }
    }
}