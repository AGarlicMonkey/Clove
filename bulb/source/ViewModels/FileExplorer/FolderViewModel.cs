using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Windows;
using System.Windows.Input;
using Membrane = membrane;

namespace Bulb {
    public class FolderViewModel : DirectoryItemViewModel {
        public override ObjectType Type => ObjectType.Directory;

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

        public ICommand NewFolderCommand { get; }

        private readonly FileSystemWatcher watcher;

        public FolderViewModel(string path)
            : this(new DirectoryInfo(path), null) {
        }

        public FolderViewModel(DirectoryInfo directory, DirectoryItemViewModel parent)
            : base(directory.Name, directory.FullName, parent) {
            NewFolderCommand = new RelayCommand(() => CreateNewFolder());

            watcher = new FileSystemWatcher(FullPath, "*.*") {
                EnableRaisingEvents = true
            };
            //Watcher events come from a different thread. So pass them through the app's dispatcher
            watcher.Created += (sender, e) => Application.Current.Dispatcher.Invoke(() => OnFileCreated(e));
            watcher.Deleted += (sender, e) => Application.Current.Dispatcher.Invoke(() => OnFileDeleted(e));

            foreach (DirectoryInfo dir in directory.EnumerateDirectories()) {
                _ = CreateItem(dir);
            }
            foreach (FileInfo file in directory.EnumerateFiles()) {
                _ = CreateItem(file);
            }
        }

        public override bool CanDropFile(string file) => Membrane.FileSystemHelpers.isFileSupported(file);

        public override void OnFileDropped(string file) {
            string fileName = Path.GetFileNameWithoutExtension(file);
            string assetFileLocation = $"{FullPath}{Path.DirectorySeparatorChar}{fileName}.clvasset";
            string fileRelativePath = MakeRelativePath(assetFileLocation, file);

            string vfsPath = $"{Name}{Path.DirectorySeparatorChar}{Path.GetFileName(file)}".Replace($"content{Path.DirectorySeparatorChar}", ""); //Remove 'content' from the desired VFS path as this is where the VFS searches from

            Membrane.FileSystemHelpers.createAssetFile(assetFileLocation, file, fileRelativePath, vfsPath);
        }

        private void OnItemOpened(DirectoryItemViewModel item) => OnOpened?.Invoke(item);

        private void OnItemDeleted(DirectoryItemViewModel item) => File.Delete(item.FullPath);

        private void OnFileCreated(FileSystemEventArgs eventArgs) {
            if (File.GetAttributes(eventArgs.FullPath).HasFlag(FileAttributes.Directory)) {
                _ = CreateItem(new DirectoryInfo(eventArgs.FullPath));
            } else if (Membrane.FileSystemHelpers.isAssetFile(eventArgs.FullPath)) {
                _ = CreateItem(new FileInfo(eventArgs.FullPath));
            }
        }

        private void OnFileDeleted(FileSystemEventArgs eventArgs) {
            foreach (DirectoryItemViewModel item in AllItems) {
                if (item.Name == eventArgs.Name) {
                    _ = AllItems.Remove(item);
                    if (item is FolderViewModel folderVm) {
                        _ = SubDirectories.Remove(item);
                        //TODO: Loop through and delete children
                    } else if (item is FileViewModel fileVm) {
                        _ = Files.Remove(item);
                        Membrane.FileSystemHelpers.removeAssetFile(fileVm.AssetGuid, fileVm.AssetType);
                    }

                    item.OnOpened -= OnItemOpened;
                    item.OnDeleted -= OnItemDeleted;

                    break;
                }
            }
        }

        /// <summary>
        /// Creates a new folder inside this current folder
        /// </summary>
        private void CreateNewFolder() => _ = Directory.CreateDirectory($"{FullPath}{Path.DirectorySeparatorChar}NewFolder");

        private DirectoryItemViewModel CreateItem(DirectoryInfo directory) {
            var vm = new FolderViewModel(directory, this);
            vm.OnOpened += OnItemOpened;
            vm.OnDeleted += OnItemDeleted;

            SubDirectories.Add(vm);
            AllItems.Add(vm);

            return vm;
        }

        private DirectoryItemViewModel CreateItem(FileInfo file) {
            var vm = new FileViewModel(file, this);
            vm.OnOpened += OnItemOpened;
            vm.OnDeleted += OnItemDeleted;

            Files.Add(vm);
            AllItems.Add(vm);

            return vm;
        }

        //Gets the relative path - TODO: Update to .Net5.0+ to use Path.GetRelativePath
        private static string MakeRelativePath(string fromPath, string toPath) {
            if (string.IsNullOrEmpty(fromPath)) {
                throw new ArgumentNullException("fromPath");
            }

            if (string.IsNullOrEmpty(toPath)) {
                throw new ArgumentNullException("toPath");
            }

            var fromUri = new Uri(fromPath);
            var toUri = new Uri(toPath);

            if (fromUri.Scheme != toUri.Scheme) { return toPath; } // path can't be made relative.

            Uri relativeUri = fromUri.MakeRelativeUri(toUri);
            string relativePath = Uri.UnescapeDataString(relativeUri.ToString());

            if (toUri.Scheme.Equals("file", StringComparison.InvariantCultureIgnoreCase)) {
                relativePath = relativePath.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);
            }

            return relativePath;
        }
    }
}