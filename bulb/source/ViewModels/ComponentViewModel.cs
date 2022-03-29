using System;
using System.Windows.Input;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Membrane;
using System.Diagnostics;

namespace Bulb {
    /// <summary>
    /// Viewmodel for displaying components in editor
    /// </summary>
    public class ComponentViewModel : ViewModel {
        public string Name { get; }
        public string TypeName { get; }

        public ObservableCollection<TypeViewModel> Members { get; } = new ObservableCollection<TypeViewModel>();

        public ICommand RemoveComponentCommand { get; }

        public delegate void ModifyComponentHandler(string componentName, uint offset, string value);
        public ModifyComponentHandler OnModified;

        public delegate void RemoveComponentHandler(string typeName);
        public RemoveComponentHandler OnRemoved;

        private readonly TypeData componentTypeInfo;

        public ComponentViewModel(TypeData componentTypeInfo) {
            Name = componentTypeInfo.displayName;
            TypeName = componentTypeInfo.typeName;
            this.componentTypeInfo = componentTypeInfo;

            if (componentTypeInfo.dataType == DataType.Parent) {
                var members = (List<TypeData>)componentTypeInfo.data;
                foreach (var member in members) {
                    Members.Add(BuildTypeViewModel(member));
                }
            }
        }

        private TypeViewModel BuildTypeViewModel(TypeData typeData) {
            TypeViewModel vm;

            switch (typeData.dataType) {
                case DataType.Value:
                    vm = new TypeViewModel(typeData.displayName, 0, (string)typeData.data, false);
                    break;
                case DataType.Dropdown:
                    //TODO
                    vm = new TypeViewModel(typeData.displayName, 0, "DROP_DOWN", false);
                    break;
                case DataType.Parent: {
                        var members = (List<TypeData>)typeData.data;
                        List<TypeViewModel> vmMembers = new List<TypeViewModel>();

                        foreach (var member in members) {
                            vmMembers.Add(BuildTypeViewModel(member));
                        }

                        vm = new TypeViewModel(typeData.displayName, vmMembers);
                    }
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(typeData.dataType));
            }

            return vm;
        }

        private void OnValueChanged(uint offset, string value) {
            //TODO
            throw new NotImplementedException();
        }
    }
}