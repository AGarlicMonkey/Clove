#include "clvpch.h"
#include "Application.h"
#include "Clove/Window.h"
#include "Clove/Input/Input.h"
#include "Clove/LayerStack.h"
#include "Clove/Layer.h"
#include "Clove/Events/Event.h"
#include "Clove/Events/ApplicationEvent.h"
#include "Clove/ImGui/ImGuiLayer.h"

namespace clv{
	Application* Application::instance = nullptr;

	Application::Application(){
		CLV_ASSERT(!instance, "Application already exists!");
		instance = this;

		window = std::unique_ptr<Window>(Window::create({ "Clove Engine", 1920, 1080 }));
		window->setEventCallbackFunction(CLV_BIND_FUNCTION_1P(&Application::onEvent, this));

		layerStack = std::make_unique<LayerStack>(LayerStack());

		imGuiLayer = std::make_shared<ImGuiLayer>(ImGuiLayer());
		pushLayer(imGuiLayer);
	}

	Application::~Application() = default;

	void Application::run(){
		while(running){
			window->onUpdate();
			
			for(auto layer : *layerStack){
				layer->onUpdate();
			}

			imGuiLayer->begin();
			for(auto layer : *layerStack){
				layer->onImGuiRender();
			}
			imGuiLayer->end();
		}
	}

	CLV_API void Application::stop(){
		running = false;
	}

	void Application::onEvent(Event& e){
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(CLV_BIND_FUNCTION_1P(&Application::onWindowClose, this));

		if(e.isHandled()){
			return;
		}

		for(auto it = layerStack->end(); it != layerStack->begin(); ){
			(*--it)->onEvent(e);
			if(e.isHandled()){
				break;
			}
		}
	}

	void Application::pushLayer(std::shared_ptr<Layer> layer){
		layerStack->pushLayer(layer);
	}

	void Application::pushOverlay(std::shared_ptr<Layer> overlay){
		layerStack->pushOverlay(overlay);
	}

	bool Application::onWindowClose(WindowCloseEvent& e){
		running = false;
		return true;
	}
}