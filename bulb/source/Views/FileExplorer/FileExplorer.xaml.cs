using System.Windows.Controls;

namespace Bulb {
    /// <summary>
    /// A generic file explorer view. Contains both a current directory view and a directory tree view.
    /// Expected a FileExplorerViewModel
    /// </summary>
    public partial class FileExplorer : UserControl {
        public FileExplorer() {
            InitializeComponent();
        }
    }
}