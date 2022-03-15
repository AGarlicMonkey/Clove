using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Bulb {
    public partial class DirectoryViewItem : Button {
        private DirectoryItemViewModel ViewModel => DataContext as DirectoryItemViewModel;
        private bool renaming = false;

        public DirectoryViewItem() {
            InitializeComponent();

            StaticName.Visibility = Visibility.Visible;
            EditableName.Visibility = Visibility.Collapsed;
        }

        protected override void OnMouseMove(MouseEventArgs e) {
            base.OnMouseMove(e);

            if (e.LeftButton == MouseButtonState.Pressed && !renaming) {
                var data = new DragDropData {
                    assetViewModel = ViewModel
                };

                var dataObject = new DataObject();
                dataObject.SetData(typeof(DragDropData), data);
                _ = DragDrop.DoDragDrop(this, data, DragDropEffects.All);

                e.Handled = true;
            }
        }

        protected override void OnDragEnter(DragEventArgs e) {
            base.OnDragEnter(e);
            DragBorder.BorderThickness = new Thickness(3);
            DragBorder.Padding = new Thickness(0);
            e.Handled = true;
        }

        protected override void OnDragLeave(DragEventArgs e) {
            base.OnDragLeave(e);
            DragBorder.BorderThickness = new Thickness(0);
            DragBorder.Padding = new Thickness(3);
            e.Handled = true;
        }

        protected override void OnDragOver(DragEventArgs e) {
            base.OnDragOver(e);

            e.Effects = DragDropEffects.None;

            if (CanDrop(e.Data)) {
                e.Effects = DragDropEffects.Move;
            }

            e.Handled = true;
        }

        protected override void OnDrop(DragEventArgs e) {
            base.OnDrop(e);

            e.Effects = DragDropEffects.None;

            if (CanDrop(e.Data)) {
                var data = ((DragDropData)e.Data.GetData(typeof(DragDropData)));
                ViewModel.OnFileDropped(data.assetViewModel);
                e.Effects = DragDropEffects.Move;
            }

            DragBorder.BorderThickness = new Thickness(0);
            DragBorder.Padding = new Thickness(3);

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

        private void BeginRename(object sender, RoutedEventArgs e) {
            StaticName.Visibility = Visibility.Collapsed;
            EditableName.Visibility = Visibility.Visible;

            EditableName.Text = Path.GetFileNameWithoutExtension(ViewModel.Name);
            EditableName.Focus();

            EditableName.LostFocus += EditableName_LostFocus;

            e.Handled = renaming = true;
        }

        private void EndRename(bool confirmed) {
            StaticName.Visibility = Visibility.Visible;
            EditableName.Visibility = Visibility.Collapsed;

            if (confirmed) {
                ViewModel.Rename(EditableName.Text);
            }

            EditableName.LostFocus -= EditableName_LostFocus;

            renaming = false;
        }

        private void EditableName_LostFocus(object sender, RoutedEventArgs e) {
            EndRename(confirmed: false);
            e.Handled = true;
        }

        private void EditableName_KeyDown(object sender, KeyEventArgs e) {
            if (renaming) {
                switch (e.Key) {
                    case Key.Enter:
                        EndRename(confirmed: true);
                        e.Handled = true;
                        break;
                    case Key.Escape:
                        EndRename(confirmed: false);
                        e.Handled = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}