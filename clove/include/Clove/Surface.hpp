#pragma once

#include <Clove/Delegate/MultiCastDelegate.hpp>
#include <Clove/Maths/Vector.hpp>

namespace clove {
    class Keyboard;
    class Mouse;
}

namespace clove {
    /**
     * @brief Provides an abstraction over a renderable area that handles input.
     */
    class Surface {
        //FUNCTIONS
    public:
        virtual ~Surface() = default;

        virtual MultiCastDelegate<void(vec2ui const &)> &onSurfaceResize() = 0;

        virtual void processInput() = 0;

        virtual vec2i getSize() const = 0;

        virtual Keyboard &getKeyboard() = 0;
        virtual Mouse &getMouse()       = 0;
    };
}