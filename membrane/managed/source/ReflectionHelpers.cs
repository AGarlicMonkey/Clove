using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Membrane {
    [StructLayout(LayoutKind.Sequential)]
    public struct TypeInfo {
        string typeName;
        string displayName;
    }

    public class ReflectionHelpers {
        public static List<TypeInfo> GetEditorVisibleComponents() {
            List<TypeInfo> visibleComponents;

            try {
                uint numInfos = 0;
                getEditorVisibleComponents(null, ref numInfos);
                TypeInfo[] infos = new TypeInfo[numInfos];
                getEditorVisibleComponents(infos, ref numInfos);

                visibleComponents = new List<TypeInfo>(infos);
            } catch(Exception e) {
                //TODO: Log

                visibleComponents = new List<TypeInfo>();
            }

            return visibleComponents;
        }

        public static List<TypeInfo> GetEditorVisibleSubSystems() {
            List<TypeInfo> visibleSubSystems;

            try {
                uint numInfos = 0;
                getEditorVisibleSubSystems(null, ref numInfos);
                TypeInfo[] infos = new TypeInfo[numInfos];
                getEditorVisibleSubSystems(infos, ref numInfos);

                visibleSubSystems = new List<TypeInfo>(infos);
            } catch(Exception e) {
                //TODO: Log

                visibleSubSystems = new List<TypeInfo>();
            }

            return visibleSubSystems;
        }

        [DllImport("MembraneNative.dll")]
        private extern static void getEditorVisibleComponents([Out] TypeInfo[] data, ref uint num);
        [DllImport("MembraneNative.dll")]
        private extern static void getEditorVisibleSubSystems([Out] TypeInfo[] data, ref uint num);
    }
}