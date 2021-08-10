using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Windows.Input;
using System.Windows;

using Membrane = garlic.membrane;
using System;
using System.Linq;

namespace Garlic.Bulb {
    /// <summary>
    /// The View Model for the editor's current scene.
    /// </summary>
    public class SceneViewModel : ViewModel {
        public ICommand CreateEntityCommand { get; }
        public ICommand DeleteEntityCommand { get; }

        public ObservableCollection<EntityViewModel> Entities { get; } = new ObservableCollection<EntityViewModel>();
        public ObservableCollection<ComponentMenuItemViewModel> ComponentMenuItems { get; } = new ObservableCollection<ComponentMenuItemViewModel>();

        public EntityViewModel SelectedEntity {
            get => selectedEntity;
            set {
                selectedEntity = value;
                OnPropertyChanged(nameof(SelectedEntity));
                OnPropertyChanged(nameof(EntityViewVisibility));
            }
        }
        private EntityViewModel selectedEntity;

        public Visibility EntityViewVisibility => selectedEntity != null ? Visibility.Visible : Visibility.Collapsed;

        public SceneViewModel() {
            //Bind to messages
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnEntityCreated>(OnEntityCreated);
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnEntityDeleted>(OnEntityDeleted);
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnComponentCreated>(OnComponentCreated);

            //Set up commands
            CreateEntityCommand = new RelayCommand(() => Membrane.MessageHandler.sendMessage(new Membrane.Editor_CreateEntity()));
            DeleteEntityCommand = new RelayCommand<uint>((id) => Membrane.MessageHandler.sendMessage(new Membrane.Editor_DeleteEntity() { entity = id }));
        }

        public SceneViewModel(List<Membrane.Entity> entities) : this() {
            //TODO: Move to function
            foreach (Membrane.Entity entity in entities) {
                var entityVm = new EntityViewModel(entity.components) {
                    EntityId = entity.id,
                    Name = entity.name,
                    OnSelected = SelectEntity
                };

                Entities.Add(entityVm);
            }
        }

        private void OnEntityCreated(Membrane.Engine_OnEntityCreated message) {
            var entityVm = new EntityViewModel {
                EntityId = message.entity,
                Name = message.name,
                OnSelected = SelectEntity
            };

            Entities.Add(entityVm);
        }

        private void OnEntityDeleted(Membrane.Engine_OnEntityDeleted message) {
            foreach (EntityViewModel entity in Entities) {
                if (entity.EntityId == message.entity) {
                    Entities.Remove(entity);
                    return;
                }
            }

            if (SelectedEntity?.EntityId == message.entity) {
                SelectedEntity = null;
            }
        }

        private void OnComponentCreated(Membrane.Engine_OnComponentCreated message) {
            //Remove the created component from the selected entity's menu item list
            if (SelectedEntity != null && message.entity == SelectedEntity.EntityId) {
                foreach (var menuItem in ComponentMenuItems) {
                    if (menuItem.ComponentType == message.componentType) {
                        ComponentMenuItems.Remove(menuItem);
                        return;
                    }
                }
            }
        }

        private void SelectEntity(EntityViewModel entity) {
            SelectedEntity = entity;
            UpdateAvailableComponents(SelectedEntity);
        }

        private void CreateComponent(Membrane.ComponentType componentType) {
            if (SelectedEntity != null) {
                SelectedEntity.AddComponent(componentType);
            } else {
                Membrane.Log.write(Membrane.LogLevel.Warning, $"CreateComponent called with unkown type: {componentType}");
            }
        }

        private void UpdateAvailableComponents(EntityViewModel entity) {
            List<Membrane.ComponentType> entitiesComponents = new List<Membrane.ComponentType>();
            foreach (ComponentViewModel component in entity.Components) {
                entitiesComponents.Add(component.Type);
            }

            //Get a list of components the entity does not have
            IEnumerable<Membrane.ComponentType> missingComponents = Enum.GetValues(typeof(Membrane.ComponentType)).Cast<Membrane.ComponentType>().Except(entitiesComponents);

            ComponentMenuItems.Clear();
            foreach (Membrane.ComponentType componentType in missingComponents) {
                ComponentMenuItems.Add(new ComponentMenuItemViewModel(componentType, new RelayCommand(() => CreateComponent(componentType))));
            }
        }
    }
}