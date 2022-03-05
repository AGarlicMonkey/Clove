using System;
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
                    assetFullPath = ViewModel.FullPath
                };

                if (ViewModel.Type == ObjectType.File) {
                    data.assetGuid = membrane.FileSystemHelpers.getAssetFileGuid(ViewModel.FullPath);
                }

                var dataObject = new DataObject();
                dataObject.SetData(typeof(DragDropData), data);
                _ = DragDrop.DoDragDrop(this, data, DragDropEffects.All);
            }
        }

        private void Button_DragOver(object sender, DragEventArgs e) => e.Effects = CanDrop(e.Data) ? DragDropEffects.Move : DragDropEffects.None;

        private void Button_Drop(object sender, DragEventArgs e) {
            if (CanDrop(e.Data)) {
                var data = ((DragDropData)e.Data.GetData(typeof(DragDropData)));
                string filePath = data.assetFullPath;
                ViewModel.OnFileDropped(filePath);
            }
        }

        private bool CanDrop(IDataObject dataObject) {
            if (dataObject.GetDataPresent(typeof(DragDropData))) {
                var data = ((DragDropData)dataObject.GetData(typeof(DragDropData)));
                return data.assetFullPath != ViewModel.FullPath && ViewModel.CanDropFile(data.assetFullPath);
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

        private void EditableName_LostFocus(object sender, RoutedEventArgs e) => EndRename(confirmed: false);

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