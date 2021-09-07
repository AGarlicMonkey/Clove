#pragma once

#include <Clove/Expected.hpp>
#include <any>
#include <memory>
#include <stdexcept>

namespace clove {
    class VulkanDevice;
}

namespace clove {
    Expected<std::unique_ptr<VulkanDevice>, std::runtime_error> createVulkanDevice(std::any nativeWindow) noexcept;
}