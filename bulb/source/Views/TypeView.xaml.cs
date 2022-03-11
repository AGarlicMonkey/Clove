using System.Windows;
using System.Windows.Controls;

namespace Bulb {
    public partial class TypeView : UserControl {
        private TypeViewModel ViewModel => DataContext as TypeViewModel;
        private readonly int size = 1;

        public TypeView() {
            InitializeComponent();
        }

        private void StaticValueBox_DragEnter(object sender, DragEventArgs e) {
            StaticValueBorder.BorderThickness = new Thickness(size);
            StaticValueBorder.Padding = new Thickness(0);
            e.Handled = true;
        }

        private void StaticValueBox_DragLeave(object sender, DragEventArgs e) {
            StaticValueBorder.BorderThickness = new Thickness(0);
            StaticValueBorder.Padding = new Thickness(size);
            e.Handled = true;
        }

        private void StaticValueBox_DragOver(object sender, DragEventArgs e) {
            e.Effects = DragDropEffects.None;

            if (e.Data.GetDataPresent(typeof(DragDropData))) {
                var data = (DragDropData)e.Data.GetData(typeof(DragDropData));
                if (data.assetViewModel.Type == ObjectType.File) {
                    e.Effects = DragDropEffects.Link;
                }
            }
        }

        private void StaticValueBox_Drop(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(typeof(DragDropData))) {
                var data = (DragDropData)e.Data.GetData(typeof(DragDropData));
                
                if(data.assetViewModel.Type == ObjectType.File) {
                    ViewModel.Value = (data.assetViewModel as FileViewModel).AssetGuid.ToString();
                    e.Effects = DragDropEffects.Link;
                }
            }

            StaticValueBorder.BorderThickness = new Thickness(0);
            StaticValueBorder.Padding = new Thickness(size);
            e.Handled = true;
        }
    }
}