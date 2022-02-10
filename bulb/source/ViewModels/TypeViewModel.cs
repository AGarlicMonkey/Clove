using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows;

using Membrane = membrane;

namespace Bulb {
    /// <summary>
    /// Viewmodel for each editor visible type.
    /// </summary>
    public class TypeViewModel : ViewModel {
        public string Name { get; }

        public string Value {
            get => value;
            set {
                this.value = value;
                OnPropertyChanged(nameof(Value));

                if(DropdownVisibility == Visibility.Visible && DropdownItemTypeInfos != null) {
                    Members.Clear();
                    Members.Add(DropdownItemTypeInfos[DropdownItems.IndexOf(value)]);
                }

                if (value.Length != 0) {
                    OnValueChanged?.Invoke(offsetIntoParent, this.value);
                }
            }
        }
        private string value;
        public Visibility ValueVisibility { get; }

        public ObservableCollection<TypeViewModel> Members { get; }
        public Visibility MembersVisibility { get; }

        public ObservableCollection<string> DropdownItems { get; }
        public Visibility DropdownVisibility { get; }
        private List<TypeViewModel> DropdownItemTypeInfos { get; }

        public delegate void ValueChangedHandler(uint offset, string value);
        public ValueChangedHandler OnValueChanged;

        private readonly uint offsetIntoParent;

        public TypeViewModel(string displayName, List<TypeViewModel> members) {
            Name = displayName;
            Members = new ObservableCollection<TypeViewModel>(members);

            MembersVisibility = Visibility.Visible;
            ValueVisibility = Visibility.Collapsed;
            DropdownVisibility = Visibility.Collapsed;
        }

        public TypeViewModel(string displayName, uint offset, List<string> dropdownItems, int selection, List<TypeViewModel> members) {
            Name = displayName;
            offsetIntoParent = offset;
            DropdownItems = new ObservableCollection<string>(dropdownItems);
            value = DropdownItems[selection]; //Bypass property to prevent delegate being fired.

            if (members != null) {
                MembersVisibility = Visibility.Visible;
                DropdownItemTypeInfos = members;

                Members = new ObservableCollection<TypeViewModel>();
                Members.Add(DropdownItemTypeInfos[selection]);
            } else {
                MembersVisibility = Visibility.Collapsed;
            }

            ValueVisibility = Visibility.Collapsed;
            DropdownVisibility = Visibility.Visible;
        }

        public TypeViewModel(string displayName, uint offset, string value) {
            Name = displayName;
            offsetIntoParent = offset;
            this.value = value; //Bypass property to prevent delegate being fired.

            MembersVisibility = Visibility.Collapsed;
            ValueVisibility = Visibility.Visible;
            DropdownVisibility = Visibility.Collapsed;
        }
    }
}