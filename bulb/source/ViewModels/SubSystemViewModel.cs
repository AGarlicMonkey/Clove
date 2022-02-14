namespace Bulb {
    public class SubSystemViewModel : ViewModel {
        public string DisplayName { get; }
        public string TypeName { get; }

        public SubSystemViewModel(string displayName, string typeName) {
            DisplayName = displayName;
            TypeName = typeName;
        }
    }
}