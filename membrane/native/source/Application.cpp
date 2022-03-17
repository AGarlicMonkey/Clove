#include "Membrane/Application.hpp"

#include "Membrane/EditorVFS.hpp"
#include "Membrane/EditorSubSystem.hpp"

#include <Clove/Application.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <Clove/Reflection/Reflection.hpp>

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

CLOVE_DECLARE_LOG_CATEGORY(MembraneApplication)

using namespace clove;
using namespace membrane;

namespace {
    std::string_view constexpr dllPath{ GAME_MODULE_DIR "/" GAME_NAME ".dll" };
}

static Application *cloveApp{ nullptr };
static HINSTANCE gameLibrary{ nullptr };

bool tryLoadGameDll(std::string_view path);

void initialise() {
    //No op - will do something when clove is decoupled from it's window
}

bool isRunning() {
    return cloveApp->getState() != Application::State::Stopped;
}

void tick() {
    cloveApp->tick();
}

void shutDown() {
    serialiser::Node projectNode{};
    serialiser::Node &assetManagerNode{ projectNode["assets"] };

    cloveApp->getAssetManager()->serialise(assetManagerNode);
    cloveApp->shutdown();

    std::ofstream fileStream{ GAME_DIR "/" GAME_NAME ".clvproj", std::ios::out | std::ios::trunc };
    fileStream << emittYaml(projectNode);
}

void startSession() {
    //TODO: Merge this into initialise

    auto result{ loadYaml(GAME_DIR "/" GAME_NAME ".clvproj") };
    if(result.hasValue()) {
        cloveApp->getAssetManager()->deserialise(result.getValue()["assets"]);
    }

    cloveApp->pushSubSystem<EditorSubSystem>(cloveApp->getEntityManager());
}

HWND createChildWindow(HWND parent, int width, int height) {
    std::filesystem::path const contentDir{ GAME_DIR "/content" };
    auto vfs{ std::make_unique<EditorVFS>(contentDir) };
    if(!std::filesystem::exists(contentDir)) {
        std::filesystem::create_directories(contentDir);
    }

    //TEMP: Create the application here. It would be better to initialise Clove when creating this instance but it is currently tightly couple to having a window open.
    cloveApp = clove::Application::create(GraphicsApi::Vulkan, AudioApi::OpenAl, Window::Descriptor{ "test", width, height, parent }, std::move(vfs)).release();

    return std::any_cast<HWND>(cloveApp->getWindow()->getNativeWindow());
}

void loadGameDll() {
    std::filesystem::path const gameOutputDir{ GAME_OUTPUT_DIR };
    if(gameOutputDir.empty()) {
        CLOVE_LOG(MembraneApplication, clove::LogLevel::Error, "GAME_OUTPUT_DIR is not defined. Please define this for BulbMembrane and point it to build output location.");
        return;
    }

    std::string const gameName{ GAME_NAME };
    if(gameName.empty()) {
        CLOVE_LOG(MembraneApplication, clove::LogLevel::Error, "GAME_NAME is not defined. Please define this for BulbMembrane to provide the name of the target to build.");
        return;
    }

    std::filesystem::path const gameModuleDir{ GAME_MODULE_DIR };
    if(gameModuleDir.empty()) {
        CLOVE_LOG(MembraneApplication, clove::LogLevel::Error, "GAME_MODULE_DIR is not defined. Please define this for BulbMembrane and point it to the game dll to load.");
        return;
    }

    std::optional<std::filesystem::path> fallbackDll{};
    if(gameLibrary != nullptr) {
        CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "Unloading {0} to prepare for compilation and reload", gameName);

        if(OnModuleRemovedFn onModuleRemoved{ (OnModuleRemovedFn)GetProcAddress(gameLibrary, "onModuleRemoved") }; onModuleRemoved != nullptr) {
            (onModuleRemoved)();
        } else {
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "onModuleRemoved function not defined in client application.");
        }

        FreeLibrary(gameLibrary);

        try {
            fallbackDll = gameModuleDir / (gameName + "_copy.dll");
            std::filesystem::copy_file(dllPath, fallbackDll.value(), std::filesystem::copy_options::overwrite_existing);
        } catch(std::exception e) {
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Warning, "Could not create fallback dll:");
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Warning, "\t{0}", e.what());
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Warning, "Compilation failure of new module will result cause the editor to crash.");
        }
    }

    std::filesystem::remove(dllPath);

    CLOVE_LOG(MembraneApplication, clove::LogLevel::Debug, "Configuring and compiling {0}", gameName);

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
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Debug, "Attempting to load fallback Dll...");

            if(!tryLoadGameDll(fallbackDll->string())) {
                throw std::runtime_error{ "Loading of backup game Dll failed." };
            }
        } else {
            throw std::runtime_error{ "Loading of game Dll failed. Unable to fall back onto a previously compiled Dll." };
        }
    }

    CLOVE_LOG(MembraneApplication, clove::LogLevel::Info, "Successfully loaded {0} dll", gameName);
}

bool tryLoadGameDll(std::string_view path) {
    if(gameLibrary = LoadLibrary(path.data()); gameLibrary != nullptr) {
        //Set up module's application
        if(LinkApplicationFn linkAppProc{ (LinkApplicationFn)GetProcAddress(gameLibrary, "linkApplication") }) {
            linkAppProc(&clove::Application::get());

            CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "'linkApplication' was successfully called.");
        } else {
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "'linkApplication' was skipped. Likely not used in client code.");
        }

        //Set up module's logger
        if(LinkLoggerFn linkLogProc{ (LinkLoggerFn)GetProcAddress(gameLibrary, "linkLogger") }) {
            linkLogProc(&clove::Logger::get());

            CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "'linkLogger' was successfully called.");
        } else {
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "'linkLogger' was skipped. Likely not used in client code.");
        }

        //Set up module's reflection system
        if(LinkReflectionFn linkReflecProc{ (LinkReflectionFn)GetProcAddress(gameLibrary, "linkReflection") }) {
            linkReflecProc(&clove::reflection::internal::Registry::get());

            CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "'linkReflection' was successfully called.");
        } else {
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "'linkReflection' was skipped. Likely not used in client code.");
        }

        if(OnModuleLoadedFn onModuleLoaded{ (OnModuleLoadedFn)GetProcAddress(gameLibrary, "onModuleLoaded") }; onModuleLoaded != nullptr) {
            (onModuleLoaded)();
        } else {
            CLOVE_LOG(MembraneApplication, clove::LogLevel::Trace, "onModuleLoaded function not defined in client application.");
        }
    } else {
        CLOVE_LOG(MembraneApplication, clove::LogLevel::Error, "Could not load game dll. File does not exist");
        return false;
    }

    return true;
}