using System.Windows;
using System.Windows.Controls;

namespace Bulb {
    /// <summary>
    /// A UserControl that displays the entire contents of a single directory in a grid.
    /// </summary>
    public partial class DirectoryView : UserControl {
        private DirectoryItemViewModel ViewModel => DataContext as DirectoryItemViewModel;

        public DirectoryView() {
            InitializeComponent();
        }

        protected override void OnDragOver(DragEventArgs e) {
            base.OnDragOver(e);

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

        protected override void OnDrop(DragEventArgs e) {
            base.OnDrop(e);

            e.Effects = DragDropEffects.None;

            if (e.Data.GetDataPresent(DataFormats.FileDrop)) {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

                foreach (string file in files) {
                    ViewModel.OnFileDropped(file);
                }

                e.Effects = DragDropEffects.Copy;
            }

            e.Handled = true;
        }
    }
}