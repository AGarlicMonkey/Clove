using System;
using System.Runtime.InteropServices;

namespace Membrane {
    public class Application {
        [DllImport("MembraneNative.dll", EntryPoint = "initialise")]
        public static extern void Initialise();
        [DllImport("MembraneNative.dll", EntryPoint = "isRunning")]
        public static extern bool IsRunning();
        [DllImport("MembraneNative.dll", EntryPoint = "tick")]
        public static extern void Tick();
        [DllImport("MembraneNative.dll", EntryPoint = "shutdown")]
        public static extern void Shutdown();

        [DllImport("MembraneNative.dll", EntryPoint = "startSession")]
        public static extern void StartSession();

        [DllImport("MembraneNative.dll", EntryPoint = "createChildWindow")]
        public static extern IntPtr CreateChildWindow(IntPtr parent, int width, int height);

        [DllImport("MembraneNative.dll", EntryPoint = "loadGameDll")]
        public static extern void LoadGameDll();

        
    }
}