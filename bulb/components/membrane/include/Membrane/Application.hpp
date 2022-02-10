#pragma once

#include <Clove/Application.hpp>
#include <Clove/ECS/Entity.hpp>

namespace clove {
    class GraphicsImageRenderTarget;
}

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
        void render(System::IntPtr backBuffer);
        void shutdown();

        void resize(int width, int height);

        System::String ^resolveVfsPath(System::String ^path);

        static System::String ^getProjectVersion();

    private:
        void setEditorMode(Editor_Stop ^message);
        void setRuntimeMode(Editor_Play ^message);

        bool tryLoadGameDll(std::string_view path);
    };
}