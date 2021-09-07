#pragma once

#include <Clove/Expected.hpp>
#include <memory>
#include <stdexcept>

namespace clove {
    enum class AudioApi {
        OpenAl
    };

    class AhaDevice;
}

namespace clove {
    Expected<std::unique_ptr<AhaDevice>, std::runtime_error> createAhaDevice(AudioApi api) noexcept;
}