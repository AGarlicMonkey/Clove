﻿using System;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Threading;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.ComponentModel;

namespace Editor
{
    /// <summary>
    /// Interaction logic for Editor.xaml
    /// </summary>
    public partial class EditorApp : Application
    {
        private MainWindow editorWindow;

        private Garlic.Application engineApp;

        private WriteableBitmap imageSource; //Owning this here for now as the UI thread needs to lock it
        private IntPtr backBuffer;
        private object backBufferLock = new object();

        private Thread engineThread;
        private bool exitThread = false;

        private Size size = new Size();
        private bool sizeChanged = false;

        private void EditorStartup(object sender, StartupEventArgs e)
        {
            editorWindow = new MainWindow();
            MainWindow = editorWindow;
            editorWindow.Show();

            //Initialise and start the application loop
            int width = editorWindow.RenderArea.ActualWidth > 0 ? (int)editorWindow.RenderArea.ActualWidth : 1;
            int height = editorWindow.RenderArea.ActualHeight > 0 ? (int)editorWindow.RenderArea.ActualHeight : 1;

            CreateImageSource(new Size(width, width));

            engineApp = new Garlic.Application(width, height);

            engineThread = new Thread(new ThreadStart(RunEngineApplication));
            engineThread.Name = "Garlic application thread";
            engineThread.Start();

            //Notify the app when ever we change size
            editorWindow.RenderArea.SizeChanged += OnRenderAreaSizeChanged;

            //Make sure we notify the thread when we want to close
            editorWindow.Closing += StopEngine;
        }

        private void OnRenderAreaSizeChanged(object sender, SizeChangedEventArgs e)
        {
            size = e.NewSize;
            CreateImageSource(size);
            sizeChanged = true;
        }

        private void StopEngine(object sender, CancelEventArgs e)
        {
            exitThread = true;
            engineThread.Join();
        }

        private void CreateImageSource(Size size)
        {
            lock (backBufferLock)
            {
                imageSource = new WriteableBitmap((int)size.Width, (int)size.Height, 96, 96, PixelFormats.Pbgra32, null);
                backBuffer = imageSource.BackBuffer;
            }
            editorWindow.RenderArea.Source = imageSource;
        }

        private void RunEngineApplication()
        {
            while (!exitThread)
            {
                if (sizeChanged)
                {
                    engineApp.resize((int)size.Width, (int)size.Height);
                    sizeChanged = false;
                }

                if (engineApp.isRunning())
                {
                    //Update the application
                    engineApp.tick();

                    //Render to image
                    lock (backBufferLock)
                    {
                        engineApp.render(backBuffer);
                    }

                    //Update the image source through the dispatcher on the thread that owns the image
                    Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Render, (Action)(() =>
                    {
                        imageSource.Lock();
                        imageSource.AddDirtyRect(new Int32Rect(0, 0, (int)imageSource.Width, (int)imageSource.Height));
                        imageSource.Unlock();
                    }));
                }
                else
                {
                    //Return to avoid calling shutdown if the app exits by itself
                    return;
                }
            }

            engineApp.shutdown();
        }
    }
}
