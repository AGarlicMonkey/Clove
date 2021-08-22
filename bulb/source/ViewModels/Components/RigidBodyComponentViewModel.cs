using Membrane = membrane;

namespace Bulb {
    public class RigidBodyComponentViewModel : ComponentViewModel {
        public string Mass {
            get => mass;
            set {
                mass = value;
                massNum = EditorPropertyHelper.InputStringToFloat(mass);

                OnRigidBodyChanged?.Invoke(massNum);
                OnPropertyChanged(nameof(Mass));
            }
        }
        private string mass;
        private float massNum;

        public delegate void RigdBodyChangedHandler(float mass);
        public RigdBodyChangedHandler OnRigidBodyChanged;

        public RigidBodyComponentViewModel(Membrane.RigidBodyComponentInitData initData) 
            : base($"{Membrane.ComponentType.RigidBody}", Membrane.ComponentType.RigidBody) {
            Update(initData.mass);
        }

        public void Update(float mass) {
            massNum = mass;
            this.mass = massNum.ToString();
            OnPropertyChanged(nameof(Mass));
        }

        public void RefreshValues() {
            Update(massNum);
        }
    }
}