#include "Clove/Platform/Windows/WindowsWindow.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

#define CLV_WINDOWS_QUIT 25397841//Note: this number is completely random

namespace clove {
    WindowsWindow::WindowsWindow(Descriptor const &descriptor)
        : Window(keyboardDispatcher, mouseDispatcher) {
        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "Creating window: {0} ({1}, {2})", descriptor.title, descriptor.width, descriptor.height);

        instance = GetModuleHandle(nullptr);

        WNDCLASSEX windowClass {
            .cbSize        = sizeof(windowClass),
            .style         = CS_OWNDC,
            .lpfnWndProc   = HandleMsgSetup,
            .cbClsExtra    = 0,
            .cbWndExtra    = 0,
            .hInstance     = instance,
            .hIcon         = nullptr,
            .hCursor       = nullptr,
            .hbrBackground = nullptr,
            .lpszMenuName  = nullptr,
            .lpszClassName = className,
        };
        RegisterClassEx(&windowClass);

        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "Windows class registered");

        std::string const wideTitle(descriptor.title.begin(), descriptor.title.end());

        DWORD const windowStyle{ WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX | WS_SYSMENU | WS_VISIBLE };

        RECT windowRect {
            .left   = 0,
            .top    = 0,
            .right  = descriptor.width,
            .bottom = descriptor.height,
        };
        AdjustWindowRect(&windowRect, windowStyle, FALSE);

        windowsHandle = CreateWindow(
            windowClass.lpszClassName,
            wideTitle.c_str(),
            windowStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            nullptr,
            instance,
            this);

        open = true;

        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "Window created");
    }

    WindowsWindow::~WindowsWindow() {
        if(isOpen()) {
            close();
        }

        UnregisterClass(className, instance);
        DestroyWindow(windowsHandle);
    }

    std::unique_ptr<Window> Window::create(Descriptor const &descriptor) {
        return std::make_unique<WindowsWindow>(descriptor);
    }

    std::any WindowsWindow::getNativeWindow() const {
        return windowsHandle;
    }

    vec2i WindowsWindow::getPosition(bool clientArea) const {
        RECT windowRect{};
        if(clientArea) {
            GetClientRect(windowsHandle, &windowRect);
            MapWindowPoints(windowsHandle, HWND_DESKTOP, (LPPOINT)&windowRect, 2);
        } else {
            GetWindowRect(windowsHandle, &windowRect);
        }

        return { windowRect.left, windowRect.top };
    }

    vec2i WindowsWindow::getSize(bool clientArea) const {
        RECT windowRect{};
        if(clientArea) {
            GetClientRect(windowsHandle, &windowRect);
        } else {
            GetWindowRect(windowsHandle, &windowRect);
        }

        return { windowRect.right - windowRect.left, windowRect.bottom - windowRect.top };
    }

    void WindowsWindow::moveWindow(vec2i const &position) {
        vec2i const size{ getSize(false) };
        MoveWindow(windowsHandle, position.x, position.y, size.x, size.y, FALSE);
    }

    void WindowsWindow::resizeWindow(vec2i const &size) {
        vec2i const position{ getPosition(false) };
        MoveWindow(windowsHandle, position.x, position.y, size.x, size.y, FALSE);
    }

    bool WindowsWindow::isOpen() const {
        return open;
    }

    void WindowsWindow::close() {
        if(onWindowCloseDelegate.isBound()) {
            onWindowCloseDelegate.broadcast();
        }
        open = false;
        CloseWindow(windowsHandle);
    }

    void WindowsWindow::processInput() {
        MSG msg;
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if(msg.wParam == CLV_WINDOWS_QUIT) {
                close();
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    LRESULT CALLBACK WindowsWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if(msg == WM_NCCREATE) {
            //Extract the ptr to our window class
            CREATESTRUCTW const *const create{ reinterpret_cast<CREATESTRUCTW *>(lParam) };
            WindowsWindow *const window{ static_cast<WindowsWindow *>(create->lpCreateParams) };
            //Store our windows class into the windows api
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            //Switch over to the normal procedure handler
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowsWindow::HandleMsgThunk));

            return window->HandleMsg(hWnd, msg, wParam, lParam);
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK WindowsWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        //Forward message to our windows instance
        WindowsWindow *const window{ reinterpret_cast<WindowsWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) };
        return window->HandleMsg(hWnd, msg, wParam, lParam);
    }

    LRESULT WindowsWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        POINTS const pt{ MAKEPOINTS(lParam) };
        vec2i const pos{ pt.x, pt.y };

        switch(msg) {
            case WM_CLOSE:
                PostQuitMessage(CLV_WINDOWS_QUIT);
                return 0;

            case WM_KILLFOCUS:
                keyboardDispatcher.clearState();
                break;

                //Keyboard
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                if(!(lParam & (1 << 30)) || keyboard.isAutoRepeatEnabled()) {
                    keyboardDispatcher.onKeyPressed(static_cast<Key>(wParam));
                }
                break;

            case WM_KEYUP:
            case WM_SYSKEYUP:
                keyboardDispatcher.onKeyReleased(static_cast<Key>(wParam));
                break;

            case WM_CHAR:
                keyboardDispatcher.onChar(static_cast<char>(wParam));
                break;

                //Mouse
            case WM_MOUSEMOVE:
                if(pos.x >= 0 && pos.x < getSize(true).x && pos.y >= 0 && pos.y < getSize(true).y) {
                    mouseDispatcher.onMouseMove(pos);
                    if(!mouse.isInWindow()) {
                        mouseDispatcher.onMouseEnter();
                        SetCapture(hWnd);
                    }
                } else {
                    if(mouse.isButtonPressed(MouseButton::Left) || mouse.isButtonPressed(MouseButton::Right)) {
                        mouseDispatcher.onMouseMove(pos);
                    } else {
                        mouseDispatcher.onMouseLeave();
                        ReleaseCapture();
                    }
                }
                break;

            case WM_LBUTTONDOWN:
                mouseDispatcher.onButtonPressed(MouseButton::Left, pos);
                break;

            case WM_LBUTTONUP:
                mouseDispatcher.onButtonReleased(MouseButton::Left, pos);
                break;

            case WM_RBUTTONDOWN:
                mouseDispatcher.onButtonPressed(MouseButton::Right, pos);
                break;

            case WM_RBUTTONUP:
                mouseDispatcher.onButtonReleased(MouseButton::Right, pos);
                break;

            case WM_XBUTTONDOWN:
                mouseDispatcher.onButtonPressed(static_cast<MouseButton>(GET_KEYSTATE_WPARAM(wParam)), pos);
                break;

            case WM_XBUTTONUP:
                mouseDispatcher.onButtonReleased(static_cast<MouseButton>(GET_KEYSTATE_WPARAM(wParam)), pos);
                break;

            case WM_MOUSEWHEEL:
                mouseDispatcher.onWheelDelta(GET_WHEEL_DELTA_WPARAM(wParam), pos);
                break;

                //Window
            case WM_SIZE: {
                vec2ui const size{ pt.x, pt.y };
                onWindowResize.broadcast(size);
            } break;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}
