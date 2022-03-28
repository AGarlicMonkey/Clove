using System.Runtime.InteropServices;

namespace Membrane {
    public static class EntityComponentSystem {
        [DllImport("MembraneNative.dll", EntryPoint = "createEntity")]
        public static extern uint CreateEntity();
        [DllImport("MembraneNative.dll", EntryPoint = "deleteEntity")]
        public static extern void DeleteEntity(uint entityId);

        public static TypeInfo AddComponent(uint entityId, string componentName) {
            TypeInfo componentTypeInfo = Reflection.AllocateTypeInfo(componentName);

            if (!addComponent(entityId, componentName, ref componentTypeInfo)) {
                //TODO: Error
            } 

            return componentTypeInfo;
        }

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private static extern bool addComponent(uint entityId, string componentTypeName, [In, Out] ref TypeInfo outTypeInfo);
    }
}