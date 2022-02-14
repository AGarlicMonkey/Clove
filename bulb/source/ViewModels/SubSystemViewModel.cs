namespace Bulb {
    public class SubSystemViewModel : ViewModel {
        public string DisplayName { get; }
        public string TypeName { get; }

        public bool IsEnabled {
            get => isEnabled;
            set {
                isEnabled = value;
                OnPropertyChanged(nameof(isEnabled));

                OnToggled?.Invoke(this, value);
            }
        }
        private bool isEnabled = false;

        public delegate void ToggledHandler(SubSystemViewModel viewModel, bool value);
        public ToggledHandler OnToggled;

        public SubSystemViewModel(string displayName, string typeName) {
            DisplayName = displayName;
            TypeName = typeName;
        }
    }
}