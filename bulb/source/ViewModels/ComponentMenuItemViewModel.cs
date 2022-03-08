using System.Windows.Input;

namespace Bulb {
    /// <summary>
    /// Viewmodel for displaying available components in a menu list.
    /// </summary>
    public class ComponentMenuItemViewModel : ViewModel {
        public string Name { get; }
        public ICommand OnSelectedCommand { get; }

        public ComponentMenuItemViewModel(string name, ICommand onSelectedCommand) {
            Name = name;
            OnSelectedCommand = onSelectedCommand;
        }
    }
}