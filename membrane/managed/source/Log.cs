using System.Runtime.InteropServices;

namespace Membrane {
    public enum LogLevel {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Critical,
    };

    public static class Log {
        public delegate void LogSink([MarshalAs(UnmanagedType.BStr)] string message);

        [DllImport("MembraneNative.dll", EntryPoint = "write", CharSet = CharSet.Unicode)]
        public static extern void Write(LogLevel logLevel, string message);

        [DllImport("MembraneNative.dll", EntryPoint = "addSink", CharSet = CharSet.Unicode)]
        public static extern void AddSink(LogSink sinkFp, string pattern);
    }
}