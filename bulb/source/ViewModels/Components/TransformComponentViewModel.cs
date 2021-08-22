using Membrane = membrane;

namespace Bulb {
    /// <summary>
    /// Viewmodel representing a Transform Component
    /// </summary>
    public class TransformComponentViewModel : ComponentViewModel {
        #region Position
        private Membrane.Vector3 position = new Membrane.Vector3();

        public string PositionXValue {
            get => positionXValue;
            set {
                positionXValue = value;
                position.x = EditorPropertyHelper.InputStringToFloat(positionXValue);

                BroadcastTransformChange();
                OnPropertyChanged(nameof(PositionXValue));
            }
        }
        private string positionXValue;

        public string PositionYValue {
            get => positionYValue;
            set {
                positionYValue = value;
                position.y = EditorPropertyHelper.InputStringToFloat(positionYValue);

                BroadcastTransformChange();
                OnPropertyChanged(nameof(PositionYValue));
            }
        }
        private string positionYValue;

        public string PositionZValue {
            get => positionZValue;
            set {
                positionZValue = value;
                position.z = EditorPropertyHelper.InputStringToFloat(positionZValue);
                
                BroadcastTransformChange();
                OnPropertyChanged(nameof(PositionZValue));
            }
        }
        private string positionZValue;
        #endregion

        #region Rotation
        private Membrane.Vector3 rotation = new Membrane.Vector3();

        public string RotationXValue {
            get => rotationXValue;
            set {
                rotationXValue = value;
                rotation.x = EditorPropertyHelper.InputStringToFloat(rotationXValue);

                BroadcastTransformChange();
                OnPropertyChanged(nameof(RotationXValue));
            }
        }
        private string rotationXValue;

        public string RotationYValue {
            get => rotationYValue;
            set {
                rotationYValue = value;
                rotation.y = EditorPropertyHelper.InputStringToFloat(rotationYValue);

                BroadcastTransformChange();
                OnPropertyChanged(nameof(RotationYValue));
            }
        }
        private string rotationYValue;

        public string RotationZValue {
            get => rotationZValue;
            set {
                rotationZValue = value;
                rotation.z = EditorPropertyHelper.InputStringToFloat(rotationZValue);

                BroadcastTransformChange();
                OnPropertyChanged(nameof(RotationZValue));
            }
        }
        private string rotationZValue;
        #endregion

        #region Scale
        private Membrane.Vector3 scale = new Membrane.Vector3(1, 1, 1);

        public string ScaleXValue {
            get => scaleXValue;
            set {
                scaleXValue = value;
                scale.x = EditorPropertyHelper.InputStringToFloat(scaleXValue);

                BroadcastTransformChange();
                OnPropertyChanged(nameof(ScaleXValue));
            }
        }
        private string scaleXValue;

        public string ScaleYValue {
            get => scaleYValue;
            set {
                scaleYValue = value;
                scale.y = EditorPropertyHelper.InputStringToFloat(scaleYValue);

                BroadcastTransformChange();
                OnPropertyChanged(nameof(ScaleYValue));
            }
        }
        private string scaleYValue;

        public string ScaleZValue {
            get => scaleZValue;
            set {
                scaleZValue = value;
                scale.z = EditorPropertyHelper.InputStringToFloat(scaleZValue);

                BroadcastTransformChange();
                OnPropertyChanged(nameof(ScaleZValue));
            }
        }
        private string scaleZValue;
        #endregion

        public delegate void TransformChangedHandler(Membrane.Vector3 position, Membrane.Vector3 rotation, Membrane.Vector3 scale);
        public TransformChangedHandler OnTransformChanged;

        public TransformComponentViewModel(Membrane.TransformComponentInitData initData)
            : base($"{Membrane.ComponentType.Transform}", Membrane.ComponentType.Transform) {
            Update(initData.position, initData.rotation, initData.scale);
        }

        public void Update(Membrane.Vector3 position, Membrane.Vector3 rotation, Membrane.Vector3 scale) {
            positionXValue = position.x.ToString();
            positionYValue = position.y.ToString();
            positionZValue = position.z.ToString();
            OnPropertyChanged(nameof(PositionXValue));
            OnPropertyChanged(nameof(PositionYValue));
            OnPropertyChanged(nameof(PositionZValue));

            rotationXValue = rotation.x.ToString();
            rotationYValue = rotation.y.ToString();
            rotationZValue = rotation.z.ToString();
            OnPropertyChanged(nameof(RotationXValue));
            OnPropertyChanged(nameof(RotationYValue));
            OnPropertyChanged(nameof(RotationZValue));

            scaleXValue = scale.x.ToString();
            scaleYValue = scale.y.ToString();
            scaleZValue = scale.z.ToString();
            OnPropertyChanged(nameof(ScaleXValue));
            OnPropertyChanged(nameof(ScaleYValue));
            OnPropertyChanged(nameof(ScaleZValue));
        }

        public void RefreshValues() {
            Update(position, rotation, scale);
        }

        private void BroadcastTransformChange() {
            OnTransformChanged?.Invoke(position, rotation, scale);
        }
    }
}