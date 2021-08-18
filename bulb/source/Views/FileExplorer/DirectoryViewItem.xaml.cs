using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Bulb {
    public partial class DirectoryViewItem : Button {
        public DirectoryViewItem() {
            InitializeComponent();
        }

        protected override void OnMouseMove(MouseEventArgs e) {
            base.OnMouseMove(e);

            if (e.LeftButton == MouseButtonState.Pressed) {
                var viewModel = (DirectoryItemViewModel)DataContext;
                //Only drag-drop files for now
                if (viewModel.Type == ObjectType.File) {
                    DataObject data = new DataObject();
                    data.SetData(DataFormats.StringFormat, viewModel.VfsPath);

                    DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
                }
            }
        }
    }
}