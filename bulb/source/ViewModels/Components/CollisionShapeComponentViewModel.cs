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

        #region Sphere Properties
        public string Radius {
            get => radius;
            set {
                radius = value;
                radiusNum = EditorPropertyHelper.InputStringToFloat(radius);

                OnSphereShapeChanged?.Invoke(radiusNum);
                OnPropertyChanged(nameof(Radius));
            }
        }
        private string radius;
        private float radiusNum;

        public Visibility RadiusVisibility {
            get => radiusVisibility;
            private set {
                radiusVisibility = value;
                OnPropertyChanged(nameof(RadiusVisibility));
            }
        }
        private Visibility radiusVisibility = Visibility.Visible;
        #endregion

        #region Cube Properties
        public string HalfExtentsX {
            get => halfExtentsX;
            set {
                halfExtentsX = value;
                halfExtents.x = EditorPropertyHelper.InputStringToFloat(halfExtentsX);

                OnCubeShapeChanged?.Invoke(halfExtents);
                OnPropertyChanged(nameof(HalfExtentsX));
            }
        }
        private string halfExtentsX;

        public string HalfExtentsY {
            get => halfExtentsY;
            set {
                halfExtentsY = value;
                halfExtents.y = EditorPropertyHelper.InputStringToFloat(halfExtentsY);

                OnCubeShapeChanged?.Invoke(halfExtents);
                OnPropertyChanged(nameof(halfExtentsY));
            }
        }
        private string halfExtentsY;

        public string HalfExtentsZ {
            get => halfExtentsZ;
            set {
                halfExtentsZ = value;
                halfExtents.x = EditorPropertyHelper.InputStringToFloat(halfExtentsZ);

                OnCubeShapeChanged?.Invoke(halfExtents);
                OnPropertyChanged(nameof(halfExtentsZ));
            }
        }
        private string halfExtentsZ;

        private Membrane.Vector3 halfExtents = new Membrane.Vector3(0.5f, 0.5f, 0.5f);

        public Visibility HalfExtentsVisibility {
            get => halfExtentsVisibility;
            private set {
                halfExtentsVisibility = value;
                OnPropertyChanged(nameof(HalfExtentsVisibility));
            }
        }
        private Visibility halfExtentsVisibility = Visibility.Visible;
        #endregion

        public Membrane.ShapeType CurrentShapeType {
            get => currentShapeType;
            private set {
                currentShapeType = value;
                OnPropertyChanged(nameof(CurrentShapeType));
            }
        }
        private Membrane.ShapeType currentShapeType;

        public ObservableCollection<ShapeTypeViewModel> AvailableShapes { get; } = new ObservableCollection<ShapeTypeViewModel>();

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
            radiusNum = radius;
            this.radius = radiusNum.ToString();
            OnPropertyChanged(nameof(Radius));
        }

        public void UpdateCubeShape(Membrane.Vector3 halfExtents) {
            this.halfExtents = halfExtents;

            halfExtentsX = halfExtents.x.ToString();
            halfExtentsY = halfExtents.y.ToString();
            halfExtentsZ = halfExtents.z.ToString();

            OnPropertyChanged(nameof(HalfExtentsX));
            OnPropertyChanged(nameof(HalfExtentsY));
            OnPropertyChanged(nameof(HalfExtentsZ));
        }

        public void RefreshValues() {
            switch (CurrentShapeType) {
                case Membrane.ShapeType.Sphere:
                    UpdateSphereShape(radiusNum);
                    break;
                case Membrane.ShapeType.Cube:
                    UpdateCubeShape(halfExtents);
                    break;
            }
        }

        private void OnShapeTypeChanged(Membrane.ShapeType newType, bool invokeDelegates = true) {
            CurrentShapeType = newType;

            switch (CurrentShapeType) {
                case Membrane.ShapeType.Sphere:
                    RadiusVisibility = Visibility.Visible;
                    HalfExtentsVisibility = Visibility.Collapsed;
                    if (invokeDelegates) {
                        OnSphereShapeChanged?.Invoke(radiusNum);
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