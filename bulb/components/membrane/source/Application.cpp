#include "Membrane/Application.hpp"

#include "Membrane/EditorSubSystem.hpp"
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
#include <msclr/marshal_cppstd.h>
#include <sstream>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>

typedef void (*OnModuleLoadedFn)();
typedef void (*OnModuleRemovedFn)();

typedef void (*LinkApplicationFn)(clove::Application *app);
typedef void (*LinkLoggerFn)(clove::Logger *logger);
typedef void (*LinkReflectionFn)(clove::reflection::internal::Registry *reg);

using namespace clove;

namespace membrane {
    Application::Application(System::String ^projectFile) {
        this->projectFile = projectFile;

        std::filesystem::path const projectFilePath{ msclr::interop::marshal_as<std::string>(projectFile) };

        if(!std::filesystem::exists(projectFilePath)) {
            throw gcnew System::IO::FileNotFoundException("Could not find project file", projectFile);
        }

        auto result{ loadYaml(projectFilePath).getValue() };
        gameName = gcnew System::String{ result["project"]["name"].as<std::string>().c_str() };
        gameSourceDir = gcnew System::String{ std::filesystem::path{ projectFilePath.parent_path() / "source" }.c_str() };
        gameContentDir = gcnew System::String{ std::filesystem::path{ projectFilePath.parent_path() / "content" }.c_str() };
    }

    Application::~Application() {
        this->!Application();
    }

    Application::!Application() {
        delete app;
    }

    void Application::loadGameDll() {
        //marshal_as does not like member variables
        System::String ^gameCopy{ gameName };
        System::String ^sourceCopy{ gameSourceDir };

        std::string const nativeName{ msclr::interop::marshal_as<std::string>(gameCopy) };
        std::filesystem::path const nativeSourcePath{ msclr::interop::marshal_as<std::string>(sourceCopy) };
        std::filesystem::path const dllPath{ std::filesystem::current_path() / (nativeName + ".dll") };

        std::optional<std::filesystem::path> fallbackDll{};
        if(gameLibrary != nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Trace, "Unloading {0} to prepare for compilation and reload", nativeName);

            if(OnModuleRemovedFn onModuleRemoved{ (OnModuleRemovedFn)GetProcAddress(gameLibrary, "onModuleRemoved") }; onModuleRemoved != nullptr) {
                (onModuleRemoved)();
            } else {
                CLOVE_LOG(Membrane, clove::LogLevel::Trace, "onModuleRemoved function not defined in client application.");
            }

            FreeLibrary(gameLibrary);

            try {
                fallbackDll = std::filesystem::current_path() / (nativeName + "_copy.dll");
                std::filesystem::copy_file(dllPath, fallbackDll.value(), std::filesystem::copy_options::overwrite_existing);
            } catch(std::exception e) {
                CLOVE_LOG(Membrane, clove::LogLevel::Warning, "Could not create fallback dll:");
                CLOVE_LOG(Membrane, clove::LogLevel::Warning, "\t{0}", e.what());
                CLOVE_LOG(Membrane, clove::LogLevel::Warning, "Compilation failure of new module will result cause the editor to crash.");
            }
        }

        std::filesystem::remove(dllPath);

        CLOVE_LOG(Membrane, clove::LogLevel::Debug, "Compiling {0}", nativeName);

        {
            std::stringstream buildStream{};
            buildStream << "cmake --build " << ROOT_DIR"/build" << " --target " << nativeName << " --config Debug";
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

        CLOVE_LOG(Membrane, clove::LogLevel::Info, "Successfully loaded {0} dll", nativeName);
    }

    void Application::startSession() {
        System::String ^copy{ projectFile };
        std::filesystem::path const nativeProjectFile{ msclr::interop::marshal_as<std::string>(copy) };

        auto result{ loadYaml(nativeProjectFile) };
        if(result.hasValue()) {
            app->getAssetManager()->deserialise(result.getValue()["assets"]);
        }

        app->pushSubSystem<EditorSubSystem>(app->getEntityManager());
    }

    bool Application::isRunning() {
        return app->getState() != clove::Application::State::Stopped;
    }

    void Application::tick() {
        app->tick();
    }

    void Application::shutdown() {
        System::String ^copy{ projectFile };
        std::filesystem::path const nativeProjectFile{ msclr::interop::marshal_as<std::string>(copy) };

        serialiser::Node projectNode{ loadYaml(nativeProjectFile).getValue() };
        serialiser::Node &assetManagerNode{ projectNode["assets"] };

        app->getAssetManager()->serialise(assetManagerNode);
        app->shutdown();

        std::ofstream fileStream{ nativeProjectFile, std::ios::out | std::ios::trunc };
        fileStream << emittYaml(projectNode);
    }

    System::String ^Application::getContentPath() {
        return gameContentDir;
    }

    System::String ^Application::getProjectVersion() {
        return gcnew System::String{ CLOVE_VERSION };
    }

    System::IntPtr Application::createChildWindow(System::IntPtr parent, int32_t width, int32_t height) {
        System::String ^copy{ gameContentDir };
        std::filesystem::path const nativeContentDir{ msclr::interop::marshal_as<std::string>(copy) };

        auto vfs{ std::make_unique<EditorVFS>(nativeContentDir) };
        if(!std::filesystem::exists(nativeContentDir)) {
            std::filesystem::create_directories(nativeContentDir);
        }

        //TEMP: Create the application here. It would be better to initialise Clove when creating this instance but it is currently tightly couple to having a window open.
        app = clove::Application::create(GraphicsApi::Vulkan, AudioApi::OpenAl, Window::Descriptor{ "test", width, height, reinterpret_cast<HWND>(parent.ToPointer()) }, std::move(vfs)).release();

        MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_Stop ^>(this, &Application::setEditorMode));
        MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_Play ^>(this, &Application::setRuntimeMode));

        return System::IntPtr{ std::any_cast<HWND>(app->getWindow()->getNativeWindow()) };
    }

    void Application::setEditorMode(Editor_Stop ^ message) {
        app->popSubSystem<RuntimeSubSystem>();
        app->pushSubSystem<EditorSubSystem>(app->getEntityManager());
    }

    void Application::setRuntimeMode(Editor_Play ^ message) {
        app->popSubSystem<EditorSubSystem>();
        app->pushSubSystem<RuntimeSubSystem>(app->getEntityManager());
    }

    bool Application::tryLoadGameDll(std::filesystem::path const &path) {
        if(gameLibrary = LoadLibrary(path.string().c_str()); gameLibrary != nullptr) {
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
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not load game dll. File does not exist: {0}", path.string());
            return false;
        }

        return true;
    }
}