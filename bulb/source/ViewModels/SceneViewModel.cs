using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Windows.Input;
using System.Windows;

using Membrane = membrane;

namespace Bulb {
    /// <summary>
    /// The View Model for the editor's current scene.
    /// </summary>
    public class SceneViewModel : ViewModel {
        public ICommand CreateEntityCommand { get; }
        public ICommand DeleteEntityCommand { get; }

        public ObservableCollection<EntityViewModel> Entities { get; } = new ObservableCollection<EntityViewModel>();

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
                    break;
                }
            }

            if (SelectedEntity?.EntityId == message.entity) {
                SelectedEntity = null;
            }
        }

        private void SelectEntity(EntityViewModel entity) {
            SelectedEntity = entity;
        }
    }
}