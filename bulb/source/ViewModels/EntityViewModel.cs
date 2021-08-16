using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Input;

using Membrane = garlic.membrane;

namespace Garlic.Bulb {
    public class EntityViewModel : ViewModel {
        public uint EntityId { get; set; }

        public string Name {
            get => name;
            set {
                name = value;
                OnPropertyChanged(nameof(Name));

                var message = new Membrane.Editor_UpdateName {
                    entity = EntityId,
                    name = name
                };

                Membrane.MessageHandler.sendMessage(message);
            }
        }
        private string name = "New Entity";

        public ObservableCollection<ComponentViewModel> Components { get; } = new ObservableCollection<ComponentViewModel>();
        public ObservableCollection<ComponentMenuItemViewModel> ComponentMenuItems { get; } = new ObservableCollection<ComponentMenuItemViewModel>();

        public ICommand SelectedCommand { get; }

        public delegate void SelectionHandler(EntityViewModel viewModel);
        public SelectionHandler OnSelected;

        public EntityViewModel() {
            //Bind to messages
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnComponentCreated>(OnComponentCreated);
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnRigidBodyChanged>(OnRigidBodyChanged);
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnTransformChanged>(OnTransformChanged);
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnSphereShapeChanged>(OnSphereShapeChanged);
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnCubeShapeChanged>(OnCubeShapeChanged);

            //Set up commands
            SelectedCommand = new RelayCommand(() => OnSelected?.Invoke(this));
        }

        public EntityViewModel(List<Membrane.Component> components) : this() {
            foreach (Membrane.Component component in components) {
                Components.Add(CreateComponentViewModel(component.type, component.initData));
                RefreshAvailableComponents();
            }
        }

        public void AddComponent(Membrane.ComponentType type) {
            var message = new Membrane.Editor_CreateComponent {
                entity = EntityId,
                componentType = type
            };

            Membrane.MessageHandler.sendMessage(message);
        }

        private void OnComponentCreated(Membrane.Engine_OnComponentCreated message) {
            if (EntityId == message.entity) {
                Components.Add(CreateComponentViewModel(message.componentType, message.data));
                RefreshAvailableComponents();
            }
        }

        private ComponentViewModel CreateComponentViewModel(Membrane.ComponentType type, object initData) {
            ComponentViewModel componentVm;

            switch (type) {
                case Membrane.ComponentType.Transform: {
                    var transformComp = new TransformComponentViewModel {
                        OnTransformChanged = UpdateTransform
                    };

                    componentVm = transformComp;
                }
                break;
                case Membrane.ComponentType.StaticModel: {
                    var modelComp = new StaticModelComponentViewModel(initData as Membrane.StaticModelComponentInitData) {
                        OnStaticModelChanged = UpdateStaticModel
                    };

                    componentVm = modelComp;
                }
                break;
                case Membrane.ComponentType.RigidBody: {
                    var rigidBodyComp = new RigidBodyComponentViewModel(initData as Membrane.RigidBodyComponentInitData) {
                        OnRigidBodyChanged = UpdateRigidBody
                    };

                    componentVm = rigidBodyComp;
                }
                break;
                case Membrane.ComponentType.CollisionShape: {
                    var collisionShapeComp = new CollisionShapeComponentViewModel(initData as Membrane.CollisionShapeComponentInitData) {
                        OnSphereShapeChanged = UpdateSphereShape,
                        OnCubeShapeChanged = UpdateCubeShape
                    };

                    componentVm = collisionShapeComp;
                }
                break;
                default:
                    componentVm = new ComponentViewModel($"{type}", type);
                    break;
            }

            return componentVm;
        }

        private void RefreshAvailableComponents() {
            List<Membrane.ComponentType> entitiesComponents = new List<Membrane.ComponentType>();
            foreach (ComponentViewModel component in Components) {
                entitiesComponents.Add(component.Type);
            }

            IEnumerable<Membrane.ComponentType> missingComponents = Enum.GetValues(typeof(Membrane.ComponentType)).Cast<Membrane.ComponentType>().Except(entitiesComponents);

            ComponentMenuItems.Clear();
            foreach (Membrane.ComponentType componentType in missingComponents) {
                ComponentMenuItems.Add(new ComponentMenuItemViewModel(componentType, new RelayCommand(() => AddComponent(componentType))));
            }
        }

        private void UpdateTransform(Membrane.Vector3 position, Membrane.Vector3 rotation, Membrane.Vector3 scale) {
            var message = new Membrane.Editor_UpdateTransform {
                entity = EntityId,
                position = position,
                rotation = rotation,
                scale = scale
            };

            Membrane.MessageHandler.sendMessage(message);
        }

        private void UpdateStaticModel(string meshPath, string diffusePath, string specularPath) {
            var message = new Membrane.Editor_UpdateStaticModel {
                entity = EntityId,
                meshPath = meshPath,
                diffusePath = diffusePath,
                specularPath = specularPath
            };

            Membrane.MessageHandler.sendMessage(message);
        }

        private void UpdateRigidBody(float mass) {
            var message = new Membrane.Editor_UpdateRigidBody {
                entity = EntityId,
                mass = mass
            };

            Membrane.MessageHandler.sendMessage(message);
        }

        private void UpdateSphereShape(float radius) {
            var message = new Membrane.Editor_UpdateSphereShape {
                entity = EntityId,
                radius = radius
            };

            Membrane.MessageHandler.sendMessage(message);
        }

        private void UpdateCubeShape(Membrane.Vector3 halfExtents) {
            var message = new Membrane.Editor_UpdateCubeShape {
                entity = EntityId,
                halfExtents = halfExtents
            };

            Membrane.MessageHandler.sendMessage(message);
        }

        private void OnRigidBodyChanged(Membrane.Engine_OnRigidBodyChanged message) {
            if (EntityId == message.entity) {
                foreach (ComponentViewModel comp in Components) {
                    if (comp.GetType() == typeof(RigidBodyComponentViewModel)) {
                        var rigidBody = comp as RigidBodyComponentViewModel;
                        rigidBody.Update(message.mass);
                        break;
                    }
                }
            }
        }

        private void OnTransformChanged(Membrane.Engine_OnTransformChanged message) {
            if (EntityId == message.entity) {
                foreach (ComponentViewModel comp in Components) {
                    if (comp.GetType() == typeof(TransformComponentViewModel)) {
                        var transform = comp as TransformComponentViewModel;
                        transform.Update(message.position, message.rotation, message.scale);
                        break;
                    }
                }
            }
        }

        private void OnSphereShapeChanged(Membrane.Engine_OnSphereShapeChanged message) {
            if (EntityId == message.entity) {
                foreach (ComponentViewModel comp in Components) {
                    if (comp.GetType() == typeof(CollisionShapeComponentViewModel)) {
                        var collisionShape = comp as CollisionShapeComponentViewModel;
                        collisionShape.UpdateSphereShape(message.radius);
                        break;
                    }
                }
            }

        }

        private void OnCubeShapeChanged(Membrane.Engine_OnCubeShapeChanged message) {
            if (EntityId == message.entity) {
                foreach(ComponentViewModel comp in Components) {
                    if (comp.GetType() == typeof(CollisionShapeComponentViewModel)) {
                        var collisionShape = comp as CollisionShapeComponentViewModel;
                        collisionShape.UpdateCubeShape(message.halfExtents);
                        break;
                    }
                }
            }
        }
    }
}