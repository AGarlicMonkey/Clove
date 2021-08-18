using Membrane = membrane;

namespace Bulb {
    public class RigidBodyComponentViewModel : ComponentViewModel {
        public string Mass {
            get => mass.ToString();
            set {
                float.TryParse(value, out float number);
                OnRigidBodyChanged?.Invoke(number);
            }
        }
        private float mass;

        public delegate void RigdBodyChangedHandler(float mass);
        public RigdBodyChangedHandler OnRigidBodyChanged;

        public RigidBodyComponentViewModel(Membrane.RigidBodyComponentInitData initData) 
            : base($"{Membrane.ComponentType.RigidBody}", Membrane.ComponentType.RigidBody) {
            mass = initData.mass;
        }

        public void Update(float mass) {
            this.mass = mass;

            OnPropertyChanged(nameof(Mass));
        }
    }
}