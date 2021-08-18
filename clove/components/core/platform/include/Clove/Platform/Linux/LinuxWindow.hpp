#pragma once

#include "Clove/Platform/Linux/CloveLinux.hpp"
#include "Clove/Platform/Window.hpp"

namespace clove {
    class GhaFactory;
}

namespace clove {
    class LinuxWindow : public Window {
        //VARIABLES
    private:
        bool open{ false };

        Screen *screen{ nullptr };
        int32_t screenID;
        ::Window window{ 0 };

        Atom atomWmDeleteWindow{};

        vec2i prevConfigureNotifySize{};

        Keyboard::Dispatcher keyboardDispatcher{};
        Mouse::Dispatcher mouseDispatcher{};

        //FUNCTIONS
    public:
        LinuxWindow() = delete;
        LinuxWindow(Descriptor const &descriptor);

        LinuxWindow(LinuxWindow const &other)     = delete;
        LinuxWindow(LinuxWindow &&other) noexcept = delete;

        LinuxWindow &operator=(LinuxWindow const &other) = delete;
        LinuxWindow &operator=(LinuxWindow &&other) noexcept = delete;

        ~LinuxWindow();

        std::any getNativeWindow() const override;

        vec2i getPosition(bool clientArea) const override;
        vec2i getSize(bool clientArea) const override;

        void moveWindow(vec2i const &position) override;
        void resizeWindow(vec2i const &size) override;

        bool isOpen() const override;

        void close() override;

    protected:
        void processInput() override;

        void closeWindow(::Window window);
    };
}