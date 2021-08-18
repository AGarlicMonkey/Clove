#include "Clove/Platform/Linux/LinuxWindow.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

namespace clove {
    LinuxWindow::LinuxWindow(Descriptor const &descriptor)
        : Window(keyboardDispatcher, mouseDispatcher) {
        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "Creating window: {0} ({1}, {2})", descriptor.title, descriptor.width, descriptor.height);
        CLOVE_ASSERT(window == 0, "Window already exists! Currently only a single window on linux is supported");

        if(display == nullptr) {
            //Make the connection to the client, where to display the window
            display = XOpenDisplay(nullptr);
        }

        if(display == nullptr) {
            //TODO: Exception
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Could not open display");
            return;
        }

        screen   = DefaultScreenOfDisplay(display);//NOLINT Get the screen of the display
        screenID = DefaultScreen(display);         //NOLINT

        XSetWindowAttributes windowAttribs{
            .background_pixel  = WhitePixel(display, screenID),//NOLINT clang-tidy complains about a cast inside this macro
            .border_pixel      = BlackPixel(display, screenID),//NOLINT clang-tidy complains about a cast inside this macro
            .event_mask        = ExposureMask,
            .override_redirect = 1,
            .colormap          = XDefaultColormap(display, screenID),
        };

        window = XCreateWindow(display, RootWindow(display, screenID), 0, 0, descriptor.width, descriptor.height, 0, screen->depths[0].depth, InputOutput, screen->root_visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttribs);//NOLINT

        //Remap the delete window message so we can gracefully close the application
        atomWmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", 0);
        XSetWMProtocols(display, window, &atomWmDeleteWindow, 1);

        int constexpr discardEvents{ 1 };
        XSync(display, discardEvents);

        long constexpr keyboardMask{ KeyPressMask | KeyReleaseMask | KeymapStateMask };
        long constexpr mouseMask{ PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask };
        XSelectInput(display, window, keyboardMask | mouseMask | StructureNotifyMask);

        XStoreName(display, window, descriptor.title.c_str());

        XClearWindow(display, window);
        XMapRaised(display, window);

        open = true;

        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "Window created");
    }

    LinuxWindow::~LinuxWindow() {
        XCloseDisplay(display);
    }

    std::unique_ptr<Window> Window::create(Descriptor const &descriptor) {
        return std::make_unique<LinuxWindow>(descriptor);
    }

    std::any LinuxWindow::getNativeWindow() const {
        return std::make_pair(display, window);
    }

    vec2i LinuxWindow::getPosition(bool clientArea) const {
        int32_t x{ 0 };
        int32_t y{ 0 };
        ::Window child{};
        XTranslateCoordinates(display, window, XRootWindowOfScreen(screen), 0, 0, &x, &y, &child);

        if(clientArea) {
            return { x, y };
        } else {
            XWindowAttributes attribs{};
            XGetWindowAttributes(display, window, &attribs);
            return { x - attribs.x, y - attribs.y };
        }
    }

    vec2i LinuxWindow::getSize(bool clientArea) const {
        XWindowAttributes attribs{};
        XGetWindowAttributes(display, window, &attribs);
        if(clientArea) {
            return { attribs.width, attribs.height };
        } else {
            /* 
            There is no easy way to get the border height. So instead add on the 
            difference between the client pos y and window pos y
            */
            int32_t clientY{ getPosition(true).y };
            int32_t windowY{ getPosition(false).y };

            return { attribs.width + attribs.border_width, attribs.height + (clientY - windowY) };
        }
    }

    void LinuxWindow::moveWindow(vec2i const &position) {
        XMoveWindow(display, window, position.x, position.y);
    }

    void LinuxWindow::resizeWindow(vec2i const &size) {
        XResizeWindow(display, window, size.x, size.y);
    }

    bool LinuxWindow::isOpen() const {
        return open;
    }

    void LinuxWindow::close() {
        closeWindow(window);
    }

    void LinuxWindow::processInput() {
        while(XPending(display) > 0) {
            XEvent xevent{};
            XNextEvent(display, &xevent);

            switch(xevent.type) {
                case ClientMessage:
                    if(xevent.xclient.data.l[0] == atomWmDeleteWindow) {
                        //Need to use the window from the event here.
                        closeWindow(xevent.xclient.window);
                        //Flush the output buffer before exiting. Not doing this step causes the whole OS to freeze.
                        int constexpr discardEvents{ 1 };
                        XSync(display, discardEvents);
                    }
                    break;

                //Keyboard
                case FocusOut:
                    keyboardDispatcher.clearState();
                    break;

                case KeymapNotify:
                    //Refresh key mappings if the user rebinds keys
                    XRefreshKeyboardMapping(&xevent.xmapping);
                    break;

                case KeyPress: {
                    bool isRepeat = false;

                    if(XEventsQueued(display, QueuedAlready) != 0) {
                        XEvent nextEvent;
                        XPeekEvent(display, &nextEvent);

                        isRepeat = nextEvent.type == KeyPress && nextEvent.xkey.keycode == xevent.xkey.keycode;
                        if(isRepeat) {
                            //Consume the next KeyPress event (as that is auto repeat)
                            XNextEvent(display, &xevent);
                        }
                    }

                    if(!isRepeat || keyboard.isAutoRepeatEnabled()) {
                        KeySym xkeysym{ XLookupKeysym(&xevent.xkey, 0) };
                        keyboardDispatcher.onKeyPressed(static_cast<Key>(xkeysym));
                    }
                } break;

                case KeyRelease: {
                    KeySym xkeysym{ XLookupKeysym(&xevent.xkey, 0) };
                    keyboardDispatcher.onKeyReleased(static_cast<Key>(xkeysym));
                } break;

                    //TODO: Char (I don't think Xlib has a 'typed' event)

                //Mouse
                case MotionNotify:
                    mouseDispatcher.onMouseMove(vec2i{ xevent.xbutton.x, xevent.xbutton.y });
                    break;

                case EnterNotify:
                    mouseDispatcher.onMouseEnter();
                    break;

                case LeaveNotify:
                    mouseDispatcher.onMouseLeave();
                    break;

                case ButtonPress:
                    if(xevent.xbutton.button == Button4) {
                        mouseDispatcher.onWheelDelta(CLV_WHEEL_DELTA, vec2i{ xevent.xbutton.x, xevent.xbutton.y });
                    } else if(xevent.xbutton.button == Button5) {
                        mouseDispatcher.onWheelDelta(-CLV_WHEEL_DELTA, vec2i{ xevent.xbutton.x, xevent.xbutton.y });
                    } else {
                        mouseDispatcher.onButtonPressed(static_cast<MouseButton>(xevent.xbutton.button), vec2i{ xevent.xbutton.x, xevent.xbutton.y });
                    }
                    break;

                case ButtonRelease:
                    mouseDispatcher.onButtonReleased(static_cast<MouseButton>(xevent.xbutton.button), vec2i{ xevent.xbutton.x, xevent.xbutton.y });
                    break;

                //Window
                case ConfigureNotify: {
                    XConfigureEvent xce{ xevent.xconfigure };
                    if(static_cast<uint32_t>(xce.width) != prevConfigureNotifySize.x || static_cast<uint32_t>(xce.height) != prevConfigureNotifySize.y) {
                        const vec2i size{ xce.width, xce.height };
                        prevConfigureNotifySize = size;
                        onWindowResize.broadcast(size);
                    }
                } break;
            }
        }
    }

    void LinuxWindow::closeWindow(::Window window) {
        if(onWindowCloseDelegate.isBound()) {
            onWindowCloseDelegate.broadcast();
        }
        open = false;
        XDestroyWindow(display, window);
    }
}