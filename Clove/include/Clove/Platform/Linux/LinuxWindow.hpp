#pragma once

#include <Clove/Core/Maths/Vector.hpp>
#include "Clove/Platform/Core/Window.hpp"

namespace clv::plt{
	struct LinuxData{
		Display* display = nullptr;
		::Window* window;

		XVisualInfo** visual = nullptr;
	};

    class LinuxWindow : public Window{
        //VARIABLES
	private:
		Display* display = nullptr;
		Screen* screen = nullptr;
		int32 screenID;
        ::Window window;

        XEvent xevent;

		XVisualInfo* visual = nullptr; //TODO: unique_ptr with custom deleter
		XSetWindowAttributes windowAttribs;

		Atom atomWmDeleteWindow;

		mth::vec2i prevConfigureNotifySize = {};

		LinuxData data;

        //FUNCTIONS
    public:
        LinuxWindow() = delete;
		LinuxWindow(const WindowProps& props);
		LinuxWindow(const Window& parentWindow, const mth::vec2i& position, const mth::vec2i& size);

		LinuxWindow(const LinuxWindow& other) = delete;
		LinuxWindow(LinuxWindow&& other) noexcept = delete;

		LinuxWindow& operator=(const LinuxWindow& other) = delete;
		LinuxWindow& operator=(LinuxWindow&& other) noexcept = delete;

		virtual ~LinuxWindow();

		virtual void* getNativeWindow() const override;

        virtual mth::vec2i getPosition() const override;
        virtual mth::vec2i getSize() const override;

        virtual void moveWindow(const mth::vec2i& position) override;
        virtual void resizeWindow(const mth::vec2i& size) override;

	protected:
		virtual void processInput() override;
    };
}