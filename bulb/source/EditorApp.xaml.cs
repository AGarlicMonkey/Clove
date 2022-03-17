using System;
using System.Windows;
using System.Windows.Media;

namespace Bulb {
    /// <summary>
    /// Interaction logic for Editor.xaml
    /// </summary>
    public partial class EditorApp : Application {
        private MainWindow editorWindow;

        private EditorSessionViewModel sessionViewModel;

        public IntPtr OpenChildWindow(IntPtr parent, int width, int height) {
            IntPtr href = Membrane.Application.CreateChildWindow(parent, width, height);

            //TEMP: Currently starting a session when we open a window. This is only temporary until Clove supports multiple windows.
            Membrane.Application.LoadGameDll();
            Membrane.Application.StartSession();

            sessionViewModel.Start(Membrane.FileSystemHelpers.GetContentPath()); //TEMP: Remove with multiple window support

            return href;
        }

        private void EditorStartup(object sender, StartupEventArgs e) {
            //Initialise the engine
            Membrane.Application.Initialise();

            sessionViewModel = new EditorSessionViewModel(".");
            sessionViewModel.OnCompileGame = () => Membrane.Application.LoadGameDll();

            // Membrane.Log.addSink((string message) => sessionViewModel.Log.LogText += message, "%v");

            editorWindow = new MainWindow {
                DataContext = sessionViewModel
            };
            editorWindow.Closed += ShutdownEngine;

            editorWindow.Show();
            MainWindow = editorWindow;

            CompositionTarget.Rendering += RunEngineApplication;
        }

        private void ShutdownEngine(object sender, EventArgs e) => Membrane.Application.Shutdown();

        private void RunEngineApplication(object sender, EventArgs e) {
            if (Membrane.Application.IsRunning()) {
                Membrane.Application.Tick();
            }
        }
    }
}
