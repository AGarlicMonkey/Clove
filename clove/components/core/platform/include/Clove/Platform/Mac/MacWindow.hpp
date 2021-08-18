#pragma once

#include "Clove/Platform/Mac/CloveMac.hpp"
#include "Clove/Platform/Window.hpp"

namespace clove {
    class Window;
    class MacWindow;
}

@interface MacWindowProxy : NSWindow <NSWindowDelegate>

@property clove::MacWindow *cloveWindow;

@end

namespace clove {
    class MacWindow : public Window {
        //VARIABLES
    private:
        bool open = false;

        MacWindowProxy *windowProxy;

        Keyboard::Dispatcher keyboardDispatcher{};
        Mouse::Dispatcher mouseDispatcher{};

        //FUNCTIONS
    public:
        MacWindow() = delete;
        MacWindow(Descriptor const &descriptor);

        MacWindow(MacWindow const &other)     = delete;
        MacWindow(MacWindow &&other) noexcept = delete;

        MacWindow &operator=(MacWindow const &other) = delete;
        MacWindow &operator=(MacWindow &&other) noexcept = delete;

        ~MacWindow();

        void processInput() override;

        std::any getNativeWindow() const override;

        vec2i getPosition(bool clientArea) const override;
        vec2i getSize(bool clientArea) const override;

        void moveWindow(vec2i const &position) override;
        void resizeWindow(vec2i const &size) override;

        bool isOpen() const override;

        void close() override;

        void handleNsEvent(NSEvent *event);
    };
}
