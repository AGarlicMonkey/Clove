﻿using System.Windows;

using Membrane = garlic.membrane;

namespace Garlic.Bulb {
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {
		public MainWindow() {
			InitializeComponent();
		}

		private void UserControl_GotFocus(object sender, RoutedEventArgs e) {
			Membrane.Log.write(Membrane.LogLevel.Debug, "Render area focus recieved");
		}

		private void UserControl_KeyUp(object sender, System.Windows.Input.KeyEventArgs e) {
			Membrane.Log.write(Membrane.LogLevel.Debug, $"Key {e.Key} up on render area called.");
		}

		private void RenderAreaFocusHandler_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e) {
			Membrane.Log.write(Membrane.LogLevel.Debug, "Render area mouse enter");
		}

		private void RenderAreaFocusHandler_MouseLeave(object sender, System.Windows.Input.MouseEventArgs e) {
			Membrane.Log.write(Membrane.LogLevel.Debug, "Render area mouse leave");
		}
	}
}
