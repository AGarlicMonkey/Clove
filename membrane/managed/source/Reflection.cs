using System;
using System.Collections.Generic;
using System.Diagnostics;
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
        Empty,
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

        public ulong offset;//TODO: use nuint
        public ulong size;//TODO: use nuint
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
            int memberCount = getMemberCountWithTypeName(typeName);
            if (memberCount > 0) {
                return new MemberInfo[memberCount];
            } else {
                return null;
            }
        }

        internal static MemberInfo[] AllocateMemberArray(ulong typeId) {
            int memberCount = getMemberCountWithTypeId(typeId);
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
        /// <param name="offsetIntoParent">Used when accessing member memory. Should always be 0 when called externally.</param>
        /// <returns></returns>
        internal static TypeData ConstructTypeData(TypeInfo typeInfo, MemberInfo[] typeMembers) {
            TypeData DoConstruction(TypeInfo info, MemberInfo[] members, ulong offsetIntoParent) {
                TypeData typeData = new TypeData {
                    typeName = info.typeName,
                    displayName = info.displayName,
                    dataType = DataType.Empty,
                    data = null
                };

                if (members != null) {
                    typeData.dataType = DataType.Parent;

                    List<TypeData> memberInfos = new List<TypeData>();
                    foreach (var member in members) {
                        TypeData memberData;

                        if (isTypeIdReflected(member.typeId)) {
                            TypeInfo memberTypeInfo = new TypeInfo();
                            MemberInfo[] memberMembers = AllocateMemberArray(member.typeId);

                            getTypeInfoFromTypeId(member.typeId, ref memberTypeInfo, memberMembers);

                            Debug.Assert(memberTypeInfo.typeMemory == IntPtr.Zero);
                            memberTypeInfo.typeMemory = info.typeMemory;

                            memberData = DoConstruction(memberTypeInfo, memberMembers, member.offset);
                            memberData.displayName = member.name; //Make sure the display name is the name of the acutal member

                        } else {
                            //TODO: This might have to be called down into unmanaged code - see EditorEditableMember::onEditorGetValue
                            //TEMP: If the size of the member is different to the size of a float - do nothing for now. It is not a value data type

                            Debug.Assert(member.size == sizeof(float));

                            memberData = new TypeData {
                                typeName = null,
                                displayName = member.name,
                                dataType = DataType.Value,
                            };

                            unsafe {
                                byte* mem = (byte*)info.typeMemory.ToPointer();
                                ulong totalOffset = offsetIntoParent + member.offset;

                                float memberValue;
                                Buffer.MemoryCopy(mem + totalOffset, &memberValue, sizeof(float), member.size);
                                memberData.data = memberValue.ToString();
                            }
                        }

                        Debug.Assert(memberData != null);
                        memberInfos.Add(memberData);
                    }

                    typeData.data = memberInfos;
                }

                return typeData;
            }

            return DoConstruction(typeInfo, typeMembers, offsetIntoParent: 0);
        }

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static void getEditorVisibleComponents([MarshalAs(UnmanagedType.LPArray), Out] AvailableTypeInfo[] data, ref uint num);
        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static void getEditorVisibleSubSystems([MarshalAs(UnmanagedType.LPArray), Out] AvailableTypeInfo[] data, ref uint num);

        [DllImport("MembraneNative.dll")]
        private extern static bool isTypeIdReflected(ulong typeId);

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private extern static int getMemberCountWithTypeName(string typeName);
        [DllImport("MembraneNative.dll")]
        private extern static int getMemberCountWithTypeId(ulong typeId);

        [DllImport("MembraneNative.dll")]
        private static extern void getTypeInfoFromTypeId(ulong typeId, ref TypeInfo outTypeInfo, [MarshalAs(UnmanagedType.LPArray), Out] MemberInfo[] outTypeMemberInfo);
    }
}