using Membrane = garlic.membrane;

namespace Garlic.Bulb {
    class StaticModelComponentViewModel : ComponentViewModel {
        public string MeshPath {
            get => meshPath;
            set {
                meshPath = value;
                OnPropertyChanged(nameof(MeshPath));
                OnStaticModelChanged?.Invoke(meshPath, diffuseTexturePath, specularTexturePath);
            }
        }
        private string meshPath;

        public string DiffuseTexturePath {
            get => diffuseTexturePath;
            set {
                diffuseTexturePath = value;
                OnPropertyChanged(nameof(DiffuseTexturePath));
                OnStaticModelChanged?.Invoke(meshPath, diffuseTexturePath, specularTexturePath);
            }
        }
        private string diffuseTexturePath;

        public string SpecularTexturePath {
            get => specularTexturePath;
            set {
                specularTexturePath = value;
                OnPropertyChanged(nameof(SpecularTexturePath));
                OnStaticModelChanged?.Invoke(meshPath, diffuseTexturePath, specularTexturePath);
            }
        }
        private string specularTexturePath;

        public delegate void StaticModelChangedHandler(string meshPath, string diffusePath, string specularPath);
        public StaticModelChangedHandler OnStaticModelChanged;

        public StaticModelComponentViewModel(Membrane.StaticModelComponentInitData initData)
            : base($"{Membrane.ComponentType.StaticModel}", Membrane.ComponentType.StaticModel) {
            if (initData != null) {
                meshPath = initData.meshPath;
                diffuseTexturePath = initData.diffusePath;
                specularTexturePath = initData.specularPath;
            }
        }
    }
}