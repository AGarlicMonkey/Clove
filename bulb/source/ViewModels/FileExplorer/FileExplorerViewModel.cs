using System.Windows.Input;

namespace Bulb {
    /// <summary>
    /// Manages the current position in the file explorer.
    /// </summary>
    public class FileExplorerViewModel : ViewModel {
        /// <summary>
        /// The root directory this explorer views from.
        /// </summary>
        public DirectoryItemViewModel RootDirectory {
            get => rootDirectory;
            private set {
                rootDirectory = value;
                OnPropertyChanged(nameof(RootDirectory));
            }
        }
        private DirectoryItemViewModel rootDirectory;

        /// <summary>
        /// The current directory this explorer has opened into.
        /// </summary>
        public DirectoryItemViewModel CurrentDirectory {
            get => currentDirectory;
            private set {
                currentDirectory = value;
                OnPropertyChanged(nameof(currentDirectory));
                OnPropertyChanged(nameof(CanStepBack));
            }
        }
        private DirectoryItemViewModel currentDirectory;

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

                CurrentDirectory = item;
                CurrentDirectory.OnOpened += OnItemOpened;
            }

            //TODO: Support opening files
        }
    }
}