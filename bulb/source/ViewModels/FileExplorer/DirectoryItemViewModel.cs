using System.Diagnostics;
using System.IO;
using System.Windows;
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
        /// Name of this item.
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
        /// The Virtual File System path of this item.
        /// </summary>
        public string VfsPath { get; protected set; }

        /// <summary>
        /// Full system path of this item.
        /// </summary>
        public string FullPath => ConvertVfsPathToSystemPath(VfsPath);

        public abstract ObjectType Type { get; }

        /// <summary>
        /// Parent directory of this item.
        /// </summary>
        public DirectoryItemViewModel Parent { get; private set; }

        public ICommand OpenCommand { get; }
        public ICommand DeleteCommand { get; }

        public delegate void ItemEventHandler(DirectoryItemViewModel item);
        public ItemEventHandler OnOpened;
        public ItemEventHandler OnDeleted;

        protected DirectoryItemViewModel(DirectoryItemViewModel parent) {
            OpenCommand = new RelayCommand(() => OnOpened?.Invoke(this));
            DeleteCommand = new RelayCommand(() => OnDeleted?.Invoke(this));
            Parent = parent;
        }

        /// <summary>
        /// Called by the view when a file has been dropped onto it. Only works if this item is a directory.
        /// </summary>
        /// <param name="file">The full path of the file</param>
        public void OnFileDropped(string file) {
            Debug.Assert(Type == ObjectType.Directory, "Cannot perform a drop operation on a file");

            var fileInfo = new FileInfo(file);
            string originalPath = file;
            string newPath = Path.Combine(FullPath, fileInfo.Name);

            File.Copy(originalPath, newPath);
        }

        public void UpdateName(string name, string vfsPath) {
            Name = name;
            VfsPath = vfsPath;
        }

        protected static string ConvertVfsPathToSystemPath(string vfsPath) => ((EditorApp)Application.Current).resolveVfsPath(vfsPath);

        protected static string ConvertSystemPathToVfsPath(string systemPath) {
            string rootSystem = ConvertVfsPathToSystemPath(".");
            return systemPath.Remove(0, rootSystem.Length).Replace("\\", "/").Insert(0, ".");
        }
    }
}