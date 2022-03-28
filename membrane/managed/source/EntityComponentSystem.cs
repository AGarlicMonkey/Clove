using System.Runtime.InteropServices;

namespace Membrane {
    public static class EntityComponentSystem {
        [DllImport("MembraneNative.dll", EntryPoint = "createEntity")]
        public static extern uint CreateEntity();
        [DllImport("MembraneNative.dll", EntryPoint = "deleteEntity")]
        public static extern void DeleteEntity(uint entityId);

        public static TypeData AddComponent(uint entityId, string componentName) {
            TypeInfo componentTypeInfo = new TypeInfo();
            MemberInfo[] componentMembers = Reflection.AllocateMemberArray(componentName);

            if (!addComponent(entityId, componentName, ref componentTypeInfo, componentMembers)) {
                return null;
            }

            return Reflection.ConstructTypeData(componentTypeInfo, componentMembers);
        }

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private static extern bool addComponent(uint entityId, string componentTypeName, ref TypeInfo outTypeInfo, [MarshalAs(UnmanagedType.LPArray), Out] MemberInfo[] outTypeMemberInfo);
    }
}