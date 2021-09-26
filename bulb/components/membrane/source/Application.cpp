#include "Membrane/Application.hpp"

#include "Membrane/EditorSubSystem.hpp"
#include "Membrane/EditorViewport.hpp"
#include "Membrane/MessageHandler.hpp"
#include "Membrane/Messages.hpp"
#include "Membrane/RuntimeSubSystem.hpp"

#include <Clove/Application.hpp>
#include <Clove/Components/StaticModelComponent.hpp>
#include <Clove/Components/TransformComponent.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Graphics/GhaBuffer.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Graphics/GraphicsAPI.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Rendering/GraphicsImageRenderTarget.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <filesystem>
#include <msclr/marshal_cppstd.h>

CLOVE_DECLARE_LOG_CATEGORY(Membrane)

typedef void (*setUpEditorApplicationFn)(clove::Application *app);

namespace membrane {
    static std::filesystem::path const cachedProjectsPath{ "projects.yaml" };

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

        viewport = gcnew EditorViewport{};

        //Use pair as there seems to be an issue when using structured bindings
        auto pair{ clove::Application::createHeadless(GraphicsApi::Vulkan, AudioApi::OpenAl, std::move(renderTargetImageDescriptor), viewport->getKeyboard(), viewport->getMouse()) };
        app          = pair.first.release();
        renderTarget = pair.second;
    }

    Application::~Application() {
        this->!Application();
    }

    Application::!Application() {
        delete app;
    }

    bool Application::hasDefaultProject() {
#if 0
        if(std::filesystem::exists(cachedProjectsPath) && !std::filesystem::is_empty(cachedProjectsPath)) {
            auto loadResult{ clove::loadYaml(cachedProjectsPath) };
            clove::serialiser::Node rootNode{ loadResult.getValue() };

            return std::filesystem::exists(rootNode["projects"]["default"].as<std::string>());
        }
#endif
        return false;
    }

    void Application::openProject(System::String ^ projectPath) {
        std::string projectPathString{ msclr::interop::marshal_as<std::string>(projectPath) };

        openProjectInternal(projectPathString);
    }

    void Application::openDefaultProject() {
        CLOVE_ASSERT(std::filesystem::exists(cachedProjectsPath));

        auto loadResult{ clove::loadYaml(cachedProjectsPath) };
        clove::serialiser::Node rootNode{ loadResult.getValue() };

        openProjectInternal(rootNode["projects"]["default"].as<std::string>());
    }

    bool Application::isRunning() {
        return app->getState() != clove::Application::State::Stopped;
    }

    void Application::tick() {
        viewport->processInput();
        app->tick();
    }

    void Application::render(System::IntPtr backBuffer) {
        auto const renderTargetBuffer{ renderTarget->getNextReadyBuffer() };
        size_t constexpr bbp{ 4 };
        renderTargetBuffer->read(backBuffer.ToPointer(), 0, width * height * bbp);
    }

    void Application::shutdown() {
        if(isInEditorMode) {
            app->getSubSystem<EditorSubSystem>().saveScene();
        }
        app->shutdown();
    }

    void Application::resize(int width, int height) {
        renderTarget->resize({ width, height });

        this->width  = width;
        this->height = height;
    }

    System::String ^ Application::resolveVfsPath(System::String ^ path) {
        System::String ^ managedPath { path };
        std::string unManagedPath{ msclr::interop::marshal_as<std::string>(managedPath) };
        return gcnew System::String(app->getFileSystem()->resolve(unManagedPath).c_str());
    }

    void Application::openProjectInternal(std::filesystem::path const projectPath) {
        clove::serialiser::Node editorData{};
        editorData["projects"]["version"] = 1;
        editorData["projects"]["default"] = projectPath.string();

        std::ofstream fileStream{ cachedProjectsPath, std::ios::out | std::ios::trunc };
        fileStream << clove::emittYaml(editorData);

        std::filesystem::path const rootProjectPath{ projectPath.parent_path() };
        clove::serialiser::Node const projectData{ clove::loadYaml(projectPath).getValue() };

        std::string const gameName{ projectData["name"].as<std::string>() };
        std::string const gameDll{ projectData["library"].as<std::string>() };

        //Make sure the project has been configured and the game dll has been compiled at this point
        CLOVE_LOG(Membrane, clove::LogLevel::Debug, "Performing first time set up of {0}", gameName);

#if 0//TODO
        {
            std::stringstream configureStream{};
            configureStream << "cmake -G \"Visual Studio 16 2019\" " << rootProjectPath;
            std::system(configureStream.str().c_str());
        }
#endif

        //TODO: Remove BINARY_DIR - see ProjectCreator.cs
#ifndef BINARY_DIR
    #define BINARY_DIR "."
#endif

        {
            std::stringstream buildStream{};
            buildStream << "cmake --build " << BINARY_DIR << " --target " << gameName << " --config Debug";
            std::system(buildStream.str().c_str());
        }

        if(gameLibrary = LoadLibrary(gameDll.c_str()); gameLibrary != nullptr) {
            if(setUpEditorApplicationFn setUpEditorApplication{ (setUpEditorApplicationFn)GetProcAddress(gameLibrary, "setUpEditorApplication") }; setUpEditorApplication != nullptr) {
                (setUpEditorApplication)(app);
            } else {
                throw gcnew System::Exception("Could not load game initialise function");
            }
        } else {
            throw gcnew System::Exception("Could not load game dll");
        }

        CLOVE_LOG(Membrane, clove::LogLevel::Info, "Successfully loaded {0} dll", gameName);

        auto *vfs{ app->getFileSystem() };
        vfs->mount(rootProjectPath / "content", ".");
        std::filesystem::create_directories(vfs->resolve("."));

        app->pushSubSystem<EditorSubSystem>();

        MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_Stop ^>(this, &Application::setEditorMode));
        MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_Play ^>(this, &Application::setRuntimeMode));
    }

    void Application::setEditorMode(Editor_Stop ^ message) {
        app->popSubSystem<RuntimeSubSystem>();
        app->pushSubSystem<EditorSubSystem>();
        isInEditorMode = true;
    }

    void Application::setRuntimeMode(Editor_Play ^ message) {
        app->getSubSystem<EditorSubSystem>().saveScene();

        app->popSubSystem<EditorSubSystem>();
        app->pushSubSystem<RuntimeSubSystem>();
        isInEditorMode = false;
    }
}