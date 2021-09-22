using Microsoft.Win32;
using System.IO;
using System.Windows;

using Membrane = membrane;

namespace Bulb {
    public partial class ProjectSelector : Window {
        public delegate void ProjectSelectedDelegate(string path);
        public ProjectSelectedDelegate OnProjectSelected;

        public ProjectSelector() {
            InitializeComponent();

            SubTitle.Text = $"Clove Engine - {Membrane.Application.getProjectVersion()}";
        }

        private void NewProjectButtonClick(object sender, RoutedEventArgs e) {
            var dialog = new SaveFileDialog();
            dialog.Filter = "Clove project files (*.clvproj)|*.clvproj";
            if (dialog.ShowDialog() == true) {
                File.Create(dialog.FileName);
                OnProjectSelected?.Invoke(dialog.FileName);
            }
        }

        private void OpenProjectButtonClick(object sender, RoutedEventArgs e) {
            var dialog = new OpenFileDialog();
            dialog.Filter = "Clove project files (*.clvproj)|*.clvproj";
            if (dialog.ShowDialog() == true) {
                OnProjectSelected?.Invoke(dialog.FileName);
            }
        }
    }
}
