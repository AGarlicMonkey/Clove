#pragma once

#include <Clove/Application.hpp>
#include <Clove/ECS/Entity.hpp>

namespace clove {
    class GraphicsImageRenderTarget;
}

namespace membrane {
    class EditorLayer;
    class RuntimeLayer;
    class ViewportSurface;
    ref class Editor_Stop;
    ref class Editor_Play;
}

namespace membrane {
    /**
     * @brief Translates a Clove session into C++/CLI
     */
public ref class Application {
    private:
        clove::Application *app;
        clove::GraphicsImageRenderTarget *renderTarget;
        ViewportSurface *surface;

        std::shared_ptr<EditorLayer> *editorLayer;
        std::shared_ptr<RuntimeLayer> *runtimeLayer;

        int width;
        int height;

    public:
        Application(int const width, int const height);
        ~Application();
        !Application();

        bool isRunning();
        void tick();
        void render(System::IntPtr backBuffer);
        void shutdown();

        void resize(int width, int height);

        System::String ^resolveVfsPath(System::String ^path);

    private:
        void setEditorMode(Editor_Stop ^message);
        void setRuntimeMode(Editor_Play ^message);
    };
}