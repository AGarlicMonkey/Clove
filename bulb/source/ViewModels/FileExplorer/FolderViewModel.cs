using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
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

        public FolderViewModel(string path)
            : this(new DirectoryInfo(path), null) {
        }

        public FolderViewModel(DirectoryInfo directory, FolderViewModel parent)
            : base(directory.Name, directory.FullName, parent) {
            NewFolderCommand = new RelayCommand(() => CreateNewFolder());

            foreach (DirectoryInfo dir in directory.EnumerateDirectories()) {
                AddItem(new FolderViewModel(dir, parent: this));
            }
            foreach (FileInfo file in directory.EnumerateFiles()) {
                AddItem(new FileViewModel(file, parent: this));
            }
        }

        public override bool CanDropFile(string file) {
            if (Path.GetDirectoryName(file) == FullPath) {
                return false;
            }

            if (File.GetAttributes(file).HasFlag(FileAttributes.Directory)) {
                return true;
            } else {
                return Path.GetDirectoryName(file) != FullPath && Membrane.FileSystemHelpers.isFileSupported(file);
            }
        }

        public override void OnFileDropped(string file) {
            if (!Membrane.FileSystemHelpers.isAssetFile(file) && Membrane.FileSystemHelpers.isFileSupported(file)) {
                //Create an asset file from the external dropped file
                string fileName = Path.GetFileNameWithoutExtension(file);
                string assetFileLocation = $"{FullPath}{Path.DirectorySeparatorChar}{fileName}.clvasset";
                string fileRelativePath = MakeRelativePath(assetFileLocation, file);

                string vfsPath = $"{VfsPath}/{Path.GetFileName(file)}".Replace($"content/", ""); //Remove 'content' from the desired VFS path as this is where the VFS searches from

                Membrane.FileSystemHelpers.createAssetFile(assetFileLocation, file, fileRelativePath, vfsPath);

                AddItem(new FileViewModel(new FileInfo(assetFileLocation), parent: this));
            }
        }

        public override void OnFileDropped(DirectoryItemViewModel file) {
            if (file != null && !AllItems.Contains(file)) {
                //Move the asset file into this folder
                var fileParent = file.Parent;

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
            if (item is FileViewModel fileVm) {
                RemoveItem(fileVm);
                Membrane.FileSystemHelpers.removeAssetFile(fileVm.AssetGuid, fileVm.AssetType);
                File.Delete(fileVm.FullPath);
            } else if (item is FolderViewModel folderVm) {
                RemoveItem(folderVm);
                RemoveAllFilesInDirectory(folderVm);
                Directory.Delete(folderVm.FullPath, recursive: true);
            }
        }

        private void CreateNewFolder() {
            DirectoryInfo directoryInfo = Directory.CreateDirectory($"{FullPath}{Path.DirectorySeparatorChar}NewFolder");
            AddItem(new FolderViewModel(directoryInfo, parent: this));
        }

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