#include "Core/Input/Input.hpp"

#include "Core/Platform/Application.hpp"
#include "Core/Platform/Window.hpp"

namespace clv{
	namespace input{
		bool isKeyPressed(Key key){
			return plt::Application::get().getWindow().getKeyboard().isKeyPressed(key);
		}

		bool isMouseButtonPressed(MouseButton button){
			return plt::Application::get().getWindow().getMouse().isButtonPressed(button);
		}

		std::pair<int32, int32> getMousePosition(){
			return plt::Application::get().getWindow().getMouse().getPosition();
		}

		int32 getMouseX(){
			auto[x, y] = getMousePosition();
			return x;
		}

		int32 getMouseY(){
			auto[x, y] = getMousePosition();
			return y;
		}
	}
}