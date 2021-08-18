#pragma once

#include "Clove/Audio/AhaFactory.hpp"

namespace clove {
    class OpenAlFactory : public AhaFactory {
        //FUNCTIONS
    public:
        OpenAlFactory();

        OpenAlFactory(OpenAlFactory const &other) = delete;
        OpenAlFactory(OpenAlFactory &&other) noexcept;

        OpenAlFactory &operator=(OpenAlFactory const &other) = delete;
        OpenAlFactory &operator=(OpenAlFactory &&other) noexcept;

        ~OpenAlFactory();

        Expected<std::unique_ptr<AhaBuffer>, std::runtime_error> createAudioBuffer(AhaBuffer::Descriptor descriptor) override;
        Expected<std::unique_ptr<AhaSource>, std::runtime_error> createAudioSource() override;
        Expected<std::unique_ptr<AhaListener>, std::runtime_error> createAudioListener() override;
    };
}
