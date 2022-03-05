using System.Windows.Input;

namespace Bulb {
    public enum ObjectType {
        Directory,
        File
    }

    /// <summary>
    /// Contains information for an entire single directory or file.
    /// </summary>
    public abstract class DirectoryItemViewModel : ViewModel {
        /// <summary>
        /// Name of this item with extension.
        /// </summary>
        public string Name {
            get => name;
            protected set {
                name = value;
                OnPropertyChanged(nameof(Name));
            }
        }
        private string name;

        /// <summary>
        /// Full system path of this item.
        /// </summary>
        public string FullPath { get; protected set; }

        /// <summary>
        /// Parent directory of this item.
        /// </summary>
        public FolderViewModel Parent { get; }

        public abstract ObjectType Type { get; }

        public ICommand OpenCommand { get; }
        public ICommand DeleteCommand { get; }

        public delegate void ItemEventHandler(DirectoryItemViewModel item);
        public ItemEventHandler OnOpened;
        public ItemEventHandler OnDeleted;

        protected DirectoryItemViewModel(string itemName, string itemFullPath, FolderViewModel parent) {
            Name = itemName;
            FullPath = itemFullPath;
            OpenCommand = new RelayCommand(() => OnOpened?.Invoke(this));
            DeleteCommand = new RelayCommand(() => OnDeleted?.Invoke(this));
            Parent = parent;
        }

        /// <summary>
        /// Renames this item and updates correlating file. 
        /// </summary>
        /// <param name="newName">New name for this file. It should not include any extensions.</param>
        public abstract void Rename(string newName);

        /// <summary>
        /// Checks if file can be dropped onto this item.
        /// </summary>
        /// <param name="file"></param>
        /// <returns></returns>
        public abstract bool CanDropFile(string file);

        /// <summary>
        /// Perform a drop operation for file onto this item.
        /// </summary>
        /// <param name="file">The full path of the file</param>
        public abstract void OnFileDropped(string file);
    }
}