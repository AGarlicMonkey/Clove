using System;
using System.Windows.Controls;
using System.Windows.Input;

namespace Bulb {
    /// <summary>
    /// Manages the current position in the file explorer.
    /// </summary>
    public class FileExplorerViewModel : ViewModel {
        /// <summary>
        /// The root directory this explorer views from.
        /// </summary>
        public FolderViewModel RootDirectory {
            get => rootDirectory;
            private set {
                rootDirectory = value;
                OnPropertyChanged(nameof(RootDirectory));
            }
        }
        private FolderViewModel rootDirectory;

        /// <summary>
        /// The current directory this explorer has opened into.
        /// </summary>
        public FolderViewModel CurrentDirectory {
            get => currentDirectory;
            private set {
                currentDirectory = value;

                OnPropertyChanged(nameof(CurrentDirectory));
                OnPropertyChanged(nameof(CanStepBack));
            }
        }
        private FolderViewModel currentDirectory;

        public ICommand StepBackCommand { get; }
        public bool CanStepBack => CurrentDirectory.Parent != null;

        public FileExplorerViewModel(string rootPath) {
            var directoryViewModel = new FolderViewModel(rootPath);
            directoryViewModel.OnOpened += OnItemOpened;

            rootDirectory = directoryViewModel;
            CurrentDirectory = directoryViewModel;

            StepBackCommand = new RelayCommand(() => {
                if (CanStepBack) {
                    CurrentDirectory = CurrentDirectory.Parent;
                }
            });
        }

        private void OnItemOpened(DirectoryItemViewModel item) {
            if (item.Type == ObjectType.Directory) {
                if (CurrentDirectory != RootDirectory) {
                    CurrentDirectory.OnOpened -= OnItemOpened;
                }

                CurrentDirectory = item as FolderViewModel;
                CurrentDirectory.OnOpened += OnItemOpened;
            }

            //TODO: Support opening files
        }
    }
}