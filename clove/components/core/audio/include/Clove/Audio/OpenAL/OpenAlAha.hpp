#pragma once

#include <Clove/Expected.hpp>
#include <memory>
#include <stdexcept>

namespace clove {
    class OpenAlDevice;
}

namespace clove {
    Expected<std::unique_ptr<OpenAlDevice>, std::runtime_error> createOpenAlDevice() noexcept;
}