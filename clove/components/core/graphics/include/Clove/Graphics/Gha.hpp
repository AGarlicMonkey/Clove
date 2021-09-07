#pragma once

#include "Clove/Graphics/GraphicsAPI.hpp"

#include <Clove/Expected.hpp>
#include <any>
#include <memory>
#include <stdexcept>

namespace clove {
    class GhaDevice;
}

namespace clove {
    /**
     * @brief Create a GHA (Graphics Hardware Api) Device to communicate with the graphics hardware.
     * @param api What API to use.
     * @param nativeWindow The window to bind to. Using an empty std::any will create a headless device.
     */
    Expected<std::unique_ptr<GhaDevice>, std::runtime_error> createGhaDevice(GraphicsApi api, std::any nativeWindow) noexcept;
}
