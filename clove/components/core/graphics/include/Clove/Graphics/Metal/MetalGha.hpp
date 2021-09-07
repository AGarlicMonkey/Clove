#pragma once

#include <Clove/Expected.hpp>
#include <any>
#include <memory>
#include <stdexcept>

namespace clove {
    class MetalDevice;
}

namespace clove {
    Expected<std::unique_ptr<MetalDevice>, std::runtime_error> createMetalDevice(std::any nativeWindow) noexcept;
}
