using System.Runtime.InteropServices;

namespace Membrane {
    public static class EntityComponentSystem {
        [DllImport("MembraneNative.dll", EntryPoint = "createEntity")]
        public static extern uint CreateEntity();
        [DllImport("MembraneNative.dll", EntryPoint = "deleteEntity")]
        public static extern void DeleteEntity(uint entityId);

        [DllImport("MembraneNative.dll", EntryPoint = "addComponent", CharSet = CharSet.Unicode)]
        public static extern void AddComponent(uint entityId, string componentTypeName);
    }
}