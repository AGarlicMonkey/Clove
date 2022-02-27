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
            //TEMP: Just allowing everythng.
            //If we wanted to only allow assets for example we should provide a way to signify that.
            e.Effects = DragDropEffects.Copy;
        }

        private void StaticValueBox_Drop(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(DataFormats.StringFormat)) {
                string dataString = (string)e.Data.GetData(DataFormats.StringFormat);
                ViewModel.Value = dataString;
            }
            StaticValueBorder.BorderThickness = new Thickness(0);
        }
    }
}