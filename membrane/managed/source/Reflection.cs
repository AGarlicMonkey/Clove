using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Membrane {
    [StructLayout(LayoutKind.Sequential)]
    public struct TypeInfo {
        public string typeName;
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

        [DllImport("MembraneNative.dll")]
        private extern static void getEditorVisibleComponents([Out] TypeInfo[] data, ref uint num);
        [DllImport("MembraneNative.dll")]
        private extern static void getEditorVisibleSubSystems([Out] TypeInfo[] data, ref uint num);
    }
}