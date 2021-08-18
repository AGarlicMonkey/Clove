#pragma once

#include "Clove/Surface.hpp"

#include <memory>

namespace clove {
    class Window;
}

namespace clove {
    /**
     * @brief A Surface backed by a platform Window.
     */
    class WindowSurface : public Surface {
        //VARIABLES
    private:
        std::unique_ptr<Window> window{};

        //FUNCTIONS
    public:
        WindowSurface() = delete;
        WindowSurface(std::unique_ptr<Window> window);

        WindowSurface(WindowSurface const &other) = delete;
        WindowSurface(WindowSurface &&other) noexcept;

        WindowSurface &operator=(WindowSurface const &other) = delete;
        WindowSurface &operator=(WindowSurface &&other) noexcept;

        ~WindowSurface();

        MultiCastDelegate<void(vec2ui const &)> &onSurfaceResize() override;

        void processInput() override;

        vec2i getSize() const override;

        Keyboard &getKeyboard() override;
        Mouse &getMouse() override;

        /**
         * @brief Returns the window this surface is backed by.
         */
        Window *getWindow() const;
    };
}