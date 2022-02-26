using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Bulb {
    public partial class DirectoryViewItem : Button {
        private DirectoryItemViewModel ViewModel => (DirectoryItemViewModel)DataContext;

        public DirectoryViewItem() {
            InitializeComponent();
        }

        protected override void OnMouseMove(MouseEventArgs e) {
            base.OnMouseMove(e);

            if (e.LeftButton == MouseButtonState.Pressed) {
                var viewModel = (DirectoryItemViewModel)DataContext;
                //Only drag-drop files for now
                if (viewModel.Type == ObjectType.File) {
                    var data = new DataObject();
                    ulong guid = membrane.FileSystemHelpers.getAssetFileGuid(ViewModel.FullPath);

                    data.SetData(DataFormats.StringFormat, guid);

                    DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
                }
            }
        }
    }
}