using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Input;
using Membrane = membrane;

namespace Bulb {
    public class FolderViewModel : DirectoryItemViewModel {
        public override ObjectType Type => ObjectType.Directory;

        /// <summary>
        /// A list of all directories within this directory.
        /// </summary>
        public ObservableCollection<FolderViewModel> SubDirectories { get; } = new ObservableCollection<FolderViewModel>();

        /// <summary>
        /// A list of all files within this directory.
        /// </summary>
        public ObservableCollection<FileViewModel> Files { get; } = new ObservableCollection<FileViewModel>();

        /// <summary>
        /// A list of every single item within this directory.
        /// </summary>
        public ObservableCollection<DirectoryItemViewModel> AllItems { get; } = new ObservableCollection<DirectoryItemViewModel>();

        public ICommand NewFolderCommand { get; }

        /// <summary>
        /// The full Virtual File System path of this item.
        /// </summary>
        public string VfsPath => Parent != null ? $"{Parent.VfsPath}/{Name}" : Name;

        private readonly FileSystemWatcher watcher;

        //Ignore deleting this file through the asset manager if an event fires for it. Requried when moving files.
        private static string ignoreDelete = null;

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
                AddItem(new FolderViewModel(dir, parent: this));
            }
            foreach (FileInfo file in directory.EnumerateFiles()) {
                AddItem(new FileViewModel(file, parent: this));
            }
        }

        public override bool CanDropFile(string file) => Path.GetDirectoryName(file) != FullPath && Membrane.FileSystemHelpers.isFileSupported(file);

        public override void OnFileDropped(string file) {
            if (!Membrane.FileSystemHelpers.isAssetFile(file)) {
                //Create an asset file from the external dropped file
                string fileName = Path.GetFileNameWithoutExtension(file);
                string assetFileLocation = $"{FullPath}{Path.DirectorySeparatorChar}{fileName}.clvasset";
                string fileRelativePath = MakeRelativePath(assetFileLocation, file);

                string vfsPath = $"{VfsPath}/{Path.GetFileName(file)}".Replace($"content/", ""); //Remove 'content' from the desired VFS path as this is where the VFS searches from

                Membrane.FileSystemHelpers.createAssetFile(assetFileLocation, file, fileRelativePath, vfsPath);
            }
        }

        public override void OnFileDropped(DirectoryItemViewModel file) {
            if (file != null && !AllItems.Contains(file)) {
                //Move the asset file into this folder
                var fileParent = file.Parent;

                watcher.EnableRaisingEvents = false;
                fileParent.watcher.EnableRaisingEvents = false;

                string newFilePath = $"{FullPath}{Path.DirectorySeparatorChar}{file.Name}";

                if (file is FileViewModel fileVm) {
                    File.Move(file.FullPath, newFilePath);

                    fileParent.RemoveItem(fileVm);
                    AddItem(fileVm);
                } else if(file is FolderViewModel folderVm){
                    Directory.Move(file.FullPath, newFilePath);

                    fileParent.RemoveItem(folderVm);
                    AddItem(folderVm);
                } else {
                    Debug.Assert(false, "Unknown directory item");
                }

                file.Parent = this;

                watcher.EnableRaisingEvents = true;
                fileParent.watcher.EnableRaisingEvents = true;
            }
        }

        public override void Rename(string newName) {
            string newPath = $"{Parent.FullPath}{Path.DirectorySeparatorChar}{newName}";

            Directory.Move(FullPath, newPath);

            Name = newName;
            FullPath = newPath;

            Reconstruct();
        }

        public override void Reconstruct() {
            FullPath = $"{Parent.FullPath}{Path.DirectorySeparatorChar}{Name}";

            foreach (DirectoryItemViewModel item in AllItems) {
                item.Reconstruct();
            }
        }

        private void OnItemOpened(DirectoryItemViewModel item) => OnOpened?.Invoke(item);

        private void OnItemDeleted(DirectoryItemViewModel item) {
            switch (item.Type) {
                case ObjectType.File:
                    File.Delete(item.FullPath);
                    break;
                case ObjectType.Directory:
                    Directory.Delete(item.FullPath, recursive: true);
                    break;
                default:
                    break;
            }
        }

        private void OnFileCreated(FileSystemEventArgs eventArgs) {
            if (File.GetAttributes(eventArgs.FullPath).HasFlag(FileAttributes.Directory)) {
                AddItem(new FolderViewModel(new DirectoryInfo(eventArgs.FullPath), parent: this));
            } else if (Membrane.FileSystemHelpers.isAssetFile(eventArgs.FullPath)) {
                AddItem(new FileViewModel(new FileInfo(eventArgs.FullPath), parent: this));
            }
        }

        private void OnFileDeleted(FileSystemEventArgs eventArgs) {
            foreach (DirectoryItemViewModel item in AllItems) {
                if (item.FullPath == eventArgs.FullPath) { //Make sure to compare full paths as some items (especially folders) could share the same name
                    if (item is FolderViewModel folderVm) {
                        RemoveItem(folderVm);

                        //If a directory still has children when we come to delete that means it was not deleted through the editor (as we do it recursively)
                        //so we need to make sure each item is removed from the asset manager.
                        RemoveAllFilesInDirectory(folderVm);
                    } else if (item is FileViewModel fileVm) {
                        RemoveItem(fileVm);

                        if (ignoreDelete == fileVm.FullPath) {
                            ignoreDelete = null;
                        } else {
                            Membrane.FileSystemHelpers.removeAssetFile(fileVm.AssetGuid, fileVm.AssetType);
                        }
                    }

                    break;
                }
            }
        }

        /// <summary>
        /// Creates a new folder inside this current folder
        /// </summary>
        private void CreateNewFolder() => _ = Directory.CreateDirectory($"{FullPath}{Path.DirectorySeparatorChar}NewFolder");

        private void AddItem(FolderViewModel folder) {
            folder.OnOpened += OnItemOpened;
            folder.OnDeleted += OnItemDeleted;

            SubDirectories.Add(folder);
            AllItems.Add(folder);
        }

        private void AddItem(FileViewModel file) {
            file.OnOpened += OnItemOpened;
            file.OnDeleted += OnItemDeleted;

            Files.Add(file);
            AllItems.Add(file);
        }

        private void RemoveItem(FolderViewModel folder) {
            folder.OnOpened -= OnItemOpened;
            folder.OnDeleted -= OnItemDeleted;

            SubDirectories.Remove(folder);
            AllItems.Remove(folder);
        }

        private void RemoveItem(FileViewModel file) {
            file.OnOpened -= OnItemOpened;
            file.OnDeleted -= OnItemDeleted;

            Files.Remove(file);
            AllItems.Remove(file);
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