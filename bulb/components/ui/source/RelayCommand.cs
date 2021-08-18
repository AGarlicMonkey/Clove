using System;
using System.Windows.Input;

namespace Bulb {
    /// <summary>
    /// Simple RelayCommand that invokes an action when executed.
    /// </summary>
    public class RelayCommand : ICommand {
        public event EventHandler CanExecuteChanged = (sender, e) => { };

        private Action action;

        public RelayCommand(Action action) => this.action = action;

        public bool CanExecute(object parameter) => true;

        public void Execute(object parameter) => action();
    }

    /// <summary>
    /// Simple RelayCommand that invokes and action with a single parameter when executed
    /// </summary>
    /// <typeparam name="ParameterType"></typeparam>
    public class RelayCommand<ParameterType> : ICommand {
        public event EventHandler CanExecuteChanged = (sender, e) => { };

        private Action<ParameterType> action;

        public RelayCommand(Action<ParameterType> action) => this.action = action;

        public bool CanExecute(object parameter) => true;

        public void Execute(object parameter) => action((ParameterType)parameter);
    }
}