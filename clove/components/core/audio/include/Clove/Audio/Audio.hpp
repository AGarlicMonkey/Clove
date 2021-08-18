#pragma once

#include <memory>

namespace clove {
    enum class AudioApi {
        OpenAl
    };

    class AhaDevice;
}

namespace clove {
    std::unique_ptr<AhaDevice> createAudioDevice(AudioApi api);
}