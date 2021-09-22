using System.Windows.Input;

using Membrane = membrane;

namespace Bulb {
    /// <summary>
    /// The View Model for the entire editor's session. Used to manage the viewmodels that make up the editor itself.
    /// </summary>
    public class EditorSessionViewModel : ViewModel {
        public ICommand LoadSceneCommand { get; }
        public ICommand SaveSceneCommand { get; }
        public ICommand PlayCommand { get; }
        public ICommand StopCommand { get; }

        public SceneViewModel Scene {
            get => scene;
            set {
                scene = value;
                OnPropertyChanged(nameof(Scene));
            }
        }
        private SceneViewModel scene = new SceneViewModel();

        public FileExplorerViewModel FileExplorer { get; }
        public LogViewModel Log { get; } = new LogViewModel();

        public string WindowTitle { get; }

        public bool IsPlayButtonEnabled {
            get => isPlayButtonEnabled;
            set {
                isPlayButtonEnabled = value;
                OnPropertyChanged(nameof(IsPlayButtonEnabled));
            }
        }
        private bool isPlayButtonEnabled = true;

        public bool IsStopButtonEnabled {
            get => isStopButtonEnabled;
            set {
                isStopButtonEnabled = value;
                OnPropertyChanged(nameof(IsStopButtonEnabled));
            }
        }
        private bool isStopButtonEnabled = false;

        public EditorSessionViewModel(string rootFilePath) {
            //Bind to messages
            Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnSceneLoaded>(OnSceneLoaded);

            //Set up commands
            LoadSceneCommand = new RelayCommand(() => {
                Membrane.MessageHandler.sendMessage(new Membrane.Editor_LoadScene());
            });
            SaveSceneCommand = new RelayCommand(() => {
                Membrane.MessageHandler.sendMessage(new Membrane.Editor_SaveScene());
            });
            PlayCommand = new RelayCommand(Play);
            StopCommand = new RelayCommand(Stop);

            FileExplorer = new FileExplorerViewModel(rootFilePath);

            WindowTitle = $"Clove - {Membrane.Application.getProjectVersion()}";
        }

        private void OnSceneLoaded(Membrane.Engine_OnSceneLoaded message) {
            Scene = new SceneViewModel(message.entities);
        }

        private void Play() {
            IsPlayButtonEnabled = false;
            IsStopButtonEnabled = true;
            Membrane.MessageHandler.sendMessage(new Membrane.Editor_Play());
        }

        private void Stop() {
            IsPlayButtonEnabled = true;
            IsStopButtonEnabled = false;
            Membrane.MessageHandler.sendMessage(new Membrane.Editor_Stop());
        }
    }
}