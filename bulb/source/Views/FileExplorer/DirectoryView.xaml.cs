using System.Windows;
using System.Windows.Controls;

namespace Bulb {
    /// <summary>
    /// A UserControl that displays the entire contents of a single directory in a grid.
    /// </summary>
    public partial class DirectoryView : UserControl {
        public DirectoryView() {
            InitializeComponent();
        }

        private void ItemsControl_Drop(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(DataFormats.FileDrop)) {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
                var viewModel = (DirectoryItemViewModel)DataContext;

                foreach(string file in files) {
                    viewModel.OnFileDropped(file);
                }
            }
        }
    }
}