#pragma once

#include <Clove/Application.hpp>
#include <Clove/ECS/Entity.hpp>

namespace membrane {
    class EditorSubSystem;
    class RuntimeSubSystem;
    ref class Editor_Stop;
    ref class Editor_Play;
}

namespace membrane {
    /**
     * @brief Translates a Clove session into C++/CLI
     */
public ref class Application {
        //VARIABLES
    private:
        clove::Application *app;

        bool isInEditorMode{ true };

        System::String ^projectFile{};
        System::String ^gameName{}; /**< The name of the game module's cmake target */
        System::String ^gameSourceDir{};
        System::String ^gameContentDir{};

        HINSTANCE gameLibrary{ nullptr };

        //FUNCTIONS
    public:
        Application(System::String ^projectFile);
        ~Application();
        !Application();

        void loadGameDll();
        void startSession();

        bool isRunning();
        void tick();
        void shutdown();

        System::String ^resolveVfsPath(System::String ^path);

        static System::String ^getProjectVersion();

        System::IntPtr createChildWindow(System::IntPtr parent, int32_t width, int32_t height);

    private:
        void setEditorMode(Editor_Stop ^message);
        void setRuntimeMode(Editor_Play ^message);

        bool tryLoadGameDll(std::filesystem::path const &path);
    };
}