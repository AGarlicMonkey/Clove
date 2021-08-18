#pragma once

#include "Clove/Platform/Window.hpp"
#include "Clove/Platform/Windows/CloveWindows.hpp"

namespace clove {
    class WindowsWindow : public Window {
        //VARIABLES
    private:
        bool open = false;

        static LPCSTR constexpr className = "Clove";

        HINSTANCE instance;
        HWND windowsHandle;

        Keyboard::Dispatcher keyboardDispatcher{};
        Mouse::Dispatcher mouseDispatcher{};

        //FUNCTIONS
    public:
        WindowsWindow() = delete;
        WindowsWindow(Descriptor const &descriptor);

        WindowsWindow(WindowsWindow const &other)     = delete;
        WindowsWindow(WindowsWindow &&other) noexcept = delete;

        WindowsWindow &operator=(WindowsWindow const &other) = delete;
        WindowsWindow &operator=(WindowsWindow &&other) noexcept = delete;

        ~WindowsWindow();

        std::any getNativeWindow() const override;

        vec2i getPosition(bool clientArea) const override;
        vec2i getSize(bool clientArea) const override;

        void moveWindow(vec2i const &position) override;
        void resizeWindow(vec2i const &size) override;

        bool isOpen() const override;

        void close() override;

    protected:
        void processInput() override;

    private:
        static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
}