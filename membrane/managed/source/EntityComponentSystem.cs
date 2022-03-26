using System.Runtime.InteropServices;

namespace Membrane {
    public class EntityComponentSystem {
        [DllImport("MembraneNative.dll", EntryPoint = "createEntity")]
        public static extern uint CreateEntity();
        [DllImport("MembraneNative.dll", EntryPoint = "deleteEntity")]
        public static extern void DeleteEntity(uint entityId);
    }
}