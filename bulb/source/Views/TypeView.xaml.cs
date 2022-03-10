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

        private void StaticValueBox_DragOver(object sender, DragEventArgs e) {
            e.Effects = DragDropEffects.None;

            if (e.Data.GetDataPresent(typeof(DragDropData))) {
                var data = (DragDropData)e.Data.GetData(typeof(DragDropData));
                if (data.assetViewModel.Type == ObjectType.File) {
                    e.Effects = DragDropEffects.Copy;
                }
            }
        }

        private void StaticValueBox_Drop(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(typeof(DragDropData))) {
                var data = (DragDropData)e.Data.GetData(typeof(DragDropData));
                
                if(data.assetViewModel.Type == ObjectType.File) {
                    ViewModel.Value = (data.assetViewModel as FileViewModel).AssetGuid.ToString();
                }

            }

            StaticValueBorder.BorderThickness = new Thickness(0);
        }
    }
}