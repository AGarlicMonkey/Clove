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
        public ObservableCollection<FolderViewModel> SubDirectories { get; } = new ObservableCollection<FolderViewModel>();

        /// <summary>
        /// A list of all files within this directory. Will be empty if this item is a file.
        /// </summary>
        public ObservableCollection<FileViewModel> Files { get; } = new ObservableCollection<FileViewModel>();

        /// <summary>
        /// A list of every single item within this directory. Will be empty if this item is a file.
        /// </summary>
        public ObservableCollection<DirectoryItemViewModel> AllItems { get; } = new ObservableCollection<DirectoryItemViewModel>();

        public ICommand NewFolderCommand { get; }

        private readonly FileSystemWatcher watcher;

        public FolderViewModel(string path)
            : this(new DirectoryInfo(path), null) {
        }

        public FolderViewModel(DirectoryInfo directory, FolderViewModel parent)
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

            string vfsPath = $"{GetVfsPath()}{Path.DirectorySeparatorChar}{Path.GetFileName(file)}".Replace($"content{Path.DirectorySeparatorChar}", ""); //Remove 'content' from the desired VFS path as this is where the VFS searches from

            Membrane.FileSystemHelpers.createAssetFile(assetFileLocation, file, fileRelativePath, vfsPath);
        }

        /// <summary>
        /// Recursively iterates up the parent chain to get the full VFS path of this folder
        /// </summary>
        /// <returns></returns>
        public string GetVfsPath() => Parent != null ? $"{Parent.GetVfsPath()}{Path.DirectorySeparatorChar}{Name}" : Name;

        private void OnItemOpened(DirectoryItemViewModel item) => OnOpened?.Invoke(item);

        private void OnItemDeleted(DirectoryItemViewModel item) {
            switch (item.Type) {
                case ObjectType.File:
                    File.Delete(item.FullPath);
                    break;
                case ObjectType.Directory:
                    Directory.Delete(item.FullPath, recursive: true);
                    break;
            }
        }

        private void OnFileCreated(FileSystemEventArgs eventArgs) {
            if (File.GetAttributes(eventArgs.FullPath).HasFlag(FileAttributes.Directory)) {
                _ = CreateItem(new DirectoryInfo(eventArgs.FullPath));
            } else if (Membrane.FileSystemHelpers.isAssetFile(eventArgs.FullPath)) {
                _ = CreateItem(new FileInfo(eventArgs.FullPath));
            }
        }

        private void OnFileDeleted(FileSystemEventArgs eventArgs) {
            foreach (DirectoryItemViewModel item in AllItems) {
                if (item.FullPath == eventArgs.FullPath) { //Make sure to compare full paths as some items (especially folders) could share the same name
                    _ = AllItems.Remove(item);
                    if (item is FolderViewModel folderVm) {
                        _ = SubDirectories.Remove(folderVm);
                        //If a directory still has children when we come to delete that means it was not deleted through the editor (as we do it recursively)
                        //so we need to make sure each item is removed from the asset manager.
                        RemoveAllFilesInDirectory(folderVm);
                    } else if (item is FileViewModel fileVm) {
                        _ = Files.Remove(fileVm);
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

        /// <summary>
        /// Recursively iterates all items in a directory and removes them from the asset manager
        /// </summary>
        /// <param name="folderVm"></param>
        private void RemoveAllFilesInDirectory(FolderViewModel folderVm) {
            foreach (FileViewModel file in folderVm.Files) {
                Membrane.FileSystemHelpers.removeAssetFile(file.AssetGuid, file.AssetType);
            }
            foreach (FolderViewModel folder in folderVm.SubDirectories) {
                RemoveAllFilesInDirectory(folder);
            }
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