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
                var viewModel = (DirectoryItemViewModel)DataContext;
                var data = new DragDropData();

                if (viewModel.Type == ObjectType.File) {
                    data.assetGuid = membrane.FileSystemHelpers.getAssetFileGuid(ViewModel.FullPath);
                }

                var dataObject = new DataObject();
                dataObject.SetData(typeof(DragDropData), data);
                _ = DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
            }
        }
    }
}