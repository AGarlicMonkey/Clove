using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace Bulb {
    /// <summary>
    /// A generic file explorer view. Contains both a current directory view and a directory tree view.
    /// Expected a FileExplorerViewModel
    /// </summary>
    public partial class FileExplorer : UserControl {
        private FileExplorerViewModel ViewModel;

        public FileExplorer() {
            InitializeComponent();

            DataContextChanged += OnDataContextChanged;
        }

        private void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs e) {
            if (ViewModel != null) {
                ViewModel.PropertyChanged -= OnViewModelPropertyChanged;
            }

            ViewModel = e.NewValue as FileExplorerViewModel;

            if (ViewModel != null) {
                ViewModel.PropertyChanged += OnViewModelPropertyChanged;
            }

            RefreshBreadcrumb();
        }

        private void OnViewModelPropertyChanged(object sender, PropertyChangedEventArgs e) {
            if (e.PropertyName == nameof(FileExplorerViewModel.CurrentDirectory)) {
                RefreshBreadcrumb();
            }
        }

        private void RefreshBreadcrumb() {
            void TraverseParents(FolderViewModel folder) {
                if (folder.Parent != null) {
                    TraverseParents(folder.Parent);

                    var slash = new TextBlock();
                    slash.Text = "/";
                    slash.Margin = new Thickness(2.5, 0, 2.5, 0);
                    BreadcrumbPanel.Children.Add(slash);
                }

                var breadcrumb = new BreadcrumbItem();
                breadcrumb.DataContext = folder;
                breadcrumb.Margin = new Thickness(2.5, 0, 2.5, 0);
                BreadcrumbPanel.Children.Add(breadcrumb);
            }

            BreadcrumbPanel.Children.Clear();
            TraverseParents(ViewModel.CurrentDirectory);
        }
    }
}