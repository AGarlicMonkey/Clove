using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Membrane {
    /// <summary>
    /// Contains basic info for a type that is available to the editor.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct AvailableTypeInfo {
        [MarshalAs(UnmanagedType.BStr)]
        public string typeName;
        [MarshalAs(UnmanagedType.BStr)]
        public string displayName;
    }

    /// <summary>
    /// Infomation about a type's member.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct MemberInfo {
        [MarshalAs(UnmanagedType.BStr)]
        public string name;
        public ulong typeId; //TODO: use nuint
    }

    /// <summary>
    /// Contains type info for an instatiated type.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TypeInfo {
        [MarshalAs(UnmanagedType.BStr)]
        public string typeName;
        [MarshalAs(UnmanagedType.BStr)]
        public string displayName;

        public MemberInfo[] members;
    }

    public static class Reflection {
        public static List<AvailableTypeInfo> GetEditorVisibleComponents() {
            uint numInfos = 0;
            getEditorVisibleComponents(null, ref numInfos);
            AvailableTypeInfo[] infos = new AvailableTypeInfo[numInfos];
            getEditorVisibleComponents(infos, ref numInfos);

            return new List<AvailableTypeInfo>(infos);
        }

        public static List<AvailableTypeInfo> GetEditorVisibleSubSystems() {
            uint numInfos = 0;
            getEditorVisibleSubSystems(null, ref numInfos);
            AvailableTypeInfo[] infos = new AvailableTypeInfo[numInfos];
            getEditorVisibleSubSystems(infos, ref numInfos);

            return new List<AvailableTypeInfo>(infos);
        }

        /// <summary>
        /// Helper function to allocate a TypeInfo before sending it off to unmanaged code.
        /// </summary>
        internal static TypeInfo AllocateTypeInfo(string typeName) {
            var typeInfo = new TypeInfo();
            
            int memberCount = getMemberCountForType(typeName);
            if(memberCount > 0) {
                typeInfo.members = new MemberInfo[memberCount];
                for (int i = 0; i < memberCount; i++) {
                    typeInfo.members[i] = new MemberInfo();
                    typeInfo.members[i].typeId = 3;
                }
            }

            return typeInfo;
        }

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static void getEditorVisibleComponents([MarshalAs(UnmanagedType.LPArray), Out] AvailableTypeInfo[] data, ref uint num);
        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static void getEditorVisibleSubSystems([MarshalAs(UnmanagedType.LPArray), Out] AvailableTypeInfo[] data, ref uint num);

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static int getMemberCountForType(string typeName);
    }
}