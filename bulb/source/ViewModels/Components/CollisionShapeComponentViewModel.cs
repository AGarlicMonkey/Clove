using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;
using System.Windows.Input;
using Membrane = membrane;

namespace Bulb {
    public class CollisionShapeComponentViewModel : ComponentViewModel {
        public class ShapeTypeViewModel {
            public string Name { get; }
            public Membrane.ShapeType Type { get; }
            public ICommand OnSelectedCommand { get; }

            public ShapeTypeViewModel(string name, Membrane.ShapeType type, ICommand onSelectedCommand) {
                Name = name;
                Type = type;
                OnSelectedCommand = onSelectedCommand;
            }
        }

        public string Radius {
            get => radius.ToString();
            set {
                float.TryParse(value, out var number);
                OnSphereShapeChanged?.Invoke(number);
            }
        }
        private float radius = 1.0f;

        public string HalfExtentsX {
            get => halfExtents.x.ToString();
            set {
                float.TryParse(value, out var number);
                OnCubeShapeChanged?.Invoke(new Membrane.Vector3(number, halfExtents.y, halfExtents.z));
            }
        }
        public string HalfExtentsY {
            get => halfExtents.y.ToString();
            set {
                float.TryParse(value, out var number);
                OnCubeShapeChanged?.Invoke(new Membrane.Vector3(halfExtents.x, number, halfExtents.z));
            }
        }
        public string HalfExtentsZ {
            get => halfExtents.z.ToString();
            set {
                float.TryParse(value, out var number);
                OnCubeShapeChanged?.Invoke(new Membrane.Vector3(halfExtents.x, halfExtents.y, number));
            }
        }
        private Membrane.Vector3 halfExtents = new Membrane.Vector3(0.5f, 0.5f, 0.5f);

        public Membrane.ShapeType CurrentShapeType {
            get => currentShapeType;
            private set {
                currentShapeType = value;
                OnPropertyChanged(nameof(CurrentShapeType));
            }
        }
        private Membrane.ShapeType currentShapeType;

        public ObservableCollection<ShapeTypeViewModel> AvailableShapes { get; } = new ObservableCollection<ShapeTypeViewModel>();

        public Visibility RadiusVisibility {
            get => radiusVisibility;
            private set {
                radiusVisibility = value;
                OnPropertyChanged(nameof(RadiusVisibility));
            }
        }
        private Visibility radiusVisibility = Visibility.Visible;

        public Visibility HalfExtentsVisibility {
            get { return halfExtentsVisibility; }
            private set {
                halfExtentsVisibility = value;
                OnPropertyChanged(nameof(HalfExtentsVisibility));
            }
        }
        private Visibility halfExtentsVisibility = Visibility.Visible;

        public delegate void CollisionShapeSphereChanged(float radius);
        public delegate void CollisionShapeCubeChanged(Membrane.Vector3 halfExtents);

        public CollisionShapeSphereChanged OnSphereShapeChanged;
        public CollisionShapeCubeChanged OnCubeShapeChanged;

        public CollisionShapeComponentViewModel(Membrane.CollisionShapeComponentInitData initData) 
            : base($"{Membrane.ComponentType.CollisionShape}", Membrane.ComponentType.CollisionShape) {
            OnShapeTypeChanged(initData.shapeType, false);
            switch (initData.shapeType) {
                case Membrane.ShapeType.Sphere:
                    UpdateSphereShape(initData.radius);
                    break;
                case Membrane.ShapeType.Cube:
                    UpdateCubeShape(initData.halfExtents);
                    break;
            }
        }

        public void UpdateSphereShape(float radius) {
            this.radius = radius;
            OnPropertyChanged(nameof(Radius));
        }

        public void UpdateCubeShape(Membrane.Vector3 halfExtents) {
            this.halfExtents = halfExtents;
            OnPropertyChanged(nameof(HalfExtentsX));
            OnPropertyChanged(nameof(HalfExtentsY));
            OnPropertyChanged(nameof(HalfExtentsZ));
        }

        private void OnShapeTypeChanged(Membrane.ShapeType newType, bool invokeDelegates = true) {
            CurrentShapeType = newType;

            switch (CurrentShapeType) {
                case Membrane.ShapeType.Sphere:
                    RadiusVisibility = Visibility.Visible;
                    HalfExtentsVisibility = Visibility.Collapsed;
                    if (invokeDelegates) {
                        OnSphereShapeChanged?.Invoke(radius);
                    }
                    break;
                case Membrane.ShapeType.Cube:
                    RadiusVisibility = Visibility.Collapsed;
                    HalfExtentsVisibility = Visibility.Visible;
                    if (invokeDelegates) {
                        OnCubeShapeChanged?.Invoke(halfExtents);
                    }
                    break;
            }

            //Update the available shapes to remove the selected one
            var shapeTypes = Enum.GetValues(typeof(Membrane.ShapeType)).Cast<Membrane.ShapeType>();

            AvailableShapes.Clear();
            foreach (Membrane.ShapeType shape in shapeTypes) {
                if (shape != CurrentShapeType) {
                    AvailableShapes.Add(new ShapeTypeViewModel($"{shape}", shape, new RelayCommand(() => OnShapeTypeChanged(shape))));
                }
            }
        }
    }
}