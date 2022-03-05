using System.Windows;
using System.Windows.Controls;

namespace Bulb {
    public partial class TypeView : UserControl {
        private TypeViewModel ViewModel => DataContext as TypeViewModel;

        public TypeView() {
            InitializeComponent();
        }

        private void StaticValueBox_DragEnter(object sender, DragEventArgs e) => StaticValueBorder.BorderThickness = new Thickness(3);

        private void StaticValueBox_DragLeave(object sender, DragEventArgs e) => StaticValueBorder.BorderThickness = new Thickness(0);

        private void StaticValueBox_DragOver(object sender, DragEventArgs e) => e.Effects = GetDragDropGuid(e.Data).hasGuid ? DragDropEffects.Copy : DragDropEffects.None;

        private void StaticValueBox_Drop(object sender, DragEventArgs e) {
            var (hasGuid, assetGuid) = GetDragDropGuid(e.Data);
            if (hasGuid) {
                ViewModel.Value = assetGuid.ToString();
            }
            StaticValueBorder.BorderThickness = new Thickness(0);
        }

        private (bool hasGuid, ulong guid) GetDragDropGuid(IDataObject dataObject) {
            if (dataObject.GetDataPresent(typeof(DragDropData))) {
                var data = ((DragDropData)dataObject.GetData(typeof(DragDropData)));
                if (data.assetGuid.HasValue) {
                    return (true, data.assetGuid.Value);
                } else {
                    return (false, 0);
                }
            } else {
                return (false, 0);
            }
        }
    }
}