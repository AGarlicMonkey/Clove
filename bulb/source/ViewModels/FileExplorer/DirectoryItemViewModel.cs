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
        /// Parent directory of this item. Modifying this will cause the item to reconstruct.
        /// </summary>
        public FolderViewModel Parent { 
            get => parent;
            set {
                if (value != parent) {
                    parent = value;
                    if (parent != null) {
                        Reconstruct();
                    }
                }
            }
        }
        private FolderViewModel parent;

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
            this.parent = parent;
        }

        /// <summary>
        /// Renames this item and updates the correlating file. 
        /// </summary>
        /// <param name="newName">New name for this file. It should not include any extensions.</param>
        public abstract void Rename(string newName);

        /// <summary>
        /// Reconstructs the path of this item. Updating the correlating file if need be.
        /// </summary>
        public abstract void Reconstruct();

        /// <summary>
        /// Checks if file can be dropped onto this item.
        /// </summary>
        /// <param name="file"></param>
        /// <returns></returns>
        public abstract bool CanDropFile(string file);

        /// <summary>
        /// Perform a drop operation for file onto this item. This is the expected code path for untracked files.
        /// </summary>
        /// <param name="file">The full file path to drop.</param>
        /// <returns></returns>
        public abstract void OnFileDropped(string file);

        /// <summary>
        /// Perform a drop operation for file onto this item.
        /// </summary>
        /// <param name="file">The viewmodel of the dropped file</param>
        public abstract void OnFileDropped(DirectoryItemViewModel file);
    }
}