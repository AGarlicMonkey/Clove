#pragma once

#include <Clove/Application.hpp>
#include <Clove/ECS/Entity.hpp>

namespace membrane {
    class EditorSubSystem;
    class RuntimeSubSystem;
    ref class EditorViewport;
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
        clove::GraphicsImageRenderTarget *renderTarget;
        EditorViewport ^viewport;

        int width;
        int height;

        bool isInEditorMode{ true };

        HINSTANCE gameLibrary{ nullptr };

        //FUNCTIONS
    public:
        Application(int const width, int const height);
        ~Application();
        !Application();

        void loadGameDll();
        void startSession();

        bool isRunning();
        void tick();
        void shutdown();

        System::String ^resolveVfsPath(System::String ^path);

        static System::String ^getProjectVersion();

        //HACK TEMP STUFF - just to test this hosting buisness
        System::IntPtr createWindow(System::IntPtr host, int32_t width, int32_t height);
        void destroyWindow();

    private:
        void setEditorMode(Editor_Stop ^message);
        void setRuntimeMode(Editor_Play ^message);

        bool tryLoadGameDll(std::string_view path);
    };
}