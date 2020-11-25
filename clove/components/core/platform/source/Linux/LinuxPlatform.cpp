#include "Clove/Platform/Linux/LinuxPlatform.hpp"

#include "Clove/Platform/Linux/LinuxWindow.hpp"

namespace garlic::clove {
    LinuxPlatform::LinuxPlatform() = default;

    LinuxPlatform::LinuxPlatform(LinuxPlatform &&other) noexcept = default;

    LinuxPlatform &LinuxPlatform::operator=(LinuxPlatform &&other) noexcept = default;

    LinuxPlatform::~LinuxPlatform() = default;

    std::shared_ptr<Window> LinuxPlatform::createWindow(WindowDescriptor const &props) {
        return std::make_shared<LinuxWindow>(props);
    }

    std::shared_ptr<Window> LinuxPlatform::createChildWindow(Window const &parentWindow, vec2i const &position, vec2i const &size) {
        return std::make_shared<LinuxWindow>(parentWindow, position, size);
    }
}