using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Membrane {
    [StructLayout(LayoutKind.Sequential)]
    public struct TypeInfo {
        [MarshalAs(UnmanagedType.BStr)]
        public string typeName;
        [MarshalAs(UnmanagedType.BStr)]
        public string displayName;
    }

    public class Reflection {
        public static List<TypeInfo> GetEditorVisibleComponents() {
            uint numInfos = 0;
            getEditorVisibleComponents(null, ref numInfos);
            TypeInfo[] infos = new TypeInfo[numInfos];
            getEditorVisibleComponents(infos, ref numInfos);

            return new List<TypeInfo>(infos);
        }

        public static List<TypeInfo> GetEditorVisibleSubSystems() {
            uint numInfos = 0;
            getEditorVisibleSubSystems(null, ref numInfos);
            TypeInfo[] infos = new TypeInfo[numInfos];
            getEditorVisibleSubSystems(infos, ref numInfos);

            return new List<TypeInfo>(infos);
        }

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static void getEditorVisibleComponents([MarshalAs(UnmanagedType.LPArray)][Out] TypeInfo[] data, ref uint num);
        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static void getEditorVisibleSubSystems([MarshalAs(UnmanagedType.LPArray)][Out] TypeInfo[] data, ref uint num);
    }
}