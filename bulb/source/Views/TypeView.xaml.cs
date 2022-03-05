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

        private void StaticValueBox_DragOver(object sender, DragEventArgs e) => e.Effects = e.Data.GetDataPresent(typeof(DragDropData)) ? DragDropEffects.Copy : DragDropEffects.None;

        private void StaticValueBox_Drop(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(typeof(DragDropData))) {
                var data = (DragDropData)e.Data.GetData(typeof(DragDropData));
                ViewModel.Value = (data.assetGuid ?? 0).ToString();
            }
            StaticValueBorder.BorderThickness = new Thickness(0);
        }
    }
}