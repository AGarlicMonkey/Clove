#include "Membrane/Application.hpp"

#include "Membrane/EditorLayer.hpp"
#include "Membrane/RuntimeLayer.hpp"
#include "Membrane/ViewportSurface.hpp"
#include "Membrane/Messages.hpp"
#include "Membrane/MessageHandler.hpp"

#include <Clove/Application.hpp>
#include <Clove/Audio/Audio.hpp>
#include <Clove/Components/StaticModelComponent.hpp>
#include <Clove/Components/TransformComponent.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Graphics/GraphicsAPI.hpp>
#include <Clove/Graphics/GhaBuffer.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Rendering/GraphicsImageRenderTarget.hpp>
#include <msclr/marshal_cppstd.h>

namespace membrane {
    Application::Application(int const width, int const height)
        : width{ width }
        , height{ height } {
        using namespace clove;

        GhaImage::Descriptor renderTargetImageDescriptor{};
        renderTargetImageDescriptor.type        = GhaImage::Type::_2D;
        renderTargetImageDescriptor.usageFlags  = GhaImage::UsageMode::ColourAttachment | GhaImage::UsageMode::TransferSource;
        renderTargetImageDescriptor.dimensions  = { width, height };
        renderTargetImageDescriptor.format      = GhaImage::Format::B8G8R8A8_SRGB;//Hard coding format to B8G8R8A8_SRGB as that is what the WriteableBitmap is set to
        renderTargetImageDescriptor.sharingMode = SharingMode::Concurrent;

        //Use pair as there seems to be an issue when using structured bindings
        auto vpSurface{ std::make_unique<ViewportSurface>() };
        surface = vpSurface.get();
        auto pair{ clove::Application::createHeadless(GraphicsApi::Vulkan, AudioApi::OpenAl, std::move(renderTargetImageDescriptor), std::move(vpSurface)) };
        app          = pair.first.release();
        renderTarget = pair.second;

        //Mount editor paths
        auto *vfs{ app->getFileSystem() };
        vfs->mount(std::filesystem::current_path() / "data", ".");//TODO: Root path of editor is unlikely to be it's working directory

        std::filesystem::create_directories(vfs->resolve("."));

        //Push layers
        editorLayer  = new std::shared_ptr<EditorLayer>();
        *editorLayer = std::make_shared<EditorLayer>();

        runtimeLayer  = new std::shared_ptr<RuntimeLayer>();
        *runtimeLayer = std::make_shared<RuntimeLayer>();

        app->pushLayer(*editorLayer);

        MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_Stop ^>(this, &Application::setEditorMode));
        MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_Play ^>(this, &Application::setRuntimeMode));
    }

    Application::~Application() {
        this->!Application();
    }

    Application::!Application() {
        delete app;
        delete editorLayer;
        delete runtimeLayer;
    }

    bool Application::isRunning() {
        return app->getState() != clove::Application::State::Stopped;
    }

    void Application::tick() {
        app->tick();
    }

    void Application::render(System::IntPtr backBuffer) {
        auto const renderTargetBuffer{ renderTarget->getNextReadyBuffer() };
        size_t constexpr bbp{ 4 };
        renderTargetBuffer->read(backBuffer.ToPointer(), 0, width * height * bbp);
    }

    void Application::shutdown() {
        app->shutdown();
    }

    void Application::resize(int width, int height) {
        clove::vec2i const size{ width, height };
        renderTarget->resize(size);
        surface->setSize(std::move(size));

        this->width  = width;
        this->height = height;
    }

    System::String ^Application::resolveVfsPath(System::String ^ path) {
        System::String ^managedPath{ path };
        std::string unManagedPath{ msclr::interop::marshal_as<std::string>(managedPath) };
        return gcnew System::String(app->getFileSystem()->resolve(unManagedPath).c_str());
    }

    void Application::setEditorMode(Editor_Stop ^message) {
        app->popLayer(*runtimeLayer);
        app->pushLayer(*editorLayer);
    }

    void Application::setRuntimeMode(Editor_Play ^message) {
        app->popLayer(*editorLayer);
        app->pushLayer(*runtimeLayer);
    }
}