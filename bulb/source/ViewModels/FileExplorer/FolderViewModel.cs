using System.Collections.ObjectModel;
using System.IO;
using System.Windows;

namespace Bulb {
    public class FolderViewModel : DirectoryItemViewModel {
        /// <summary>
        /// A list of all directories within this directory. Will be empty if this item is a file.
        /// </summary>
        public ObservableCollection<DirectoryItemViewModel> SubDirectories { get; } = new ObservableCollection<DirectoryItemViewModel>();

        /// <summary>
        /// A list of all files within this directory. Will be empty if this item is a file.
        /// </summary>
        public ObservableCollection<DirectoryItemViewModel> Files { get; } = new ObservableCollection<DirectoryItemViewModel>();

        /// <summary>
        /// A list of every single item within this directory. Will be empty if this item is a file.
        /// </summary>
        public ObservableCollection<DirectoryItemViewModel> AllItems { get; } = new ObservableCollection<DirectoryItemViewModel>();

        public override ObjectType Type => ObjectType.Directory;

        private readonly FileSystemWatcher watcher;

        public FolderViewModel(string vfsPath) : this(new DirectoryInfo(ConvertVfsPathToSystemPath(vfsPath)), null) { }

        public FolderViewModel(DirectoryInfo directory, DirectoryItemViewModel parent) 
            : base(parent) {
            Name = directory.Name;
            VfsPath = ConvertSystemPathToVfsPath(directory.FullName);

            watcher = new FileSystemWatcher(FullPath, "*.*") {
                EnableRaisingEvents = true
            };
            watcher.Created += OnFileCreated;
            watcher.Deleted += OnFileDeleted;
            watcher.Renamed += OnFileRenamed;

            foreach (DirectoryInfo dir in directory.EnumerateDirectories()) {
                DirectoryItemViewModel vm = CreateItem(dir);

                SubDirectories.Add(vm);
                AllItems.Add(vm);
            }
            foreach (FileInfo file in directory.EnumerateFiles()) {
                DirectoryItemViewModel vm = CreateItem(file);

                Files.Add(vm);
                AllItems.Add(vm);
            }
        }

        #region Item view model events
        private void OnItemOpened(DirectoryItemViewModel item) {
            //Not handled here. Just pipe up
            OnOpened?.Invoke(item);
        }

        private void OnItemDeleted(DirectoryItemViewModel item) {
            //Delete through the OS. The file system watcher will handle the event back to us.
            File.Delete(item.FullPath);
        }
        #endregion

        #region File system events
        private void OnFileCreated(object sender, FileSystemEventArgs e) {
            Application.Current.Dispatcher.Invoke(() => {
                DirectoryItemViewModel vm = File.GetAttributes(e.FullPath).HasFlag(FileAttributes.Directory)
                    ? CreateItem(new DirectoryInfo(e.FullPath))
                    : CreateItem(new FileInfo(e.FullPath));
                Files.Add(vm);
                AllItems.Add(vm);
            });
        }

        private void OnFileDeleted(object sender, FileSystemEventArgs e) {
            Application.Current.Dispatcher.Invoke(() => {
                DirectoryItemViewModel itemVm = null;
                foreach (var item in AllItems) {
                    if (item.Name == e.Name) {
                        itemVm = item;
                        break;
                    }
                }

                if (itemVm != null) {
                    DeleteItem(itemVm);
                }
            });
        }

        private void OnFileRenamed(object sender, RenamedEventArgs e) {
            Application.Current.Dispatcher.Invoke(() => {
                DirectoryItemViewModel itemVm = null;
                foreach (var item in AllItems) {
                    if (item.Name == e.OldName) {
                        itemVm = item;
                        break;
                    }
                }

                itemVm.UpdateName(e.Name, ConvertSystemPathToVfsPath(e.FullPath));
            });
        }
        #endregion

        private DirectoryItemViewModel CreateItem(DirectoryInfo directory) {
            var vm = new FolderViewModel(directory, this);
            vm.OnOpened += (DirectoryItemViewModel item) => OnItemOpened(item);
            vm.OnDeleted += (DirectoryItemViewModel item) => OnItemDeleted(item);

            return vm;
        }

        private DirectoryItemViewModel CreateItem(FileInfo file) {
            var vm = new FileViewModel(file, this);
            vm.OnOpened += (DirectoryItemViewModel item) => OnItemOpened(item);
            vm.OnDeleted += (DirectoryItemViewModel item) => OnItemDeleted(item);

            return vm;
        }

        private void DeleteItem(DirectoryItemViewModel item) {
            AllItems.Remove(item);
            if (item.Type == ObjectType.Directory) {
                SubDirectories.Remove(item);
            } else {
                Files.Remove(item);
            }
        }
    }
}