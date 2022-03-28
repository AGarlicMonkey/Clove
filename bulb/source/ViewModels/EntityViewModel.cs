using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows.Input;
using Membrane;

namespace Bulb {
    public class EntityViewModel : ViewModel {
        public uint EntityId { get; set; }

        public string Name {
            get => name;
            set {
                name = value;
                OnPropertyChanged(nameof(Name));

                //TODO: Send name change down to engine
            }
        }
        private string name = "NO NAME";

        public ObservableCollection<ComponentViewModel> Components { get; } = new ObservableCollection<ComponentViewModel>();
        public ObservableCollection<ComponentMenuItemViewModel> ComponentMenuItems { get; } = new ObservableCollection<ComponentMenuItemViewModel>();

        public ICommand SelectedCommand { get; }

        public delegate void SelectionHandler(EntityViewModel viewModel);
        public SelectionHandler OnSelected;

        public EntityViewModel() {
            SelectedCommand = new RelayCommand(() => OnSelected?.Invoke(this));

            RefreshAvailableComponents();
        }

        public EntityViewModel(List<TypeData> components) : this() {
            //TODO
            throw new NotImplementedException();
        }

        private void AddComponent(string typeName) {
            TypeData componentTypeInfo = EntityComponentSystem.AddComponent(EntityId, typeName);

            var vm = new ComponentViewModel(componentTypeInfo);
            vm.OnModified = ModifyComponent;
            vm.OnRemoved = RemoveComponent;

            Components.Add(vm);
        }

        private void ModifyComponent(string componentName, uint offset, string value) {
            //TODO
            throw new NotImplementedException();
        }

        private void RemoveComponent(string typeName) {
            //TODO
            throw new NotImplementedException();
        }

        private void RefreshAvailableComponents() {
            List<string> entitiesComponents = new List<string>();
            foreach (ComponentViewModel component in Components) {
                entitiesComponents.Add(component.Name);
            }

            ComponentMenuItems.Clear();

            List<AvailableTypeInfo> types = Reflection.GetEditorVisibleComponents();

            foreach (AvailableTypeInfo type in types) {
                if (!entitiesComponents.Contains(type.displayName)) {
                    ComponentMenuItems.Add(new ComponentMenuItemViewModel(type.displayName, new RelayCommand(() => AddComponent(type.typeName))));
                }
            }
        }
    }
}