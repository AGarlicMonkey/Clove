using System.Windows.Input;
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

        private readonly TypeInfo componentTypeInfo;

        public ComponentViewModel(TypeInfo componentTypeInfo) {
            Name = componentTypeInfo.displayName;
            TypeName = componentTypeInfo.typeName;
            this.componentTypeInfo = componentTypeInfo;

            if(componentTypeInfo.members != null) {
                foreach(var member in componentTypeInfo.members) {
                    Members.Add(new TypeViewModel(/*member.name*/ "UNKNOWN", 0, "TEST", false));
                }
            }

            // Debug.Assert(componentTypeInfo.type == Membrane.EditorTypeType.Parent);
            // var members = (List<Membrane.EditorTypeInfo>)componentTypeInfo.typeData;

            // Members = new ObservableCollection<TypeViewModel>();
            // foreach (var memberInfo in members) {
            //     Members.Add(BuildTypeViewModel(memberInfo));
            // }

            // RemoveComponentCommand = new RelayCommand(() => { OnRemoved?.Invoke(TypeName); });
        }

        // private TypeViewModel BuildTypeViewModel(Membrane.EditorTypeInfo typeInfo) {
        //     TypeViewModel vm;
        //     switch (typeInfo.type) {
        //         case Membrane.EditorTypeType.Value:
        //             vm = new TypeViewModel(typeInfo.displayName, typeInfo.offset, (string)typeInfo.typeData, typeInfo.dragDropOnly);
        //             vm.OnValueChanged += OnValueChanged;
        //             break;

        //         case Membrane.EditorTypeType.Parent: {
        //             var typeMembers = (List<Membrane.EditorTypeInfo>)typeInfo.typeData;
        //             List<TypeViewModel> viewModelMembers = new List<TypeViewModel>();

        //             foreach (Membrane.EditorTypeInfo memberInfo in typeMembers) {
        //                 viewModelMembers.Add(BuildTypeViewModel(memberInfo));
        //             }
        //             vm = new TypeViewModel(typeInfo.displayName, viewModelMembers);
        //         }
        //         break;

        //         case Membrane.EditorTypeType.Dropdown: {
        //             var dropdownData = (Membrane.EditorTypeDropdown)typeInfo.typeData;
        //             List<TypeViewModel> dropdownMembers = null;

        //             if(dropdownData.dropdownTypeInfos != null) {
        //                 dropdownMembers = new List<TypeViewModel>();

        //                 foreach (Membrane.EditorTypeInfo memberInfo in dropdownData.dropdownTypeInfos) {
        //                     dropdownMembers.Add(BuildTypeViewModel(memberInfo));
        //                 }
        //             }

        //             vm = new TypeViewModel(typeInfo.displayName, typeInfo.offset, dropdownData.dropdownItems, dropdownData.currentSelection, dropdownMembers);
        //             vm.OnValueChanged += OnValueChanged;
        //         }
        //         break;

        //         default:
        //             Debug.Assert(false, "EditorTypeType not handled");
        //             vm = new TypeViewModel("Unknown", 0, "", false);
        //             break;
        //     }

        //     return vm;
        // }

        private void OnValueChanged(uint offset, string value) {
            //OnModified?.Invoke(componentTypeInfo.typeName, offset, value);
        }
    }
}