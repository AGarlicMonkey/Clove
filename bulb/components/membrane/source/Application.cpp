#include "Membrane/Application.hpp"

#include "Membrane/EditorSubSystem.hpp"
#include "Membrane/EditorViewport.hpp"
#include "Membrane/MessageHandler.hpp"
#include "Membrane/Messages.hpp"

#include <Clove/Application.hpp>
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

#include <Clove/Reflection/Reflection.hpp>

CLOVE_DECLARE_LOG_CATEGORY(Membrane)

#ifndef GAME_OUTPUT_DIR
    #define GAME_OUTPUT_DIR ""
#endif
#ifndef GAME_NAME
    #define GAME_NAME ""
#endif
#ifndef GAME_MODULE_DIR
    #define GAME_MODULE_DIR ""
#endif
#ifndef GAME_DIR
    #define GAME_DIR ""
#endif

typedef void (*setUpEditorApplicationFn)(clove::Application *app);
typedef void (*tearDownEditorApplicationFn)(clove::Application *app);

typedef void (*setUpReflectorFn)(clove::reflection::internal::Registry *reg);

namespace {
    std::string_view constexpr dllPath{ GAME_MODULE_DIR "/" GAME_NAME ".dll" };
}

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

        auto *vfs{ app->getFileSystem() };
        vfs->mount(GAME_DIR "/content", ".");
        std::filesystem::create_directories(vfs->resolve("."));

        MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_Stop ^>(this, &Application::setEditorMode));
        MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_Play ^>(this, &Application::setRuntimeMode));
    }

    Application::~Application() {
        this->!Application();
    }

    Application::!Application() {
        delete app;
    }

    void Application::loadGameDll() {
        std::filesystem::path const gameOutputDir{ GAME_OUTPUT_DIR };
        if(gameOutputDir.empty()) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "GAME_OUTPUT_DIR is not defined. Please define this for BulbMembrane and point it to build output location."); 
            return;
        }

        std::string const gameName{ GAME_NAME };
        if(gameName.empty()) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "GAME_NAME is not defined. Please define this for BulbMembrane to provide the name of the target to build.");
            return;
        }

        std::filesystem::path const gameModuleDir{ GAME_MODULE_DIR };
        if(gameModuleDir.empty()){
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "GAME_MODULE_DIR is not defined. Please define this for BulbMembrane and point it to the game dll to load.");
            return;
        }

        std::optional<std::filesystem::path> fallbackDll{};
        if(gameLibrary != nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Trace, "Unloading {0} to prepare for compilation and reload", gameName);

            if(tearDownEditorApplicationFn tearDownEditorApplication{ (tearDownEditorApplicationFn)GetProcAddress(gameLibrary, "tearDownEditorApplication") }; tearDownEditorApplication != nullptr) {
                (tearDownEditorApplication)(app);
            } else {
                CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not load game tear down function. Please provide 'tearDownEditorApplication' in client code.");
                return;
            }

            FreeLibrary(gameLibrary);

            try {
                fallbackDll = gameModuleDir / (gameName + "_copy.dll");
                std::filesystem::copy_file(dllPath, fallbackDll.value(), std::filesystem::copy_options::overwrite_existing);
            } catch(std::exception e) {
                CLOVE_LOG(Membrane, clove::LogLevel::Error, "{0}", e.what());
                return;
            }
        }

        std::filesystem::remove(dllPath);

        CLOVE_LOG(Membrane, clove::LogLevel::Debug, "Configuring and compiling {0}", gameName);

        {
            std::stringstream configureStream{};
            configureStream << "cmake -G \"Visual Studio 16 2019\" " << ROOT_DIR;
            std::system(configureStream.str().c_str());
        }

        {
            std::stringstream buildStream{};
            buildStream << "cmake --build " << gameOutputDir.string() << " --target " << gameName << " --config Debug";
            std::system(buildStream.str().c_str());
        }

        bool const loadingSuccessful{ tryLoadGameDll(dllPath) };

        if(!loadingSuccessful) {
            if(fallbackDll.has_value()) {
                CLOVE_LOG(Membrane, clove::LogLevel::Debug, "Attempting to load fallback Dll...");

                if(!tryLoadGameDll(fallbackDll->string())) {
                    throw gcnew System::Exception{ "Loading of backup game Dll failed." };
                }
            } else {
                throw gcnew System::Exception{ "Loading of game Dll failed." };
            }
        }

        CLOVE_LOG(Membrane, clove::LogLevel::Info, "Successfully loaded {0} dll", gameName);
    }

    void Application::startSession() {
        app->pushSubSystem<EditorSubSystem>(app->getEntityManager());
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

    System::String ^ Application::getProjectVersion() {
        return gcnew System::String{ CLOVE_VERSION };
    }

    void Application::setEditorMode(Editor_Stop ^ message) {
        app->pushSubSystem<EditorSubSystem>(app->getEntityManager());
    }

    void Application::setRuntimeMode(Editor_Play ^ message) {
        app->popSubSystem<EditorSubSystem>();
    }

    bool Application::tryLoadGameDll(std::string_view path) {
        if(gameLibrary = LoadLibrary(path.data()); gameLibrary != nullptr) {
            if(setUpEditorApplicationFn setUpEditorApplication{ (setUpEditorApplicationFn)GetProcAddress(gameLibrary, "setUpEditorApplication") }; setUpEditorApplication != nullptr) {
                //Set up reflection system in module
                {
                    setUpReflectorFn proc{ (setUpReflectorFn)GetProcAddress(gameLibrary, "setUpReflector") };
                    CLOVE_ASSERT(proc);
                    proc(&clove::reflection::internal::Registry::get());
                }

                (setUpEditorApplication)(app);
            } else {
                CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not load game initialise function. Please provide 'setUpEditorApplication' in client code.");
                return false;
            }
        } else {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not load game dll. File does not exist");
            return false;
        }

        return true;
    }
}