using System.Windows.Input;

using Membrane = membrane;

namespace Bulb {
    /// <summary>
    /// The View Model for the entire editor's session. Used to manage the viewmodels that make up the editor itself.
    /// </summary>
    public class EditorSessionViewModel : ViewModel {
        private enum PlayMode {
            Playing,
            Stopped,
        }

        public ICommand LoadSceneCommand { get; }
        public ICommand SaveSceneCommand { get; }
        public ICommand PlayCommand { get; }
        public ICommand StopCommand { get; }
        public ICommand CompileCommand { get; }

        public SceneViewModel Scene {
            get => scene;
            set {
                scene = value;
                OnPropertyChanged(nameof(Scene));
            }
        }
        private SceneViewModel scene = new SceneViewModel();

        public FileExplorerViewModel FileExplorer {
            get => fileExplorer;
            set {
                fileExplorer = value;
                OnPropertyChanged(nameof(FileExplorer));
            }
        }
        private FileExplorerViewModel fileExplorer;

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

        private PlayMode playMode;

        public delegate void CompileDelegate();
        public CompileDelegate OnCompileGame;

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

            CompileCommand = new RelayCommand(() => OnCompileGame?.Invoke());

            //TEMP: See below
            //FileExplorer = new FileExplorerViewModel(rootFilePath);

            WindowTitle = $"Clove - {Membrane.Application.getProjectVersion()}";

            //Assuming we are initialised as stopped
            playMode = PlayMode.Stopped;
        }

        //TEMP: Just to make sure functions don't get called before opening a window.
        public void Start(string rootFilePath) => FileExplorer = new FileExplorerViewModel(rootFilePath);

        public void Play() {
            if(playMode == PlayMode.Playing) {
                return;
            }
            playMode = PlayMode.Playing;

            IsPlayButtonEnabled = false;
            IsStopButtonEnabled = true;
            Membrane.MessageHandler.sendMessage(new Membrane.Editor_Play());
        }

        public void Stop() {
            if(playMode == PlayMode.Stopped) {
                return;
            }
            playMode = PlayMode.Stopped;

            IsPlayButtonEnabled = true;
            IsStopButtonEnabled = false;
            Membrane.MessageHandler.sendMessage(new Membrane.Editor_Stop());
        }

        private void OnSceneLoaded(Membrane.Engine_OnSceneLoaded message) => Scene = new SceneViewModel(message.enabledSubSystems, message.entities);
    }
}