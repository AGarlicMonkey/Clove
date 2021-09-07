#include "Clove/Audio/Aha.hpp"

#include "Clove/Audio/OpenAL/OpenAlAha.hpp"
#include "Clove/Audio/OpenAL/OpenAlDevice.hpp"

namespace clove {
    Expected<std::unique_ptr<AhaDevice>, std::runtime_error> createAhaDevice(AudioApi api) noexcept {
        std::unique_ptr<AhaDevice> device{ nullptr };
        switch(api) {
            case AudioApi::OpenAl: {
                auto result{ createOpenAlDevice() };

                if(!result.hasValue()) {
                    return Unexpected{ std::move(result.getError()) };
                }else{
                    device = std::move(result.getValue());
                }
            } break;
        }

        return device;
    }
}