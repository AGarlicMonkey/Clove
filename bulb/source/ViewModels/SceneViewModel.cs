using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Windows.Input;
using System.Windows;
using Membrane;

namespace Bulb {
    /// <summary>
    /// The View Model for the editor's current scene.
    /// </summary>
    public class SceneViewModel : ViewModel {
        public ICommand CreateEntityCommand { get; }
        public ICommand DeleteEntityCommand { get; }

        public ObservableCollection<EntityViewModel> Entities { get; } = new ObservableCollection<EntityViewModel>();
        public ObservableCollection<SubSystemViewModel> SubSystems { get; } = new ObservableCollection<SubSystemViewModel>();

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

        //public SceneViewModel() : this(new List<string>(), new List<Membrane.Entity>()) { }

         public SceneViewModel(/* List<string> enabledSubSystems, List<Membrane.Entity> entities */) {
        //     //Populate available sub systems
        //     List<Membrane.AvailableTypeInfo> subSystems = Membrane.ReflectionHelper.getEditorVisibleSubSystems();
        //     foreach (Membrane.AvailableTypeInfo typeInfo in subSystems) {
        //         var vm = new SubSystemViewModel(typeInfo.displayName, typeInfo.typeName, enabledSubSystems.Contains(typeInfo.typeName));
        //         vm.OnToggled += OnSubSystemToggled;

        //         SubSystems.Add(vm);
        //     }

        //     //TODO: Move to function
        //     foreach (Membrane.Entity entity in entities) {
        //         var entityVm = new EntityViewModel(entity.components) {
        //             EntityId = entity.id,
        //             Name = entity.name,
        //             OnSelected = SelectEntity
        //         };

        //         Entities.Add(entityVm);
        //     }

        //     //Bind to messages
        //     Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnEntityCreated>(OnEntityCreated);
        //     Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnEntityDeleted>(OnEntityDeleted);

            //Set up commands
            CreateEntityCommand = new RelayCommand(CreateEntity);
            DeleteEntityCommand = new RelayCommand<uint>(DeleteEntity);
        }

        private void CreateEntity() {
            uint entityId = Membrane.EntityComponentSystem.CreateEntity();

            Entities.Add(new EntityViewModel {
                EntityId = entityId,
                Name = "New Entity",
                OnSelected = SelectEntity
            });
        }

        private void DeleteEntity(uint entityId) {
            Membrane.EntityComponentSystem.DeleteEntity(entityId);
            
            foreach(EntityViewModel entity in Entities) {
                if(entity.EntityId == entityId) {
                    Entities.Remove(entity);
                    break;
                }
            }

            if(SelectedEntity?.EntityId == entityId) {
                SelectedEntity = null;
            }
        }

        private void SelectEntity(EntityViewModel entity) {
            SelectedEntity = entity;
        }

        // private void OnSubSystemToggled(SubSystemViewModel viewModel, bool value) {
        //     if (value) {
        //         Membrane.MessageHandler.sendMessage(new Membrane.Editor_AddSubSystem() { name = viewModel.TypeName });
        //     } else {
        //         Membrane.MessageHandler.sendMessage(new Membrane.Editor_RemoveSubSystem() { name = viewModel.TypeName });
        //     }
        // }
    }
}