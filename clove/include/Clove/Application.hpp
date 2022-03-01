#pragma once

#include "Clove/FileSystem/AssetManager.hpp"
#include "Clove/FileSystem/VirtualFileSystem.hpp"
#include "Clove/SubSystem.hpp"

#include <Clove/Audio/Aha.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Graphics/GraphicsAPI.hpp>
#include <Clove/Platform/Window.hpp>
#include <chrono>
#include <map>
#include <memory>
#include <vector>

namespace clove {
    class Window;
    class Mouse;
    class Keyboard;
    class GhaDevice;
    class Renderer;
    class GraphicsImageRenderTarget;
    class AhaDevice;
    class PhysicsSubSystem;
    class RenderTarget;
}

namespace clove {
    class Application {
        //TYPES
    public:
        enum class State {
            Running,
            Stopped
        };

        //VARIABLES
    private:
        State currentState{ State::Running };//Assumed to be initialised to the running state.

        std::unique_ptr<GhaDevice> graphicsDevice;
        std::unique_ptr<AhaDevice> audioDevice;

        std::unique_ptr<Window> window{ nullptr };
        Keyboard *keyboard{ nullptr };
        Mouse *mouse{ nullptr };

        std::unique_ptr<Renderer> renderer;
        EntityManager entityManager;

        std::unique_ptr<VirtualFileSystem> fileSystem{};
        AssetManager assetManager;

        std::unordered_map<std::type_index, std::pair<SubSystem::Group, size_t>> subSystemToIndex; /**< Contains the index for each subsystem in the subSystems array. */
        std::map<SubSystem::Group, std::vector<std::unique_ptr<SubSystem>>> subSystems;

        std::chrono::steady_clock::time_point prevFrameTime;

        //FUNCTIONS
    public:
        Application() = delete;

        Application(Application const &other)     = delete;
        Application(Application &&other) noexcept = delete;

        Application &operator=(Application const &other) = delete;
        Application &operator=(Application &&other) noexcept = delete;

        ~Application();

        /**
         * @brief Creates a standard Garlic application that opens and manages it's own window.
         * @param graphicsApi Which graphics api to use.
         * @param audioApi Which audio api to use.
         * @param windowDescriptor A descriptor describing the properties of the window.
         * @return The created application instance.
         */
        static std::unique_ptr<Application> create(GraphicsApi graphicsApi, AudioApi audioApi, Window::Descriptor const &windowDescriptor, std::unique_ptr<VirtualFileSystem> fileSystem);

        static Application &get();

        template<typename SubSystemType, typename... Args>
        void pushSubSystem(Args &&...args);

        template<typename SubSystemType>
        bool hasSubSystem() const;

        template<typename SubSystemType>
        SubSystemType &getSubSystem();

        template<typename SubSystemType>
        void popSubSystem();

        inline State getState() const;

        /**
         * @brief Performs a single iteration of the main application loop.
         * @details getState should be called before calling this to check
         * if the application should still be running.
         */
        void tick();

        /**
         * @brief Transition to State::Stoped if the application is in State::Running.
         */
        void shutdown();

        /**
         * @brief Returns the current application window. Can be nullptr if the application is headless.
         * @return 
         */
        inline Window *getWindow() const;
        inline Keyboard *getKeyboard() const;
        inline Mouse *getMouse() const;

        //Devices
        inline GhaDevice *getGraphicsDevice() const;
        inline AhaDevice *getAudioDevice() const;

        //Systems
        inline Renderer *getRenderer() const;
        inline EntityManager *getEntityManager();

        inline AssetManager *getAssetManager();
        inline VirtualFileSystem *getFileSystem();

    private:
        Application(std::unique_ptr<GhaDevice> graphicsDevice, std::unique_ptr<AhaDevice> audioDevice, std::unique_ptr<Window> window, std::unique_ptr<RenderTarget> renderTarget, std::unique_ptr<VirtualFileSystem> fileSystem);
        Application(std::unique_ptr<GhaDevice> graphicsDevice, std::unique_ptr<AhaDevice> audioDevice, Keyboard *keyboard, Mouse *mouse, std::unique_ptr<RenderTarget> renderTarget, std::unique_ptr<VirtualFileSystem> fileSystem);
    };
}

#include "Application.inl"
