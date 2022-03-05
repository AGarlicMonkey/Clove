using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Bulb {
    public partial class DirectoryViewItem : Button {
        private DirectoryItemViewModel ViewModel => DataContext as DirectoryItemViewModel;

        public DirectoryViewItem() {
            InitializeComponent();
        }

        protected override void OnMouseMove(MouseEventArgs e) {
            base.OnMouseMove(e);

            if (e.LeftButton == MouseButtonState.Pressed) {
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
    }
}