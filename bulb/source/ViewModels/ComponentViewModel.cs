using System;
using System.Windows.Input;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Membrane;

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
                    if (member.dataType == DataType.Value && member.data != null) {
                        Members.Add(new TypeViewModel(member.displayName, 0, (string)member.data, false));
                    } else {
                        Members.Add(new TypeViewModel(member.displayName, 0, "UNKNOWN", false));
                    }
                }
            }
        }

        private void OnValueChanged(uint offset, string value) {
            //TODO
            throw new NotImplementedException();
        }
    }
}