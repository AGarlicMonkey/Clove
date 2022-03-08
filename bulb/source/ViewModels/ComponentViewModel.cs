using System.Windows.Input;
using System.Collections.ObjectModel;
using System.Collections.Generic;

using Membrane = membrane;
using System;
using System.Diagnostics;

namespace Bulb {
    /// <summary>
    /// Viewmodel for displaying components in editor
    /// </summary>
    public class ComponentViewModel : ViewModel {
        public string Name { get; }
        public string TypeName { get; }

        public ObservableCollection<TypeViewModel> Members { get; }

        public ICommand RemoveComponentCommand { get; }

        public delegate void ModifyComponentHandler(string componentName, uint offset, string value);
        public ModifyComponentHandler OnModified;

        public delegate void RemoveComponentHandler(string typeName);
        public RemoveComponentHandler OnRemoved;

        private readonly Membrane.EditorTypeInfo componentTypeInfo;

        public ComponentViewModel(Membrane.EditorTypeInfo componentTypeInfo) {
            Name = componentTypeInfo.displayName;
            TypeName = componentTypeInfo.typeName;
            this.componentTypeInfo = componentTypeInfo;

            Debug.Assert(componentTypeInfo.type == Membrane.EditorTypeType.Parent);
            var members = (List<Membrane.EditorTypeInfo>)componentTypeInfo.typeData;

            Members = new ObservableCollection<TypeViewModel>();
            foreach (var memberInfo in members) {
                Members.Add(BuildTypeViewModel(memberInfo));
            }

            RemoveComponentCommand = new RelayCommand(() => { OnRemoved?.Invoke(TypeName); });
        }

        private TypeViewModel BuildTypeViewModel(Membrane.EditorTypeInfo typeInfo) {
            TypeViewModel vm;
            switch (typeInfo.type) {
                case Membrane.EditorTypeType.Value:
                    vm = new TypeViewModel(typeInfo.displayName, typeInfo.offset, (string)typeInfo.typeData);
                    vm.OnValueChanged += OnValueChanged;
                    break;

                case Membrane.EditorTypeType.Parent: {
                    var typeMembers = (List<Membrane.EditorTypeInfo>)typeInfo.typeData;
                    List<TypeViewModel> viewModelMembers = new List<TypeViewModel>();

                    foreach (Membrane.EditorTypeInfo memberInfo in typeMembers) {
                        viewModelMembers.Add(BuildTypeViewModel(memberInfo));
                    }
                    vm = new TypeViewModel(typeInfo.displayName, viewModelMembers);
                }
                break;

                case Membrane.EditorTypeType.Dropdown: {
                    var dropdownData = (Membrane.EditorTypeDropdown)typeInfo.typeData;
                    List<TypeViewModel> dropdownMembers = null;

                    if(dropdownData.dropdownTypeInfos != null) {
                        dropdownMembers = new List<TypeViewModel>();

                        foreach (Membrane.EditorTypeInfo memberInfo in dropdownData.dropdownTypeInfos) {
                            dropdownMembers.Add(BuildTypeViewModel(memberInfo));
                        }
                    }

                    vm = new TypeViewModel(typeInfo.displayName, typeInfo.offset, dropdownData.dropdownItems, dropdownData.currentSelection, dropdownMembers);
                    vm.OnValueChanged += OnValueChanged;
                }
                break;

                default:
                    Debug.Assert(false, "EditorTypeType not handled");
                    vm = new TypeViewModel("Unknown", 0, "");
                    break;
            }

            return vm;
        }

        private void OnValueChanged(uint offset, string value) {
            OnModified?.Invoke(componentTypeInfo.typeName, offset, value);
        }
    }
}