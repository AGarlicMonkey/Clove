#include "Clove/WindowSurface.hpp"

#include <Clove/Platform/Window.hpp>

namespace clove {
    WindowSurface::WindowSurface(std::unique_ptr<Window> window)
        : window{ std::move(window) } {
    }

    WindowSurface::WindowSurface(WindowSurface &&other) noexcept = default;

    WindowSurface &WindowSurface::operator=(WindowSurface &&other) noexcept = default;

    WindowSurface::~WindowSurface() = default;

    MultiCastDelegate<void(vec2ui const &)> &WindowSurface::onSurfaceResize() {
        return window->onWindowResize;
    }

    void WindowSurface::processInput() {
        window->processInput();
    }

    vec2i WindowSurface::getSize() const {
        return window->getSize(true);
    }

    Keyboard &WindowSurface::getKeyboard() {
        return window->getKeyboard();
    }

    Mouse &WindowSurface::getMouse() {
        return window->getMouse();
    }

    Window *WindowSurface::getWindow() const {
        return window.get();
    }
}