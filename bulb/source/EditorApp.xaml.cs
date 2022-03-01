using System.Threading;
using System.Windows;
using System.ComponentModel;

using Membrane = membrane;
using System;
using System.Diagnostics;
using System.Windows.Media;
using System.Runtime.InteropServices;

namespace Bulb {
    /// <summary>
    /// Interaction logic for Editor.xaml
    /// </summary>
    public partial class EditorApp : Application {
        private MainWindow editorWindow;

        private EditorSessionViewModel sessionViewModel;

        private Membrane.Application engineApp;

        public IntPtr OpenChildWindow(IntPtr hwndParent, int width, int height) {
            IntPtr href = engineApp.createChildWindow(hwndParent, width, height);

            //TEMP: Currently starting a session when we open a window. This is only temporary until Clove supports multiple windows.
            engineApp.loadGameDll();
            engineApp.startSession();

            sessionViewModel.Start(Membrane.FileSystemHelpers.getContentPath()); //TEMP: Remove with multiple window support

            return href;
        }

        private void EditorStartup(object sender, StartupEventArgs e) {
            //Initialise the engine
            engineApp = new Membrane.Application();

            sessionViewModel = new EditorSessionViewModel(".");
            sessionViewModel.OnCompileGame = () => {
                engineApp.loadGameDll();
            };

            Membrane.Log.addSink((string message) => sessionViewModel.Log.LogText += message, "%v");

            editorWindow = new MainWindow {
                DataContext = sessionViewModel
            };
            editorWindow.Closed += ShutdownEngine;

            editorWindow.Show();
            MainWindow = editorWindow;

            CompositionTarget.Rendering += RunEngineApplication;
        }

        public string ResolveVfsPath(string path) => engineApp.resolveVfsPath(path);

        private void ShutdownEngine(object sender, EventArgs e) => engineApp.shutdown();

        private void RunEngineApplication(object sender, EventArgs e) {
            if (engineApp.isRunning()) {
                //Send any editor events to the engine
                Membrane.MessageHandler.flushEditor();

                //Update the application
                engineApp.tick();

                //Send any engine events to the editor
                Membrane.MessageHandler.flushEngine(Current.Dispatcher);
            }
        }
    }
}
