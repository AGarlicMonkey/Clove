using System.Windows.Input;
using Membrane = membrane;

namespace Bulb {
    /// <summary>
    /// The base viewmodel for all components
    /// </summary>
    public class ComponentViewModel : ViewModel {
        public string Name { get; }
        public Membrane.ComponentType Type { get; }

        public ICommand RemoveComponentCommand { get; }

        public delegate void RemoveComponentHandler(Membrane.ComponentType Type);
        public RemoveComponentHandler OnRemoved;

        public ComponentViewModel(string name, Membrane.ComponentType type) {
            Name = name;
            Type = type;

            RemoveComponentCommand = new RelayCommand(() => OnRemoved?.Invoke(Type));
        }
    }
}