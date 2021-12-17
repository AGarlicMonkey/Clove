#include "Clove/Application.hpp"

#include "Clove/InputEvent.hpp"
#include "Clove/Rendering/HighDefinitionRenderer.hpp"
#include "Clove/Rendering/GraphicsImageRenderTarget.hpp"
#include "Clove/Rendering/SwapchainRenderTarget.hpp"
#include "Clove/SubSystems/AudioSubSystem.hpp"
#include "Clove/SubSystems/PhysicsSubSystem.hpp"
#include "Clove/SubSystems/RenderSubSystem.hpp"
#include "Clove/SubSystems/TransformSubSystem.hpp"

#include <Clove/Audio/AhaDevice.hpp>
#include <Clove/Definitions.hpp>
#include <Clove/Graphics/Gha.hpp>
#include <Clove/Graphics/GhaDevice.hpp>

namespace clove {
    Application *Application::instance{ nullptr };

    Application::~Application() {
        //Tell all subSystems they have been detached when the application is shutdown
        for(auto &&[key, group] : subSystems) {
            for(auto &subSystem : group) {
                subSystem->onDetach();
            }
        }
    }

    std::unique_ptr<Application> Application::create(GraphicsApi graphicsApi, AudioApi audioApi, Window::Descriptor const &windowDescriptor) {
        CLOVE_LOG(CloveApplication, LogLevel::Info, "Creating windowed application ({0}, {1}).", windowDescriptor.width, windowDescriptor.height);

        auto window{ Window::create(windowDescriptor) };
        auto *windowPtr{ window.get() };

        auto graphicsDevice{ createGhaDevice(graphicsApi, window->getNativeWindow()).getValue() };
        auto audioDevice{ createAhaDevice(audioApi).getValue() };

        uint32_t constexpr swapchainImageCount{ 3 };
        auto renderTarget{ std::make_unique<SwapchainRenderTarget>(*window, graphicsDevice.get(), swapchainImageCount) };

        std::unique_ptr<Application> app{ new Application{ std::move(graphicsDevice), std::move(audioDevice), std::move(window), std::move(renderTarget) } };
        windowPtr->onWindowCloseDelegate.bind(&Application::shutdown, app.get());

        return app;
    }

    std::pair<std::unique_ptr<Application>, GraphicsImageRenderTarget *> Application::createHeadless(GraphicsApi graphicsApi, AudioApi audioApi, GhaImage::Descriptor renderTargetDescriptor, Keyboard *keyboard, Mouse *mouse) {
        CLOVE_LOG(CloveApplication, LogLevel::Info, "Creating headless application.");

        auto graphicsDevice{ createGhaDevice(graphicsApi, std::any{}).getValue() };
        auto audioDevice{ createAhaDevice(audioApi).getValue() };

        auto renderTarget{ std::make_unique<GraphicsImageRenderTarget>(renderTargetDescriptor, graphicsDevice->getGraphicsFactory()) };
        auto *renderTargetPtr{ renderTarget.get() };

        std::unique_ptr<Application> app{ new Application{ std::move(graphicsDevice), std::move(audioDevice), keyboard, mouse, std::move(renderTarget) } };

        return { std::move(app), renderTargetPtr };
    }

    void Application::set(Application *app) {
        instance = app;
    }

    Application &Application::get() {
        CLOVE_ASSERT_MSG(instance != nullptr, "Attempted to get Application before it has been created.");
        return *instance;
    }

    void Application::tick() {
        if(window != nullptr){
            window->processInput();
        }

        //If the previous processInput call closed the window we don't want to run the rest of the function.
        if(currentState != State::Running) {
            return;
        }

        auto const currFrameTime{ std::chrono::steady_clock::now() };
        std::chrono::duration<float> const deltaSeonds{ currFrameTime - prevFrameTime };
        prevFrameTime = currFrameTime;

        renderer->begin();

        while(auto keyEvent{ keyboard->getKeyEvent() }) {
            InputEvent const event{ *keyEvent };
            for(auto &&[key, group] : subSystems) {
                for(auto &subSystem : group) {
                    if(subSystem->onInputEvent(event) == InputResponse::Consumed) {
                        break;
                    }
                }
            }
        }
        while(auto mouseEvent{ mouse->getEvent() }) {
            InputEvent const event{ *mouseEvent };
            for(auto &&[key, group] : subSystems) {
                for(auto &subSystem : group) {
                    if(subSystem->onInputEvent(event) == InputResponse::Consumed) {
                        break;
                    }
                }
            }
        }

        for(auto &&[key, group] : subSystems) {
            for(auto &subSystem : group) {
                subSystem->onUpdate(deltaSeonds.count());
            }
        }

        renderer->end();
    }

    void Application::shutdown() {
        currentState = State::Stopped;
    }

    Application::Application(std::unique_ptr<GhaDevice> graphicsDevice, std::unique_ptr<AhaDevice> audioDevice, std::unique_ptr<Window> window, std::unique_ptr<RenderTarget> renderTarget)
        : Application{ std::move(graphicsDevice), std::move(audioDevice), &window->getKeyboard(), &window->getMouse(), std::move(renderTarget) } {
        this->window = std::move(window);
    }

    Application::Application(std::unique_ptr<GhaDevice> graphicsDevice, std::unique_ptr<AhaDevice> audioDevice, Keyboard *keyboard, Mouse *mouse, std::unique_ptr<RenderTarget> renderTarget)
        : graphicsDevice{ std::move(graphicsDevice) }
        , audioDevice{ std::move(audioDevice) }
        , keyboard{ keyboard }
        , mouse{ mouse }
        , assetManager{ &fileSystem } {
        CLOVE_ASSERT_MSG(instance == nullptr, "Only one Application can be active");
        instance = this;

        prevFrameTime = std::chrono::steady_clock::now();

        //Systems
        renderer = std::make_unique<HighDefinitionRenderer>(this->graphicsDevice.get(), std::move(renderTarget));
    }
}
