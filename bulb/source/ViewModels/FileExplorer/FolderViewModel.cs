using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Windows;

using Membrane = membrane;

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

        public FolderViewModel(string path) : this(new DirectoryInfo(path), null) { }

        public FolderViewModel(DirectoryInfo directory, DirectoryItemViewModel parent)
            : base(directory.Name, directory.FullName, parent) {

            watcher = new FileSystemWatcher(FullPath, "*.*") {
                EnableRaisingEvents = true
            };
            watcher.Created += OnFileCreated;

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

            Membrane.FileSystemHelpers.createAssetFile(assetFileLocation, fileRelativePath, vfsPath);
        }

        #region Item view model events
        private void OnItemOpened(DirectoryItemViewModel item) => OnOpened?.Invoke(item);

        private void OnItemDeleted(DirectoryItemViewModel item) => File.Delete(item.FullPath);
        #endregion

        #region File system events
        private void OnFileCreated(object sender, FileSystemEventArgs e) {
            Application.Current.Dispatcher.Invoke(() => {
                if (File.GetAttributes(e.FullPath).HasFlag(FileAttributes.Directory)) {
                    _ = CreateItem(new DirectoryInfo(e.FullPath));
                } else if (Membrane.FileSystemHelpers.isAssetFile(e.FullPath)) {
                    _ = CreateItem(new FileInfo(e.FullPath));
                }
            });
        }
        #endregion

        private DirectoryItemViewModel CreateItem(DirectoryInfo directory) {
            var vm = new FolderViewModel(directory, this);
            vm.OnOpened += (DirectoryItemViewModel item) => OnItemOpened(item);
            vm.OnDeleted += (DirectoryItemViewModel item) => OnItemDeleted(item);

            SubDirectories.Add(vm);
            AllItems.Add(vm);

            return vm;
        }

        private DirectoryItemViewModel CreateItem(FileInfo file) {
            var vm = new FileViewModel(file, this);
            vm.OnOpened += (DirectoryItemViewModel item) => OnItemOpened(item);
            vm.OnDeleted += (DirectoryItemViewModel item) => OnItemDeleted(item);

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

            Uri fromUri = new Uri(fromPath);
            Uri toUri = new Uri(toPath);

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