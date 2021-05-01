using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows;

using Membrane = garlic.membrane;

namespace Garlic.Bulb {
	/// <summary>
	/// The View Model for the editor's current scene.
	/// </summary>
	public class SceneViewModel : ViewModel {
		public ICommand CreateEntityCommand { get; }
		public ICommand AddComponentCommand { get; }

		public ObservableCollection<EntityViewModel> Entities { get; } = new ObservableCollection<EntityViewModel>();

		public EntityViewModel SelectedEntity {
			get { return selectedEntity; }
			set {
				selectedEntity = value;
				OnPropertyChanged(nameof(SelectedEntity));
				OnPropertyChanged(nameof(EntityViewVisibility));
			}
		}
		private EntityViewModel selectedEntity;

		public Visibility EntityViewVisibility {
			get {
				return selectedEntity != null ? Visibility.Visible : Visibility.Collapsed;
			}
		}

		public SceneViewModel() {
			//Bind to messages
			Membrane.MessageHandler.bindToMessage<Membrane.Engine_OnEntityCreated>(OnEntityCreated);

			//Set up commands
			CreateEntityCommand = new RelayCommand(() => Membrane.MessageHandler.sendMessage(new Membrane.Editor_CreateEntity()));
			AddComponentCommand = new RelayCommand<Membrane.ComponentType>(CreateComponent);
		}

		public SceneViewModel(List<Membrane.Entity> entities) : this() {
			//TODO: Move to function
			foreach (var entity in entities){
				var entityVm = new EntityViewModel(entity.components);
				entityVm.EntityId = entity.id;
				entityVm.Name = entity.name;
				entityVm.OnSelected = (EntityViewModel viewModel) => SelectedEntity = viewModel;

				Entities.Add(entityVm);
			}
		}

		private void OnEntityCreated(Membrane.Engine_OnEntityCreated message) {
			var entityVm = new EntityViewModel();
			entityVm.EntityId = message.entity;
			entityVm.Name = message.name;
			entityVm.OnSelected = (EntityViewModel viewModel) => SelectedEntity = viewModel;

			Entities.Add(entityVm);
		}

		private void CreateComponent(Membrane.ComponentType componentType) {
			if(SelectedEntity != null) {
				SelectedEntity.AddComponent(componentType);
            } else {
				Membrane.Log.write(Membrane.LogLevel.Warning, $"CreateComponent called with unkown type: {componentType}");
            }
        }
	};
}