using System.Windows;
using System.Windows.Controls;

namespace Bulb {
    /// <summary>
    /// A UserControl that displays the entire contents of a single directory in a grid.
    /// </summary>
    public partial class DirectoryView : UserControl {
        private DirectoryItemViewModel ViewModel => (DirectoryItemViewModel)DataContext;

        public DirectoryView() {
            InitializeComponent();
        }

        private void ItemsControl_DragOver(object sender, DragEventArgs e) {
            e.Effects = DragDropEffects.None;

            if (e.Data.GetDataPresent(DataFormats.FileDrop)) {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

                bool canHandleAllFiles = true;
                foreach (string file in files) {
                    canHandleAllFiles = canHandleAllFiles && ViewModel.CanDropFile(file);
                }

                if (canHandleAllFiles) {
                    e.Effects = DragDropEffects.Copy;
                }
            }

            e.Handled = true;
        }

        private void ItemsControl_Drop(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(DataFormats.FileDrop)) {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

                foreach (string file in files) {
                    ViewModel.OnFileDropped(file);
                }
            }

            e.Handled = true;
        }
    }
}