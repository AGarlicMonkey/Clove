using System.Windows.Controls;

namespace Bulb {
    /// <summary>
    /// A UserControl that displays the contents of a directory in a tree view.
    /// Expects a DirectoryViewModel
    /// </summary>
    public partial class DirectoryTree : UserControl {
        public DirectoryTree() {
            InitializeComponent();
        }
    }
}