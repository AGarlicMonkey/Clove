using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace Bulb {
    

    public class EngineWindowHost : HwndHost {
        private readonly int width;
        private readonly int height;

        private IntPtr hwnd;

        public EngineWindowHost(int width, int height) {
            this.width = width;
            this.height = height;
        }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent) {
            hwnd = (Application.Current as EditorApp).Start(hwndParent.Handle, width, height);
            return new HandleRef(this, hwnd);
        }

        protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled) {
            handled = false;
            return IntPtr.Zero;
        }

        protected override void DestroyWindowCore(HandleRef hwnd) {
            //TODO: Clean up window here - will get destroyed with the app so for now it's fine
            //DestroyWindow(hwnd.Handle);
        }
    }

    public partial class Viewport : UserControl, IDisposable {
        private EngineWindowHost windowHost;
        private bool disposedValue;

        public Viewport() {
            InitializeComponent();
            Loaded += OnViewportLoaded;
        }

        public void Dispose() {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing) {
            if (!disposedValue) {
                if (disposing) {
                    windowHost.Dispose();
                }

                disposedValue = true;
            }
        }

        private void OnViewportLoaded(object sender, RoutedEventArgs e) {
            Loaded -= OnViewportLoaded;

            windowHost = new EngineWindowHost((int)ActualWidth, (int)ActualHeight);
            Content = windowHost;
        }
    }
}