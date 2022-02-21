#include "Membrane/Application.hpp"

#include "Membrane/EditorSubSystem.hpp"
#include "Membrane/EditorViewport.hpp"
#include "Membrane/MembraneLog.hpp"
#include "Membrane/MessageHandler.hpp"
#include "Membrane/Messages.hpp"
#include "Membrane/RuntimeSubSystem.hpp"
#include "Membrane/EditorVFS.hpp"

#include <Clove/Application.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Graphics/GhaBuffer.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Graphics/GraphicsAPI.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Reflection/Reflection.hpp>
#include <Clove/Rendering/GraphicsImageRenderTarget.hpp>
#include <msclr/marshal_cppstd.h>
#include <sstream>

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

typedef void (*OnModuleLoadedFn)();
typedef void (*OnModuleRemovedFn)();

typedef void (*LinkApplicationFn)(clove::Application *app);
typedef void (*LinkLoggerFn)(clove::Logger *logger);
typedef void (*LinkReflectionFn)(clove::reflection::internal::Registry *reg);

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

        std::filesystem::path const contentDir{ GAME_DIR "/content" };
        auto vfs{ std::make_unique<EditorVFS>(contentDir) };
        if(!std::filesystem::exists(contentDir)) {
            std::filesystem::create_directories(contentDir);
        }

        //Use pair as there seems to be an issue when using structured bindings
        auto pair{ clove::Application::createHeadless(GraphicsApi::Vulkan, AudioApi::OpenAl, std::move(renderTargetImageDescriptor), viewport->getKeyboard(), viewport->getMouse(), std::move(vfs)) };
        app          = pair.first.release();
        renderTarget = pair.second;

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
        if(gameModuleDir.empty()) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "GAME_MODULE_DIR is not defined. Please define this for BulbMembrane and point it to the game dll to load.");
            return;
        }

        std::optional<std::filesystem::path> fallbackDll{};
        if(gameLibrary != nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Trace, "Unloading {0} to prepare for compilation and reload", gameName);

            if(OnModuleRemovedFn onModuleRemoved{ (OnModuleRemovedFn)GetProcAddress(gameLibrary, "onModuleRemoved") }; onModuleRemoved != nullptr) {
                (onModuleRemoved)();
            } else {
                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "onModuleRemoved function not defined in client application.");
            }

            FreeLibrary(gameLibrary);

            try {
                fallbackDll = gameModuleDir / (gameName + "_copy.dll");
                std::filesystem::copy_file(dllPath, fallbackDll.value(), std::filesystem::copy_options::overwrite_existing);
            } catch(std::exception e) {
                CLOVE_LOG(Membrane, clove::LogLevel::Warning, "Could not create fallback dll:");
                CLOVE_LOG(Membrane, clove::LogLevel::Warning, "\t{0}", e.what());
                CLOVE_LOG(Membrane, clove::LogLevel::Warning, "Compilation failure of new module will result cause the editor to crash.");
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
                throw gcnew System::Exception{ "Loading of game Dll failed. Unable to fall back onto a previously compiled Dll." };
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

    System::String ^ Application::getProjectVersion() {
        return gcnew System::String{ CLOVE_VERSION };
    }

    void Application::setEditorMode(Editor_Stop ^ message) {
        app->popSubSystem<RuntimeSubSystem>();
        app->pushSubSystem<EditorSubSystem>(app->getEntityManager());
    }

    void Application::setRuntimeMode(Editor_Play ^ message) {
        app->popSubSystem<EditorSubSystem>();
        app->pushSubSystem<RuntimeSubSystem>(app->getEntityManager());
    }

    bool Application::tryLoadGameDll(std::string_view path) {
        if(gameLibrary = LoadLibrary(path.data()); gameLibrary != nullptr) {
            //Set up module's application
            if(LinkApplicationFn linkAppProc{ (LinkApplicationFn)GetProcAddress(gameLibrary, "linkApplication") }) {
                linkAppProc(&clove::Application::get());

                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "'linkApplication' was successfully called.");
            } else {
                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "'linkApplication' was skipped. Likely not used in client code.");
            }

            //Set up module's logger
            if(LinkLoggerFn linkLogProc{ (LinkLoggerFn)GetProcAddress(gameLibrary, "linkLogger") }) {
                linkLogProc(&clove::Logger::get());

                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "'linkLogger' was successfully called.");
            } else {
                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "'linkLogger' was skipped. Likely not used in client code.");
            }

            //Set up module's reflection system
            if(LinkReflectionFn linkReflecProc{ (LinkReflectionFn)GetProcAddress(gameLibrary, "linkReflection") }) {
                linkReflecProc(&clove::reflection::internal::Registry::get());

                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "'linkReflection' was successfully called.");
            } else {
                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "'linkReflection' was skipped. Likely not used in client code.");
            }

            if(OnModuleLoadedFn onModuleLoaded{ (OnModuleLoadedFn)GetProcAddress(gameLibrary, "onModuleLoaded") }; onModuleLoaded != nullptr) {
                (onModuleLoaded)();
            } else {
                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "onModuleLoaded function not defined in client application.");
            }
        } else {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not load game dll. File does not exist");
            return false;
        }

        return true;
    }
}