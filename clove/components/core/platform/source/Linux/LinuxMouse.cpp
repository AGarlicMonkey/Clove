#include "Clove/Platform/Input/Mouse.hpp"
#include "Clove/Platform/Linux/CloveLinux.hpp"

#include <X11/cursorfont.h>

namespace clove {
    vec2i Mouse::getPosition() const {//NOLINT It's designed for this function to not be static
        ::Window rootReturn{};
        ::Window childReturn{};
        int rootX{ 0 };//Relative to the root window
        int rootY{ 0 };//Relative to the root window
        int mouseX{ 0 };
        int mouseY{ 0 };
        unsigned int mask{};

        XQueryPointer(display, XDefaultRootWindow(display), &rootReturn, &childReturn, &rootX, &rootY, &mouseX, &mouseY, &mask);

        return { mouseX, mouseY };
    }

    void Mouse::setPosition(vec2i const &position) {//NOLINT It's designed for this function to not be static
        ::Window const sourceWindow{ 0 };
        ::Window const destWindow{ XDefaultRootWindow(display) };

        int const sourceX{ 0 };
        int const sourceY{ 0 };
        unsigned int const sourceWidth{ 0 };
        unsigned int const sourceHeight{ 0 };

        XWarpPointer(display, sourceWindow, destWindow, sourceX, sourceY, sourceWidth, sourceHeight, position.x, position.y);
    }

    void Mouse::show(bool shouldShow) {//NOLINT It's designed for this function to not be static
        static char data{ 0 };
        static bool cursorSet{ false };

        if(shouldShow) {
            if(cursorSet) {
                XUndefineCursor(display, XDefaultRootWindow(display));

                Cursor cursor{ XCreateFontCursor(display, XC_arrow) };
                XDefineCursor(display, XDefaultRootWindow(display), cursor);
                XFreeCursor(display, cursor);

                cursorSet = false;
            }
        } else {
            if(!cursorSet) {
                Pixmap blank{ XCreateBitmapFromData(display, XDefaultRootWindow(display), &data, 1, 1) };
                XColor dummy{};
                Cursor cursor{ XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0) };

                XDefineCursor(display, XDefaultRootWindow(display), cursor);
                XFreeCursor(display, cursor);
                XFreePixmap(display, blank);

                cursorSet = true;
            }
        }
    }
}