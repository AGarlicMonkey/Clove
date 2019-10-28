#include "Application.hpp"

#include "Core/Platform/Platform.hpp"
#include "Core/Platform/Window.hpp"
#include "Core/Input/Input.hpp"
#include "Core/LayerStack.hpp"
#include "Core/Layer.hpp"
#include "Core/Utils/DeltaTime.hpp"
#include "Core/ECS/Manager.hpp"

#include "Core/Graphics/Renderer.hpp"
#include "Core/Graphics/Renderer2D.hpp"
#include "Core/Graphics/RenderCommand.hpp"

namespace clv{
	Application* Application::instance = nullptr;

	Application::Application(){
		initialise(plt::Platform::createWindow());
	}
    
    Application::~Application(){
        CLV_LOG_INFO("Clove shutting down...");
		gfx::Renderer::shutDown();
		gfx::Renderer2D::shutDown();
	}
    
	Application::Application(std::unique_ptr<plt::Window>&& window){
		initialise(std::move(window));
	}

	void Application::update(){
		auto currFrameTime = std::chrono::system_clock::now();
		std::chrono::duration<float> deltaSeonds = currFrameTime - prevFrameTime;
		prevFrameTime = currFrameTime;

		window->beginFrame();

		//TODO:
		//Will need process the mouse and keyboard events here eventually

		for(auto layer : *layerStack){
			layer->onUpdate(deltaSeonds.count());
		}

		gfx::Renderer::clearRenderTargets();

		gfx::Renderer::beginScene();
		gfx::Renderer2D::beginScene();

		ecsManager->update(deltaSeonds.count());

		gfx::Renderer::endScene();
		gfx::Renderer2D::endScene();

		window->endFrame();
	}

	ApplicationState Application::getState() const{
		return currentState;
	}

	void Application::stop(){
		currentState = ApplicationState::stopped;
	}

	void Application::pushLayer(std::shared_ptr<Layer> layer){
		layerStack->pushLayer(layer);
	}

	void Application::pushOverlay(std::shared_ptr<Layer> overlay){
		layerStack->pushOverlay(overlay);
	}

	Application& Application::get(){
		return *instance;
	}

	plt::Window& Application::getWindow(){
		return *window;
	}

	ecs::Manager& Application::getManager(){
		return *ecsManager;
	}

	void Application::onWindowClose(){
		stop();
	}

	void Application::initialise(std::unique_ptr<plt::Window>&& window){
		CLV_ASSERT(!instance, "Application already exists!");
		instance = this;

		plt::Platform::prepare();

		this->window = std::move(window);
		this->window->onWindowCloseDelegate.bind(&Application::onWindowClose, this);
		this->window->setVSync(true);

		gfx::RenderCommand::initialiseRenderAPI(this->window->getContext());
		gfx::RenderCommand::setClearColour({ 1.0f, 0.54f, 0.1f, 1.0f });

		gfx::Renderer::initialise();
		gfx::Renderer2D::initialise();

		ecsManager = std::make_unique<ecs::Manager>();
		layerStack = std::make_unique<LayerStack>();

		CLV_LOG_INFO("Successfully initialised Clove");

		prevFrameTime = std::chrono::system_clock::now();
	}
}