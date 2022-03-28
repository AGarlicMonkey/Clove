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

    public enum DataType {
        Value,
        Dropdown,
        Parent,
    }

    public class DropdownData {
        public int currentSelection;
        public List<string> items;
        public List<TypeData> itemTypeData;
    }

    /// <summary>
    /// Contains data for a type currently loaded into memory
    /// </summary>
    public class TypeData {
        public string typeName;
        public string displayName;

        public DataType dataType;
        public object data;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct MemberInfo {
        [MarshalAs(UnmanagedType.BStr)]
        public string name;

        public ulong offset;
        public ulong size;
        public ulong typeId; //TODO: use nuint
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct TypeInfo {
        [MarshalAs(UnmanagedType.BStr)]
        public string typeName;
        [MarshalAs(UnmanagedType.BStr)]
        public string displayName;

        public ulong size;
        public IntPtr typeMemory;
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
        /// Helper function to allocate a MemberInfo array before sending it off to managed code.
        /// </summary>
        internal static MemberInfo[] AllocateMemberArray(string typeName) {
            int memberCount = getMemberCountForType(typeName);
            if (memberCount > 0) {
                return new MemberInfo[memberCount];
            } else {
                return null;
            }
        }

        /// <summary>
        /// Helper function to convert type info recieved from unmanaged code into an easily consumable managed type.
        /// </summary>
        /// <param name="typeInfo"></param>
        /// <param name="typeMembers"></param>
        /// <param name="typeMemory">Memory from managed code for this type.</param>
        /// <returns></returns>
        unsafe internal static TypeData ConstructTypeData(TypeInfo typeInfo, MemberInfo[] typeMembers) {
            TypeData typeData = new TypeData {
                typeName = typeInfo.typeName,
                displayName = typeInfo.displayName
            };

            if (typeMembers != null) {
                typeData.dataType = DataType.Parent;

                List<TypeData> memberInfos = new List<TypeData>();
                foreach (var member in typeMembers) {
                    var memberData = new TypeData {
                        typeName = member.name,
                        displayName = member.name,
                        dataType = DataType.Value,
                        data = null,
                    };

                    //TODO: This might have to be called down into unmanaged code - see EditorEditableMember::onEditorGetValue
                    //TEMP: If the size of the member is different to the size of a float - do nothing for now. It is not a value data type
                    if (member.size == sizeof(float)) {
                        byte* mem = (byte*)typeInfo.typeMemory.ToPointer();

                        float memberValue;
                        Buffer.MemoryCopy(mem + member.offset, &memberValue, sizeof(float), member.size);
                        memberData.data = memberValue.ToString();
                    }

                    memberInfos.Add(memberData);
                }

                typeData.data = memberInfos;
            } else {
                //TODO
                typeData.dataType = DataType.Value;
                typeData.data = null;
            }

            return typeData;
        }

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static void getEditorVisibleComponents([MarshalAs(UnmanagedType.LPArray), Out] AvailableTypeInfo[] data, ref uint num);
        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static void getEditorVisibleSubSystems([MarshalAs(UnmanagedType.LPArray), Out] AvailableTypeInfo[] data, ref uint num);

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static int getMemberCountForType(string typeName);
    }
}